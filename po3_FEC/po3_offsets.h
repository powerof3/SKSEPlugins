#pragma once

#include "main.h"

//-----------------------------------------------------------------------------

typedef float(*_GetHeadingAngle)(VMState * registry, uint32_t stackID, TESObjectREFR *source, TESObjectREFR *target);
extern const _GetHeadingAngle GetHeadingAngle;

typedef void(*_ApplyHavokImpulse)(VMState * registry, uint32_t stackID, TESObjectREFR *target, float afX, float afY, float afZ, float magnitude);
extern const _ApplyHavokImpulse ApplyHavokImpulse;

typedef bool(*_StartQuest)(VMState * registry, uint32_t stackID, TESQuest *quest);
extern const _StartQuest StartQuest;

typedef void(*_StopQuest)(VMState * registry, uint32_t stackID, TESQuest *quest);
extern const _StopQuest StopQuest;

typedef TESObjectREFR*(*_GetReference)(VMState* registry, uint32_t stackID, BGSRefAlias * refAlias);
extern const _GetReference GetReference;

typedef UInt32(*_PlaySound3D)(VMState * registry, uint32_t stackID, TESSound * sound, TESObjectREFR* akSource);
extern const _PlaySound3D PlaySound3D;

typedef void(*_ShowNotification)(const char* message, const char* soundEditorID, bool one);
extern const _ShowNotification ShowNotification;