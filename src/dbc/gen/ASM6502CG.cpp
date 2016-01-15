#include <dbc/gen/ASM6502CG.h>
#include <dbc/Utility.h>

using namespace dbc;

static std::string CurrentOp = "\tlda";
static int16 CurrentReturnOffset = 0;
static bool GotOnScope = false;
static int16 CurrentVarOffset = 0;
static uint16 ScopeDeep = 10;
static uint16 IfDeep = 10;
static std::string ScopeName = "global_";
static std::string CmpLabel0 = "";
static std::string CmpLabel1 = "";
static std::string CurrentFunc = "";
static bool UsingMul = false;
static bool UsingDiv = false;
static bool UsingShl = false;
static bool UsingShr = false;
static bool UsingMod = false;

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
	return "";
}

std::string dbc::gen::ASM6502CG::GenConst(LeafPtr Node)
{
	if (Node->Type == LeafType::CONST_BOOLEAN)
	{
		return GenConstBoolean(Node);
	}
	else if (Node->Type == LeafType::CONST_NUMBER)
	{
		return GenConstNumber(Node);
	}
	else if (Node->Type == LeafType::CONST_IDENTIFIER)
	{
		return GenConstIdentifier(Node);
	}
	return "";
}

std::string dbc::gen::ASM6502CG::GenConstIdentifier(LeafPtr Node)
{
	std::string Out;
	int16 Addr = GetLocal(Node->STRING);
	if (Addr == -1)
	{
		Out += "; getting global var " + std::string(Node->STRING) + "\n";
		Addr = GetGlobal(Node->STRING);
		Out += " $" + NumberToHex<uint16>(Addr);
	}
	else
	{
		Out += "; getting local var " + std::string(Node->STRING) + "\n";
		if (GotOnScope)
		{
			Out += "\tlda #$" + NumberToHex<uint8>(0xFF + Addr);
		}
		else
		{
			Out += "\tlda #$" + NumberToHex<uint8>(4 + Addr);
		}
		Out += "\n\tclc\n"
			"\tadc base_pointer\n"
			"\ttax\n"
			"\tlda stack_tail, x";
	}
	return Out + "\n";
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
	else if (Node->Type == LeafType::EXPR_CALL)
	{
		return GenExprCall(Node);
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
	return "";
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
	return GenCode(Node->Left);
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
	std::string Out;
	Out += GenExpr(Node->Left);
	Out += "\n\tsta tmp\n";
	Out += GenExpr(Node->Right);
	Out += "\tcmp tmp\n\t";
	if (Node->Type == LeafType::EXPR_CMPEQU)
	{
		Out += "beq " + CmpLabel0 + "\n";
		Out += "\tjmp " + CmpLabel1 + "\n";
	}
	else if (Node->Type == LeafType::EXPR_CMPGT)
	{
		Out += "beq " + CmpLabel1 + "\n";
		Out += "\tbcs " + CmpLabel0 + "\n";
	}
	else if (Node->Type == LeafType::EXPR_CMPGTEQU)
	{
		Out += "bcs " + CmpLabel0 + "\n";
		Out += "\tjmp " + CmpLabel1 + "\n";
	}
	else if (Node->Type == LeafType::EXPR_CMPLT)
	{
		Out += "bcc " + CmpLabel0 + "\n";
		Out += "\tjmp " + CmpLabel1 + "\n";
	}
	else if (Node->Type == LeafType::EXPR_CMPLTEQU)
	{
		Out += "bcc " + CmpLabel0 + "\n";
		Out += "\tbeq " + CmpLabel0 + "\n";
		Out += "\tjmp " + CmpLabel1 + "\n";
	}
	else if (Node->Type == LeafType::EXPR_CMPNOTEQU)
	{
		Out += "\tbne " + CmpLabel0 + "\n";
		Out += "\tjmp " + CmpLabel1 + "\n";
	}
	return Out;
}

std::string dbc::gen::ASM6502CG::GenExprLogical(LeafPtr Node)
{
	return std::string();
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
	return "";
}

std::string dbc::gen::ASM6502CG::GenDeclVar(LeafPtr Node)
{
	std::string VarDecl;
	if (InFunction)
	{
		AddLocal(Node->Left->STRING);
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
		VarDecl += "\n\tsta $" + NumberToHex<uint16_t>(GlobalAddr++) + "\n";
	}
	return VarDecl;
}

std::string dbc::gen::ASM6502CG::GenDeclFunc(LeafPtr Node)
{
	InFunction = true;
	ScopeName = "local_" + std::string(Node->Left->Left->STRING);
	CurrentFunc = Node->Left->Left->STRING;
	std::string FuncDecl = "; func " + CurrentFunc + "(";
	LeafPtr VarList = Node->Left->Right;
	CurrentReturnOffset = 4;
	while (VarList != nullptr && VarList->Type == LeafType::DECL_VARLIST)
	{
		if (VarList->Left != nullptr &&
			VarList->Left->Type == LeafType::CONST_IDENTIFIER) 
		{
			AddLocal(VarList->Left->STRING);
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
	/*FuncDecl += "; start prologue\n"
		"\tlda base_pointer\n"
		"\tpha\n"
		"\ttsx\n"
		"\tstx base_pointer\n"
		"; end prologue\n";*/
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
	/*FuncDecl += "; start epilogue\n"
		"\tldx base_pointer\n"
		"\ttxs\n"
		"\tpla\n"
		"\tsta base_pointer\n"
		"\trts\n"
		"; end epilogue\n";*/
	ExitScope();
	InFunction = false;
	ScopeName = "global_";
	return FuncDecl;
}

std::string dbc::gen::ASM6502CG::GenVarList(LeafPtr Node)
{
	return std::string();
}

std::string dbc::gen::ASM6502CG::GenStmtReturn(LeafPtr Node)
{
	std::string Out = GenCode(Node->Left);
	Out += "\n\ttay\n"
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
	std::string Out;
	EnterScope();
	ExitScope();
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
		Out += "; setting local var " + std::string(Node->Left->STRING) + "\n";
		Out += "\tlda #$" + NumberToHex<uint8>(0xFF - Addr);
		Out += "\n\tclc\n"
			"\tadc base_pointer\n"
			"\ttax\n"
			"\tlda stack_tail, x";
	}
	return Out + "\n";
}

std::string dbc::gen::ASM6502CG::GenStmtIf(LeafPtr Node)
{
	std::string Out;
	if (IfDeep == 0)
	{
		Out += "; if statement\n";
	}
	EnterScope();
	CmpLabel0 = ScopeName + std::to_string(ScopeDeep) + "_" + std::to_string(IfDeep);
	++IfDeep;
	CmpLabel1 = ScopeName + std::to_string(ScopeDeep) + "_" + std::to_string(IfDeep);
	if (Node->Right != nullptr)
	{
		if (IsCompare(Node->Left))
		{
			Out += GenExprCompare(Node->Left);
		}
		else if (IsLogical(Node->Left))
		{
			Out += GenExprLogical(Node->Left);
		}
		else if (IsExprGen(Node->Left))
		{
			Out += GenExpr(Node->Left);
			Out += "\n\tcmp #$01\n";
			Out += "\tbeq " + CmpLabel0;
			Out += "\n\tjmp " + CmpLabel1;
			Out += "\n";
		}
		else if (IsConst(Node->Left))
		{
			Out += GenConst(Node->Left);
			Out += "\n\tcmp #$01\n";
			Out += "\tbeq " + CmpLabel0;
			Out += "\n\tjmp " + CmpLabel1;
			Out += "\n";
		}
		else if (Node->Left->Type == LeafType::EXPR_CALL)
		{
			Out += GenExprCall(Node->Left);
			Out += "\n\tcmp #$01\n";
			Out += "\tbeq " + CmpLabel0;
			Out += "\n\tjmp " + CmpLabel1;
			Out += "\n";
		}
		Out += CmpLabel0 + ":\n";
		Out += GenCode(Node->Right->Left);
		Out += CmpLabel1 + ":\n";
	}
	else
	{
		Out += CmpLabel0 + ":\n";
		Out += GenCode(Node->Left);
		Out += CmpLabel1 + ":\n";
	}
	if (Node->Right != nullptr && 
		Node->Right->Right != nullptr)
	{
		if (Node->Right->Right->Type == LeafType::STMT_ELSEIF)
		{
			Out += "; elseif statement\n";
			Out += GenStmtIf(Node->Right->Right);
		}
		else if (Node->Right->Right->Type == LeafType::STMT_ELSE)
		{
			Out += "; else statement\n";
			Out += GenStmtIf(Node->Right->Right);
		}
	}
	ExitScope();
	IfDeep -= 2;
	return Out;
}

void dbc::gen::ASM6502CG::AddGlobal(char * VarName)
{
	if (GetGlobal(VarName) == -1)
	{
		GlobalVariables[VarName] = GlobalAddr;
	}
}

void dbc::gen::ASM6502CG::AddLocal(char * VarName)
{
	if (CurrentScope != nullptr)
	{
		CurrentScope->Variables.push_back(LocalVar(VarName, static_cast<int16>(CurrentScope->Variables.size())));
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
	GotOnScope = false;
	if (Scopes.size() > 0)
	{
		for (VarScope& CurrentScope : Scopes)
		{
			if (&CurrentScope == this->CurrentScope)
			{
				GotOnScope = true;
			}
			else
			{
				GotOnScope = false;
			}
			for (LocalVar& Var : CurrentScope.Variables)
			{
				if (strcmp(Var.VarName, VarName) == 0)
				{
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
