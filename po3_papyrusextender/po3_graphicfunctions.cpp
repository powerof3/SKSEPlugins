#include "po3_graphicfunctions.h"

//--------------------------------------------------------------------------------------------

extern const SKSETaskInterface* g_task;

//-----------------------MAIN--------------------------------------------------------

void SetShaderPropertyRGBTint(NiGeometry* geometry)
{
	if (!geometry)
	{
		return;
	}

	auto shaderProperty = niptr_cast<BSShaderProperty>(geometry->m_spEffectState);
	if (!shaderProperty)
	{
		return;
	}

	auto lightingShader = ni_cast(shaderProperty, BSLightingShaderProperty);
	if (lightingShader)
	{
		auto material = lightingShader->material;

		if (material && material->GetShaderType() == BSShaderMaterial::kShaderType_FaceGen)
		{
			auto tintedMaterial = static_cast<BSTintedShaderMaterial*>(CreateShaderMaterial(BSShaderMaterial::kShaderType_FaceGenRGBTint));
			tintedMaterial->CopyFrom(material);
			lightingShader->SetFlags(0x0A, false);
			lightingShader->SetFlags(0x15, true);
			lightingShader->SetMaterial(tintedMaterial, 1);
			lightingShader->InitializeShader(geometry);
		}
	}
}

void SetShaderPropertyAlpha(NiGeometry* geometry, float alpha, bool onlySkin)
{
	if (!geometry)
	{
		return;
	}

	auto shaderProperty = niptr_cast<BSShaderProperty>(geometry->m_spEffectState);
	if (!shaderProperty)
	{
		return;
	}

	auto lightingShader = ni_cast(shaderProperty, BSLightingShaderProperty);
	if (lightingShader)
	{
		auto material = lightingShader->material;

		if (material)
		{
			if (onlySkin)
			{
				if (material->GetShaderType() == BSShaderMaterial::kShaderType_FaceGenRGBTint || material->GetShaderType() == BSShaderMaterial::kShaderType_FaceGen)
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

void SetArmorSkinAlpha(Actor* thisActor, UInt32 slotMask, float alpha)
{
	TESObjectARMO* skinarmor = GetSkinForm(thisActor, slotMask);

	if (!skinarmor)
	{
		return;
	}

	TESObjectARMA* foundAddon = GetArmorAddonByMask(thisActor->race, skinarmor, slotMask);

	if (foundAddon)
	{
		NiAVObject* armorNode = VisitArmorAddon(thisActor, skinarmor, foundAddon);

		if (armorNode)
		{
			auto node = armorNode->GetAsNiNode();

			if (node)
			{
				for (UInt32 i = 0; i < node->GetArrayCount(); i++)
				{
					auto object = node->GetAt(i);

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

void ReplaceTextureSet(NiGeometry* geometry, BGSTextureSet* sourceTXST, BGSTextureSet* targetTXST, SInt32 textureType)
{
	if (!geometry)
	{
		return;
	}

	auto shaderProperty = niptr_cast<BSShaderProperty>(geometry->m_spEffectState);
	if (!shaderProperty)
	{
		return;
	}

	auto lightingShader = ni_cast(shaderProperty, BSLightingShaderProperty);
	if (lightingShader)
	{
		auto material = lightingShader->material;

		if (material)
		{
			std::string sourcePath = material->textureSet->GetTexturePath(0);
			std::string targetPath = sourceTXST->GetTexturePath(0);

			//making everything lowercase
			std::transform(sourcePath.begin(), sourcePath.end(), sourcePath.begin(), ::tolower);
			std::transform(targetPath.begin(), targetPath.end(), targetPath.begin(), ::tolower);

			//CK texturesets start without "textures\" path while vanilla nifs always start with it.	
			size_t data_pos = sourcePath.find(R"(data\textures\)", 0, 14);
			size_t txt_pos = sourcePath.find(R"(textures\)", 0, 9);

			if (data_pos != std::string::npos)
			{
				sourcePath.erase(data_pos, 14); //removing "data\textures\"
			}
			else if (txt_pos != std::string::npos)
			{
				sourcePath.erase(txt_pos, 9); //removing "textures\"
			}

			if (sourcePath != targetPath)
			{
				return;
			}

			if (textureType == -1)
			{
				material->ReleaseTextures();
				material->SetTextureSet(targetTXST);
			}
			else
			{
				if (textureType < BSTextureSet::kNumTextures)
				{
					BSShaderTextureSet* newTextureSet = BSShaderTextureSet::Create();

					for (UInt32 i = 0; i < BSTextureSet::kNumTextures; i++)
					{
						const char* texturePath = material->textureSet->GetTexturePath(i);
						newTextureSet->SetTexturePath(i, texturePath);
					}

					newTextureSet->SetTexturePath(textureType, targetTXST->GetTexturePath(textureType));
					material->ReleaseTextures();
					material->SetTextureSet(newTextureSet);
				}
			}

			lightingShader->InvalidateTextures(0);
			lightingShader->InitializeShader(geometry);
		}
	}
}

void ReplaceSkinTXST(NiGeometry* geometry, BGSTextureSet* TXST, SInt32 textureType)
{
	if (!geometry)
	{
		return;
	}

	auto shaderProperty = niptr_cast<BSShaderProperty>(geometry->m_spEffectState);
	if (!shaderProperty)
	{
		return;
	}

	auto lightingShader = ni_cast(shaderProperty, BSLightingShaderProperty);
	if (lightingShader)
	{
		auto material = lightingShader->material;

		if (material && (material->GetShaderType() == BSShaderMaterial::kShaderType_FaceGenRGBTint || material->GetShaderType() == BSShaderMaterial::kShaderType_FaceGen))
		{
			if (textureType == -1)
			{
				material->ReleaseTextures();
				material->SetTextureSet(TXST);
			}
			else
			{
				if (textureType < BSTextureSet::kNumTextures)
				{
					BSShaderTextureSet* newTextureSet = BSShaderTextureSet::Create();

					for (UInt32 i = 0; i < BSTextureSet::kNumTextures; i++)
					{
						const char* texturePath = material->textureSet->GetTexturePath(i);
						newTextureSet->SetTexturePath(i, texturePath);
					}

					newTextureSet->SetTexturePath(textureType, TXST->GetTexturePath(textureType));
					material->ReleaseTextures();
					material->SetTextureSet(newTextureSet);
				}
			}

			lightingShader->InvalidateTextures(0);
			lightingShader->InitializeShader(geometry);
		}
	}
}

void SetArmorSkinTXST(Actor* thisActor, BGSTextureSet* TXST, UInt32 slotMask, SInt32 textureType)
{
	TESObjectARMO* skinarmor = GetSkinForm(thisActor, slotMask);

	if (!skinarmor)
	{
		return;
	}

	g_task->AddTask([thisActor, skinarmor, TXST, slotMask, textureType]()
	{
		TESObjectARMA* foundAddon = GetArmorAddonByMask(thisActor->race, skinarmor, slotMask);

		if (foundAddon)
		{
			auto armorNode = VisitArmorAddon(thisActor, skinarmor, foundAddon);

			if (armorNode)
			{
				auto node = armorNode->GetAsNiNode();

				if (node)
				{
					for (UInt32 i = 0; i < node->GetArrayCount(); i++)
					{
						NiAVObject* object = node->GetAt(i);

						if (object)
						{
							ReplaceSkinTXST(object->GetAsNiGeometry(), TXST, textureType);
						}
					}
				}
				else
				{
					ReplaceSkinTXST(armorNode->GetAsNiGeometry(), TXST, textureType);
				}
			}
		}
	});
}

//--------------------------------------------------------------------------------------

void SetShaderPropertyMLP(NiGeometry* geometry, NiGeometry* templateGeometry)
{
	if (!geometry || !templateGeometry)
	{
		return;
	}

	auto shaderProperty = niptr_cast<BSShaderProperty>(geometry->m_spEffectState);
	auto templateShaderProperty = niptr_cast<BSShaderProperty>(templateGeometry->m_spEffectState);

	if (!shaderProperty || !templateShaderProperty)
	{
		return;
	}

	auto lightingShader = ni_cast(shaderProperty, BSLightingShaderProperty);
	auto templateLightingShader = ni_cast(templateShaderProperty, BSLightingShaderProperty);

	if (lightingShader && templateLightingShader)
	{
		auto material = lightingShader->material;
		auto templateMaterial = static_cast<BSLightingShaderMaterialMultiLayerParallax*>(templateLightingShader->material);

		if (material && templateMaterial && (material->GetShaderType() == BSShaderMaterial::kShaderType_FaceGen || material->GetShaderType() == BSShaderMaterial::kShaderType_FaceGenRGBTint))
		{
			auto newMaterial = static_cast<BSLightingShaderMaterialMultiLayerParallax*>(CreateShaderMaterial(BSShaderMaterial::kShaderType_MultilayerParallax));

			newMaterial->CopyFrom(templateMaterial);

			newMaterial->fEnvmapScale = templateMaterial->fEnvmapScale;
			newMaterial->fParallaxInnerLayerUScale = templateMaterial->fParallaxInnerLayerUScale;
			newMaterial->fParallaxInnerLayerVScale = templateMaterial->fParallaxInnerLayerVScale;
			newMaterial->fParallaxLayerThickness = templateMaterial->fParallaxLayerThickness;
			newMaterial->fParallaxRefractionScale = templateMaterial->fParallaxRefractionScale;

			if (material->GetShaderType() == BSShaderMaterial::kShaderType_FaceGen)
			{
				lightingShader->SetFlags(0x0A, false); //disable facegen

			}
			else
			{
				lightingShader->SetFlags(0x15, false); //rgb tint
			}

			lightingShader->SetFlags(0x25, false); //vertex colors
			lightingShader->SetFlags(0x2F, false); //env_light fade
			lightingShader->SetFlags(0x39, false); //soft lighting

			lightingShader->SetFlags(0x38, true); //mlp
			lightingShader->SetFlags(0x3B, true); //backlighting

			BSShaderTextureSet* newTextureSet = BSShaderTextureSet::Create();
			for (UInt32 i = 0; i < BSTextureSet::kNumTextures; i++)
			{
				const char* texturePath = templateMaterial->textureSet->GetTexturePath(i);
				newTextureSet->SetTexturePath(i, texturePath);
			}
			newTextureSet->SetTexturePath(1, material->textureSet->GetTexturePath(1));

			lightingShader->SetMaterial(newMaterial, 1);
			lightingShader->material->ReleaseTextures();
			lightingShader->material->SetTextureSet(newTextureSet);

			lightingShader->InvalidateTextures(0);
			lightingShader->InitializeShader(geometry);
		}
	}
}

NiGeometry* GetArmorGeometry(Actor* thisActor, UInt32 slotMask, SInt32 shaderType)
{
	TESObjectARMO* skinarmor = GetSkinForm(thisActor, slotMask);

	if (!skinarmor)
	{
		return nullptr;
	}

	TESObjectARMA* foundAddon = GetArmorAddonByMask(thisActor->race, skinarmor, slotMask);

	if (foundAddon)
	{
		NiAVObject* armorNode = VisitArmorAddon(thisActor, skinarmor, foundAddon);

		if (armorNode)
		{
			NiNode* node = armorNode->GetAsNiNode();

			if (node)
			{
				for (UInt32 i = 0; i < node->GetArrayCount(); i++)
				{
					NiAVObject* object = node->GetAt(i);

					if (object && GetShaderPropertyType(object->GetAsNiGeometry()) == shaderType)
					{
						return object->GetAsNiGeometry();
					}
				}
			}
			else
			{
				if (GetShaderPropertyType(armorNode->GetAsNiGeometry()) == shaderType)
				{
					return armorNode->GetAsNiGeometry();
				}
			}
		}
	}

	return nullptr;
}

//-------------------------MISC ---------------------------------------------------------

NiGeometry* GetHeadPartGeometry(Actor* actor, UInt32 partType)
{
	BSFaceGenNiNode* faceNode = actor->GetFaceGenNiNode();
	auto actorBase = actor->GetActorBase();

	if (faceNode && actorBase)
	{
		BGSHeadPart* facePart = actorBase->GetCurrentHeadPartByType(partType);
		if (facePart)
		{
			NiAVObject* headNode = faceNode->GetObjectByName(facePart->partName);
			if (headNode)
			{
				NiGeometry* geometry = headNode->GetAsNiGeometry();
				if (geometry)
				{
					return geometry;
				}
			}
		}
	}

	return nullptr;
}

NiAVObject* VisitArmorAddon(Actor* actor, TESObjectARMO* armor, TESObjectARMA* arma)
{
	char addonString[MAX_PATH];
	memset(addonString, 0, MAX_PATH);
	arma->GetNodeName(addonString, actor, armor, -1);

	BSFixedString rootName("NPC Root [Root]");

	NiNode* skeletonRoot[2];
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
			NiAVObject* root = skeletonRoot[i]->GetObjectByName(rootName);
			if (root)
			{
				NiNode* rootNode = root->GetAsNiNode();
				if (rootNode)
				{
					BSFixedString addonName(addonString); // Find the Armor name from the root
					NiAVObject* armorNode = skeletonRoot[i]->GetObjectByName(addonName);

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

SInt32 GetShaderPropertyType(NiGeometry* geometry)
{
	if (!geometry)
	{
		return -1;
	}

	auto shaderProperty = niptr_cast<BSShaderProperty>(geometry->m_spEffectState);
	if (!shaderProperty)
	{
		return -1;
	}

	auto lightingShader = ni_cast(shaderProperty, BSLightingShaderProperty);
	if (lightingShader)
	{
		auto material = lightingShader->material;

		if (material)
		{
			return material->GetShaderType();
		}
	}

	return -1;
}

UInt32 GetShaderPropertyModdedSkin(NiGeometry* geometry, bool isBodyGeometry)
{
	if (!geometry)
	{
		return 0;
	}

	auto shaderProperty = niptr_cast<BSShaderProperty>(geometry->m_spEffectState);
	if (!shaderProperty)
	{
		return 0;
	}

	auto lightingShader = ni_cast(shaderProperty, BSLightingShaderProperty);
	if (lightingShader)
	{
		auto material = lightingShader->material;

		if (material)
		{
			auto alpha = material->alpha;

			if (isBodyGeometry)
			{
				if (alpha != 1.0)
				{
					return 1;
				}

				return 0;
			}

			auto shaderType = material->GetShaderType();

			if (alpha != 1.0)
			{
				if (shaderType == BSShaderMaterial::kShaderType_FaceGenRGBTint)
				{
					return 3;
				}

				return 1;
			}
			
			if (shaderType == BSShaderMaterial::kShaderType_FaceGenRGBTint)
			{
				return 2;
			}
		}
	}

	return 0;
}

//color mixing algorithm
UInt8 colorMix(UInt8 a, UInt8 b, float t) //t is percentage blend
{
	return round(sqrt((1 - t) * pow(b, 2) + t * pow(a, 2)));
}

//luminance detection algorithm
float calculateLuminance(UInt8 R, UInt8 G, UInt8 B) //returns luminance between 0-1.0
{
	return (0.2126 * R + 0.7152 * G + 0.0722 * B) / 255;
}

//get worn form
TESObjectARMO* GetWornForm(Actor* thisActor, UInt32 mask)
{
	auto exChanges = static_cast<ExtraContainerChanges*>(thisActor->extraData.GetByType(ExtraDataType::ContainerChanges)); //loop through caster inventory
	InventoryChanges* changes = exChanges ? exChanges->changes : nullptr;
	ExtraWorn* worn = nullptr;
	ExtraWornLeft* wornLeft = nullptr;

	if (changes && changes->entryList)
	{
		for (auto& data : *changes->entryList)
		{
			if (!data)
			{
				continue;
			}

			if (!data->baseForm)
			{
				continue;
			}

			if (!data->baseForm->IsArmor())
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

					if (armor)
					{
						for (auto& armorAddon : armor->armorAddons)
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
			auto actorBase = thisActor->GetActorBase();

			if (actorBase)
			{
				equipped = actorBase->skin; // Check ActorBase

				TESRace* race = actorBase->race;

				if (!equipped && race)
				{
					equipped = race->skin; // Check Race
				}
			}
		}
	}

	return equipped;
}

//get armor addon with mask
TESObjectARMA* GetArmorAddonByMask(TESRace* race, TESObjectARMO* armor, UInt32 mask)
{
	for (auto& currentAddon : armor->armorAddons)
	{
		if (currentAddon && currentAddon->IsValidRace(race) && (currentAddon->GetSlotMask() & mask) == mask)
		{
			return currentAddon;
		}
	}
	return nullptr;
}