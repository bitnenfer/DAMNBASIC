#include <dasm/Emitter.h>

#define EmitterError(Message, ...) \
	if (CurrentNode != nullptr) fprintf(stderr, "Line: %u - Emitter Error: ", CurrentNode->Line);\
	fprintf(stderr, Message, ##__VA_ARGS__); \
	PAUSE; \
	exit(-1);

using namespace dasm;

struct WordData
{
	dasm::uint8 HighByte;
	dasm::uint8 LowByte;
	dasm::uint16 Value;
};

static WordData GetByte(dasm::uint16 Word)
{
	WordData Data{static_cast<dasm::uint8>(
		Word & 255), 
		static_cast<dasm::uint8>(Word / 256), 
		Word};
	return Data;
}

static WordData Hex16ToByte(std::string& Hex)
{
	dasm::uint16 Value = static_cast<dasm::uint16>(strtoul(Hex.c_str(), NULL, 16));
	return GetByte(Value);
}

inline static dasm::uint8 Hex8ToByte(std::string& Hex)
{
	return static_cast<dasm::uint8>(strtoul(Hex.c_str(), NULL, 16));
}

void dasm::Emitter::Emit(NodePtr RootNode)
{
	// Scan constants
	NodePtr Line = RootNode;
	while (Line != nullptr)
	{
		if (Line->Type == ENode::LINE &&
			Line->Left->Type == ENode::DECL_VAR)
		{
			EmitDeclVar(Line->Left);
		}
		Line = Line->Right;
	}
	// Scan labels
	ResolveLables(RootNode);
	// Emit the resto of the code.
	Line = RootNode;
	while (Line != nullptr)
	{
		if (Line->Type == ENode::LINE)
		{
			LastNode = Line->Left;
			EmitLine(Line->Left);
		}
		else
		{
			NodePtr CurrentNode = Line;
			EmitterError("Invalid Line Node");
		}
		Line = Line->Right;
	}
}

void dasm::Emitter::EmitLine(NodePtr CurrentNode)
{
	if (CurrentNode != nullptr)
	{
		if (CurrentNode->Type == ENode::INSTRUCTION)
		{
			EmitInstruction(CurrentNode);
		}
		else if (CurrentNode->Type == ENode::ORIGIN)
		{
			EmitOrigin(CurrentNode);
		}
	}
	else
	{
		EmitterError("Invalid Line");
	}
}

void dasm::Emitter::EmitInstruction(NodePtr CurrentNode)
{
	uint8 OpCode = GetAddress(CurrentNode->OpCode, CurrentNode->AddrMode);
	if (CurrentNode->OpCode == EOpCode::NOOP ||
		OpCode == NOOP)
	{
		EmitterError("Invalid OpCode");
	}
	PushByte(OpCode);
	if (CurrentNode->AddrMode != EAddressMode::IMP &&
		CurrentNode->AddrMode != EAddressMode::A)
	{
		if (CurrentNode->AddrMode == EAddressMode::REL)
		{
			if (CurrentNode->Left->Type == ENode::IDE_ADDRESS)
			{
				if (Constants.find(CurrentNode->Left->STRING) != Constants.end())
				{
					AddressData Value = Constants[CurrentNode->Left->STRING];
					uint8 Val = static_cast<uint8>(Value.Value);
					int8 CheckOutOfBounds = Val - CurrentOrigin - 1;
					if (CheckOutOfBounds < -128 || CheckOutOfBounds > 127)
					{
						EmitterError("Branch %s is out of range by %d bytes", CurrentNode->Left->STRING.c_str(), CheckOutOfBounds);
					}
					PushByte(static_cast<uint8>(Value.Value) - Buffer.size() + 1);
				}
			}
			else
			{
				int32 CheckOutOfBounds = CurrentNode->Left->UINT8 - CurrentOrigin + 1;
				if (CheckOutOfBounds < -128 || CheckOutOfBounds > 127)
				{
					EmitterError("Relative address is out of range by %d bytes", CheckOutOfBounds);
				}
				PushByte(CurrentNode->Left->UINT8 - Buffer.size() - 1);
			}
		}
		else if (CurrentNode->Left->Type != ENode::IDE_VALUE &&
			CurrentNode->Left->Type != ENode::IDE_ADDRESS)
		{
			if (CurrentNode->Left->Is8Bit)
			{
				if (CurrentNode->Left->Type == ENode::DEC_VALUE)
				{
					PushByte(CurrentNode->Left->UINT8);
				}
				else
				{
					PushByte(Hex8ToByte(CurrentNode->Left->STRING));
				}
			}
			else
			{
				WordData Word = Hex16ToByte(CurrentNode->Left->STRING);
				PushByte(Word.HighByte);
				PushByte(Word.LowByte);
			}
		}
		else if(CurrentNode->Left->Type == ENode::IDE_ADDRESS)
		{
			if (Constants.find(CurrentNode->Left->STRING) != Constants.end())
			{
				AddressData Value = Constants[CurrentNode->Left->STRING];
				if (Value.Force16Bit == true)
				{
					WordData Word = GetByte(Value.Value);
					PushByte(CurrentOriginH + Word.HighByte);
					PushByte(CurrentOriginL + Word.LowByte);
				}
				else
				{
					PushByte(static_cast<uint8>(Value.Value));
				}
			}
		}
	}
}

void dasm::Emitter::EmitDirective(NodePtr CurrentNode)
{}

void dasm::Emitter::EmitDeclVar(NodePtr CurrentNode)
{
	AddressData AD{
		static_cast<dasm::uint16>(strtoul(CurrentNode->Right->STRING.c_str(), NULL, 16)),
		CurrentNode->Right->STRING.length() > 2};

	Constants[CurrentNode->Left->STRING] = AD;
}

void dasm::Emitter::EmitLabel(NodePtr CurrentNode)
{
	uint16 Offset = 0;
	uint16 Addr = (Offset - OriginOffset) + CurrentOrigin;
	AddressData AD{Addr, true};
	Constants[CurrentNode->Left->STRING] = AD;
}

void dasm::Emitter::EmitOrigin(NodePtr CurrentNode)
{
	if (CurrentNode->Left->Is8Bit)
	{
		EmitterError("Origin should be in 16 bit size");
	}
	if (!OriginSet)
	{
		OriginSet = true;
		WordData Word = Hex16ToByte(CurrentNode->Left->STRING);
		CurrentOrigin = Word.Value;
		PushByte(Word.HighByte);
		PushByte(Word.LowByte);
		OriginOffset += 2;
	}
	else
	{
		if (LastNode->Type == ENode::ORIGIN)
		{			
			WordData Word = Hex16ToByte(CurrentNode->Left->STRING);
			CurrentOrigin = Word.Value;
			Buffer[Buffer.size() - 2] = Word.HighByte;
			Buffer[Buffer.size() - 1] = Word.LowByte;
		}
		else
		{
			WordData Word = Hex16ToByte(CurrentNode->Left->STRING);
			uint16 NewOrigin = Word.Value;
			if (NewOrigin > CurrentOrigin)
			{
				uint16 Idx;
				for (Idx = CurrentOrigin; Idx < NewOrigin; ++Idx)
				{
					PushByte(0);
					OriginOffset += 1;
				}
			}
			else
			{
				EmitterError("Can't have an origin with lower value than last origin.");
			}
			CurrentOrigin = NewOrigin;
		}		
	}
	CurrentOriginL = (CurrentOrigin / 256);
	CurrentOriginH = (CurrentOrigin & 255);
}

void dasm::Emitter::PushByte(BYTE value)
{
	Buffer.push_back(value);
}

void dasm::Emitter::ResolveLables(NodePtr Root)
{
	NodePtr Current = Root;
	uint32 Address = 0;
	while (Current != nullptr && Current->Type == ENode::LINE)
	{
		NodePtr Left = Current->Left;
		if (Left->Type != ENode::ORIGIN &&
			Left->Type != ENode::DECL_VAR &&
			Left->Type != ENode::DIR_INCLUDE &&
			Left->Type != ENode::DIR_WORD &&
			Left->Type != ENode::DIR_BYTE)
		{
			if (Left->Type == ENode::INSTRUCTION)
			{
				Address += ResolveBytes(Left);
			}
			if (Left->Type == ENode::LABEL)
			{
				AddressData AD{Address, true};
				Constants[Left->Left->STRING] = AD;
			}
			else 
			{
				++Address;
			}
		}
		Current = Current->Right;
	}
}

int32 dasm::Emitter::ResolveBytes(NodePtr Op)
{
	if (Op->AddrMode == EAddressMode::AB ||
		Op->AddrMode == EAddressMode::ABX ||
		Op->AddrMode == EAddressMode::ABY ||
		Op->AddrMode == EAddressMode::IND)
	{
		return 2;
	}
	else if (Op->AddrMode == EAddressMode::IDX ||
			 Op->AddrMode == EAddressMode::IDY ||
			 Op->AddrMode == EAddressMode::ZP ||
			 Op->AddrMode == EAddressMode::ZPX || 
			 Op->AddrMode == EAddressMode::ZPY ||
			 Op->AddrMode == EAddressMode::IMM ||
			 Op->AddrMode == EAddressMode::REL)
	{
		return 1;
	}
	return 0;
}
