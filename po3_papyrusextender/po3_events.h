#pragma once

#include "po3_functions.h"

class po3_SKSEvents
{
public:
	static void RegisterForCellFullyLoaded_Alias(BGSRefAlias * thisAlias);
	static void UnregisterForCellFullyLoaded_Alias(BGSRefAlias * thisAlias);

	static void RegisterForCellFullyLoaded_ActiveEffect(ActiveEffect * thisEffect);
	static void UnregisterForCellFullyLoaded_ActiveEffect(ActiveEffect * thisEffect);

	static void RegisterForMagicEffectRemove_Alias(BGSRefAlias * thisAlias, TESObjectREFR * ref);
	static void UnregisterForMagicEffectRemove_Alias(BGSRefAlias * thisAlias, TESObjectREFR * ref);

	static void RegisterForMagicEffectRemove_ActiveEffect(ActiveEffect * thisEffect, TESObjectREFR * ref);
	static void UnregisterForMagicEffectRemove_ActiveEffect(ActiveEffect * thisEffect, TESObjectREFR * ref);

	static void RegisterForObjectGrabRelease_Alias(BGSRefAlias * thisAlias);
	static void UnregisterForObjectGrabRelease_Alias(BGSRefAlias * thisAlias);

	static void RegisterForObjectGrabRelease_ActiveEffect(ActiveEffect * thisEffect);
	static void UnregisterForObjectGrabRelease_ActiveEffect(ActiveEffect * thisEffect);

	static void RegisterForObjectLoaded_Alias(BGSRefAlias * thisAlias, UInt32 formType);
	static void UnregisterForObjectLoaded_Alias(BGSRefAlias * thisAlias, UInt32 formType);

	static void RegisterForObjectLoaded_ActiveEffect(ActiveEffect * thisEffect, UInt32 formType);
	static void UnregisterForObjectLoaded_ActiveEffect(ActiveEffect * thisEffect, UInt32 formType);

	static void RegisterForQuestStartStop_Alias(BGSRefAlias * thisAlias);
	static void UnregisterForQuestStartStop_Alias(BGSRefAlias * thisAlias);

	static void RegisterForQuestStartStop_ActiveEffect(ActiveEffect * thisEffect);
	static void UnregisterForQuestStartStop_ActiveEffect(ActiveEffect * thisEffect);
};
