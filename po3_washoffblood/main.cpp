#include "main.h"
#include "event.h"
#include "simpleini-4.17\SimpleIni.h"

//---------------------------------------------------------------------------------------
const SKSETaskInterface * task = nullptr;
bool npcBlood = false;
//----------------------------------------------------------------------------------------

const std::string & SKYGetRuntimeDirectory(void)
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
			else
			{
				_MESSAGE("no slash in runtime path? (%s)", runtimePath.c_str());
			}
		}
		else
		{
			_MESSAGE("couldn't find runtime path (len = %d, err = %08X)", runtimePathLength, GetLastError());
		}
	}

	return s_runtimeDirectory;
}

//-------------------------------------------------------------------------------------------------------

const char * SKYGetConfigPath()
{
	static std::string s_configPath;
	const char * c_configPath;

	if (s_configPath.empty())
	{
		std::string	runtimePath = SKYGetRuntimeDirectory();
		if (!runtimePath.empty())
		{
			s_configPath = runtimePath + "Data\\SKSE\\Plugins\\po3_WashThatBloodOff.ini";
			c_configPath = s_configPath.c_str();

			_MESSAGE("config path : %s", c_configPath);
		}
	}

	return c_configPath;
}

//-------------------------------------------------------------------------------------------------------

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

	npcBlood = ini.GetBoolValue("Settings", "NPCs", false);

	return true;
}

//---------------------------------------------------------------------------------------------------------

bool SinkAnimationGraphEventHandler(BSTEventSink<BSAnimationGraphEvent>* a_sink, Actor * thisActor)
{
	BSAnimationGraphManagerPtr graphManager;
	thisActor->GetAnimationGraphManager(graphManager);
	if (graphManager)
	{
		bool sinked = false;
		for (auto& animationGraph : graphManager->animationGraphs)
		{
			if (sinked)
			{
				break;
			}
			BSTEventSource<BSAnimationGraphEvent>* eventSource = animationGraph->GetBSAnimationGraphEventSource();
			for (auto& sink : eventSource->eventSinks)
			{
				if (sink == a_sink)
				{
					sinked = true;
					break;
				}
			}
		}
		if (!sinked)
		{
			graphManager->animationGraphs.front()->GetBSAnimationGraphEventSource()->AddEventSink(a_sink);
			return true;
		}
	}
	return false;
}

//---------------------------------------------------------------------------------------------

class TESObjectLoadedEventHandler : public BSTEventSink<TESObjectLoadedEvent>
{
public:
	using EventResult = EventResult;

	static TESObjectLoadedEventHandler* GetSingleton()
	{
		static TESObjectLoadedEventHandler singleton;
		return &singleton;
	}

	virtual EventResult ReceiveEvent(TESObjectLoadedEvent* evn, BSTEventSource<TESObjectLoadedEvent>* a_evnSource) override
	{
		if (!evn || !evn->loaded)
		{
			return kEvent_Continue;
		}

		if (!npcBlood)
		{
			if (evn->formID == g_thePlayer->formID)
			{
				(SinkAnimationGraphEventHandler(BSAnimationGraphEventHandler::GetSingleton(), g_thePlayer));
			}
		}
		else
		{
			Actor * thisActor = DYNAMIC_CAST<Actor*>(LookupFormByID(evn->formID));

			if (!thisActor)
			{
				return kEvent_Continue;
			}

			if ((thisActor->race->data.raceFlags & TESRace::kSwims) != TESRace::kSwims)
			{
				return kEvent_Continue;
			}

			if ((thisActor->race->data.raceFlags & TESRace::kDontShowBloodDecal) == TESRace::kDontShowBloodDecal)
			{
				return kEvent_Continue;
			}

			SinkAnimationGraphEventHandler(BSAnimationGraphEventHandler::GetSingleton(), thisActor);
		}

		return kEvent_Continue;
	}

protected:
	TESObjectLoadedEventHandler() = default;
	TESObjectLoadedEventHandler(const TESObjectLoadedEventHandler&) = delete;
	TESObjectLoadedEventHandler(TESObjectLoadedEventHandler&&) = delete;
	virtual ~TESObjectLoadedEventHandler() = default;

	TESObjectLoadedEventHandler& operator=(const TESObjectLoadedEventHandler&) = delete;
	TESObjectLoadedEventHandler& operator=(TESObjectLoadedEventHandler&&) = delete;
};

//---------------------------------------------------------------------------------------------

class TESSwitchRaceCompleteEventHandler : public BSTEventSink<TESSwitchRaceCompleteEvent>
{
public:
	using EventResult = EventResult;


	static TESSwitchRaceCompleteEventHandler* GetSingleton()
	{
		static TESSwitchRaceCompleteEventHandler singleton;
		return &singleton;
	}


	virtual EventResult ReceiveEvent(TESSwitchRaceCompleteEvent* evn, BSTEventSource<TESSwitchRaceCompleteEvent>* a_eventSource) override
	{
		if (!evn || !evn->actor)
		{
			return kEvent_Continue;
		}

		if (!npcBlood)
		{
			PlayerCharacter* player = (g_thePlayer);

			if (evn->actor == player)
			{
				(SinkAnimationGraphEventHandler(BSAnimationGraphEventHandler::GetSingleton(), player));
			}
		}
		else
		{
			auto thisActor = evn->actor;

			if ((thisActor->race->data.raceFlags & TESRace::kSwims) != TESRace::kSwims)
			{
				return kEvent_Continue;
			}

			if ((thisActor->race->data.raceFlags & TESRace::kDontShowBloodDecal) == TESRace::kDontShowBloodDecal)
			{
				return kEvent_Continue;
			}

			SinkAnimationGraphEventHandler(BSAnimationGraphEventHandler::GetSingleton(), thisActor);
		}

		return kEvent_Continue;
	}

protected:
	TESSwitchRaceCompleteEventHandler() = default;
	TESSwitchRaceCompleteEventHandler(const TESSwitchRaceCompleteEventHandler&) = delete;
	TESSwitchRaceCompleteEventHandler(TESSwitchRaceCompleteEventHandler&&) = delete;
	virtual ~TESSwitchRaceCompleteEventHandler() = default;

	TESSwitchRaceCompleteEventHandler& operator=(const TESSwitchRaceCompleteEventHandler&) = delete;
	TESSwitchRaceCompleteEventHandler& operator=(TESSwitchRaceCompleteEventHandler&&) = delete;
};

//----------------------------------------------------------------------------------------------

void OnInit(SKSEMessagingInterface::Message* msg)
{
	if (msg->type == SKSEMessagingInterface::kMessage_DataLoaded)
	{
		auto sourceHolder = ScriptEventSourceHolder::GetInstance();
		sourceHolder->GetEventSource<TESObjectLoadedEvent>()->AddEventSink(TESObjectLoadedEventHandler::GetSingleton());
		_MESSAGE("Registered object loaded event handler");
		sourceHolder->GetEventSource<TESSwitchRaceCompleteEvent>()->AddEventSink(TESSwitchRaceCompleteEventHandler::GetSingleton());
		_MESSAGE("Registered race switch event handler");
	}
}

//-------------------------------------------------------------------------------------------------

class po3_WashThatBloodOff : public SKSEPlugin
{
public:
	po3_WashThatBloodOff()
	{
	}

	virtual bool InitInstance() override
	{
		if (!Requires(kSKSEVersion_1_7_3, SKSEMessagingInterface::Version_2, SKSETaskInterface::Version_2))
		{
			gLog << "ERROR: your skse version is too old." << std::endl;
			return false;
		}

		SetName("po3_WashThatBloodOff");
		SetVersion(2.0);

		return true;
	}

	virtual bool OnLoad() override
	{
		SKSEPlugin::OnLoad();

		ReadINI();

		const SKSEMessagingInterface *message = GetInterface(SKSEMessagingInterface::Version_2);
		if (message)
		{
			message->RegisterListener("SKSE", OnInit);
		}

		task = GetInterface(SKSETaskInterface::Version_2);

		_MESSAGE("");

		return true;
	}

	virtual void OnModLoaded() override
	{

	}
} thePlugin;