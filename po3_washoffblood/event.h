#pragma once

#include "main.h"

class BSAnimationGraphEventHandler : public BSTEventSink<BSAnimationGraphEvent>
{
public:
	using EventResult = EventResult;

	static BSAnimationGraphEventHandler* GetSingleton();
	virtual EventResult ReceiveEvent(BSAnimationGraphEvent* a_event, BSTEventSource<BSAnimationGraphEvent>* a_eventSource) override;

protected:
	BSAnimationGraphEventHandler() = default;
	BSAnimationGraphEventHandler(const BSAnimationGraphEventHandler&) = delete;
	BSAnimationGraphEventHandler(BSAnimationGraphEventHandler&&) = delete;
	virtual ~BSAnimationGraphEventHandler() = default;

	BSAnimationGraphEventHandler& operator=(const BSAnimationGraphEventHandler&) = delete;
	BSAnimationGraphEventHandler& operator=(BSAnimationGraphEventHandler&&) = delete;
};

//--------------------------------------------------------------------------------

class po3_TaskRemoveChildNode : public TaskDelegate
{
public:

	virtual void Run();
	virtual void Dispose();

	po3_TaskRemoveChildNode(NiNode  * parent, NiAVObject * decal, NiAVObject * weapon);

	NiNode * m_parent;
	NiAVObject * m_decal;
	NiAVObject * m_weapon;
};