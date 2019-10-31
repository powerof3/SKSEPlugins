#include "main.h"
#include "simpleini-4.17/SimpleIni.h"

//--------------------------------------

SInt32 actorCount = 0;

bool forceStealth = false;
bool fixSubtitles = true;

TESRace* mannequinRace = nullptr;
SpellItem* deathEffectsAbility = nullptr;
Actor* loadScreenDetector = nullptr;

BGSKeyword* frostKYWD = nullptr;

const UInt32 mannequinRaceID = 0x0010760A;
const UInt32 magicDamageFrostID = 0x0001CEAE;

//--------------------------------------

const std::string& SKYGetRuntimeDirectory()
{
	static std::string s_runtimeDirectory;

	if (s_runtimeDirectory.empty())
	{
		// can't determine how many bytes we'll need, hope it's not more than MAX_PATH
		char	runtimePathBuf[MAX_PATH];
		UInt32	runtimePathLength = GetModuleFileName(GetModuleHandle(NULL), runtimePathBuf, sizeof(runtimePathBuf));

		if (runtimePathLength && (runtimePathLength < sizeof(runtimePathBuf)))
		{
			std::string	runtimePath(runtimePathBuf, runtimePathLength);

			// truncate at last slash
			std::string::size_type	lastSlash = runtimePath.rfind('\\');
			if (lastSlash != std::string::npos)	// if we don't find a slash something is VERY WRONG
			{
				s_runtimeDirectory = runtimePath.substr(0, lastSlash + 1);
			}
		}
	}

	return s_runtimeDirectory;
}

//----------------------------------------------------------------------------------------

const char* SKYGetConfigPath()
{
	static std::string s_configPath;

	if (s_configPath.empty())
	{
		const auto& runtimePath = SKYGetRuntimeDirectory();
		if (!runtimePath.empty())
		{
			s_configPath = runtimePath + R"(Data\SKSE\Plugins\po3_FEC.ini)";

			_MESSAGE("config path : %s", s_configPath.c_str());
		}
	}

	return s_configPath.c_str();
}

//----------------------------------------------------------------------------------------

bool ReadINI()
{
	CSimpleIniA ini;
	ini.SetUnicode();
	SI_Error rc = ini.LoadFile(SKYGetConfigPath());

	if (rc < 0)
	{
		_MESSAGE("couldn't read INI file!");
		return false;
	}

	forceStealth = ini.GetBoolValue("Stealth Settings", "Force Stealth", false);

	fixSubtitles = ini.GetBoolValue("Frost Settings", "Subtitle Fix", true);
	
	return true;
}

//----------------------------------------------------------------------------------------

void GetFormsFromMod()
{
	static UInt32 formID00 = 0;
	static UInt32 formID01 = 0;

	auto dataHandler = TESDataHandler::GetSingleton();
	UInt32 id = dataHandler->GetModIndex("FireBurns.esp");

	if (id != 0xFF)
	{
		formID00 = (id << 24) | 0x04CC4A4; //spell
		formID01 = (id << 24) | 0x03E3659; //actor
	}
	else
	{
		return;
	}

	if (formID00)
	{
		deathEffectsAbility = DYNAMIC_CAST<SpellItem*>(LookupFormByID(formID00));
	}
	if (formID01)
	{
		loadScreenDetector = DYNAMIC_CAST<Actor*>(LookupFormByID(formID01));
	}

	mannequinRace = DYNAMIC_CAST<TESRace*>(LookupFormByID(mannequinRaceID));
	frostKYWD = DYNAMIC_CAST<BGSKeyword*>(LookupFormByID(magicDamageFrostID));
}

//----------------------------------------------------------------------------------------

bool IsActorValid(Actor* actor)
{
	if (actor->IsEssensial() || actor->IsPlayerTeammate() || actor->IsChild() || actor->IsCommandedActor())
	{
		return false;
	}

	return true;
}

//---------------------------------------------------------------------------------------

namespace BSTaskPool_Hook //runs very frame but only if new actors are added to high process array
{
	void _stdcall Inner()
	{
		auto singleton = Unknown012E32E8::GetSingleton();

		if (actorCount == singleton->numActorsInHighProcess)
		{
			return;
		}

		actorCount = singleton->numActorsInHighProcess;
		TESObjectREFRPtr refPtr;
		Actor* actor = nullptr;

		for (auto& refHandle : singleton->actorsHigh)
		{
			LookupREFRByHandle(refHandle, refPtr);

			actor = niptr_cast<Actor>(refPtr);

			if (actor)
			{
				if (IsActorValid(actor))
				{
					actor->AddSpell(deathEffectsAbility);
				}
			}
		}
	}

	__declspec(naked) void Outer()
	{
		_asm {
			push ecx; // protect
			call Inner; // stdcall
			pop  ecx; // restore
			mov  eax, 0x006A0950; //cobbAPI hooks before us in subroutine
			call eax; //
			mov  ecx, 0x00691100;
			jmp  ecx;
		}
	}

	void Apply()
	{
		WriteRelJump(0x006910FB, reinterpret_cast<UInt32>(&Outer)); //bstaskpool
	}
}

//---------------------------------------------------------------------------------------

void FixFrozenDeathSubtitles()
{
	auto dataHandler = TESDataHandler::GetSingleton();

	for (auto& topic : dataHandler->topics)
	{
		if (topic->data.subtype == TESTopic::Data::Subtype::kDeath)
		{
			auto count = topic->infoCount;

			for (UInt32 i = 0; i < count; i++)
			{
				auto topicInfo = topic->infoTopics[i];

				if (topicInfo && (topicInfo->dialogFlags & TESTopicInfo::DialogFlag::kDialogFlag_HasNoLipFile) == 0)
				{
					auto newNode = new Condition::Node;
					
					newNode->next = nullptr;
					newNode->comparisonValue = 0.0;
					newNode->functionID = Condition::FunctionID::kHasMagicEffectKeyword;
					newNode->param1 = frostKYWD;

					if (topicInfo->conditions.head == nullptr)
					{
						topicInfo->conditions.head = newNode;						
					}
					else
					{
						auto lastNode = topicInfo->conditions.head;

						while (lastNode->next != nullptr)
						{
							lastNode = lastNode->next;
						}

						lastNode->next = newNode;
					}					
				}
			}
		}
	}
}

//----------------------------------------------------------------------------------------

class TESResetEventHandler : public BSTEventSink<TESResetEvent> //manages AI reset for frozen NPCS
{
public:
	using EventResult = EventResult;


	static TESResetEventHandler* GetSingleton()
	{
		static TESResetEventHandler singleton;
		return &singleton;
	}

	virtual EventResult ReceiveEvent(TESResetEvent* evn, BSTEventSource<TESResetEvent>* a_evnSource) override
	{
		if (!evn || evn->unk00 == nullptr)
		{
			return kEvent_Continue;
		}

		auto thisActor = DYNAMIC_CAST<Actor*>(evn->unk00);

		if (!thisActor)
		{
			return kEvent_Continue;
		}

		if (thisActor->race == mannequinRace || thisActor->IsAIEnabled())
		{
			return kEvent_Continue;
		}

		thisActor->EnableAI(true);

		return kEvent_Continue;
	}

protected:
	TESResetEventHandler() = default;
	TESResetEventHandler(const TESResetEventHandler&) = delete;
	TESResetEventHandler(TESResetEventHandler&&) = delete;
	virtual ~TESResetEventHandler() = default;

	TESResetEventHandler& operator=(const TESResetEventHandler&) = delete;
	TESResetEventHandler& operator=(TESResetEventHandler&&) = delete;
};

//----------------------------------------------------------------------------------------------------

void OnInit(SKSEMessagingInterface::Message* msg)
{
	if (msg->type == SKSEMessagingInterface::kMessage_DataLoaded)
	{
		GetFormsFromMod();

		BSTaskPool_Hook::Apply();
		_MESSAGE("Registered frame hook");

		if (forceStealth)
		{
			Detection_Hook::Apply();
			_MESSAGE("Registered detection hook");

			DetectionInterceptor::GetInstance();
		}
		else
		{
			_MESSAGE("Forced stealth is off for FEC - Load Screen Detector.");
		}

		if (fixSubtitles)
		{
			_MESSAGE("Patched death dialogue from showing up on frozen NPCs.");
			FixFrozenDeathSubtitles();
		}
		else
		{
			_MESSAGE("Subtitle fix is off.");
		}

		ScriptEventSourceHolder::GetInstance()->GetEventSource<TESResetEvent>()->AddEventSink(TESResetEventHandler::GetSingleton());
		_MESSAGE("Registered object reset event handler");

		_MESSAGE("");
	}
	else if (msg->type == SKSEMessagingInterface::kMessage_PostLoadGame)
	{
		if (forceStealth && loadScreenDetector)
		{
			DetectionInterceptor::GetInstance().SetActorUnseen(loadScreenDetector, true);
		}
	}
}

//----------------------------------------------------------------------------------------

class po3_FEC : public SKSEPlugin
{
public:
	po3_FEC()
	{
	}

	virtual bool InitInstance() override
	{
		if (!Requires(kSKSEVersion_1_7_3, SKSEMessagingInterface::Version_2))
		{
			gLog << "ERROR: your skse version is too old." << std::endl;
			return false;
		}

		if (!Requires("FireBurns.esp"))
		{
			gLog << "ERROR: FEC's esp (FireBurns.esp) not found. Aborting" << std::endl;
			return false;
		}

		SetName("FEC");
		SetVersion(3.0f);

		return true;
	}

	virtual bool OnLoad() override
	{
		SKSEPlugin::OnLoad();

		ReadINI();

		const SKSEMessagingInterface* message = GetInterface(SKSEMessagingInterface::Version_2);
		message->RegisterListener("SKSE", OnInit);

		_MESSAGE("");

		return true;
	}

	virtual void OnModLoaded() override
	{

	}
} thePlugin;