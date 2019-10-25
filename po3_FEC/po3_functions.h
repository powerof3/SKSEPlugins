#pragma once

#include "main.h"

#include "Skyrim/NetImmerse/NiGeometry.h"
#include "Skyrim/NetImmerse/NiMaterial.h"

namespace po3_functions
{
	//-------------------------MISC ------------------------------------------------------

	NiAVObject* GetHeadPartObject(Actor * actor, UInt32 partType);

	NiGeometry * GetHeadPartGeometry(Actor * actor, UInt32 partType);

	NiAVObject * VisitArmorAddon(Actor * actor, TESObjectARMO * armor, TESObjectARMA * arma);

	SInt32 GetShaderPropertyType(NiGeometry * geometry);

	TESObjectARMO * GetWornForm(Actor* thisActor, UInt32 mask);

	TESObjectARMO* GetSkinForm(Actor* thisActor, UInt32 mask);

	TESObjectARMA * GetArmorAddonByMask(TESRace * race, TESObjectARMO * armor, UInt32 mask);

	//-------------------------------------------------------------------------------------

	void SetShaderPropertyAlpha(NiGeometry * geometry, float alpha, bool onlySkin);

	void SetArmorSkinAlpha(Actor * thisActor, UInt32 slotMask, float alpha);

	//-------------------------------------------------------------------------------------

	void SetSkinAlpha(Actor * thisActor, float alpha);

	//--------------------------------------------------------------------------------------

	void DisableHeadPart(Actor * thisActor);
}
