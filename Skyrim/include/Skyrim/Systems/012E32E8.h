#pragma once

#include "Skyrim/TESForms/World/TESObjectREFR.h"
#include "Skyrim/TESForms/Objects/TESEffectShader.h"
#include "Skyrim/TempEffects/ShaderReferenceEffect.h"

#include <Skyrim/NetImmerse/NiRefObject.h>

class BGSLoadGameBuffer;
class BGSSaveFormBuffer;
class Unknown012E32E8
{
public:
	static Unknown012E32E8* GetSingleton()
	{
		return *(Unknown012E32E8**)0x012E32E8; // or should it be *(ClassName*)0x012E32E8 ?
	};
	//

	UInt8  unk000;                   // 008
	bool   enableDetection;          // 001 // if true, then AI detection is enabled; see console:ToggleDetection
	bool   unk002;                   // 002 // if true, then AI detection stats should be printed to the screen; see console:ToggleDetectionStats
	UInt8  unk003;                   // 003
	UInt32 unk004;                   // 004
	bool   enableHighProcess;        // 008 // if true, then AI processing for actors in high is on; see console:ToggleHighProcess
	bool   enableLowProcess;         // 009 // if true, then AI processing for actors in low is on; see console:ToggleLowProcess
	bool   enableMiddleHighProcess;  // 00A // if true, then AI processing for actors in middle-high is on; see console:ToggleMiddleHighProcess
	bool   enableMiddleLowProcess;   // 00B // if true, then AI processing for actors in middle-low is on; see console:ToggleMiddleLowProcess
	bool   enableAISchedules;        // 00C // if true, then AI processing for actors' editor schedules is on; see console:ToggleAISchedules
	UInt8  unk00D;                   // 00D // something to do with subtitles?
	UInt8  unk00E;                   // 00E
	UInt8  unk00F;                   // 00F
	SInt32 numActorsInHighProcess;   // 010 // returned by Console:GetActorsInHigh // reset to 0 in DoAIProcessing and then incremented for every high AI processed
	UInt32 unk014;                   // 014
	UInt32 unk018;                   // 018
	UInt32 unk01C[(0x028 - 0x01C) / sizeof(UInt32)]; // 01C
	//
	// The next four arrays track actors in various process states. The first 
	// state is "high process;" the next three are lower process levels. 
	// Skyrim only has four arrays here, and it has console commands related 
	// to every process level except Middle, so my assumption is that Skyrim 
	// doesn't have the "middle" process level anymore.
	//
	BSTArray<RefHandle>  actorsHigh; // 028 // ref handles for Actors in high process. the TDETECT command loops over this to delete AI/detection state in actors. general AI processing loops over this as well.
	BSTArray<RefHandle>  actorsLow;  // 034 // verified by memory inspection: this had the most handles of all four arrays, so it has to be Low
	BSTArray<RefHandle>  actorsMiddleLow; // 040
	BSTArray<RefHandle>  actorsMiddleHigh; // 04C
	//
	BSTArray<UInt32>* unk058; // 058 // initialized to &this->unk28 // referenced by opcode at 00529EF2 // examine subroutine 00529EC0 for further information
	BSTArray<UInt32>* unk05C; // 05C // initialized to &this->unk40
	BSTArray<UInt32>* unk060; // 060 // initialized to &this->unk4C
	BSTArray<UInt32>* unk064; // 064 // initialized to &this->unk34
	BSSimpleList<UInt32>* unk068[7]; // 068 // templated type unknown
	BSTArray<NiRefObject> unk084; // 084
	BSSpinLock unk084Lock; // 090
	BSTArray<ShaderReferenceEffect*> activeEffectShaders;      // 098
	BSSpinLock                       activeEffectShaderLock;   // 0A4
	UInt32	unk0AC[(0xB8 - 0x0AC) / sizeof(UInt32)];
	BSSpinLock	unk0B8Lock;   // 0B8
	UInt32	unk0C0[(0xC8 - 0x0C0) / sizeof(UInt32)];
	BSTArray<RefHandle>  unkC8; // C8 // list of ref handles, possibly refs queued to reset
	BSSimpleList<RefHandle>	unkD4; // D4 // list of ref handles
	UInt32 unk0DC[(0x108 - 0x0DC) / sizeof(UInt32)]; // 0DC
	float  unk108; // 108 // related to Z-keying?
	UInt32 unk10C;
	UInt32 unk110;
	UInt32 unk114;
	UInt32 unk118;
	bool   enableActorAI;        // 11C // if true, then all AI processing is on
	bool   enableActorMovement;  // 11D // if true, then all actor movement processing is on
	bool   enableActorAnimation; // 11E // if true, then all actor animation processing is on
	UInt8  unk11F;               // 11F // modified by opcode at 004A6BDE
	// ...

	typedef void(*HandleCallback)(UInt32& handle);
	struct HandleFunctor
	{
		virtual void Execute(UInt32 handle);
	};

	//
	// NOTE: Not all of the functions below should be called by mods.
	//
	DEFINE_MEMBER_FN(AddHandleToUnk0C8, void, 0x00756940, Actor* actor); // aborts if actor is already in the array
	DEFINE_MEMBER_FN(AppendValueToArray, void, 0x00756370, UInt32 value, UInt32 which); // appends to one of unk028, unk034, unk040, or unk04C; no bounds-checking on (which)
	DEFINE_MEMBER_FN(DoAIProcessing, void, 0x0075CBB0, UInt32, UInt32); // no-oping this has the same effect as global TAI
	DEFINE_MEMBER_FN(DoMovementProcessing, void, 0x00756460, UInt32, UInt32); // no-oping this has the same effect as TMOVE
	DEFINE_MEMBER_FN(Load, void, 0x007549B0, BGSLoadGameBuffer*);
	DEFINE_MEMBER_FN(RemoveValueInArray, void, 0x007563E0, UInt32 value, UInt32 which); // finds and removes from one of unk028, unk034, unk040, or unk04C; no bounds-checking on (which)
	DEFINE_MEMBER_FN(RemoveHandleFromUnkD4, void, 0x00756720, UInt32& refHandle);
	DEFINE_MEMBER_FN(ResetAllDetection, void, 0x00542970, bool* unused);
	DEFINE_MEMBER_FN(ForEachActorInHighProcess, void, 0x006931E0, HandleFunctor& functor); // for each of this->unk028, call functor->Unk_00(item)
	DEFINE_MEMBER_FN(ForEachActorInHighProcess_B, void, 0x006A09E0, HandleCallback func); // for each of this->unk028, runs func(&item); terminates early if func returns false
	DEFINE_MEMBER_FN(Save, void, 0x007544F0, BGSSaveFormBuffer*);
	DEFINE_MEMBER_FN(SearchUnk68ListFor, UInt32, 0x00754440, UInt32 whichList, UInt32 searchFor);
	DEFINE_MEMBER_FN(StopEffectShader, void, 0x00754840, TESObjectREFR* ref, TESEffectShader* shader);
	DEFINE_MEMBER_FN(StopArtObject, void, 0x00754900, TESObjectREFR* ref, BGSArtObject* art);
	DEFINE_MEMBER_FN(Subroutine00753F80, float, 0x00753F80);
	DEFINE_MEMBER_FN(Subroutine00753F90, void, 0x00753F90, float);
	DEFINE_MEMBER_FN(Subroutine00754750, void, 0x00754750, UInt32);
	DEFINE_MEMBER_FN(Subroutine00754900, void, 0x00754900, TESObjectREFR*, void*); // stops a "model reference shader"?
	DEFINE_MEMBER_FN(Subroutine0075D280, void, 0x0075D280);
	DEFINE_MEMBER_FN(Subroutine0075D2A0, void, 0x0075D2A0);
	DEFINE_MEMBER_FN(Subroutine0075E100, void, 0x0075E100); // checks whether animation processing is enabled; does stuff based on that
	DEFINE_MEMBER_FN(ToggleMovementProcessing, void, 0x00754000); // toggles the value of unk11D
	DEFINE_MEMBER_FN(ToggleAnimationProcessing, void, 0x00754020); // toggles the value of unk11E
};
static_assert(offsetof(Unknown012E32E8, actorsHigh) >= 0x028, "Unknown012E32E8::unk084 is too early!");
static_assert(offsetof(Unknown012E32E8, actorsHigh) <= 0x028, "Unknown012E32E8::unk084 is too late!");
static_assert(offsetof(Unknown012E32E8, actorsLow) >= 0x034, "Unknown012E32E8::unk084 is too early!");
static_assert(offsetof(Unknown012E32E8, actorsLow) <= 0x034, "Unknown012E32E8::unk084 is too late!");
static_assert(offsetof(Unknown012E32E8, actorsMiddleLow) >= 0x040, "Unknown012E32E8::unk084 is too early!");
static_assert(offsetof(Unknown012E32E8, actorsMiddleLow) <= 0x040, "Unknown012E32E8::unk084 is too late!");
static_assert(offsetof(Unknown012E32E8, actorsMiddleHigh) >= 0x04C, "Unknown012E32E8::unk084 is too early!");
static_assert(offsetof(Unknown012E32E8, actorsMiddleHigh) <= 0x04C, "Unknown012E32E8::unk084 is too late!");

static_assert(offsetof(Unknown012E32E8, unk084Lock) >= 0x090, "Unknown012E32E8::unk0B8 is too early!");
static_assert(offsetof(Unknown012E32E8, unk084Lock) <= 0x090, "Unknown012E32E8::unk0B8 is too late!");

static_assert(offsetof(Unknown012E32E8, activeEffectShaders) >= 0x098, "Unknown012E32E8::unk0B8 is too early!");
static_assert(offsetof(Unknown012E32E8, activeEffectShaders) <= 0x098, "Unknown012E32E8::unk0B8 is too late!");

static_assert(offsetof(Unknown012E32E8, activeEffectShaderLock) >= 0x0A4, "Unknown012E32E8::unk0B8 is too early!");
static_assert(offsetof(Unknown012E32E8, activeEffectShaderLock) <= 0x0A4, "Unknown012E32E8::unk0B8 is too late!");

static_assert(offsetof(Unknown012E32E8, unk0B8Lock) >= 0x0B8, "Unknown012E32E8::unk0B8 is too early!");
static_assert(offsetof(Unknown012E32E8, unk0B8Lock) <= 0x0B8, "Unknown012E32E8::unk0B8 is too late!");

static_assert(offsetof(Unknown012E32E8, unkC8) >= 0x0C8, "Unknown012E32E8::unk0C8 is too early!");
static_assert(offsetof(Unknown012E32E8, unkC8) <= 0x0C8, "Unknown012E32E8::unk0C8 is too late!");

static_assert(offsetof(Unknown012E32E8, enableActorAnimation) >= 0x11E, "Unknown012E32E8::unk0C8 is too early!");
static_assert(offsetof(Unknown012E32E8, enableActorAnimation) <= 0x11E, "Unknown012E32E8::unk0C8 is too late!");

// Refernces to enableActorAI:
// 53B1B9 (ToggleAI)
// 6D5FEE (Actor::Subroutine006D5B70)
// 7480BB (PlayerCharacter::Subroutine00748010)
// 75CBEF
// 75D190 (constructor)
// 75D280 (...::Subroutine0075D280)
// 75D2C6 (...::Subroutine0075D2A0)
