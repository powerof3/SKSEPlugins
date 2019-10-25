#pragma once

#include <SKSE.h>
#include "SKSE/GameTypes.h"
#include "SKSE/GameData.h"
#include "SKSE/GameExtraData.h"
#include "SKSE/GameEvents.h"
#include "SKSE/GameFormComponents.h"
#include "SKSE/GameForms.h"
#include "SKSE/GameObjects.h"
#include "SKSE/GameReferences.h"
#include "SKSE/GameRTTI.h"
#include "SKSE/NiObjects.h"
#include "SKSE/NiControllers.h"
#include "SKSE/NiNodes.h"
#include "SKSE/PluginAPI.h"
#include "SKSE/DebugLog.h"

#include "Skyrim\BSSystem\BSTCreateFactory.h"

#include "Skyrim/NetImmerse/NiGeometry.h"
#include "Skyrim/NetImmerse/NiMaterial.h"

#include "Skyrim/Systems/012E32E8.h"
#include "Skyrim/Systems/GameTime.h"
#include "Skyrim/TempEffects/ShaderReferenceEffect.h"

#include "SKSE/PapyrusFunctions.h"
#include "SKSE/PapyrusEvents.h"

#include <algorithm>

#include <cmath>
#include <optional>
#include <random>
#include <ShlObj.h>
#include <vector>
#include <corecrt_math_defines.h>
#include <map>

//---------------------------------------------------------------------------------------------------------------------

static const UInt32 KeywordActorTypeNPCID = 0x00013794;
static const UInt32 ArtSoulTrapTargetEffectsID = 0x000506D6;

//---------------------------------------------------------------------------------------------------------------------

class PO3_SKSEFunctions
{
public:

	//--------------------------------------------------------------------------------------------
	// ACTOR
	//--------------------------------------------------------------------------------------------

	static void GetHairColor(Actor* thisActor, BGSColorForm* color);

	static void SetHairColor(Actor* thisActor, BGSColorForm* color);

	static void GetSkinColor(Actor* thisActor, BGSColorForm* color);

	static void SetSkinColor(Actor* thisActor, BGSColorForm* color);

	static void MixColorWithSkinTone(Actor* thisActor, BGSColorForm* color, bool manualMode, float percentage);

	static void SetSkinAlpha(Actor* thisActor, float alpha);

	static void EquipArmorIfSkinVisible(Actor* thisActor, TESObjectARMO* armorToCheck, TESObjectARMO* armorToEquip);

	static void ReplaceArmorTextureSet(Actor* thisActor, TESObjectARMO* thisArmor, BGSTextureSet* sourceTXST, BGSTextureSet* targetTXST, SInt32 textureType);

	static void ReplaceSkinTextureSet(Actor* thisActor, BGSTextureSet* maleTXST, BGSTextureSet* femaleTXST, UInt32 slotMask, SInt32 textureType);

	static void ReplaceFaceTextureSet(Actor* thisActor, BGSTextureSet* maleTXST, BGSTextureSet* femaleTXST, SInt32 textureType);

	static BGSTextureSet* GetHeadPartTextureSet(Actor* thisActor, UInt32 type);

	static void SetHeadPartTextureSet(Actor* thisActor, BGSTextureSet* headpartTXST, UInt32 type);

	static void SetHeadPartAlpha(Actor* thisActor, UInt32 partType, float alpha);

	static void ToggleSkinnedDecalNode(Actor* thisActor, bool disable);

	static void RemoveFaceGenNode(Actor* thisActor);

	static bool IsActorSoulTrapped(Actor* thisActor);

	static VMArray<TESForm*> AddAllEquippedItemsToArray(Actor* thisActor);

	static void ResetActor3D(Actor* thisActor);

	static void DecapitateActor(Actor* thisActor);
	
	static float GetTimeDead(Actor* thisActor);

	static float GetTimeOfDeath(Actor* thisActor);

	static TESPackage* GetRunningPackage(Actor* thisActor);

	static bool IsActorInWater(Actor* thisActor);

	static float GetActorAlpha(Actor* thisActor);

	static float GetActorRefraction(Actor* thisActor);

	static void SetActorRefraction(Actor* thisActor, float refraction);

	static SInt32 GetDeadState(Actor* thisActor);

	static void SetShaderType(Actor* thisActor, TESObjectARMO* templateArmor);

	//--------------------------------------------------------------------------------------------
	// ARRAY
	//--------------------------------------------------------------------------------------------

	static bool AddStringToArray(BSFixedString thisString, VMArray<BSFixedString> stringArray);

	static bool AddActorToArray(Actor* thisActor, VMArray<Actor*> actorArray);

	static UInt32 ArrayStringCount(BSFixedString thisString, VMArray<BSFixedString> stringArray);

	static VMArray<BSFixedString> SortArrayString(VMArray<BSFixedString> stringArray);

	//--------------------------------------------------------------------------------------------
	// EFFECTSHADER
	//--------------------------------------------------------------------------------------------

	static float GetEffectShaderFullParticleCount(TESEffectShader* thisEffectShader);

	static void SetEffectShaderFullParticleCount(TESEffectShader* thisEffectShader, float particleCount);

	static float GetEffectShaderPersistentParticleCount(TESEffectShader* thisEffectShader);

	static void SetEffectShaderPersistentParticleCount(TESEffectShader* thisEffectShader, float particleCount);

	static bool IsEffectShaderFlagSet(TESEffectShader* thisEffectShader, UInt32 flag);

	static void SetEffectShaderFlag(TESEffectShader* thisEffectShader, UInt32 flag);

	static void ClearEffectShaderFlag(TESEffectShader* thisEffectShader, UInt32 flag);

	//--------------------------------------------------------------------------------------------
	// FORM
	//--------------------------------------------------------------------------------------------

	static void ReplaceKeywordOnForm(TESForm* thisForm, BGSKeyword* KYWDtoRemove, BGSKeyword* KYWDtoAdd);

	//--------------------------------------------------------------------------------------------
	// GAME
	//--------------------------------------------------------------------------------------------

	static bool IsPluginInstalled(BSFixedString name);

	static VMArray<TESForm*> GetAllSpellsInMod(BSFixedString modName, VMArray<BGSKeyword*> keywords, bool isPlayable);

	static VMArray<TESForm*> GetAllRacesInMod(BSFixedString modName, VMArray<BGSKeyword*> keywords);

	static void AddAllGameSpellsToList(BGSListForm* thisList, VMArray<BGSKeyword*> keywords, bool isPlayable);

	static void AddAllGameRacesToList(BGSListForm* thisList, VMArray<BGSKeyword*> keywords);

	static VMArray<Actor*> GetActorsByProcessingLevel(UInt32 level);

	//--------------------------------------------------------------------------------------------
	// LIGHT
	//--------------------------------------------------------------------------------------------

	static float GetLightRadius(TESObjectLIGH* thisLight);

	static void SetLightRadius(TESObjectLIGH* thisLight, float radius);

	static float GetLightFade(TESObjectLIGH* thisLight);

	static void SetLightFade(TESObjectLIGH* thisLight, float fadeValue);

	static BGSColorForm* GetLightColor(TESObjectLIGH* thisLight);

	static void SetLightColor(TESObjectLIGH* thisLight, BGSColorForm* colorform);

	static UInt32 GetLightTypeInternal(TESObjectLIGH* thisLight);

	static UInt32 GetLightType(TESObjectLIGH* thisLight);

	static void SetLightType(TESObjectLIGH* thisLight, UInt32 lightType);

	static float GetLightFOV(TESObjectLIGH* thisLight);

	static void SetLightFOV(TESObjectLIGH* thisLight, float FOV);

	static float GetLightShadowDepthBias(TESObjectREFR* thisLightObject);

	static void SetLightShadowDepthBias(TESObjectREFR* thisLightObject, float depthBias);

	//--------------------------------------------------------------------------------------------
	// LOCATION
	//--------------------------------------------------------------------------------------------

	static BGSLocation* GetParentLocation(BGSLocation* thisLocation);

	static void SetParentLocation(BGSLocation* thisLocation, BGSLocation* newLocation);

	//--------------------------------------------------------------------------------------------
	// MATHS
	//--------------------------------------------------------------------------------------------

	static VMArray<float> GetPositionAsArray(TESObjectREFR* ref);

	static VMArray<float> GetRotationAsArray(TESObjectREFR* ref);

	static float GenerateRandomFloat(float afMin, float afMax);

	static UInt32 GenerateRandomInt(UInt32 afMin, UInt32 afMax);

	//--------------------------------------------------------------------------------------------
	// MAGICEFFECT
	//--------------------------------------------------------------------------------------------

	static VMArray<EffectSetting*> GetAllActiveEffectsOnActor(Actor* thisActor, bool showInactive);

	static BSFixedString GetEffectArchetypeInternal(EffectSetting* mgef);

	static bool HasMagicEffectWithArchetype(Actor* thisActor, BSFixedString archetype);

	static UInt32 GetEffectArchetypeAsInt(EffectSetting* mgef);

	static BSFixedString GetEffectArchetypeAsString(EffectSetting* mgef);

	static BGSSoundDescriptorForm* GetMagicEffectSound(EffectSetting* mgef, UInt32 type);

	static void SetMagicEffectSound(EffectSetting* mgef, BGSSoundDescriptorForm* mgefSound, UInt32 type);

	//--------------------------------------------------------------------------------------------
	// OBJECTREFERENCE
	//--------------------------------------------------------------------------------------------

	static bool IsLoadDoor(TESObjectREFR* thisDoor);

	static bool CanItemBeTaken(InventoryEntryData* data, bool noEquipped, bool noFavourited, bool noQuestItem);

	static void AddAllInventoryItemsToList(TESObjectREFR* thisRef, BGSListForm* thisList, bool noEquipped, bool noFavourited, bool noQuestItem);

	static VMArray<TESForm*> AddAllInventoryItemsToArray(TESObjectREFR* thisRef, bool noEquipped, bool noFavourited, bool noQuestItem);

	static void ReplaceKeywordOnRef(TESObjectREFR* thisRef, BGSKeyword* KYWDtoRemove, BGSKeyword* KYWDtoAdd);

	static void Apply2DHavokImpulse(TESObjectREFR* source, TESObjectREFR* target, float afZ, float magnitude);

	static void Apply3DHavokImpulse(TESObjectREFR* source, TESObjectREFR* target, float magnitude);

	static void MoveToNearestNavmeshLocation(TESObjectREFR* target);

	static VMArray<TESEffectShader*>GetAllEffectShaders(TESObjectREFR* thisRef);

	static UInt32 HasEffectShader(TESObjectREFR* thisRef, TESEffectShader* effectShader);

	static VMArray<BGSArtObject*> GetAllArtObjects(TESObjectREFR* thisRef);

	static UInt32 HasArtObject(TESObjectREFR* thisRef, BGSArtObject* artObject);

	static void StopArtObject(TESObjectREFR* thisRef, BGSArtObject* artObject);

	static Actor* GetActorCause(TESObjectREFR* thisRef);

	static Actor* GetClosestActorFromRef(TESObjectREFR* thisRef, float radius);

	static Actor* GetRandomActorFromRef(TESObjectREFR* thisRef, float radius);

	//--------------------------------------------------------------------------------------------
	// PACKAGE
	//--------------------------------------------------------------------------------------------

	static SInt32 GetPackageType(TESPackage* package);

	//--------------------------------------------------------------------------------------------
	// PROJECTILE
	//--------------------------------------------------------------------------------------------

	static float GetProjectileSpeed(BGSProjectile* thisProjectile);

	static void SetProjectileSpeed(BGSProjectile* thisProjectile, float speed);

	static float GetProjectileRange(BGSProjectile* thisProjectile);

	static void SetProjectileRange(BGSProjectile* thisProjectile, float range);

	static float GetProjectileGravity(BGSProjectile* thisProjectile);

	static void SetProjectileGravity(BGSProjectile* thisProjectile, float gravity);

	static float GetProjectileImpactForce(BGSProjectile* thisProjectile);

	static void SetProjectileImpactForce(BGSProjectile* thisProjectile, float impactForce);

	static UInt32 GetProjectileType(BGSProjectile* thisProjectile);

	//--------------------------------------------------------------------------------------------
	// SOUND
	//--------------------------------------------------------------------------------------------

	static void SetSoundDescriptor(TESSound* thisSound, BGSSoundDescriptorForm* thisSoundDescriptor);

	static UInt32 GetSpellType(SpellItem* thisSpell);

	//--------------------------------------------------------------------------------------------
	// VISUALEFFECT
	//--------------------------------------------------------------------------------------------

	static BGSArtObject* GetArtObject(BGSReferenceEffect* visualEffect);

	static void SetArtObject(BGSReferenceEffect* visualEffect, BGSArtObject* art);

	//--------------------------------------------------------------------------------------------
	// WIND
	//--------------------------------------------------------------------------------------------

	static UInt32 GetWindSpeedAsInt(TESWeather* thisWeather);

	static float GetWindSpeedAsFloat(TESWeather* thisWeather);

	static SInt32 GetWeatherType(TESWeather* thisWeather);

	//---------------------------------------------------------------------------------------------

	static bool Register(VMState* state);
};
