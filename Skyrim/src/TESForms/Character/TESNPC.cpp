#include "Skyrim/TESForms/Character/TESNPC.h"
#include "Skyrim/TESForms/Character/BGSHeadPart.h"

typedef BGSHeadPart ** (*_GetActorBaseOverlays)(TESNPC * npc);
const _GetActorBaseOverlays GetActorBaseOverlays = (_GetActorBaseOverlays)0x00568280;

typedef UInt32(*_GetNumActorBaseOverlays)(TESNPC * npc);
const _GetNumActorBaseOverlays GetNumActorBaseOverlays = (_GetNumActorBaseOverlays)0x005682F0;

BGSHeadPart * TESNPC::GetCurrentHeadPartByType(UInt32 type)
{
	BGSHeadPart * facePart = NULL;
	if (this->HasOverlays())
	{
		facePart = GetHeadPartOverlayByType(type);
	}
	else
	{
		facePart = GetHeadPartByType(type);
	}

	return facePart;
}

BGSHeadPart * TESNPC::GetHeadPartByType(UInt32 type)
{
	if (headparts)
	{
		for (UInt32 i = 0; i < numHeadParts; i++)
		{
			if (headparts[i] && headparts[i]->type == type)
			{
				return headparts[i];
			}
		}
	}

	return NULL;
}

BGSHeadPart * TESNPC::GetHeadPartOverlayByType(UInt32 type)
{
	UInt32 numOverlays = GetNumActorBaseOverlays(this);
	BGSHeadPart ** overlays = GetActorBaseOverlays(this);
	if (overlays)
	{
		for (UInt32 i = 0; i < numOverlays; i++)
		{
			if (overlays[i]->type == type)
			{
				return overlays[i];
			}
		}
	}

	return NULL;
}

TESNPC * TESNPC::GetRootTemplate()
{
	TESNPC * node = NULL;
	node = nextTemplate;
	if (node)
	{
		while (node->nextTemplate)
		{
			node = node->nextTemplate;
		}
	}

	return node;
}