#pragma once

#include "BSExtraData.h"

struct DialogueData;
class TESTopic;

// 28
class ExtraSayTopicInfo : public BSExtraData
{
public:
	enum { kExtraTypeID = (UInt32)ExtraDataType::SayTopicInfo };

	ExtraSayTopicInfo();
	virtual ~ExtraSayTopicInfo();

	TESTopic*		topic;					//08
	DialogueData*	dialogueData;			//0C
	bool			unk10;
	float			subtitleSpeechDelay;	//014
	UInt32			unk18;
	SInt32			unk1C;
	UInt8			unk20;
	UInt32			unk24;					//024

	DEFINE_MEMBER_FN(ctor1, ExtraSayTopicInfo*, 0x422C80);
	DEFINE_MEMBER_FN(ctor2, ExtraSayTopicInfo*, 0x422CB0, UInt32);
};
static_assert(sizeof(ExtraSayTopicInfo) == 0x28);
