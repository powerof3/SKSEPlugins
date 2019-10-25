#include "po3_functions.h"

extern const SKSETaskInterface * task;

namespace po3_functions
{
	//-------------------------MISC ------------------------------------------------------

	NiAVObject* GetHeadPartObject(Actor * actor, UInt32 partType)
	{
		BSFaceGenNiNode * faceNode = actor->GetFaceGenNiNode();
		TESNPC * actorBase = DYNAMIC_CAST<TESNPC*>(actor->baseForm);

		if (faceNode && actorBase)
		{
			BGSHeadPart * facePart = actorBase->GetCurrentHeadPartByType(partType);
			if (facePart)
			{
				NiAVObject * headNode = faceNode->GetObjectByName(facePart->partName);
				if (headNode)
				{
					return headNode;
				}
			}
		}

		return nullptr;
	}


	NiGeometry * GetHeadPartGeometry(Actor * actor, UInt32 partType)
	{
		BSFaceGenNiNode * faceNode = actor->GetFaceGenNiNode();
		TESNPC * actorBase = DYNAMIC_CAST<TESNPC*>(actor->baseForm);

		if (faceNode && actorBase)
		{
			BGSHeadPart * facePart = actorBase->GetCurrentHeadPartByType(partType);
			if (facePart)
			{
				NiAVObject * headNode = faceNode->GetObjectByName(facePart->partName);
				if (headNode)
				{
					NiGeometry * geometry = headNode->GetAsNiGeometry();
					if (geometry)
					{
						return geometry;
					}
				}
			}
		}

		return nullptr;
	}

	NiAVObject * VisitArmorAddon(Actor * actor, TESObjectARMO * armor, TESObjectARMA * arma)
	{
		char addonString[MAX_PATH];
		memset(addonString, 0, MAX_PATH);
		arma->GetNodeName(addonString, actor, armor, -1);

		BSFixedString rootName("NPC Root [Root]");

		NiNode * skeletonRoot[2];
		skeletonRoot[0] = actor->GetNiRootNode(0);
		skeletonRoot[1] = actor->GetNiRootNode(1);

		// Skip second skeleton, it's the same as the first
		if (skeletonRoot[1] == skeletonRoot[0])
		{
			skeletonRoot[1] = nullptr;
		}

		for (UInt32 i = 0; i <= 1; i++)
		{
			if (skeletonRoot[i])
			{
				NiAVObject * root = skeletonRoot[i]->GetObjectByName(rootName);
				if (root)
				{
					NiNode * rootNode = root->GetAsNiNode();
					if (rootNode)
					{
						BSFixedString addonName(addonString); // Find the Armor name from the root
						NiAVObject * armorNode = skeletonRoot[i]->GetObjectByName(addonName);

						if (armorNode)
						{
							return armorNode;
						}
					}
				}
			}
		}

		return nullptr;
	}

	SInt32 GetShaderPropertyType(NiGeometry * geometry)
	{
		if (!geometry)
		{
			return -1;
		}

		BSShaderProperty * shaderProperty = niptr_cast<BSShaderProperty>(geometry->m_spEffectState);
		if (!shaderProperty)
		{
			return -1;
		}

		BSLightingShaderProperty * lightingShader = ni_cast(shaderProperty, BSLightingShaderProperty);
		if (lightingShader)
		{
			BSLightingShaderMaterial * material = (BSLightingShaderMaterial *)lightingShader->material;

			if (material)
			{
				return material->GetShaderType();
			}
		}

		return -1;
	}

	//get worn form
	TESObjectARMO * GetWornForm(Actor* thisActor, UInt32 mask)
	{
		ExtraContainerChanges* exChanges = static_cast<ExtraContainerChanges*>(thisActor->extraData.GetByType(ExtraDataType::ContainerChanges)); //loop through caster inventory
		InventoryChanges *changes = exChanges ? exChanges->changes : nullptr;
		ExtraWorn* worn = 0;
		ExtraWornLeft* wornLeft = 0;

		if (changes && changes->entryList)
		{
			for (auto& data : *changes->entryList)
			{
				if (!(data->baseForm)->IsArmor())
				{
					continue;
				}

				if (!data->extraList)
				{
					continue;
				}

				for (auto& extraList : *data->extraList)
				{
					worn = static_cast<ExtraWorn*>(extraList->GetByType(ExtraDataType::Worn));
					wornLeft = static_cast<ExtraWornLeft*>(extraList->GetByType(ExtraDataType::WornLeft));

					if (worn || wornLeft)
					{
						auto armor = DYNAMIC_CAST<TESObjectARMO*>(data->baseForm);

						for (auto &armorAddon : DYNAMIC_CAST<TESObjectARMO*>(data->baseForm)->armorAddons)
						{
							if (armorAddon->HasPartOf(mask))
							{
								return armor;
							}
						}
					}
				}
			}
		}

		return nullptr;
	}

	//get skin form
	TESObjectARMO* GetSkinForm(Actor* thisActor, UInt32 mask)
	{
		TESObjectARMO* equipped = nullptr;

		if (thisActor)
		{
			equipped = GetWornForm(thisActor, mask);

			if (!equipped)
			{
				TESNPC * actorBase = DYNAMIC_CAST<TESNPC*>(thisActor->baseForm);

				if (actorBase)
				{
					equipped = actorBase->skin; // Check ActorBase
				}

				TESRace * actorRace = thisActor->race;

				if (!equipped && actorRace)
				{
					equipped = actorRace->skin;
				}

				TESRace * race = actorBase->race;

				if (!equipped && race)
				{
					equipped = race->skin; // Check  actorbase race
				}
			}
		}

		return equipped;
	}

	//get armor addon with mask
	TESObjectARMA * GetArmorAddonByMask(TESRace * race, TESObjectARMO * armor, UInt32 mask)
	{
		for (auto &currentAddon : armor->armorAddons)
		{
			if (currentAddon && currentAddon->IsValidRace(race) && (currentAddon->GetSlotMask() & mask) == mask)
			{
				return currentAddon;
			}
		}
		return nullptr;
	}

	//-------------------------------------------------------------------------------------

	void SetShaderPropertyAlpha(NiGeometry * geometry, float alpha, bool onlySkin)
	{
		if (!geometry)
		{
			return;
		}

		BSShaderProperty * shaderProperty = niptr_cast<BSShaderProperty>(geometry->m_spEffectState);
		if (!shaderProperty)
		{
			return;
		}

		BSLightingShaderProperty * lightingShader = ni_cast(shaderProperty, BSLightingShaderProperty);
		if (lightingShader)
		{
			BSLightingShaderMaterial * material = (BSLightingShaderMaterial *)lightingShader->material;

			if (material)
			{
				if (onlySkin)
				{
					if ((material->GetShaderType() == BSShaderMaterial::kShaderType_FaceGenRGBTint) || (material->GetShaderType() == BSShaderMaterial::kShaderType_FaceGen))
					{
						material->alpha = alpha;
					}
				}
				else
				{
					material->alpha = alpha;
				}
			}
		}
	}

	void SetArmorSkinAlpha(Actor * thisActor, UInt32 slotMask, float alpha)
	{
		TESObjectARMO * skinarmor = GetSkinForm(thisActor, slotMask);

		if (!skinarmor)
		{
			return;
		}

		TESObjectARMA * foundAddon = GetArmorAddonByMask(thisActor->race, skinarmor, slotMask);

		if (foundAddon)
		{
			NiAVObject * armorNode = VisitArmorAddon(thisActor, skinarmor, foundAddon);

			if (armorNode)
			{
				NiNode * node = armorNode->GetAsNiNode();

				if (node)
				{
					for (UInt32 i = 0; i < node->GetArrayCount(); i++)
					{
						NiAVObject * object = node->GetAt(i);

						if (object)
						{
							SetShaderPropertyAlpha(object->GetAsNiGeometry(), alpha, true);
						}
					}
				}
				else
				{
					SetShaderPropertyAlpha(armorNode->GetAsNiGeometry(), alpha, true);
				}
			}
		}
	}

	//-------------------------------------------------------------------------------------

	void SetSkinAlpha(Actor * thisActor, float alpha)
	{
		task->AddTask([thisActor, alpha]()
		{
			NiGeometry * geometry = GetHeadPartGeometry(thisActor, BGSHeadPart::kTypeFace);
			SetShaderPropertyAlpha(geometry, alpha, true);

			SetArmorSkinAlpha(thisActor, BGSBipedObjectForm::kPart_Body, alpha);
			SetArmorSkinAlpha(thisActor, BGSBipedObjectForm::kPart_Hands, alpha);
			SetArmorSkinAlpha(thisActor, BGSBipedObjectForm::kPart_Feet, alpha);
			SetArmorSkinAlpha(thisActor, BGSBipedObjectForm::kPart_Tail, alpha); //tail
			SetArmorSkinAlpha(thisActor, BGSBipedObjectForm::kPart_Unnamed21, alpha); //decap
		});
	}

	//--------------------------------------------------------------------------------------

	void DisableHeadPart(Actor * thisActor)
	{
		BSFaceGenNiNode * faceNode = thisActor->GetFaceGenNiNode();
		NiNode * parent = faceNode->m_parent;

		if (faceNode && parent)
		{
			parent->RemoveChild(faceNode);
		}
	}
}

