#pragma once

#include <dasm/Node.h>
#include <dasm/Types.h>
#include <vector>
#include <unordered_map>

namespace dasm
{
class Emitter
{
	using BYTE = uint8;
	enum BinType
	{
		PRG,
		iNES
	};
public:
	void Emit(NodePtr RootNode, BinType type);
	inline std::vector<BYTE> GetObjectCode()
	{
		return Buffer;
	}

protected:
	void EmitLine(NodePtr CurrentNode);
	void EmitInstruction(NodePtr CurrentNode);
	void EmitDirective(NodePtr CurrentNode);
	void EmitDeclVar(NodePtr CurrentNode);
	void EmitLabel(NodePtr CurrentNode);
	void EmitOrigin(NodePtr CurrentNode);
	void EmitByteDirective(NodePtr CurrentNode);
	void PushByte(BYTE value);
	void ResolveLables(NodePtr Root);
	int32 ResolveBytes(NodePtr Op);

private:
	struct AddressData
	{
		uint16 Value;
		bool Force16Bit;
	};
	bool OriginSet = false;
	NodePtr LastNode = nullptr;
	std::vector<BYTE> Buffer;
	std::unordered_map<std::string, AddressData> Constants;
	uint16 CurrentOrigin = 0;
	uint8 CurrentOriginH;
	uint8 CurrentOriginL;
	uint16 OriginOffset = 0;
	uint16 BufferOffsetOrigin = 0;
};
}