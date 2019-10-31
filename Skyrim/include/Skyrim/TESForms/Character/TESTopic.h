#pragma once

#include "../../FormComponents/TESForm.h"
#include "../../FormComponents/TESFullName.h"
#include "../../BSCore/BSTList.h"
#include "TESTopicInfo.h"

class Actor;
class TESQuest;
class TESTopic;
class TESTopicInfo;
class BGSSoundDescriptorForm;

// 1C
struct DialogueData
{
	// 24
	struct ResponseData
	{
		// @members
		BSString				responseText;		// 00
		UInt32 					emotionType;		// 08
		UInt32 					emotionValue;		// 0C
		BSFixedString			voiceFileName;		// 10
		TESIdleForm*			idleSpeaker;		// 14
		TESIdleForm*			idleListener;		// 18
		BGSSoundDescriptorForm* sound;				// 1C
		UInt8					unk20;				// 20
		UInt8					unk21;				// 21

		DEFINE_MEMBER_FN(ctor, ResponseData*, 0x006731A0, TESTopic* topic, TESTopicInfo* topicInfo, Actor* speaker, TESTopicInfo::ResponseData* rawData);
	};
	STATIC_ASSERT(sizeof(ResponseData) == 0x24);


	// @members
	BSSimpleList<ResponseData*>			dataList;		// 00
	BSSimpleList<ResponseData*>::iterator	current;		// 08
	TESTopicInfo* topicInfo;	// 0C
	TESTopic* topic;		// 10
	TESQuest* quest;		// 14
	Actor* speaker;		// 18

private:
	DEFINE_MEMBER_FN(ctor, DialogueData*, 0x00672BB0, TESQuest* owner, TESTopic* topic, TESTopicInfo* topicInfo, Actor* speaker);
	DEFINE_MEMBER_FN_const(GetCurrentDialogueText, const char*, 0x006FCB60);	// { return (current) ? current->text.c_str() : nullptr; }
};
STATIC_ASSERT(sizeof(DialogueData) == 0x1C);


/*==============================================================================
class TESTopic +0000 (_vtbl=010A6B1C)
0000: class TESTopic
0000: |   class TESForm
0000: |   |   class BaseFormComponent
0014: |   class TESFullName
0014: |   |   class BaseFormComponent
==============================================================================*/
// 3C
class TESTopic : public TESForm,
	public TESFullName				// 14
{
public:
	enum { kTypeID = (UInt32)FormType::Topic };

	struct Data	// DATA
	{
		enum class TopicFlag : UInt8
		{
			kNone = 0,
			kDoAllBeforeRepeating = 1 << 0
		};


		enum class Category : UInt8
		{
			kTopic = 0,
			kFavor = 1,
			kScene = 2,
			kCombat = 3,
			kFavors = 4,
			kDetection = 5,
			kService = 6,
			kMiscellaneous = 7
		};


		enum class Subtype : UInt8
		{
			kCustom = 0,
			kForceGreet = 1,
			kRumors = 2,
			kUnk3 = 3,	// custom?
			kIntimidate = 4,
			kFlatter = 5,
			kBribe = 6,
			kAskGift = 7,
			kGift = 8,
			kAskFavor = 9,
			kFavor = 10,
			kShowRelationships = 11,
			kFollow = 12,
			kReject = 13,
			kScene = 14,
			kShow = 15,
			kAgree = 16,
			kRefuse = 17,
			kExitFavorState = 18,
			kMoralRefusal = 19,
			kFlyingMountLand = 20,
			kFlyingMountCancelLand = 21,
			kFlyingMountAcceptTarget = 22,
			kFlyingMountRejectTarget = 23,
			kFlyingMountNoTarget = 24,
			kFlyingMountDestinationReached = 25,
			kAttack = 26,
			kPowerAttack = 27,
			kBash = 28,
			kHit = 29,
			kFlee = 30,
			kBleedout = 31,
			kAvoidThreat = 32,
			kDeath = 33,
			kGroupStrategy = 34,
			kBlock = 35,
			kTaunt = 36,
			kAllyKilled = 37,
			kSteal = 38,
			kYield = 39,
			kAcceptYield = 40,
			kPickpocketCombat = 41,
			kAssault = 42,
			kMurder = 43,
			kAssaultNPC = 44,
			kMurderNPC = 45,
			kPickpocketNPC = 46,
			kStealFromNPC = 47,
			kTrespassAgainstNPC = 48,
			kTrespass = 49,
			kWereTransformCrime = 50,
			kVoicePowerStartShort = 51,
			kVoicePowerStartLong = 52,
			kVoicePowerEndShort = 53,
			kVoicePowerEndLong = 54,
			kAlertIdle = 55,
			kLostIdle = 56,
			kNormalToAlert = 57,
			kAlertToCombat = 58,
			kNormalToCombat = 59,
			kAlertToNormal = 60,
			kCombatToNormal = 61,
			kCombatToLost = 62,
			kLostToNormal = 63,
			kLostToCombat = 64,
			kDetectFriendDie = 65,
			kServiceRefusal = 66,
			kRepair = 67,
			kTravel = 68,
			kTraining = 69,
			kBarterExit = 70,
			kRepairExit = 71,
			kRecharge = 72,
			kRechargeExit = 73,
			kTrainingExit = 74,
			kObserveCombat = 75,
			kNoticeCorpse = 76,
			kTimeToGo = 77,
			kGoodBye = 78,
			kHello = 79,
			kSwingMeleeWeapon = 80,
			kShootBow = 81,
			kZKeyObject = 82,
			kJump = 83,
			kKnockOverObject = 84,
			kDestroyObject = 85,
			kStandonFurniture = 86,
			kLockedObject = 87,
			kPickpocketTopic = 88,
			kPursueIdleTopic = 89,
			kSharedInfo = 90,
			kPlayerCastProjectileSpell = 91,
			kPlayerCastSelfSpell = 92,
			kPlayerShout = 93,
			kIdle = 94,
			kEnterSprintBreath = 95,
			kEnterBowZoomBreath = 96,
			kExitBowZoomBreath = 97,
			kActorCollidewithActor = 98,
			kPlayerinIronSights = 99,
			kOutofBreath = 100,
			kCombatGrunt = 101,
			kLeaveWaterBreath = 102
		};


		TopicFlag	topicFlags;	// 0
		Category	category;	// 1
		Subtype		subtype;	// 2
		UInt8		unk4;		// 3
	};
	STATIC_ASSERT(sizeof(Data) == 0x4);

	//explicit TESTopic(UInt32 arg1);						// 0057BE4F

	virtual ~TESTopic();									// 0057BEB0

	// @overide
	virtual const char* GetName(void);					// 0057BBE0 { return unk38 ? unk38 : ""; }
	virtual bool			SetName(const char* str);		// 0057BBA0 { bool r = (unk38 == str); unk38 = str; return r; }
	virtual bool			Unk_34(void) override;			// 009B86F0 { return true; }
	virtual bool			Unk_36(FormType arg) override;	// 0057BD60 { return arg1 == 0x4C; }


	DEFINE_MEMBER_FN(CreateDialogueData, DialogueData*, 0x0057C980, Actor* speaker, Actor* listener, TESTopicInfo* topicInfo, UInt32 arg4, UInt32 arg5);


	// @members
	//void				** _vtbl;	// 00 - 010A6B1C
	Data				data;		// 1C - init'd 0
	UInt32				unk20;		// 20 - init'd 0x32000000
	BGSDialogueBranch*	branch;		// 24 - init'd 0
	TESQuest*			questOwner;	// 28 - init'd 0
	TESTopicInfo**		infoTopics;	// 2C - init'd 0
	UInt32				infoCount;	// 30- - buf[unk30]
	UInt32				fileOffset;	// 34 - len
	BSFixedString		editorID;	// 38 - init'd 0
};
static_assert(sizeof(TESTopic) == 0x3C,"not valid");
