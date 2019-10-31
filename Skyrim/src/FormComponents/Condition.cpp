#include "Skyrim/FormComponents/Condition.h"

Condition::ComparisonFlags::ComparisonFlags() :
	isOR(false),
	usesAliases(false),
	global(false),
	usePackData(false),
	swapTarget(false),
	opCode(Opcode::kOpcode_Equal)
{}

Condition::Node::Node() :
	next(nullptr),
	comparisonValue(0.0),
	handle(0),
	unk0C(0),
	functionID(static_cast<FunctionID>(-1)),
	unk12(0),
	unk13(0),
	param1(nullptr),
	param2(nullptr),
	comparisonType(),
	referenceType(ReferenceTypes::kReferenceType_Subject)
{}

Condition::Condition() :
	head(nullptr)
{}

Condition::~Condition()
{
	auto cur = head;
	while (cur) {
		auto next = cur->next;
		delete cur;
		cur = next;
	}
	head = nullptr;
}