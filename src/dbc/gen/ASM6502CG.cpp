#include <dbc/gen/ASM6502CG.h>
#include <dbc/Utility.h>
#include <algorithm>

using namespace dbc;

static constexpr uint16 DefaultCount = 0;
static std::string CurrentOp = "lda";
static int16 CurrentReturnOffset = 0;
static bool GotOnScope = false;
static int16 CurrentVarOffset = 0;
static uint16 ScopeDeep = DefaultCount;
static uint16 WhileCount = DefaultCount;
static uint16 AndCount = DefaultCount;
static uint16 IfCount = DefaultCount;
static uint16 ElseIfCount = DefaultCount;
static uint16 CmpExprCount = DefaultCount;
static uint16 OrExprCount = DefaultCount;
static uint16 AndExprCount = DefaultCount;
static uint64 IfAccum = DefaultCount;
static uint64 WhileAccum = DefaultCount;
static std::string CmpLabel0 = "";
static std::string CmpLabel1 = "";
static std::string CurrentFunc = "";
static std::string CmpLabelSufix = "";
static bool ForceJmp = true;
static bool UsingMul = false;
static bool UsingDiv = false;
static bool UsingShl = false;
static bool UsingShr = false;
static bool UsingMod = false;
static bool UsingNot = false;
static LeafPtr StaticLeafPtr = nullptr;
static constexpr const char NewLine[] = "\r";
static constexpr const char LabelTail[] = "\r";
static size_t MainLine = 0;

dbc::gen::ASM6502CG::ASM6502CG(bool Verbose)
{
	this->Verbose = Verbose;
	if (Verbose) AddLine("; DAMNBASIC to 6502 assembly code generated.");
	if (Verbose) AddLine("; Setting origin to $0600");

	AddLabel("main");
	AddLine("*=$1000");
	AddLine("jmp " + GetLabelId("main"));
	AddLine(GetLabelId("main") + std::string(LabelTail));
	PushScope("global");
}

void dbc::gen::ASM6502CG::LoadDependencies()
{
	if (UsingMul)
	{
		AddLine("sysmathmul\rpla \rsta $2002\rpla\rsta $2005\rpla\rtax\rpla \rtay \rlda #$00\rsta $2009\rstx $2008\rsysmathmul0\rclc\rinc $2009\rdex\rcpx #$00\rbne sysmathmul0\rldx $2008\rdey\rcpy #$00\rbne sysmathmul0\rsec\rldy $2009\rtya\rpha\rlda $2005\rpha\rlda $2002\rpha\rrts");
	}
	if (UsingDiv)
	{
		AddLine("sysmathdiv\rpla \rsta $2002\rpla\rsta $2005\rpla\rtax\rpla \rtay \rlda #$00\rsta $2007\rstx $2008\rsty $2009\rsysmathdiv0\rdex\rdey\rcpx #$00\rbne sysmathdiv0\rldx $2008\rclc\rinc $2007\rcpy #$00\rbne sysmathdiv0\rcpy $2009\rbeq sysmathdiv0\rbcc sysmathdiv0\rldy $2007\rtya\rpha\rlda $2005\rpha\rlda $2002\rpha\rrts");
	}
	if (UsingShl)
	{
		AddLine("sysbitwiseshl\rpla \rsta $2002\rpla\rsta $2003   \rpla\rtax\rpla \rsysbitwiseshl0\rasl a\rdex\rcpx #$00\rbne sysbitwiseshl0\rpha \rlda $2003\rpha\rlda $2002\rpha\rrts");
	}
	if (UsingShr)
	{
		AddLine("sysbitwiseshr\rpla \rsta $2002\rpla\rsta $2003   \rpla\rtax\rpla \rsysbitwiseshr0\rlsr a\rdex\rcpx #$00\rbne sysbitwiseshr0\rpha \rlda $2003\rpha\rlda $2002\rpha\rrts");
	}
	if (UsingMod)
	{
		AddLine("sysmathmod\rpla\rsta $2002\rpla\rsta $2003\rpla\rsta $2004\rpla\rsec\rsysmathmod0\rsbc $2004\rbcs sysmathmod0\radc $2004\rpha\rlda $2003\rpha\rlda $2002\rpha\rrts");
	}
}

void dbc::gen::ASM6502CG::Generate(LeafPtr Node, const char * File, bool IsMain)
{
	if (IsMain && Verbose)
	{
		printf("Generating 6502 Assembly Code...\r");
	}
	CurrentFile = File;
	LeafPtr Current = Node;

	if (Verbose) AddLine("; Global Data");
	ScanningGlobals = true;
	while (Current != nullptr && Current->Type != LeafType::ENDOFFILE)
	{
		if (Current->Left != nullptr && Current->Left->Type == LeafType::DECL_VAR)
		{
			StaticLeafPtr = Current->Left;
			AddLine(GenDeclVar(Current->Left));
		}

		if (Current->Right != nullptr)
		{
			Current = Current->Right;
		}
		else
		{
			break;
		}
	}
	ScanningGlobals = false;
	if (Verbose) AddLine("; Code");
	if (IsMain)
	{
		MainLine = Lines.size();
		AddLine("jsr error");
		if (Verbose) AddLine("; store return value of main at accumulator");
		AddLine("pla");
		AddLine("brk");
	}
	Current = Node;
	EnterScope();
	CurrentVarOffset = 0;
	while (Current != nullptr && Current->Type != LeafType::ENDOFFILE)
	{
		StaticLeafPtr = Current->Left;
		AddLine(GenCode(Current->Left));
		if (Current->Right != nullptr)
		{
			Current = Current->Right;
		}
		else
		{
			break;
		}
	}
	ExitScope();
	AddLine("brk");
	if (IsMain)
	{
		LoadDependencies();
		Lines[MainLine] = "jsr " + GetLabelId("funcmain");
	}

}

std::string dbc::gen::ASM6502CG::GenCode(LeafPtr Node)
{
	if (Node == nullptr)
	{
		return "";
	}
	StaticLeafPtr = Node;
	if (IsExprGen(Node))
	{
		return GenExpr(Node);
	}
	else if (IsConst(Node))
	{
		return GenConst(Node);
	}
	else if (Node->Type == LeafType::STMT_IF)
	{
		return GenStmtIf(Node);
	}
	else if (Node->Type == LeafType::DECL_FUNC)
	{
		return GenDeclFunc(Node);
	}
	else if (Node->Type == LeafType::DECL_VAR)
	{
		return GenDeclVar(Node);
	}
	else if (Node->Type == LeafType::STMT_RETURN)
	{
		return GenStmtReturn(Node);
	}
	else if (Node->Type == LeafType::STMT_ASSIGN)
	{
		return GenStmtAssign(Node);
	}
	else if (Node->Type == LeafType::STMT_WHILE)
	{
		return GenStmtWhile(Node);
	}
	else if (Node->Type == LeafType::STMT)
	{
		std::string Out;
		if (Node->Left != nullptr)
		{
			Out += GenCode(Node->Left);
		}
		if (Node->Right != nullptr)
		{
			Out += GenCode(Node->Right);
		}
		return Out;
	}
	else if (Node->Type == LeafType::STMT_SEQ)
	{
		std::string Out;
		if (Node->Left != nullptr)
		{
			Out += GenCode(Node->Left);
		}
		Out += GenCode(Node->Right);
	}
	return Verbose ? "INVALID" : "";;
}

std::string dbc::gen::ASM6502CG::GenConst(LeafPtr Node)
{
	if (Node->Type == LeafType::CONST_BOOLEAN)
	{
		return GenConstBoolean(Node) + "\r";
	}
	else if (Node->Type == LeafType::CONST_NUMBER)
	{
		return GenConstNumber(Node) + "\r";
	}
	else if (Node->Type == LeafType::CONST_IDENTIFIER)
	{
		return GenConstIdentifier(Node) + "\r";
	}
	return Verbose ? "INVALID" : "";;
}

std::string dbc::gen::ASM6502CG::GenConstIdentifier(LeafPtr Node)
{
	std::string Out;
	int16 Addr = GetLocal(Node->STRING);
	if (Addr == -1)
	{
		if (Verbose) Out += "; getting global var " + std::string(Node->STRING) + "\r";
		Addr = GetGlobal(Node->STRING);
		Out += "lda $" + NumberToHex<uint16>(Addr);
	}
	else
	{
		if (GotOnScope)
		{
			if (Verbose) Out += "; getting local var " + std::string(Node->STRING) + "\r";
			Out += "lda #$" + NumberToHex<uint8>(0xFF + Addr);
		}
		else
		{
			if (Verbose) Out += "; getting arg var " + std::string(Node->STRING) + "\r";
			Out += "lda #$" + NumberToHex<uint8>(4 + Addr);
		}
		Out += "\rclc\r"
			"adc $2000\r"
			"tax\r"
			"lda $0100, x";
	}
	return Out;
}

std::string dbc::gen::ASM6502CG::GenConstBoolean(LeafPtr Node)
{
	return CurrentOp + "#$" + NumberToHex<uint8>(Node->UINT8);
}

std::string dbc::gen::ASM6502CG::GenConstNumber(LeafPtr Node)
{
	return CurrentOp + "#$" + NumberToHex<uint8>(Node->UINT8);
}

std::string dbc::gen::ASM6502CG::GenExpr(LeafPtr Node)
{
	if (Node->Type == LeafType::EXPR_ADD)
	{
		return GenExprAdd(Node);
	}
	else if (Node->Type == LeafType::EXPR_SUB)
	{
		return GenExprSub(Node);
	}
	else if (Node->Type == LeafType::EXPR_MUL)
	{
		return GenExprMul(Node);
	}
	else if (Node->Type == LeafType::EXPR_DIV)
	{
		return GenExprDiv(Node);
	}
	else if (IsCompare(Node))
	{
		return GenExprCompare(Node);
	}
	else if (IsConst(Node))
	{
		return GenConst(Node);
	}
	else if (IsBitwise(Node))
	{
		return GenExprBitwise(Node);
	}
	else if (IsLogical(Node))
	{
		return GenExprLogical(Node);
	}
	else if (Node->Type == LeafType::EXPR_NEGATE)
	{
		return GenExprNegate(Node);
	}
	else if (Node->Type == LeafType::EXPR_CALL)
	{
		return GenExprCall(Node);
	}
	else if (Node->Type == LeafType::EXPR_NOT)
	{
		return GenExprNot(Node);
	}
	else if (Node->Type == LeafType::EXPR_MOD)
	{
		UsingMod = true;
		std::string Out;
		if (Verbose) Out = "; modulus\r";
		Out += GenCode(Node->Left);
		Out += "pha\r";
		Out += GenCode(Node->Right);
		Out += "pha\r";
		Out += "jsr sysmathmod\r";
		Out += "pla\r";
		return Out;
	}
	return Verbose ? "INVALID" : "";;
}

std::string dbc::gen::ASM6502CG::GenExprNot(LeafPtr Node)
{
	std::string Out;
	if (Verbose) Out = "; not expression\r";
	UsingNot = true;
	Out += GenExpr(Node->Left);
	UsingNot = false;
	return Out;
}

std::string dbc::gen::ASM6502CG::GenExprCall(LeafPtr Node)
{
	std::string Out;
	if (Verbose) Out = ";alloc return\r";
	Out += "lda #$0\rpha\r";
	LeafPtr ExprList = Node->Right;
	while (ExprList != nullptr && ExprList->Type == LeafType::EXPR_LIST)
	{
		if (ExprList->Left != nullptr)
		{
			if (IsExprGen(ExprList->Left))
			{
				Out += GenCode(ExprList->Left);
				Out += "pha\r";
			}
			else if (IsConst(ExprList->Left))
			{
				Out += GenConst(ExprList->Left);
				Out += "pha\r";
			}
			else if (ExprList->Left->Type == LeafType::EXPR_CALL)
			{
				Out += GenExprCall(ExprList->Left);
				Out += "pha\r";
			}
		}
		ExprList = ExprList->Right;
	}
	Out += "jsr " + GetLabelId("func" + std::string(Node->Left->STRING));
	ExprList = Node->Right;
	if (Verbose) Out += "\r; clean up";
	Out += "\r";
	while (ExprList != nullptr && ExprList->Type == LeafType::EXPR_LIST)
	{
		if (ExprList->Left != nullptr)
		{
			if (IsExprGen(ExprList->Left))
			{
				Out += "pla\r";
			}
			else if (IsConst(ExprList->Left))
			{
				Out += "pla\r";
			}
			else if (ExprList->Left->Type == LeafType::EXPR_CALL)
			{
				Out += "pla\r";
			}
		}
		ExprList = ExprList->Right;
	}
	Out += "pla\r";
	return Out;
}

std::string dbc::gen::ASM6502CG::GenExprNegate(LeafPtr Node)
{
	std::string Out;
	Out += GenExpr(Node->Left);
	Out += "eor #$FF\r"
		"sec\r"
		"adc #0\r";
	return Out;
}

std::string dbc::gen::ASM6502CG::GenExprMul(LeafPtr Node)
{
	UsingMul = true;
	std::string Out;
	if (Verbose) Out = "; multiplication\r";
	Out += GenCode(Node->Left);
	Out += "pha\r";
	Out += GenCode(Node->Right);
	Out += "pha\rclc\r";
	Out += "jsr sysmathmul\r";
	Out += "pla\r";
	return Out;
}

std::string dbc::gen::ASM6502CG::GenExprDiv(LeafPtr Node)
{
	UsingDiv = true;
	std::string Out;
	if (Verbose) Out = "; divition\r";
	Out += GenCode(Node->Left);
	Out += "pha\r";
	Out += GenCode(Node->Right);
	Out += "pha\rsec\r";
	Out += "jsr sysmathdiv\r";
	Out += "pla\r";
	return Out;
}

std::string dbc::gen::ASM6502CG::GenExprAdd(LeafPtr Node)
{
	std::string Out;
	if (Verbose) Out = "; addition\r";
	Out += GenCode(Node->Left);
	Out += "pha\r";
	Out += GenCode(Node->Right);
	Out += "sta $2001\r";
	Out += "pla\r";
	Out += "clc\r";
	Out += "adc $2001\r";
	return Out;
}

std::string dbc::gen::ASM6502CG::GenExprSub(LeafPtr Node)
{
	std::string Out;
	if (Verbose) Out = "; subraction\r";
	Out += GenCode(Node->Left);
	Out += "pha\r";
	Out += GenCode(Node->Right);
	Out += "sta $2001\r";
	Out += "pla\r";
	Out += "sec\r";
	Out += "sbc $2001\r";
	return Out;
}

std::string dbc::gen::ASM6502CG::GenExprCompare(LeafPtr Node)
{
	PushScope("cmpexpr" + std::to_string(CmpExprCount) + "");
	++CmpExprCount;
	std::string Out;
	Out += GenExpr(Node->Right);
	Out += "sta $2001\r";
	Out += GenExpr(Node->Left);
	Out += "\rclc\r";
	Out += "cmp $2001\r";
	--CmpExprCount;
	std::string TrueBranch = GetScopeName() + "true" + std::to_string(CmpExprCount);
	std::string FalseBranch = GetScopeName() + "false" + std::to_string(CmpExprCount);
	AddLabel(TrueBranch);
	AddLabel(FalseBranch);
	if (Node->Type == LeafType::EXPR_CMPEQU)
	{
		Out += "beq " + GetLabelId(TrueBranch) + "\r";
		Out += "jmp " + GetLabelId(FalseBranch) + "\r";
	}
	else if (Node->Type == LeafType::EXPR_CMPNOTEQU)
	{
		Out += "bne " + GetLabelId(TrueBranch) + "\r";
		Out += "jmp " + GetLabelId(FalseBranch) + "\r";
	}
	else if (Node->Type == LeafType::EXPR_CMPGT)
	{
		Out += "beq " + GetLabelId(FalseBranch) + "\r";
		Out += "bcs " + GetLabelId(TrueBranch) + "\r";
	}
	else if (Node->Type == LeafType::EXPR_CMPLT)
	{
		Out += "bcc " + GetLabelId(TrueBranch) + "\r";
		Out += "jmp " + GetLabelId(FalseBranch) + "\r";
	}
	else if (Node->Type == LeafType::EXPR_CMPGTEQU)
	{
		Out += "bcs " + GetLabelId(TrueBranch) + "\r";
		Out += "jmp " + GetLabelId(FalseBranch) + "\r";
	}
	else if (Node->Type == LeafType::EXPR_CMPLTEQU)
	{
		Out += "bcc " + GetLabelId(TrueBranch) + "\r";
		Out += "beq " + GetLabelId(TrueBranch) + "\r";
		Out += "jmp " + GetLabelId(FalseBranch) + "\r";
	}
	Out += GetLabelId(FalseBranch) + LabelTail;
	if (UsingNot)
	{
		Out += "lda #$01\r";
	}
	else
	{
		Out += "lda #$00\r";
	}
	AddLabel(GetScopeName() + "endcmp" + std::to_string(CmpExprCount));
	Out += "jmp " + GetLabelId(GetScopeName() + "endcmp" + std::to_string(CmpExprCount)) + "\r";
	Out += GetLabelId(TrueBranch) + LabelTail;
	if (!UsingNot)
	{
		Out += "lda #$01\r";
	}
	else
	{
		Out += "lda #$00\r";
	}
	AddLabel(GetScopeName() + "endcmp" + std::to_string(CmpExprCount));
	Out += GetLabelId(GetScopeName() + "endcmp" + std::to_string(CmpExprCount)) + LabelTail;
	PopScope();
	return Out;
}

std::string dbc::gen::ASM6502CG::GenExprLogical(LeafPtr Node)
{
	std::string Out;
	if (Node->Type == LeafType::EXPR_AND)
	{
		PushScope("andexpr" + std::to_string(AndExprCount) + "");
		std::string ScopeName = GetScopeName();
		std::string AndEndName = ScopeName + "endand" + std::to_string(AndExprCount);
		std::string AndBranchTrue = ScopeName + "endandtrue" + std::to_string(AndExprCount);
		std::string AndBranchFalse = ScopeName + "endandfalse" + std::to_string(AndExprCount);
		AddLabel(AndEndName);
		AddLabel(AndBranchTrue);
		AddLabel(AndBranchFalse);
		++AndExprCount;
		if (Verbose) Out += "; and expression\r";
		// check lhs or expr is valid
		PushScope("lhs");
		Out += GenExpr(Node->Left);
		PopScope();
		if (Verbose) Out += ";check lhs and expr is valid\r";
		Out += "ldx #$00\r";
		Out += "stx $2001\r";
		Out += "clc\r";
		Out += "cmp $2001\r";
		Out += "beq " + GetLabelId(AndBranchFalse) + "\r";
		// check rhs or expr is valid
		PushScope("rhs");
		Out += GenExpr(Node->Right);
		PopScope();
		if (Verbose) Out += ";check rhs and expr is valid\r";
		Out += "ldx #$00\r";
		Out += "stx $2001\r";
		Out += "clc\r";
		Out += "cmp $2001\r";
		Out += "beq " + GetLabelId(AndBranchFalse) + "\r";
		Out += "jmp " + GetLabelId(AndBranchTrue) + "\r";
		--AndExprCount;
		Out += GetLabelId(AndBranchFalse) + LabelTail;
		if (UsingNot)
		{
			Out += "lda #$01\r";
		}
		else
		{
			Out += "lda #$00\r";
		}
		Out += "jmp " + GetLabelId(AndEndName) + "\r";
		Out += GetLabelId(AndBranchTrue) + LabelTail;
		if (!UsingNot)
		{
			Out += "lda #$01\r";
		}
		else
		{
			Out += "lda #$00\r";
		}
		PopScope();
		Out += GetLabelId(AndEndName) + LabelTail;
	}
	else if (Node->Type == LeafType::EXPR_OR)
	{
		PushScope("orexpr" + std::to_string(OrExprCount) + "");
		std::string ScopeName = GetScopeName();
		std::string OrEndName = ScopeName + "endor" + std::to_string(OrExprCount);
		std::string OrBranchTrue = ScopeName + "endortrue" + std::to_string(OrExprCount);
		std::string OrBranchFalse = ScopeName + "endorfalse" + std::to_string(OrExprCount);
		AddLabel(OrEndName);
		AddLabel(OrBranchTrue);
		AddLabel(OrBranchFalse);
		++OrExprCount;
		if (Verbose) Out += "; or expression\r";
		// check lhs or expr is valid
		PushScope("lhs");
		Out += GenExpr(Node->Left);
		PopScope();
		if (Verbose) Out += ";check lhs or expr is valid\r";
		Out += "ldx #$00\r";
		Out += "stx $2001\r";
		Out += "clc\r";
		Out += "cmp $2001\r";
		Out += "bne " + GetLabelId(OrBranchTrue) + "\r";
		// check rhs or expr is valid
		PushScope("rhs");
		Out += GenExpr(Node->Right);
		PopScope();
		if (Verbose) Out += ";check rhs or expr is valid\r";
		Out += "ldx #$00\r";
		Out += "stx $2001\r";
		Out += "clc\r";
		Out += "cmp $2001\r";
		Out += "bne " + GetLabelId(OrBranchTrue) + "\r";
		Out += "jmp " + GetLabelId(OrBranchFalse) + "\r";
		--OrExprCount;
		Out += GetLabelId(OrBranchFalse) + LabelTail;
		if (UsingNot)
		{
			Out += "lda #$01\r";
		}
		else
		{
			Out += "lda #$00\r";
		}
		Out += "jmp " + GetLabelId(OrEndName) + "\r";
		Out += GetLabelId(OrBranchTrue) + LabelTail;
		if (!UsingNot)
		{
			Out += "lda #$01\r";
		}
		else
		{
			Out += "lda #$00\r";
		}
		PopScope();
		Out += GetLabelId(OrEndName) + LabelTail;
	}
	return Out;
}

std::string dbc::gen::ASM6502CG::GenExprBitwise(LeafPtr Node)
{
	std::string Out;
	if (Node->Type == LeafType::EXPR_SHL)
	{
		if (Verbose) Out += "; shift left\r";
		UsingShl = true;
		Out += GenCode(Node->Left);
		Out += "pha\r";
		Out += GenCode(Node->Right);
		Out += "pha\rclc\r";
		Out += "jsr sysbitwiseshl\r";
		Out += "pla\r";
		return Out;
	}
	else if (Node->Type == LeafType::EXPR_SHR)
	{
		if (Verbose) Out += "; shift right\r";
		UsingShr = true;
		Out += GenCode(Node->Left);
		Out += "pha\r";
		Out += GenCode(Node->Right);
		Out += "pha\rclc\r";
		Out += "jsr sysbitwiseshr\r";
		Out += "pla\r";
		return Out;
	}
	else if (Node->Type == LeafType::EXPR_BAND)
	{
		if (Verbose) Out += "; bitwise and\r";
		Out += GenCode(Node->Right);
		Out += "sta $2001\r";
		Out += GenCode(Node->Left);
		Out += "and $2001\r";
		return Out;
	}
	else if (Node->Type == LeafType::EXPR_BOR)
	{
		if (Verbose) Out += "; bitwise or\r";
		Out += GenCode(Node->Right);
		Out += "sta $2001\r";
		Out += GenCode(Node->Left);
		Out += "ora $2001\r";
		return Out;
	}
	else if (Node->Type == LeafType::EXPR_XOR)
	{
		if (Verbose) Out += "; bitwise exclusive or\r";
		Out += GenCode(Node->Right);
		Out += "sta $2001\r";
		Out += GenCode(Node->Left);
		Out += "eor $2001\r";
		return Out;
	}
	return Out;
}

std::string dbc::gen::ASM6502CG::GenExprList(LeafPtr Node)
{
	return Verbose ? "INVALID" : "";;
}

std::string dbc::gen::ASM6502CG::GenDeclVar(LeafPtr Node)
{
	std::string VarDecl;
	if (InFunction)
	{
		AddLocal(Node->Left->STRING, true);
		if (Verbose) VarDecl += "; local var " + std::string(Node->Left->STRING) + "\r";
		if (IsConst(Node->Right))
		{
			VarDecl += GenConst(Node->Right);
		}
		else if (IsExprGen(Node->Right))
		{
			VarDecl += GenExpr(Node->Right);
		}
		if (Verbose) VarDecl += "; set value local var " + std::string(Node->Left->STRING) + "\r";
		VarDecl += "pha\r";
	}
	else if (ScanningGlobals)
	{
		AddGlobal(Node->Left->STRING);
		if (Verbose) VarDecl += "; global var " + std::string(Node->Left->STRING) + "\r";
		VarDecl += GenCode(Node->Right);
		VarDecl += "sta $" + NumberToHex<uint16_t>(GlobalAddr++) + "\r";
	}
	return VarDecl;
}

std::string dbc::gen::ASM6502CG::GenDeclFunc(LeafPtr Node)
{
	InFunction = true;
	PushScope("local" + std::string(Node->Left->Left->STRING));
	CurrentFunc = Node->Left->Left->STRING;
	std::string FuncDecl;
	if (Verbose) FuncDecl = "; func " + CurrentFunc + "(";
	LeafPtr VarList = Node->Left->Right;
	CurrentReturnOffset = 4;
	while (VarList != nullptr && VarList->Type == LeafType::DECL_VARLIST)
	{
		if (VarList->Left != nullptr &&
			VarList->Left->Type == LeafType::CONST_IDENTIFIER)
		{
			AddLocal(VarList->Left->STRING, false);
			if (Verbose) FuncDecl += VarList->Left->STRING;
			++CurrentReturnOffset;
		}
		VarList = VarList->Right;
		if (VarList != nullptr &&
			VarList->Left != nullptr)
		{
			if (Verbose) FuncDecl += ", ";
		}
	}
	if (Verbose) FuncDecl += ")\r";
	AddLabel("func" + CurrentFunc);
	AddLabel("funcend" + CurrentFunc);
	FuncDecl += GetLabelId("func" + CurrentFunc) + LabelTail;
	EnterScope();
	// Prologue
	if (Verbose) FuncDecl += "; start prologue\r";
	FuncDecl += "lda $2000\r";
	FuncDecl += "pha\r";
	FuncDecl += "tsx\r";
	FuncDecl += "stx $2000\r";
	if (Verbose) FuncDecl += "; end prologue\r";
	if (Verbose) FuncDecl += "; function body start\r";
	if (Node->Right != nullptr)
	{
		FuncDecl += GenCode(Node->Right);
	}
	// Epilogue
	if (Verbose) FuncDecl += "; function body end\r";
	FuncDecl += GetLabelId("funcend" + CurrentFunc) + LabelTail;
	if (Verbose) FuncDecl += "; start epilogue\r";
	FuncDecl += "ldx $2000\r";
	FuncDecl += "txs\r";
	FuncDecl += "pla\r";
	FuncDecl += "sta $2000\r";
	FuncDecl += "rts\r";
	if (Verbose) FuncDecl += "; end epilogue\r";
	ExitScope();
	InFunction = false;
	PopScope();
	IfAccum = DefaultCount;
	WhileAccum = DefaultCount;
	return FuncDecl;
}

std::string dbc::gen::ASM6502CG::GenVarList(LeafPtr Node)
{
	return std::string();
}

std::string dbc::gen::ASM6502CG::GenStmtReturn(LeafPtr Node)
{
	std::string Out = GenCode(Node->Left);
	Out += "tay\r";
	if (Verbose) Out += "; store it at return space.\r";

	Out += "lda #$" + NumberToHex<uint8>(static_cast<uint8>(CurrentReturnOffset));
	Out += "\rclc\r"
		"adc $2000\r"
		"tax\r"
		"tya\r"
		"sta $0100, x\r"
		"jmp " + GetLabelId("funcend" + CurrentFunc) + "\r";
	return Out;
}

std::string dbc::gen::ASM6502CG::GenStmtWhile(LeafPtr Node)
{
	++WhileAccum;
	std::string Out;
	PushScope("whileaccum" + std::to_string(IfAccum));
	if (Verbose) Out = "; while statement\r";
	// output check expression
	std::string WhileName = GetScopeName() + "whilecheck" + std::to_string(WhileCount);
	AddLabel(WhileName);
	Out += GetLabelId(WhileName) + LabelTail;
	PushScope("whilestmt" + std::to_string(IfAccum) + "" + std::to_string(WhileCount) + "");
	Out += GenExprCheck(Node->Left);
	PopScope();
	// end of check
	Out += "ldx #$00\r";
	Out += "stx $2001\r";
	Out += "cmp $2001\r";
	AddLabel(GetScopeName() + "endwhile" + std::to_string(WhileCount));
	Out += "beq " + GetLabelId(GetScopeName() + "endwhile" + std::to_string(WhileCount)) + "\r";
	++WhileCount;
	// output body
	AddLabel(GetScopeName() + "whilebody" + std::to_string(WhileCount));
	Out += GetLabelId(GetScopeName() + "whilebody" + std::to_string(WhileCount)) + LabelTail;
	EnterScope();
	PushScope("whilestmt" + std::to_string(IfCount) + "");
	if (Verbose) Out += "; while stmt body\r";
	Out += GenCode(Node->Right);
	PopScope();
	ExitScope();
	Out += "jmp " + GetLabelId(WhileName) + "\r";
	--WhileCount;
	AddLabel(GetScopeName() + "endwhile" + std::to_string(WhileCount));
	Out += GetLabelId(GetScopeName() + "endwhile" + std::to_string(WhileCount)) + LabelTail;
	PopScope();
	return Out;
}

std::string dbc::gen::ASM6502CG::GenStmtAssign(LeafPtr Node)
{
	std::string Out;
	int16 Addr = GetLocal(Node->Left->STRING);
	if (Addr == -1)
	{
		if (Verbose) Out += "; setting global var " + std::string(Node->Left->STRING) + "\r";
		Out += GenCode(Node->Right);
		Addr = GetGlobal(Node->Left->STRING);
		Out += "\rsta $" + NumberToHex<uint16>(Addr);
	}
	else
	{
		Out += GenCode(Node->Right);
		if (Verbose) Out += "; setting local var " + std::string(Node->Left->STRING) + "\r";
		Out += "sta $2001\r";
		if (GotOnScope)
		{
			Out += "lda #$" + NumberToHex<uint8>(0xFF + Addr);
		}
		else
		{
			Out += "lda #$" + NumberToHex<uint8>(4 + Addr);
		}
		Out += "\rclc\r"
			"adc $2000\r"
			"tax\r"
			"lda $2001\r"
			"sta $0100, x";
	}
	return Out + "\r";
}

std::string dbc::gen::ASM6502CG::GenStmtIf(LeafPtr Node)
{
	++IfAccum;
	std::string Out;
	if (Verbose) Out = "; if statement\r";
	// output check expression
	PushScope("ifaccum" + std::to_string(IfAccum));
	AddLabel(GetScopeName() + "ifcheck" + std::to_string(IfCount));
	Out += GetLabelId(GetScopeName() + "ifcheck" + std::to_string(IfCount)) + LabelTail;
	PushScope("ifstmt" + std::to_string(IfCount) + "");
	Out += GenExprCheck(Node->Left);
	PopScope();
	// end of check
	Out += "ldx #$00\r";
	Out += "stx $2001\r";
	Out += "cmp $2001\r";
	++IfCount;
	if (Node->Right->Right == nullptr)
	{
		AddLabel(GetScopeName() + "endif" + std::to_string(IfCount));
		Out += "beq " + GetLabelId(GetScopeName() + "endif" + std::to_string(IfCount)) + "\r";
	}
	else
	{
		if (Node->Right->Right->Type == LeafType::STMT_ELSEIF)
		{
			AddLabel(GetScopeName() + "elseifcheck" + std::to_string(ElseIfCount));
			Out += "beq " + GetLabelId(GetScopeName() + "elseifcheck" + std::to_string(ElseIfCount)) + "\r";
		}
		else if (Node->Right->Right->Type == LeafType::STMT_ELSE)
		{
			AddLabel(GetScopeName() + "elsebody" + std::to_string(IfCount));
			Out += "beq " + GetLabelId(GetScopeName() + "elsebody" + std::to_string(IfCount)) + "\r";
		}
	}
	// output body
	AddLabel(GetScopeName() + "ifbody" + std::to_string(IfCount));
	Out += GetLabelId(GetScopeName() + "ifbody" + std::to_string(IfCount)) + LabelTail;
	EnterScope();
	PushScope("ifstmt" + std::to_string(IfCount) + "");
	if (Verbose) Out += "; if stmt body\r";
	Out += GenCode(Node->Right->Left);
	PopScope();
	ExitScope();
	AddLabel(GetScopeName() + "endif" + std::to_string(IfCount));
	Out += "jmp " + GetLabelId(GetScopeName() + "endif" + std::to_string(IfCount)) + "\r";
	// check for elseif and else
	if (Node->Right->Right != nullptr)
	{
		if (Node->Right->Right->Type == LeafType::STMT_ELSEIF)
		{
			Out += GenStmtElseIf(Node->Right->Right);
		}
		else if (Node->Right->Right->Type == LeafType::STMT_ELSE)
		{
			Out += GenStmtElse(Node->Right->Right);
		}
	}
	AddLabel(GetScopeName() + "endif" + std::to_string(IfCount));
	Out += GetLabelId(GetScopeName() + "endif" + std::to_string(IfCount)) + LabelTail;
	--IfCount;
	PopScope();
	return Out;
}

std::string dbc::gen::ASM6502CG::GenStmtElseIf(LeafPtr Node)
{
	EnterScope();
	std::string Out;
	if (Verbose)  Out = "; elseif statement\r";
	std::string ScopeName = GetScopeName();
	// output check expression
	AddLabel(ScopeName + "elseifcheck" + std::to_string(ElseIfCount));
	Out += GetLabelId(ScopeName + "elseifcheck" + std::to_string(ElseIfCount)) + LabelTail;
	PushScope("elseifstmt" + std::to_string(ElseIfCount) + "");
	Out += GenExprCheck(Node->Left);
	PopScope();
	// end of check
	Out += "ldx #0\r";
	Out += "stx $2001\r";
	Out += "cmp $2001\r";
	++ElseIfCount;
	if (Node->Right->Right == nullptr)
	{
		AddLabel(ScopeName + "endif" + std::to_string(IfCount));
		Out += "beq " + GetLabelId(ScopeName + "endif" + std::to_string(IfCount)) + "\r";
	}
	else
	{
		if (Node->Right->Right->Type == LeafType::STMT_ELSEIF)
		{
			AddLabel(ScopeName + "elseifcheck" + std::to_string(ElseIfCount));
			Out += "beq " + GetLabelId(ScopeName + "elseifcheck" + std::to_string(ElseIfCount)) + "\r";
		}
		else if (Node->Right->Right->Type == LeafType::STMT_ELSE)
		{
			AddLabel(ScopeName + "elsebody" + std::to_string(IfCount));
			Out += "beq " + GetLabelId(ScopeName + "elsebody" + std::to_string(IfCount)) + "\r";
		}
	}
	// output body
	AddLabel(ScopeName + "elseifbody" + std::to_string(ElseIfCount));
	Out += GetLabelId(ScopeName + "elseifbody" + std::to_string(ElseIfCount)) + LabelTail;
	PushScope("elseifstmt" + std::to_string(ElseIfCount) + "");
	if (Verbose) Out += "; elseif stmt body\r";
	Out += GenCode(Node->Right->Left);
	PopScope();
	ExitScope();
	// end of body
	AddLabel(ScopeName + "endif" + std::to_string(ElseIfCount));
	Out += "jmp " + GetLabelId(ScopeName + "endif" + std::to_string(ElseIfCount)) + "\r";
	// check for elseif and else
	if (Node->Right->Right != nullptr)
	{
		if (Node->Right->Right->Type == LeafType::STMT_ELSEIF)
		{
			Out += GenStmtElseIf(Node->Right->Right);
		}
		else if (Node->Right->Right->Type == LeafType::STMT_ELSE)
		{
			Out += GenStmtElse(Node->Right->Right);
		}
	}
	--ElseIfCount;
	return Out;
}

std::string dbc::gen::ASM6502CG::GenStmtElse(LeafPtr Node)
{
	EnterScope();
	std::string Out;
	if (Verbose) Out = "; else statement\r";
	// output body
	AddLabel(GetScopeName() + "elsebody" + std::to_string(IfCount));
	Out += GetLabelId(GetScopeName() + "elsebody" + std::to_string(IfCount)) + LabelTail;
	PushScope("elsestmt" + std::to_string(IfCount) + "");
	if (Verbose) Out += "; else stmt body\r";
	Out += GenCode(Node->Left);
	PopScope();
	ExitScope();
	// endof body
	AddLabel(GetScopeName() + "endif" + std::to_string(IfCount));
	Out += "jmp " + GetLabelId(GetScopeName() + "endif" + std::to_string(IfCount)) + "\r";
	return Out;
}

std::string dbc::gen::ASM6502CG::GenExprCheck(LeafPtr Node)
{
	std::string Out;
	if (IsCompare(Node))
	{
		Out += GenExprCompare(Node);
	}
	else if (IsLogical(Node))
	{
		Out += GenExprLogical(Node);
	}
	else if (IsExprGen(Node))
	{
		Out += GenExpr(Node);
	}
	else if (IsConst(Node))
	{
		Out += GenConst(Node);
	}
	else if (Node->Type == LeafType::EXPR_CALL)
	{
		Out += GenExprCall(Node);
	}
	return Out;
}

void dbc::gen::ASM6502CG::AddGlobal(char * VarName)
{
	if (GetGlobal(VarName, false) == -1)
	{
		GlobalVariables[VarName] = GlobalAddr;
	}
}

void dbc::gen::ASM6502CG::AddLocal(char * VarName, bool Local)
{
	if (CurrentScope != nullptr)
	{
		CurrentScope->Variables.push_back(LocalVar(VarName, static_cast<int16>(CurrentScope->Variables.size() + (Local ? 1 : 0)), Local));
	}
}

int16 dbc::gen::ASM6502CG::GetGlobal(char * VarName, bool YieldError)
{
	for (auto iter : GlobalVariables)
	{
		if (iter.first == VarName)
		{
			return GlobalVariables[VarName];
		}
	}
	if (YieldError)
	{
		if (StaticLeafPtr != nullptr)
		{
			fprintf(stderr, "At Line %lu, ", static_cast<unsigned long int>(StaticLeafPtr->Line));
		}
		fprintf(stderr, "Code Generation Error: Undefined global variable %s\r", VarName);
		PAUSE;
		exit(-1);
	}
	return -1;
}

int16 dbc::gen::ASM6502CG::GetLocal(char * VarName)
{
	if (Scopes.size() > 0)
	{
		GotOnScope = false;
		for (VarScope& CurrentScope : Scopes)
		{
			for (LocalVar& Var : CurrentScope.Variables)
			{
				if (strcmp(Var.VarName, VarName) == 0)
				{
					GotOnScope = Var.Local;
					return Var.Offset;
				}
			}
		}
	}
	return -1;
}

void dbc::gen::ASM6502CG::EnterScope()
{
	Scopes.push_back(VarScope());
	CurrentScope = &Scopes[Scopes.size() - 1];
	CurrentVarOffset += 4;
	++ScopeDeep;
}

void dbc::gen::ASM6502CG::ExitScope()
{
	if (Scopes.size() > 0)
	{
		Scopes.pop_back();
	}
	if (Scopes.size() > 0)
	{
		--ScopeDeep;
		CurrentVarOffset -= 4;
		CurrentScope = &Scopes[Scopes.size() - 1];
	}
	else
	{
		CurrentScope = nullptr;
	}
}
