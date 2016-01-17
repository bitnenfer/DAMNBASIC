#include <dbc/gen/ASM6502CG.h>
#include <dbc/Utility.h>

using namespace dbc;

static constexpr uint16 DefaultCount = 10;
static std::string CurrentOp = "\tlda";
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

static std::vector<std::string> ScopeNameStack;

inline static void PushScope(std::string Name)
{
	ScopeNameStack.push_back(Name);
}

inline static void PopScope()
{
	ScopeNameStack.pop_back();
}

static std::string GetScopeName()
{
	if (ScopeNameStack.size() > 0)
	{
		std::string Out;
		for (std::string& Scope : ScopeNameStack)
		{
			Out += Scope;
		}
		return Out;
	}
	return "global_";
}

template <typename I> static std::string NumberToHex(I w, size_t hex_len = sizeof(I) << 1)
{
	static const char* digits = "0123456789ABCDEF";
	std::string rc(hex_len, '0');
	for (size_t i = 0, j = (hex_len - 1) * 4; i < hex_len; ++i, j -= 4)
		rc[i] = digits[(w >> j) & 0x0f];
	return rc;
}

dbc::gen::ASM6502CG::ASM6502CG()
{
	AddLine("; DAMNBASIC to 6502 assembly code generated.");
	AddLine("\tjmp main");
	AddLine("\tbase_pointer = $1000\n"
			"\tstack_tail = $0100\n"
			"\ttmp = $1001");
	AddLine("main:\n");
	PushScope("global_");
}

void dbc::gen::ASM6502CG::LoadDependencies()
{
	if (UsingMul)
	{
		AddLine("__sys_math_mul:\n    pla \n    sta $1002\n    pla\n    sta $1005\n    pla\n    tax\n    pla \n    tay \n    lda #$00\n    sta $1009\n    stx $1008\n__sys_math_mul0:\n    clc\n    inc $1009\n    dex\n    cpx #$00\n    bne __sys_math_mul0\n    ldx $1008\n    dey\n    cpy #$00\n    bne __sys_math_mul0\n    sec\n    ldy $1009\n    tya\n    pha\n    lda $1005\n    pha\n    lda $1002\n    pha\n    rts");
	}
	if (UsingDiv)
	{
		AddLine("__sys_math_div:\n    pla \n    sta $1002\n    pla\n    sta $1005\n    pla\n    tax\n    pla \n    tay \n    lda #$00\n    sta $1007\n    stx $1008\n    sty $1009\n__sys_math_div0:\n    dex\n    dey\n    cpx #$00\n    bne __sys_math_div0\n    ldx $1008\n    clc\n    inc $1007\n    cpy #$00\n    bne __sys_math_div0\n    cpy $1009\n    beq __sys_math_div0\n    bcc __sys_math_div0\n    ldy $1007\n    tya\n    pha\n    lda $1005\n    pha\n    lda $1002\n    pha\n    rts");
	}
	if (UsingShl)
	{
		AddLine("__sys_bitwise_shl:\n    pla \n    sta $1002\n    pla\n    sta $1003   \n    pla\n    tax\n    pla \n__sys_bitwise_shl0:\n    asl a\n    dex\n    cpx #$00\n    bne __sys_bitwise_shl0\n    pha \n    lda $1003\n    pha\n    lda $1002\n    pha\n    rts");
	}
	if (UsingShr)
	{
		AddLine("__sys_bitwise_shr:\n    pla \n    sta $1002\n    pla\n    sta $1003   \n    pla\n    tax\n    pla \n__sys_bitwise_shr0:\n    lsr a\n    dex\n    cpx #$00\n    bne __sys_bitwise_shr0\n    pha \n    lda $1003\n    pha\n    lda $1002\n    pha\n    rts");
	}
	if (UsingMod)
	{
		AddLine("__sys_math_mod:\n    pla\n    sta $1002\n    pla\n    sta $1003\n    pla\n    sta $1004\n    pla\n    sec\n__sys_math_mod0:\n    sbc $1004\n    bcs __sys_math_mod0\n    adc $1004\n    pha\n    lda $1003\n    pha\n    lda $1002\n    pha\n    rts");
	}
}

void dbc::gen::ASM6502CG::Generate(LeafPtr Node, const char * File, bool IsMain)
{
	if (IsMain)
	{
		printf("Generating 6502 Assembly Code...\n");
	}
	CurrentFile = File;
	LeafPtr Current = Node;

	AddLine("; Global Data");
	ScanningGlobals = true;
	while (Current != nullptr && Current->Type != LeafType::ENDOFFILE)
	{
		if (Current->Left != nullptr && Current->Left->Type == LeafType::DECL_VAR)
		{
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
	AddLine("; Code");
	if (IsMain)
	{
		AddLine("\tjsr func_main");
		AddLine("\tbrk");
	}
	Current = Node;
	EnterScope();
	CurrentVarOffset = 0;
	while (Current != nullptr && Current->Type != LeafType::ENDOFFILE)
	{
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
	AddLine("\tbrk");
	if (IsMain)
	{
		LoadDependencies();
	}
}

std::string dbc::gen::ASM6502CG::GenCode(LeafPtr Node)
{
	if (Node == nullptr)
	{
		return "***INVALID***";
	}
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
	return "***INVALID***";
}

std::string dbc::gen::ASM6502CG::GenConst(LeafPtr Node)
{
	if (Node->Type == LeafType::CONST_BOOLEAN)
	{
		return GenConstBoolean(Node) + "\n";
	}
	else if (Node->Type == LeafType::CONST_NUMBER)
	{
		return GenConstNumber(Node) + "\n";
	}
	else if (Node->Type == LeafType::CONST_IDENTIFIER)
	{
		return GenConstIdentifier(Node) + "\n";
	}
	return "***INVALID***";
}

std::string dbc::gen::ASM6502CG::GenConstIdentifier(LeafPtr Node)
{
	std::string Out;
	int16 Addr = GetLocal(Node->STRING);
	if (Addr == -1)
	{
		Out += "; getting global var " + std::string(Node->STRING) + "\n";
		Addr = GetGlobal(Node->STRING);
		Out += "\tlda $" + NumberToHex<uint16>(Addr);
	}
	else
	{
		if (GotOnScope)
		{
			Out += "; getting local var " + std::string(Node->STRING) + "\n";
			Out += "\tlda #$" + NumberToHex<uint8>(0xFF + Addr);
		}
		else
		{
			Out += "; getting arg var " + std::string(Node->STRING) + "\n";
			Out += "\tlda #$" + NumberToHex<uint8>(4 + Addr);
		}
		Out += "\n\tclc\n"
			"\tadc base_pointer\n"
			"\ttax\n"
			"\tlda stack_tail, x";
	}
	return Out;
}

std::string dbc::gen::ASM6502CG::GenConstBoolean(LeafPtr Node)
{
	return CurrentOp + " #$" + NumberToHex<uint8>(Node->UINT8);
}

std::string dbc::gen::ASM6502CG::GenConstNumber(LeafPtr Node)
{
	return CurrentOp + " #$" + NumberToHex<uint8>(Node->UINT8);
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
		std::string Out = "; modulus\n" + GenCode(Node->Left);
		Out += "\n\tpha\n";
		Out += GenCode(Node->Right);
		Out += "\n\tpha\n";
		Out += "\tjsr __sys_math_mod\n";
		Out += "\tpla\n";
		return Out;
	}
	return "***INVALID***";
}

std::string dbc::gen::ASM6502CG::GenExprNot(LeafPtr Node)
{
	std::string Out = "; not expression\n";
	UsingNot = true;
	Out += GenExpr(Node->Left);
	UsingNot = false;
	return Out;
}

std::string dbc::gen::ASM6502CG::GenExprCall(LeafPtr Node)
{
	std::string Out = ";alloc return\n\tlda #$0\n\tpha\n";
	LeafPtr ExprList = Node->Right;
	while (ExprList != nullptr && ExprList->Type == LeafType::EXPR_LIST)
	{
		if (ExprList->Left != nullptr)
		{
			if (IsExprGen(ExprList->Left))
			{
				Out += GenCode(ExprList->Left);
				Out += "\n\tpha\n";
			}
			else if (IsConst(ExprList->Left))
			{
				Out += GenConst(ExprList->Left);
				Out += "\n\tpha\n";
			}
			else if (ExprList->Left->Type == LeafType::EXPR_CALL)
			{
				Out += GenExprCall(ExprList->Left);
				Out += "\n\tpha\n";
			}
		}
		ExprList = ExprList->Right;
	}
	Out += "\tjsr func_" + std::string(Node->Left->STRING);
	ExprList = Node->Right;
	Out += "\n; clean up\n";
	while (ExprList != nullptr && ExprList->Type == LeafType::EXPR_LIST)
	{
		if (ExprList->Left != nullptr)
		{
			if (IsExprGen(ExprList->Left))
			{
				Out += "\tpla\n";
			}
			else if (IsConst(ExprList->Left))
			{
				Out += "\tpla\n";
			}
			else if (ExprList->Left->Type == LeafType::EXPR_CALL)
			{
				Out += "\tpla\n";
			}
		}
		ExprList = ExprList->Right;
	}
	Out += "\tpla\n";
	return Out;
}

std::string dbc::gen::ASM6502CG::GenExprNegate(LeafPtr Node)
{
	std::string Out;
	Out += GenExpr(Node->Left);
	Out += "\teor #$FF\n"
		"\tsec\n"
		"\tadc #0\n";
	return Out;
}

std::string dbc::gen::ASM6502CG::GenExprMul(LeafPtr Node)
{
	UsingMul = true;
	std::string Out = "; multiplication\n" + GenCode(Node->Left);
	Out += "\n\tpha\n";
	Out += GenCode(Node->Right);
	Out += "\n\tpha\n\tclc\n";
	Out += "\tjsr __sys_math_mul\n";
	Out += "\tpla\n";
	return Out;
}

std::string dbc::gen::ASM6502CG::GenExprDiv(LeafPtr Node)
{
	UsingDiv = true;
	std::string Out = "; divition\n" + GenCode(Node->Left);
	Out += "\n\tpha\n";
	Out += GenCode(Node->Right);
	Out += "\n\tpha\n\tsec\n";
	Out += "\tjsr __sys_math_div\n";
	Out += "\tpla\n";
	return Out;
}

std::string dbc::gen::ASM6502CG::GenExprAdd(LeafPtr Node)
{
	std::string Out = "; addition\n" + GenCode(Node->Left);
	Out += "\tpha\n";
	Out += GenCode(Node->Right);
	Out += "\tsta tmp\n";
	Out += "\tpla\n";
	Out += "\tclc\n";
	Out += "\tadc tmp\n";
	return Out;
}

std::string dbc::gen::ASM6502CG::GenExprSub(LeafPtr Node)
{
	std::string Out = "; subraction\n" + GenCode(Node->Left);
	Out += "\tpha\n";
	Out += GenCode(Node->Right);
	Out += "\tsta tmp\n";
	Out += "\tpla\n";
	Out += "\tsec\n";
	Out += "\tsbc tmp\n";
	return Out;
}

std::string dbc::gen::ASM6502CG::GenExprCompare(LeafPtr Node)
{
	PushScope("cmpexpr_" + std::to_string(CmpExprCount) + "_");
	++CmpExprCount;
	std::string Out;
	Out += GenExpr(Node->Right);
	Out += "\tsta tmp\n";
	Out += GenExpr(Node->Left);
	Out += "\tclc\n";
	Out += "\tcmp tmp\n";
	--CmpExprCount;
	std::string TrueBranch = GetScopeName() + "true_" + std::to_string(CmpExprCount);
	std::string FalseBranch = GetScopeName() + "false_" + std::to_string(CmpExprCount);
	if (Node->Type == LeafType::EXPR_CMPEQU)
	{
		Out += "\tbeq " + TrueBranch + "\n";
		Out += "\tjmp " + FalseBranch + "\n";
	}
	else if (Node->Type == LeafType::EXPR_CMPNOTEQU)
	{
		Out += "\tbne " + TrueBranch + "\n";
		Out += "\tjmp " + FalseBranch + "\n";
	}
	else if (Node->Type == LeafType::EXPR_CMPGT)
	{
		Out += "\tbeq " + FalseBranch + "\n";
		Out += "\tbcs " + TrueBranch + "\n";
	}
	else if (Node->Type == LeafType::EXPR_CMPLT)
	{
		Out += "\tbcc " + TrueBranch + "\n";
		Out += "\tjmp " + FalseBranch + "\n";
	}
	else if (Node->Type == LeafType::EXPR_CMPGTEQU)
	{
		Out += "\tbcs " + TrueBranch + "\n";
		Out += "\tjmp " + FalseBranch + "\n";
	}
	else if (Node->Type == LeafType::EXPR_CMPLTEQU)
	{
		Out += "\tbcc " + TrueBranch + "\n";
		Out += "\tbeq " + TrueBranch + "\n";
		Out += "\tjmp " + FalseBranch + "\n";
	}
	Out += FalseBranch + ":\n";
	Out += "\tlda #$00\n";
	Out += "\tjmp " + GetScopeName() + "endcmp_" + std::to_string(CmpExprCount) + "\n";
	Out += TrueBranch + ":\n";
	Out += "\tlda #$01\n";
	Out += GetScopeName() + "endcmp_" + std::to_string(CmpExprCount) + ":\n";
	PopScope();
	return Out;
}

std::string dbc::gen::ASM6502CG::GenExprLogical(LeafPtr Node)
{
	std::string Out;
	if (Node->Type == LeafType::EXPR_AND)
	{
		PushScope("andexpr_" + std::to_string(AndExprCount) + "_");
		std::string ScopeName = GetScopeName();
		std::string AndEndName = ScopeName + "endand_" + std::to_string(AndExprCount);
		std::string AndBranchTrue = ScopeName + "endand_true_" + std::to_string(AndExprCount);
		std::string AndBranchFalse = ScopeName + "endand_false_" + std::to_string(AndExprCount);
		++AndExprCount;
		Out += "; and expression\n";
		// check lhs or expr is valid
		PushScope("lhs_");
		Out += GenExpr(Node->Left);
		PopScope();
		Out += ";check lhs and expr is valid\n";
		Out += "\tldx #$00\n";
		Out += "\tstx tmp\n";
		Out += "\tclc\n";
		Out += "\tcmp tmp\n";
		Out += "\tbeq " + AndBranchFalse + "\n";
		// check rhs or expr is valid
		PushScope("rhs_");
		Out += GenExpr(Node->Right);
		PopScope();
		Out += ";check rhs and expr is valid\n";
		Out += "\tldx #$00\n";
		Out += "\tstx tmp\n";
		Out += "\tclc\n";
		Out += "\tcmp tmp\n";
		Out += "\tbeq " + AndBranchFalse + "\n";
		Out += "\tjmp " + AndBranchTrue + "\n";
		--AndExprCount;
		Out += AndBranchFalse + ":\n";
		Out += "\tlda #$00\n";
		Out += "\tjmp " + AndEndName + "\n";
		Out += AndBranchTrue + ":\n";
		Out += "\tlda #$01\n";
		PopScope();
		Out += AndEndName + ":\n";
	}
	else if (Node->Type == LeafType::EXPR_OR)
	{
		PushScope("orexpr_" + std::to_string(OrExprCount) + "_");
		std::string ScopeName = GetScopeName();
		std::string OrEndName = ScopeName + "endor_" + std::to_string(OrExprCount);
		std::string OrBranchTrue = ScopeName + "endor_true_" + std::to_string(OrExprCount);
		std::string OrBranchFalse = ScopeName + "endor_false_" + std::to_string(OrExprCount);
		++OrExprCount;
		Out += "; or expression\n";
		// check lhs or expr is valid
		PushScope("lhs_");
		Out += GenExpr(Node->Left);
		PopScope();
		Out += ";check lhs or expr is valid\n";
		Out += "\tldx #$00\n";
		Out += "\tstx tmp\n";
		Out += "\tclc\n";
		Out += "\tcmp tmp\n";
		Out += "\tbne " + OrBranchTrue + "\n";
		// check rhs or expr is valid
		PushScope("rhs_");
		Out += GenExpr(Node->Right);
		PopScope();
		Out += ";check rhs or expr is valid\n";
		Out += "\tldx #$00\n";
		Out += "\tstx tmp\n";
		Out += "\tclc\n";
		Out += "\tcmp tmp\n";
		Out += "\tbne " + OrBranchTrue + "\n";
		Out += "\tjmp " + OrBranchFalse + "\n";
		--OrExprCount;
		Out += OrBranchFalse + ":\n";
		Out += "\tlda #$00\n";
		Out += "\tjmp " + OrEndName + "\n";
		Out += OrBranchTrue + ":\n";
		Out += "\tlda #$01\n";
		PopScope();
		Out += OrEndName + ":\n";
	}	
	return Out;
}

std::string dbc::gen::ASM6502CG::GenExprBitwise(LeafPtr Node)
{
	std::string Out;
	if (Node->Type == LeafType::EXPR_SHL)
	{
		Out += "; shift left\n";
		UsingShl = true;
		Out += GenCode(Node->Left);
		Out += "\n\tpha\n";
		Out += GenCode(Node->Right);
		Out += "\n\tpha\n\tclc\n";
		Out += "\tjsr __sys_bitwise_shl\n";
		Out += "\tpla\n";
		return Out;
	}
	else if (Node->Type == LeafType::EXPR_SHR)
	{
		Out += "; shift right\n";
		UsingShr = true;
		Out += GenCode(Node->Left);
		Out += "\n\tpha\n";
		Out += GenCode(Node->Right);
		Out += "\n\tpha\n\tclc\n";
		Out += "\tjsr __sys_bitwise_shr\n";
		Out += "\tpla\n";
		return Out;
	}
	else if (Node->Type == LeafType::EXPR_BAND)
	{
		Out += "; bitwise and\n";
		Out += GenCode(Node->Right);
		Out += "\n\tsta tmp\n";
		Out += GenCode(Node->Left);
		Out += "\n\tand tmp\n";
		return Out;
	}
	else if (Node->Type == LeafType::EXPR_BOR)
	{
		Out += "; bitwise or\n";
		Out += GenCode(Node->Right);
		Out += "\n\tsta tmp\n";
		Out += GenCode(Node->Left);
		Out += "\n\tora tmp\n";
		return Out;
	}
	else if (Node->Type == LeafType::EXPR_XOR)
	{
		Out += "; bitwise exclusive or\n";
		Out += GenCode(Node->Right);
		Out += "\n\tsta tmp\n";
		Out += GenCode(Node->Left);
		Out += "\n\teor tmp\n";
		return Out;
	}
	return Out;
}

std::string dbc::gen::ASM6502CG::GenExprList(LeafPtr Node)
{
	return "***INVALID***";
}

std::string dbc::gen::ASM6502CG::GenDeclVar(LeafPtr Node)
{
	std::string VarDecl;
	if (InFunction)
	{
		AddLocal(Node->Left->STRING, true);
		VarDecl += "; local var " + std::string(Node->Left->STRING) + "\n";
		if (IsConst(Node->Right))
		{
			VarDecl += GenConst(Node->Right);
		}
		else if (IsExprGen(Node->Right))
		{
			VarDecl += GenExpr(Node->Right);
		}
		VarDecl += "; set value local var " + std::string(Node->Left->STRING) + "\n";
		VarDecl += "\tpha\n";
	}
	else if (ScanningGlobals)
	{
		AddGlobal(Node->Left->STRING);
		VarDecl += "; global var " + std::string(Node->Left->STRING) + "\n";
		VarDecl += GenCode(Node->Right);
		VarDecl += "\tsta $" + NumberToHex<uint16_t>(GlobalAddr++) + "\n";
	}
	return VarDecl;
}

std::string dbc::gen::ASM6502CG::GenDeclFunc(LeafPtr Node)
{
	InFunction = true;
	PushScope("local_" + std::string(Node->Left->Left->STRING) + "_");
	CurrentFunc = Node->Left->Left->STRING;
	std::string FuncDecl = "; func " + CurrentFunc + "(";
	LeafPtr VarList = Node->Left->Right;
	CurrentReturnOffset = 4;
	while (VarList != nullptr && VarList->Type == LeafType::DECL_VARLIST)
	{
		if (VarList->Left != nullptr &&
			VarList->Left->Type == LeafType::CONST_IDENTIFIER)
		{
			AddLocal(VarList->Left->STRING, false);
			FuncDecl += VarList->Left->STRING;
			++CurrentReturnOffset;
		}
		VarList = VarList->Right;
		if (VarList != nullptr &&
			VarList->Left != nullptr)
		{
			FuncDecl += ", ";
		}
	}
	FuncDecl += ")\n";
	FuncDecl += "func_" + CurrentFunc + ":\n";
	EnterScope();
	// Prologue
	FuncDecl += ".byte $AD, $00, $10, $48, $BA, $8E, $00, $10\n";
	FuncDecl += "; function body start\n";
	if (Node->Right != nullptr)
	{
		FuncDecl += GenCode(Node->Right);
	}
	// Epilogue
	FuncDecl += "\n; function body end\n";
	FuncDecl += "func_end_" + CurrentFunc + ":\n";
	FuncDecl += ".byte $AE, $00, $10, $9A, $68, $8D, $00, $10, $60\n";
	ExitScope();
	InFunction = false;
	PopScope();
	return FuncDecl;
}

std::string dbc::gen::ASM6502CG::GenVarList(LeafPtr Node)
{
	return std::string();
}

std::string dbc::gen::ASM6502CG::GenStmtReturn(LeafPtr Node)
{
	std::string Out = GenCode(Node->Left);
	Out += "\ttay\n"
		"; store it at return space.\n";

	Out += "\tlda #" + NumberToHex<uint8>(static_cast<uint8>(CurrentReturnOffset));
	Out += "\n\tclc\n"
		"\tadc base_pointer\n"
		"\ttax\n"
		"\ttya\n"
		"\tsta stack_tail, x\n"
		"\tjmp func_end_" + CurrentFunc + "\n";
	return Out;
}

std::string dbc::gen::ASM6502CG::GenStmtWhile(LeafPtr Node)
{
	std::string Out = "; while statement\n";
	std::string ScopeName = GetScopeName();
	// output check expression
	std::string WhileName = ScopeName + "while_check" + std::to_string(WhileCount);
	Out += WhileName + ":\n";
	PushScope("whilestmt_" + std::to_string(WhileCount) + "_");
	Out += GenExprCheck(Node->Left);
	PopScope();
	// end of check
	Out += "\tldx #$00\n";
	Out += "\tstx tmp\n";
	Out += "\tcmp tmp\n";
	Out += "\tbeq " + ScopeName + "end_while" + std::to_string(WhileCount) + "\n";
	++WhileCount;
	// output body
	Out += ScopeName + "while_body" + std::to_string(WhileCount) + ":\n";
	EnterScope();
	PushScope("whilestmt_" + std::to_string(IfCount) + "_");
	Out += "; while stmt body\n";
	Out += GenCode(Node->Right);
	PopScope();
	ExitScope();
	Out += "\tjmp " + WhileName + "\n";
	ExitScope();
	--WhileCount;
	Out += ScopeName + "end_while" + std::to_string(WhileCount) + ":\n";
	return Out;
}

std::string dbc::gen::ASM6502CG::GenStmtAssign(LeafPtr Node)
{
	std::string Out;
	int16 Addr = GetLocal(Node->Left->STRING);
	if (Addr == -1)
	{
		Out += "; setting global var " + std::string(Node->Left->STRING) + "\n";
		Out += GenCode(Node->Right);
		Addr = GetGlobal(Node->Left->STRING);
		Out += "\n\tsta $" + NumberToHex<uint16>(Addr);
	}
	else
	{
		Out += GenCode(Node->Right);
		Out += "; setting local var " + std::string(Node->Left->STRING) + "\n";
		Out += "\tsta tmp\n";
		if (GotOnScope)
		{
			Out += "\tlda #$" + NumberToHex<uint8>(0xFF - Addr);
		}
		else
		{
			Out += "\tlda #$" + NumberToHex<uint8>(4 + Addr);
		}
		Out += "\n\tclc\n"
			"\tadc base_pointer\n"
			"\ttax\n"
			"\tlda tmp\n"
			"\tsta stack_tail, x";
	}
	return Out + "\n";
}

std::string dbc::gen::ASM6502CG::GenStmtIf(LeafPtr Node)
{
	std::string Out = "; if statement\n";
	std::string ScopeName = GetScopeName();
	// output check expression
	Out += GetScopeName() + "if_check" + std::to_string(IfCount) + ":\n";
	PushScope("ifstmt_" + std::to_string(IfCount) + "_");
	Out += GenExprCheck(Node->Left);
	PopScope();
	// end of check
	Out += "\tldx #$00\n";
	Out += "\tstx tmp\n";
	Out += "\tcmp tmp\n";
	++IfCount;
	if (Node->Right->Right == nullptr)
	{
		Out += "\tbeq " + ScopeName + "end_if" + std::to_string(IfCount) + "\n";
	}
	else
	{
		if (Node->Right->Right->Type == LeafType::STMT_ELSEIF)
		{
			Out += "\tbeq " + ScopeName + "elseif_check" + std::to_string(ElseIfCount) + "\n";
		}
		else if(Node->Right->Right->Type == LeafType::STMT_ELSE)
		{
			Out += "\tbeq " + ScopeName + "else_body" + std::to_string(IfCount) + "\n";
		}
	}
	// output body
	Out += ScopeName + "if_body" + std::to_string(IfCount) + ":\n";
	EnterScope();
	PushScope("ifstmt_" + std::to_string(IfCount) + "_");
	Out += "; if stmt body\n";
	Out += GenCode(Node->Right->Left);
	PopScope();
	ExitScope();
	Out += "\tjmp " + ScopeName + "end_if" + std::to_string(IfCount) + "\n";
	// check for elseif and else
	ExitScope();
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
	Out += ScopeName + "end_if" + std::to_string(IfCount) + ":\n";
	--IfCount;
	return Out;
}

std::string dbc::gen::ASM6502CG::GenStmtElseIf(LeafPtr Node)
{
	EnterScope();
	std::string Out = "; elseif statement\n";
	std::string ScopeName = GetScopeName();
	// output check expression
	Out += ScopeName + "elseif_check" + std::to_string(ElseIfCount) + ":\n";
	PushScope("elseifstmt_" + std::to_string(ElseIfCount) + "_");
	Out += GenExprCheck(Node->Left);
	PopScope();
	// end of check
	Out += "\tldx #0\n";
	Out += "\tstx tmp\n";
	Out += "\tcmp tmp\n";
	++ElseIfCount;
	if (Node->Right->Right == nullptr)
	{
		Out += "\tbeq " + ScopeName + "end_if" + std::to_string(IfCount) + "\n";
	}
	else
	{
		if (Node->Right->Right->Type == LeafType::STMT_ELSEIF)
		{
			Out += "\tbeq " + ScopeName + "elseif_check" + std::to_string(ElseIfCount) + "\n";
		}
		else if (Node->Right->Right->Type == LeafType::STMT_ELSE)
		{
			Out += "\tbeq " + ScopeName + "else_body" + std::to_string(IfCount) + "\n";
		}
	}
	// output body
	Out += ScopeName + "elseif_body" + std::to_string(ElseIfCount) + ":\n";
	PushScope("elseifstmt_" + std::to_string(ElseIfCount) + "_");
	Out += "; elseif stmt body\n";
	Out += GenCode(Node->Right->Left);
	PopScope();
	ExitScope();
	// end of body
	Out += "\tjmp " + ScopeName + "end_if" + std::to_string(ElseIfCount) + "\n";
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
	std::string Out = "; else statement\n";
	// output body
	Out += GetScopeName() + "else_body" + std::to_string(IfCount) + ":\n";
	PushScope("elsestmt_" + std::to_string(IfCount) + "_");
	Out += "; else stmt body\n";
	Out += GenCode(Node->Left);
	PopScope();
	ExitScope();
	// endof body
	Out += "\tjmp " + GetScopeName() + "end_if" + std::to_string(IfCount) + "\n";
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
	if (GetGlobal(VarName) == -1)
	{
		GlobalVariables[VarName] = GlobalAddr;
	}
}

void dbc::gen::ASM6502CG::AddLocal(char * VarName, bool Local)
{
	if (CurrentScope != nullptr)
	{
		CurrentScope->Variables.push_back(LocalVar(VarName, static_cast<int16>(CurrentScope->Variables.size()), Local));
	}
}

int16 dbc::gen::ASM6502CG::GetGlobal(char * VarName)
{
	for (auto iter : GlobalVariables)
	{
		if (iter.first == VarName)
		{
			return GlobalVariables[VarName];
		}
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
