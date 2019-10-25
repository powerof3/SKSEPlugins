#pragma once

#include "po3_functions.h"

void SetShaderPropertyRGBTint(NiGeometry * geometry);
void SetShaderPropertyAlpha(NiGeometry * geometry,float alpha, bool onlySkin);
void ReplaceTextureSet(NiGeometry * geometry, BGSTextureSet * sourceTXST, BGSTextureSet * targetTXST, SInt32 textureType);
void ReplaceSkinTXST(NiGeometry * geometry, BGSTextureSet * TXST, SInt32 textureType);

void SetShaderPropertyMLP(NiGeometry * geometry, NiGeometry * templateGeometry);

NiGeometry * GetArmorGeometry(Actor * thisActor, UInt32 slotMask, SInt32 shaderType);

NiAVObject * VisitArmorAddon(Actor * actor, TESObjectARMO * armor, TESObjectARMA * arma);
NiGeometry * GetHeadPartGeometry(Actor * actor, UInt32 partType);

SInt32 GetShaderPropertyType(NiGeometry * geometry);
UInt32 GetShaderPropertyModdedSkin(NiGeometry * geometry);

void SetArmorSkinAlpha(Actor * thisActor, UInt32 slotMask, float alpha);
void SetArmorSkinTXST(Actor * thisActor, BGSTextureSet * TXST, UInt32 slotMask, SInt32 textureType);

float calculateLuminance(UInt8 R, UInt8 G, UInt8 B);
UInt8 colorMix(UInt8 a, UInt8 b, float t);

TESObjectARMO* GetSkinForm(Actor* thisActor, UInt32 mask);
TESObjectARMA * GetArmorAddonByMask(TESRace * race, TESObjectARMO * armor, UInt32 mask);
