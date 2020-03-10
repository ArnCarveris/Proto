#pragma once

#include "spvgentwo/List.h"
#include "spvgentwo/Constant.h"
#include <stdint.h>
#include "ImNodesEz.h"

//forward decl
namespace spvgentwo
{
	class Instruction;
	class BasicBlock;
} // !spvgentwo

namespace proto
{
	enum class OpNodeType : uint32_t
	{
		InVar = 0, // needs load
		OutVar, // needs store
		Const,

		Equal,
		NotEqual,
		Less,
		LessEqual,
		Greater,
		GreaterEqual,

		Add,
		Sub,
		Mul,
		Div,
		Dot,

		Select,
		//Phi,

		Cast,

		NumOf
	};

	struct OpNodeDesc
	{
		const char* name; 
		uint32_t numInputs;
		uint32_t numOutputs;
	};

	constexpr OpNodeDesc g_OpNodeDesc[uint32_t(OpNodeType::NumOf)] = { 
		{ "InVar", 0u, 1u},
		{ "OutVar", 1u, 0u},
		{ "Const", 0u, 1u},
		{ "Equal", 2u, 1u},
		{ "NotEqual", 2u, 1u},
		{ "Less", 2u, 1u},
		{ "LessEqual", 2u, 1u},
		{ "Greater", 2u, 1u},
		{ "GreaterEqual", 2u, 1u},
		{ "Add", 2u, 1u},
		{ "Sub", 2u, 1u},
		{ "Mul", 2u, 1u},
		{ "Div", 2u, 1u},
		{ "Dot", 2u, 1u},
		{ "Select", 2u, 1u},
		//{ OpNodeType::Phi, ~0u, 1u},
		{ "Cast", 1u, 1u},
	};

	struct VarDesc
	{
		spvgentwo::Type type;
		spv::StorageClass storageClass = spv::StorageClass::UniformConstant;
		const char* name = nullptr;
	};

	struct ConstDesc
	{
		spvgentwo::Constant constant;
		const char* name = nullptr;
	};

	enum class Slot : int
	{
		Unknown = 0,
		InstrArg0,
		InstrArg1,
		InstrArg2
	};

	/// A structure defining a connection between two slots of two nodes.
	struct Connection
	{
		/// `id` that was passed to BeginNode() of input node.
		void* input_node = nullptr;
		/// Descriptor of input slot.
		const char* input_slot = nullptr;
		/// `id` that was passed to BeginNode() of output node.
		void* output_node = nullptr;
		/// Descriptor of output slot.
		const char* output_slot = nullptr;

		bool operator==(const Connection& other) const
		{
			return input_node == other.input_node &&
				input_slot == other.input_slot &&
				output_node == other.output_node &&
				output_slot == other.output_slot;
		}

		bool operator!=(const Connection& other) const
		{
			return !operator ==(other);
		}
	};

	class OpNodeExpr
	{
	public:
		OpNodeExpr(ImVec2 _pos = ImVec2(), spvgentwo::BasicBlock* _pBB = nullptr, OpNodeType mtype = OpNodeType::NumOf);
		~OpNodeExpr();

		void operator()(const spvgentwo::List<OpNodeExpr*>& _inputs, const spvgentwo::List<OpNodeExpr*>& _outputs);		

		OpNodeType getType() const { return m_type; }

		OpNodeDesc getInfo() { return g_OpNodeDesc[uint32_t(m_type)]; }

		void setVarDesc(const VarDesc* _pVarDesc) { m_pVarDesc = _pVarDesc; }
		void setConstDesc(const ConstDesc* _pConstDesc) { m_pConstDesc = _pConstDesc; }


		// editor note:
		void update();

		void addInputSlot(Slot _kind, const char* _pTitle);
		void addOutputSlot(Slot _kind, const char* _pTitle);

		void clear();

		static void connect(const Connection& _con);
		static void disconnect(const Connection& _con);

		bool isSelected() const { return m_selected; }
		ImVec2 getPosition() const { return m_pos; }

	private:
		bool allowedDisconnection(const Connection& _con);
		bool allowedConnection(const Connection& _con);
		spvgentwo::List<Connection>::Iterator remove(const Connection& _con);

	private:
		void makeVar();
		void makeConst();

	private:
		OpNodeType m_type = OpNodeType::NumOf;
		spvgentwo::BasicBlock* m_pBB = nullptr;
		spvgentwo::Instruction* m_pResult = nullptr;
		spvgentwo::Instruction* m_pVar = nullptr;
		const VarDesc* m_pVarDesc = nullptr;
		const ConstDesc* m_pConstDesc = nullptr;

		ImVec2 m_pos{};
		bool m_selected = false;

		spvgentwo::Vector<ImNodes::Ez::SlotInfo> m_inputSlots;
		spvgentwo::Vector<ImNodes::Ez::SlotInfo> m_outputSlots;
		spvgentwo::List<Connection> m_connections;
	};
} // !proto