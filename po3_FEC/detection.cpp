#include "Detection.h"
#include "main.h"

bool DetectionInterceptor::RefHandleList::has_actor(Actor* subject) const
{
	RefHandle handle;
	handle = subject->CreateRefHandle(handle);
	if (handle != g_invalidRefHandle)
	{
		return this->find(handle) != this->end();
	}
	return false;
};
void DetectionInterceptor::RefHandleList::edit_actor(RefHandle handle, bool state)
{
	if (handle) 
	{
		if (state)
		{
			this->insert(handle);
		}
		else
		{
			auto found = this->find(handle);
			if (found != this->end())
			{
				this->erase(found);
			}
		}
	}
}
void DetectionInterceptor::SetActorUnseen(Actor* subject, bool state)
{
	std::lock_guard<std::recursive_mutex> scoped_lock(this->lock);

	RefHandle handle = g_invalidRefHandle;
	handle = subject->CreateRefHandle(handle);
	this->forceUnseen.edit_actor(handle, state);
	_MESSAGE("%s is now undetectable.", subject->GetFullName());
}
void DetectionInterceptor::SetActorUnseeing(Actor* subject, bool state)
{
	std::lock_guard<std::recursive_mutex> scoped_lock(this->lock);
	//
	RefHandle handle = g_invalidRefHandle;
	handle = subject->CreateRefHandle(handle);
	this->forceUnseeing.edit_actor(handle, state);
	_MESSAGE("Attempted to blind actor %08X.", subject->formID);
}
bool DetectionInterceptor::IsActorUnseen(Actor* subject) 
{
	std::lock_guard<std::recursive_mutex> scoped_lock(this->lock);
	return this->forceUnseen.has_actor(subject);
};
bool DetectionInterceptor::IsActorUnseeing(Actor* subject)
{
	std::lock_guard<std::recursive_mutex> scoped_lock(this->lock);
	return this->forceUnseeing.has_actor(subject);
};
void DetectionInterceptor::GetActorStatus(Actor* subject, bool& outUnseen, bool& outUnseeing) 
{
	std::lock_guard<std::recursive_mutex> scoped_lock(this->lock);
	outUnseen = this->forceUnseen.has_actor(subject);
	outUnseeing = this->forceUnseeing.has_actor(subject);
}
__declspec(noinline) bool DetectionInterceptor::ShouldCancelDetection(Actor* seeker, Actor* target)
{
	std::lock_guard<std::recursive_mutex> scoped_lock(this->lock);
	if (this->forceUnseeing.has_actor(seeker))
	{
		return true;
	}
	if (this->forceUnseen.has_actor(target))
	{
		return true;
	}
	return false;
}
__declspec(noinline) void DetectionInterceptor::Reset() {
	std::lock_guard<std::recursive_mutex> scoped_lock(this->lock);
	this->forceUnseeing.clear();
	this->forceUnseen.clear();
}

namespace Detection_Hook
{
	bool Inner(Actor* seeker, Actor* target)
	{
		return DetectionInterceptor::GetInstance().ShouldCancelDetection(seeker, target);
	};
	__declspec(naked) void Outer()
	{
		_asm
		{
			mov  edx, dword ptr[ebp + 0x8]; // edx = Arg1;
			mov  eax, dword ptr[ebp + 0xC]; // eax = Arg2;
			push eax;
			push edx;
			call Inner;
			add  esp, 8;
			test al, al;
			jnz  lAbort;
			mov  edx, dword ptr[ebp + 0x8]
			mov  eax, 0x01310588;
			mov  eax, dword ptr[eax];
			mov  ecx, 0x0059898E;
			jmp  ecx;
		lAbort:
			mov  eax, -0x3E8; // -1000
			mov  ecx, 0x00598DA6;
			jmp  ecx;
		};
	};
	void Apply()
	{
		WriteRelJump(0x00598989, reinterpret_cast<UInt32>(&Outer)); //0x00598979
	};
};
