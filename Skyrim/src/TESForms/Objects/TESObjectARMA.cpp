#include "Skyrim.h"
#include "Skyrim/RTTI.h"
#include "Skyrim/TESForms/Objects/TESObjectARMA.h"
#include "Skyrim/TESForms/Objects/TESObjectARMO.h"
#include "Skyrim/TESForms/Character/TESRace.h"
#include "Skyrim/TESForms/Character/TESNPC.h"
#include "Skyrim/TESForms/World/TESObjectREFR.h"

#include <ShlObj.h>

bool HasArmorRace(TESRace * sourceRace, TESRace * targetRace)
{
	if (sourceRace == targetRace)
		return true;

	// Check if this race inherits the source race
	TESRace * armorRace = sourceRace->armorRace;
	while (armorRace)
	{
		if (armorRace == targetRace)
			return true;
		armorRace = armorRace->armorRace;
	}

	return false;
}

bool TESObjectARMA::IsValidRace(TESRace * sourceRace) const
{
	// Found race is the race this armor is designed for, or inherits a race this armor is designed for
	if (HasArmorRace(sourceRace, race))
	{
		return true;
	}

	for (auto &currentRace : additionalRaces)
	{
		// Source race is a found race, or inherits one of the found races
		if (currentRace && HasArmorRace(sourceRace, currentRace))
		{
			return true;
		}
	}

	return false;
}

void TESObjectARMA::GetNodeName(char * dstBuff, TESObjectREFR * refr, TESObjectARMO * armor, float weightOverride)
{
	float weight = 100.0;
	TESNPC * npc = DYNAMIC_CAST<TESNPC*>(refr->baseForm);
	if (npc && npc->nextTemplate)
	{
		TESNPC * temp = npc->GetRootTemplate();
		if (temp)
		{
			weight = temp->weight;
		}
	}
	else
	{
		weight = refr->GetWeight();
	}

	weight /= 100.0;
	if (weightOverride >= 0.0) // Determines whether to factor weight into the name, -1 true, 1 false
	{
		weight = weightOverride;
	}
	weight *= 100.0;

	UInt32 sex = npc ? npc->GetSex() : 0;
	/*sprintf_s(dstBuff, MAX_PATH, "%s (%08X)[%d]/%s (%08X) [%2.0f%%]",
		this->Unk_32(), // I really have no idea why they used a string here for a boolean value, probably legacy code
		this->formID,
		sex,
		armor->Unk_32(),
		armor->formID,
		weight);*/
	sprintf_s(dstBuff, MAX_PATH, " (%08X)[%d]/ (%08X) [%2.0f%%]",
			  this->formID,
			  sex,
			  armor->formID,
			  weight);
}