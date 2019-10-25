#include "event.h"

//-----------------------------------------------------------------------
extern const SKSETaskInterface * task;
//-----------------------------------------------------------------------

BSAnimationGraphEventHandler* BSAnimationGraphEventHandler::GetSingleton()
{
	static BSAnimationGraphEventHandler singleton;
	return &singleton;
}

auto BSAnimationGraphEventHandler::ReceiveEvent(BSAnimationGraphEvent* a_event, BSTEventSource<BSAnimationGraphEvent>* a_eventSource)
-> EventResult
{
	if (!a_event || a_event->animName != "SoundPlay.FSTSwimSwim" || !a_event->akTarget)
	{
		return kEvent_Continue;
	}

	Actor * thisActor = DYNAMIC_CAST<Actor*>(a_event->akTarget);

	if (!thisActor)
	{
		return kEvent_Continue;
	}

	NiNode * parent = thisActor->GetNiNode();

	if (parent)
	{
		BSFixedString decalName("Skinned Decal Node");
		NiAVObject * skinnedDecal = parent->GetObjectByName(decalName);

		NiAVObject * weapon = nullptr;	
		BSFixedString weaponName("weapon");
		weapon = parent->GetObjectByName(weaponName);

		if (!weapon)
		{
			BSFixedString swordName("weaponSword");
			weapon = parent->GetObjectByName(swordName);
		}
		if (!weapon)
		{
			BSFixedString daggerName("weaponDagger");
			weapon = parent->GetObjectByName(daggerName);
		}
		if (!weapon)
		{
			BSFixedString axeName("weaponAxe");
			weapon = parent->GetObjectByName(axeName);
		}
		if (!weapon)
		{
			BSFixedString maceName("weaponMace");
			weapon = parent->GetObjectByName(maceName);
		}
		if (!weapon)
		{
			BSFixedString backName("weaponBack");
			weapon = parent->GetObjectByName(backName);
		}

		task->AddTask(new po3_TaskRemoveChildNode(parent, skinnedDecal, weapon));
	}

	return kEvent_Continue;
}

//--------------------------------------------------------------------------------

bool ShaderHasFlag(NiGeometry * geometry, UInt8 flag)
{
	if (!geometry)
	{
		return false;
	}

	BSShaderProperty * shaderProperty = niptr_cast<BSShaderProperty>(geometry->m_spEffectState);

	if (!shaderProperty)
	{
		return false;
	}

	BSLightingShaderProperty * lightingShader = ni_cast(shaderProperty, BSLightingShaderProperty);

	if (lightingShader && lightingShader->HasFlags(flag))
	{
		return true;
	}

	return false;
}

//--------------------------------------------------------------------------------

po3_TaskRemoveChildNode::po3_TaskRemoveChildNode(NiNode * parent, NiAVObject * decal, NiAVObject * weapon)
{
	m_parent = parent;
	m_decal = decal;
	m_weapon = weapon;

	if (m_parent)
	{
		m_parent->IncRefCount();
	}

	if (m_decal)
	{
		m_decal->IncRefCount();
	}

	if (m_weapon)
	{
		m_weapon->IncRefCount();
	}
}

void po3_TaskRemoveChildNode::Run()
{
	if (m_decal)
	{
		m_parent->RemoveChild(m_decal);
	}
}

void po3_TaskRemoveChildNode::Dispose()
{
	if (m_parent)
	{
		m_parent->DecRefCount();
	}

	if (m_decal)
	{
		m_decal->DecRefCount();
	}

	if (m_weapon)
	{
		m_weapon->DecRefCount();
	}

	delete this;
}