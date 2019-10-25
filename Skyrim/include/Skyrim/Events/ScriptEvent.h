#pragma once

#include "../BSCore/BSTEvent.h"
#include "../BSSystem/BSTSmartPointer.h"

class TESForm;
class ActiveEffect;
class BGSLocation;
class TESObjectCELL;
class TESObjectREFR;
class Actor;

struct BGSEventProcessedEvent
{
};

// 8
struct TESActivateEvent
{
	TESObjectREFR	* target;		// 00 - TESObjectREFRPtr
	TESObjectREFR	* caster;		// 04 - TESObjectREFRPtr
};

struct TESActiveEffectApplyRemoveEvent
{
	TESObjectREFR*	            caster;			// 00
	TESObjectREFR*	            target;			// 08
	UInt16						effectHandle;	// 10
	bool						apply;			// 12
	UInt8						pad13;			// 13
	UInt32						pad14;			// 14
};

// 0C
struct TESActorLocationChangeEvent
{
	Actor * ref; 
	BGSLocation * from;
	BGSLocation * to;
};

struct TESBookReadEvent
{
};

struct TESCellAttachDetachEvent
{
	// SendEvent: 004CEB5F
	TESObjectREFR	* ref;
	bool			bAttach;
	UInt8			pad[3];
};

struct TESCellReadyToApplyDecalsEvent
{
	/*
	004CB1B8: 012E4D80.SendEvent(01310448)
	class `anonymous namespace'::DecalApplier +0000
	+0000 01 class `anonymous namespace'::DecalApplier | 01244C0C
	-->	+0000 00 class BSTEventSink<struct TESCellReadyToApplyDecalsEvent> | 01244288
	*/
	TESObjectCELL* unk00;
};

// 04
struct TESCellFullyLoadedEvent
{
	// SendEvent: 00437663 <= 4CDD8E
	TESObjectCELL	* cell;
};

// 0C
struct TESCombatEvent
{
	Actor* subject;     // 00
	Actor* target;      // 04 // The target of the subject actor's scrutiny and hostility; the person the subject is/was searching for.
	SInt32 combatState; // 08
};

// 18
struct TESContainerChangedEvent
{
	FormID		from;
	FormID		to;
	FormID		item;
	UInt32		count;
	RefHandle	toReference;
	UInt32		unk14;
};

struct TESDeathEvent
{
	//
	// This fires TWICE for an actor's death: once when they are fated to die, and 
	// again when they're actually dead; this is the distinction between Papyrus 
	// OnDying and OnDeath. The (isDead) bool indicates whether the actor is dying 
	// (false) or dead (true). Calls to victim->IsDead(0) should return the same 
	// result.
	//
	TESObjectREFR* victim; // 00
	TESObjectREFR* killer; // 04
	bool           isDead; // 08
};


struct TESDestructionStageChangedEvent
{
	TESObjectREFR* ref;      // 00
	SInt32         oldStage; // 04
	SInt32         newStage; // 08
};

struct TESEnterBleedoutEvent
{
	TESObjectREFR* actor;
};

// 10
struct TESEquipEvent
{
	//
	// Fired for unequipping or equipping items. When switching from one item to 
	// another, BSTEvents are fired in order (unequip the old; equip the new). 
	// Expect these to fire when entering an area for the first time, as actors 
	// are apparently considered to equip their armor at that moment.
	//
	// Looting an equipped item off of a corpse sends an unequip event for that 
	// corpse. Killing an actor who has spells equipped and drawn will send un-
	// equip events for their spells.
	//
	// Furthermore, this fires when "using" any item in an inventory or container, 
	// including books, ingredients, and misc-items; using them fires an equip 
	// event but no matching unequip event. Be sure to check the item type when 
	// listening to this event.
	//
	// TODO: Does this fire when using a spell tome stored in another container?
	//
	enum Type : UInt8
	{
		kType_Unequip = 0,
		kType_Equip = 1,
	};
	TESObjectREFR* actor;
	FormID sourceID; // 04 // the weapon, armor, or spell that has been (un)equipped
	UInt32 unk08;
	UInt16 unk0C;
	Type   type; // 0E
	UInt8  pad0F;
};

struct TESFormDeleteEvent
{
	FormID refrID; // 00
};

struct TESFurnitureEvent
{
	enum Type : UInt32
	{
		kType_Enter = 0,
		kType_Exit = 1,
	};
	TESObjectREFR* actor;     // 00
	TESObjectREFR* furniture; // 04
	Type           type;      // 08
	//
};

struct TESGrabReleaseEvent
{ // Fires when the player starts or stops Z-keying an object; doesn't fire for telekinesis
	TESObjectREFR* ref;        // 00 // grabbed/released ref
	bool           isGrabbing; // 04 // whether grabbing or releasing
};

struct TESHitEvent
{
	TESObjectREFR	* target;			// 00
	TESObjectREFR	* caster;			// 04
	FormID			sourceFormID;		// 08
	FormID			projectileFormID;	// 0C

	struct Flags {
		bool powerAttack : 1;
		bool sneakAttack : 1;
		bool bash : 1;
		bool blocked : 1;
	} flags;							// 10

	void			* unk14[7];			// 14
};

struct TESInitScriptEvent
{
	TESObjectREFR	* reference;
	UInt32			unk04;
};


struct TESLoadGameEvent
{
};

struct TESLockChangedEvent
{
	TESObjectREFR* ref; // 00
};

// 0C
struct TESMagicEffectApplyEvent
{
	TESObjectREFR	* target;
	TESObjectREFR	* caster;
	FormID          effectID; // 08
};

struct TESMagicWardHitEvent
{
};

struct TESMoveAttachDetachEvent
{
};

struct TESObjectLoadedEvent
{
	FormID	formID;
	bool    loaded;
};

struct TESObjectREFRTranslationEvent
{
};

struct TESOpenCloseEvent
{
	enum Type : UInt8
	{
		kType_Close = 0,
		kType_Open = 1,
	};
	TESObjectREFR* door; // 00
	TESObjectREFR* whoInteracted; // 04
	Type           type; // 08
};

struct TESPackageEvent
{
};

struct TESPerkEntryRunEvent
{
};

struct TESQuestInitEvent
{
	FormID	formId;
};

struct TESQuestStageEvent
{
	void			* finishedCallback;
	FormID			formId;
	UInt32			stage;
};

struct TESQuestStageItemDoneEvent
{
};

struct TESQuestStartStopEvent
{
	UInt32 questFormID; // quest formID
	bool   unk04; // 04 // start/stop; not sure what value has what meaning
	UInt8  unk05; // 05
	UInt16 pad06;
};

struct TESResetEvent
{
	TESObjectREFR* unk00;
};

struct TESResolveNPCTemplatesEvent
{
};

struct TESSceneEvent
{
};

struct TESSceneActionEvent
{
};

struct TESScenePhaseEvent
{
};

struct TESSellEvent
{
};

// 08
struct TESSleepStartEvent
{
	float	startTime;		// 00
	float	endTime;		// 04
};

struct TESSleepStopEvent
{
};

struct TESSpellCastEvent
{
};

struct TESPlayerBowShotEvent
{
};

struct TESTrackedStatsEvent
{
};

struct TESTrapHitEvent
{
};

struct TESTopicInfoEvent
{
	Actor	* speaker;		// 00 - NiTPointer<Actor>
	void	* unk04;		// 04 - BSTSmartPointer<REFREventCallbacks::IEventCallback>
	FormID  topicInfoID;	// 08
	bool	flag;			// 0C

	inline bool IsStarting() {
		return !flag;
	}
	inline bool IsStopping() {
		return flag;
	}
};

struct TESTriggerEvent
{
};

struct TESTriggerLeaveEvent
{
};

struct TESTriggerEnterEvent
{
};

struct TESUniqueIDChangeEvent	// see: TESV.00482050
{
	UInt32	unk00;		// always 0?
	FormID	formID1;
	FormID	formID2;
	UInt16	unk0C;
	UInt16	unk0E;
};

struct TESWaitStartEvent
{
};

struct TESWaitStopEvent
{
};

struct TESSwitchRaceCompleteEvent
{
	Actor	* actor;
};


// 9F0
struct ScriptEventSourceHolder :
	public BSTEventSource<BGSEventProcessedEvent>,				// 000
	public BSTEventSource<TESActivateEvent>,					// 030
	public BSTEventSource<TESActiveEffectApplyRemoveEvent>,		// 060
	public BSTEventSource<TESActorLocationChangeEvent>,			// 090
	public BSTEventSource<TESBookReadEvent>,					// 0C0
	public BSTEventSource<TESCellAttachDetachEvent>,			// 0F0
	public BSTEventSource<TESCellFullyLoadedEvent>,				// 120
	public BSTEventSource<TESCellReadyToApplyDecalsEvent>,		// 150
	public BSTEventSource<TESCombatEvent>,						// 180
	public BSTEventSource<TESContainerChangedEvent>,			// 1B0
	public BSTEventSource<TESDeathEvent>,						// 1E0
	public BSTEventSource<TESDestructionStageChangedEvent>,		// 210
	public BSTEventSource<TESEnterBleedoutEvent>,				// 240
	public BSTEventSource<TESEquipEvent>,						// 270
	public BSTEventSource<TESFormDeleteEvent>,					// 2A0
	public BSTEventSource<TESFurnitureEvent>,					// 2D0
	public BSTEventSource<TESGrabReleaseEvent>,					// 300
	public BSTEventSource<TESHitEvent>,							// 330
	public BSTEventSource<TESInitScriptEvent>,					// 360
	public BSTEventSource<TESLoadGameEvent>,					// 390
	public BSTEventSource<TESLockChangedEvent>,					// 3C0
	public BSTEventSource<TESMagicEffectApplyEvent>,			// 3F0
	public BSTEventSource<TESMagicWardHitEvent>,				// 420
	public BSTEventSource<TESMoveAttachDetachEvent>,			// 450
	public BSTEventSource<TESObjectLoadedEvent>,				// 480
	public BSTEventSource<TESObjectREFRTranslationEvent>,		// 4B0
	public BSTEventSource<TESOpenCloseEvent>,					// 4E0
	public BSTEventSource<TESPackageEvent>,						// 510
	public BSTEventSource<TESPerkEntryRunEvent>,				// 540
	public BSTEventSource<TESQuestInitEvent>,					// 570
	public BSTEventSource<TESQuestStageEvent>,					// 5A0
	public BSTEventSource<TESQuestStageItemDoneEvent>,			// 5D0
	public BSTEventSource<TESQuestStartStopEvent>,				// 600
	public BSTEventSource<TESResetEvent>,						// 630
	public BSTEventSource<TESResolveNPCTemplatesEvent>,			// 660
	public BSTEventSource<TESSceneEvent>,						// 690
	public BSTEventSource<TESSceneActionEvent>,					// 6C0
	public BSTEventSource<TESScenePhaseEvent>,					// 6F0
	public BSTEventSource<TESSellEvent>,						// 720
	public BSTEventSource<TESSleepStartEvent>,					// 750
	public BSTEventSource<TESSleepStopEvent>,					// 780
	public BSTEventSource<TESSpellCastEvent>,					// 7B0
	public BSTEventSource<TESPlayerBowShotEvent>,				// 7E0
	public BSTEventSource<TESTopicInfoEvent>,					// 810
	public BSTEventSource<TESTrackedStatsEvent>,				// 840
	public BSTEventSource<TESTrapHitEvent>,						// 870
	public BSTEventSource<TESTriggerEvent>,						// 8A0
	public BSTEventSource<TESTriggerEnterEvent>,				// 8D0
	public BSTEventSource<TESTriggerLeaveEvent>,				// 900
	public BSTEventSource<TESUniqueIDChangeEvent>,				// 930
	public BSTEventSource<TESWaitStartEvent>,					// 960
	public BSTEventSource<TESWaitStopEvent>,					// 990
	public BSTEventSource<TESSwitchRaceCompleteEvent>			// 9C0
{
	static ScriptEventSourceHolder* GetInstance(void) {			// 00438740
		ScriptEventSourceHolder *pThis = (ScriptEventSourceHolder*)0x012E4C30;
		//bool &bInit = *(bool*)(pThis + 1);
		//if (!bInit) {
		//	pThis->ctor();
		//	bInit = true;
		//}
		return pThis;
	}

	template <class EventT>
	BSTEventSource<EventT>* GetEventSource() {
		return static_cast<BSTEventSource<EventT>*>(this);
	}

	DEFINE_MEMBER_FN(SendActivateEvent,						void,	0x004E0450, BSTSmartPointer<TESObjectREFR> &target, BSTSmartPointer<TESObjectREFR> &caster);
	//DEFINE_MEMBER_FN(SendTopicInfoEvent,					void,	0x0057DDD0,	FormID topicInfoID, ActorPtr &speaker, UInt32 flag, BSTSmartPointer<REFREventCallbacks::IEventCallback> &arg4);
	//DEFINE_MEMBER_FN(SendTESObjectREFRTranslationEvent,	void,	0x004CB220,	UInt32 arg1, UInt32 arg2);

private:
	DEFINE_MEMBER_FN(ctor, ScriptEventSourceHolder*, 0x00436B90);

};
STATIC_ASSERT(sizeof(ScriptEventSourceHolder) == 0x9F0);


extern BSTEventSource<BGSEventProcessedEvent>&				g_eventProcessedEventSource;
extern BSTEventSource<TESActivateEvent>&					g_activateEventSource;
extern BSTEventSource<TESActiveEffectApplyRemoveEvent>&		g_activeEffectApplyRemoveEventSource;
extern BSTEventSource<TESActorLocationChangeEvent>&			g_actorLocationChangeEventSource;
extern BSTEventSource<TESBookReadEvent>&					g_bookReadEventSource;
extern BSTEventSource<TESCellAttachDetachEvent>&			g_cellAttachDetachEventSource;
extern BSTEventSource<TESCellFullyLoadedEvent>&				g_cellFullyLoadedEventSource;
extern BSTEventSource<TESCellReadyToApplyDecalsEvent>&		g_cellReadyToApplyDecalsEventSource;
extern BSTEventSource<TESCombatEvent>&						g_combatEventSource;
extern BSTEventSource<TESContainerChangedEvent>&			g_containerChangedEventSource;
extern BSTEventSource<TESDeathEvent>&						g_deathEventSource;
extern BSTEventSource<TESDestructionStageChangedEvent>&		g_destructionStageChangedEventSource;
extern BSTEventSource<TESEnterBleedoutEvent>&				g_enterBleedoutEventSource;
extern BSTEventSource<TESEquipEvent>&						g_equipEventSource;
extern BSTEventSource<TESFormDeleteEvent>&					g_formDeleteEventSource;
extern BSTEventSource<TESFurnitureEvent>&					g_furnitureEventSource;
extern BSTEventSource<TESGrabReleaseEvent>&					g_grabReleaseEventSource;
extern BSTEventSource<TESHitEvent>&							g_hitEventSource;
extern BSTEventSource<TESInitScriptEvent>&					g_initScriptEventSource;
extern BSTEventSource<TESLoadGameEvent>&					g_loadGameEventSource;
extern BSTEventSource<TESLockChangedEvent>&					g_lockChangedEventSource;
extern BSTEventSource<TESMagicEffectApplyEvent>&			g_magicEffectApplyEventSource;
extern BSTEventSource<TESMagicWardHitEvent>&				g_magicWardHitEventSource;
extern BSTEventSource<TESMoveAttachDetachEvent>&			g_moveAttachDetachEventSource;
extern BSTEventSource<TESObjectLoadedEvent>&				g_objectLoadedEventSource;
extern BSTEventSource<TESObjectREFRTranslationEvent>&		g_objectREFRTranslationEventSource;
extern BSTEventSource<TESOpenCloseEvent>&					g_openCloseEventSource;
extern BSTEventSource<TESPackageEvent>&						g_packageEventSource;
extern BSTEventSource<TESPerkEntryRunEvent>&				g_perkEntryRunEventSource;
extern BSTEventSource<TESQuestInitEvent>&					g_questInitEventSource;
extern BSTEventSource<TESQuestStageEvent>&					g_questStageEventSource;
extern BSTEventSource<TESQuestStageItemDoneEvent>&			g_questStageItemDoneEventSource;
extern BSTEventSource<TESQuestStartStopEvent>&				g_questStartStopEventSource;
extern BSTEventSource<TESResetEvent>&						g_resetEventSource;
extern BSTEventSource<TESResolveNPCTemplatesEvent>&			g_resolveNPCTemplatesEventSource;
extern BSTEventSource<TESSceneEvent>&						g_sceneEventSource;
extern BSTEventSource<TESSceneActionEvent>&					g_sceneActionEventSource;
extern BSTEventSource<TESScenePhaseEvent>&					g_scenePhaseEventSource;
extern BSTEventSource<TESSellEvent>&						g_sellEventSource;
extern BSTEventSource<TESSleepStartEvent>&					g_sleepStartEventSource;
extern BSTEventSource<TESSleepStopEvent>&					g_sleepStopEventSource;
extern BSTEventSource<TESSpellCastEvent>&					g_spellCastEventSource;
extern BSTEventSource<TESPlayerBowShotEvent>&				g_playerBowShotEventSource;
extern BSTEventSource<TESTopicInfoEvent>&					g_topicInfoEventSource;
extern BSTEventSource<TESTrackedStatsEvent>&				g_trackedStatsEventSource;
extern BSTEventSource<TESTrapHitEvent>&						g_trapHitEventSource;
extern BSTEventSource<TESTriggerEvent>&						g_triggerEventSource;
extern BSTEventSource<TESTriggerEnterEvent>&				g_triggerEnterEventSource;
extern BSTEventSource<TESTriggerLeaveEvent>&				g_triggerLeaveEventSource;
extern BSTEventSource<TESUniqueIDChangeEvent>&				g_uniqueIDChangeEventSource;
extern BSTEventSource<TESWaitStartEvent>&					g_waitStartEventSource;
extern BSTEventSource<TESWaitStopEvent>&					g_waitStopEventSource;
extern BSTEventSource<TESSwitchRaceCompleteEvent>&			g_switchRaceCompleteEventSource;
