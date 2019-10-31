#include "po3_functions.h"

const SKSETaskInterface	* g_task = nullptr;
BGSKeyword* keyword = nullptr;
BGSArtObject* art = nullptr;

//----------------------------------------------------------------------------------------

void OnInit(SKSEMessagingInterface::Message* a_msg)
{
	if (a_msg->type == SKSEMessagingInterface::kMessage_DataLoaded)
	{
		keyword = DYNAMIC_CAST<BGSKeyword*>(TESForm::LookupByID(KeywordActorTypeNPCID));
		art = DYNAMIC_CAST<BGSArtObject*>(TESForm::LookupByID(ArtSoulTrapTargetEffectsID));
	}
}

//----------------------------------------------------------------------------------------

class po3_PapyrusExtender : public SKSEPlugin
{
public:
	po3_PapyrusExtender()
	{
	}

	virtual bool InitInstance() override
	{
		if (!Requires(kSKSEVersion_1_7_3, SKSEPapyrusInterface::Version_1, SKSEMessagingInterface::Version_2, SKSETaskInterface::Version_2))
		{
			gLog << "ERROR: your skse version is too old." << std::endl;
			return false;
		}

		SetName("PapyrusExtender");
		SetVersion(2);

		return true;
	}

	virtual bool OnLoad() override
	{
		SKSEPlugin::OnLoad();

		const SKSEPapyrusInterface *papyrus = GetInterface(SKSEPapyrusInterface::Version_1);
		if (papyrus)
		{
			papyrus->Register(&PO3_SKSEFunctions::Register);
		}
		else
		{
			_MESSAGE("failed to get papyrus interface!");
		}

		g_task = GetInterface(SKSETaskInterface::Version_2);
		if (!g_task)
		{
			_MESSAGE("failed to get task interface!");
		}

		const auto messaging = GetInterface(SKSEMessagingInterface::Version_2);
		if (!messaging->RegisterListener("SKSE", OnInit))
		{
			_MESSAGE("failed to get messaging interface!");
			return false;
		}

		_MESSAGE("");

		return true;
	}

	virtual void OnModLoaded() override
	{
	}
} thePlugin;