#include "po3_events.h"

//-------------------------------------------------------------------------------------------------------


//--------------------------------------Event handlers---------------------------------------------------

EventResult OnTESCellFullyLoadedEvent(TESCellFullyLoadedEvent * evn)
{
	if (!evn || evn->cell == nullptr)
	{
		return kEvent_Continue;
	}

	static BSFixedString eventName = "OnCellFullyLoaded";

	return kEvent_Continue;
}

EventResult OnTESGrabReleaseEvent (TESGrabReleaseEvent  * evn)
{
	if (!evn || evn->ref == nullptr)
	{
		return kEvent_Continue;
	}

	BSFixedString eventName = (evn->isGrabbing) ? BSFixedString("OnObjectGrab") : BSFixedString("OnObjectRelease");

	return kEvent_Continue;
}

EventResult OnTESObjectLoadedEvent(TESObjectLoadedEvent * evn)
{
	if (!evn)
	{
		return kEvent_Continue;
	}
	
	TESForm * thisForm = LookupFormByID(evn->formID);
	
	if (thisForm == nullptr)
	{
		return kEvent_Continue;
	}

	BSFixedString eventName = (evn->loaded == 1) ? BSFixedString("OnObjectLoaded") : BSFixedString("OnObjectUnloaded");
	UInt32 formType = thisForm->GetFormType();

	return kEvent_Continue;
}

EventResult OnTESQuestStartStopEvent(TESQuestStartStopEvent * evn)
{
	if (!evn)
	{
		return kEvent_Continue;
	}
	
	TESQuest * quest = DYNAMIC_CAST< TESQuest*>(LookupFormByID(evn->questFormID));

	if (quest == nullptr)
	{
		return kEvent_Continue;
	}

	BSFixedString eventName = (evn->unk04) ? BSFixedString("OnQuestStart") : BSFixedString("OnQuestStop");

	return kEvent_Continue;
}

//----------------------------------EVENTS------------------------------------------------------

