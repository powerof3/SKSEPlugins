#pragma once

#include "main.h"
#include <mutex>
#include <set>

class DetectionInterceptor // Make actors impossible to detect, or make them incapable of detecting anyone else
{
public:
	static DetectionInterceptor& GetInstance()
	{
		static DetectionInterceptor instance;
		return instance;
	};
protected:
	//typedef std::set<RefHandle> RefHandleList;
	class RefHandleList : public std::set<RefHandle> {
	public:
		bool has_actor(Actor*) const;
		void edit_actor(RefHandle, bool state);
	};
	//
	RefHandleList forceUnseen;
	RefHandleList forceUnseeing;
	std::recursive_mutex lock;
public:
	void SetActorUnseen(Actor*, bool shouldBlind);
	void SetActorUnseeing(Actor*, bool shouldBeUndetectable);
	bool IsActorUnseen(Actor*);
	bool IsActorUnseeing(Actor*);
	void GetActorStatus(Actor*, bool& outUnseen, bool& outUnseeing);
	bool ShouldCancelDetection(Actor* seeker, Actor* target);
	void Reset();
};

namespace Detection_Hook
{
	void Apply();
};
