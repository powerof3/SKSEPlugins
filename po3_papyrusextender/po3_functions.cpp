//thanks to underthesky, davidjcobb, aers, nukem, meh321, expired, ryan for tips and help
//modifications and extra functions by powerofthree
//himika for libSkyrim

#define NOMINMAX
#include "po3_functions.h"
#include "po3_graphicfunctions.h"
#include "po3_offsets.h"

//--------------------------------------------------------------------------------------------

extern const SKSETaskInterface* g_task;

extern BGSKeyword* keyword;
extern BGSArtObject* art;

//--------------------------------------------------------------------------------------------

typedef UInt32(*_UpdateModelSkin)(NiNode*, NiColorA**);
const static _UpdateModelSkin UpdateModelSkin = (_UpdateModelSkin)0x005A9810; // Applies tint to ShaderType 5 nodes

typedef UInt32(*_UpdateModelHair)(NiNode*, NiColorA**);
const static _UpdateModelHair UpdateModelHair = (_UpdateModelHair)0x005A9890; // Applies tint to ShaderType 6 nodes

//--------------------------------------------------------------------------------------------

TESSoulGem::SoulLevel GetSoulLevel(TESSoulGem* a_soulGem, InventoryEntryData* entry)
{
	if (a_soulGem->soulSize != TESSoulGem::SoulLevel::kNone)
	{
		return a_soulGem->soulSize;
	}
	else
	{
		if (entry->extraList)
		{
			for (auto& list : *entry->extraList)
			{
				auto xSoul = static_cast<ExtraSoul*>(list->GetByType(ExtraDataType::Soul));

				if (xSoul)
				{
					return static_cast<TESSoulGem::SoulLevel>(xSoul->count);
				}
			}
		}
	}

	return TESSoulGem::SoulLevel::kNone;
}

bool VerifyKeywords(TESForm* form, VMArray<BGSKeyword*>* keywords)
{
	if (form && keywords->GetSize() > 0)
	{
		auto pKeywords = DYNAMIC_CAST<BGSKeywordForm*>(form);

		if (pKeywords)
		{
			BGSKeyword* keyword = nullptr;

			for (UInt32 k = 0; k < keywords->GetSize(); k++)
			{
				keywords->GetAt(k, keyword);

				if (keyword && pKeywords->HasKeyword(keyword))
				{
					return true;
				}
			}
		}
	}

	return false;
}

// navmesh related functions
float CalcLinearDistance(const NiPoint3& a_lhs, const NiPoint3& a_rhs)
{
	return (((a_rhs.x - a_lhs.x) * (a_rhs.x - a_lhs.x)) + ((a_rhs.y - a_lhs.y) * (a_rhs.y - a_lhs.y)) + ((a_rhs.z - a_lhs.z) * (a_rhs.z - a_lhs.z)));
}

void SetPosition(TESObjectREFR* target, NiPoint3 pos)
{
	MoveRefrToPosition(target, g_invalidRefHandle, target->GetParentCell(), target->GetWorldSpace(), pos, target->rot);
}

std::optional<NiPoint3> FindNearestVertex(TESObjectREFR* target)
{
	auto cell = target->GetParentCell();
	if (!cell || !cell->navMeshes)
	{
		return std::nullopt;
	}
	auto& navMeshes = *cell->navMeshes;

	auto shortestDistance = std::numeric_limits<float>::max();
	std::optional<NiPoint3> pos = std::nullopt;

	for (auto& navMesh : navMeshes)
	{
		if (!navMesh->kFlagIsDeleted)
		{
			continue;
		}

		for (auto& vertex : navMesh->vertices)
		{
			auto linearDistance = CalcLinearDistance(target->pos, vertex);
			if (linearDistance < shortestDistance)
			{
				shortestDistance = linearDistance;
				pos.emplace(vertex);
			}
		}
	}

	return pos;
}

// time
std::vector<SInt32> GetGameStartDate()
{
	std::vector<SInt32> vec;
	vec.reserve(3);

	auto singleton = GameTime::GetSingleton();

	TESGlobal* g_gameDay = singleton->gameDay;
	TESGlobal* g_gameMonth = singleton->gameMonth;
	TESGlobal* g_gameYear = singleton->gameYear;
	TESGlobal* g_daysElapsed = singleton->gameDaysPassed;

	if (g_gameDay && g_gameMonth && g_gameYear && g_daysElapsed)
	{
		SInt32 currentDay = g_gameDay->value;
		SInt32 currentMonth = g_gameMonth->value;
		SInt32 currentYear = g_gameYear->value;
		float  daysElapsed = g_daysElapsed->value;

		SInt32 firstYear = currentYear - (SInt32)(daysElapsed / 365);
		SInt32 firstMonth = currentMonth - (SInt32)((fmodf(daysElapsed, 365) / singleton->DAYS_IN_MONTH[currentMonth]));

		SInt32 dayOffset = fmodf(fmodf(daysElapsed, 365), 30) / 1;
		SInt32 firstDay = currentDay - dayOffset;
		if (firstDay < 0)
		{
			firstDay += singleton->DAYS_IN_MONTH[firstMonth];
		}

		vec.push_back(firstYear);
		vec.push_back(firstMonth);
		vec.push_back(firstDay);
	}
	else
	{
		vec.push_back(201);
		vec.push_back(8);
		vec.push_back(17);
	}

	return vec;
}

//--------------------------------------------------------------------------------------------
// ACTOR
//--------------------------------------------------------------------------------------------

void PO3_SKSEFunctions::GetHairColor(Actor* thisActor, BGSColorForm* color)
{
	if (!thisActor || !thisActor->Is3DLoaded() || !color)
	{
		return;
	}

	g_task->AddTask([thisActor, color]()
	{
		NiGeometry* geometry = GetHeadPartGeometry(thisActor, BGSHeadPart::kTypeHair);

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
			auto material = (BSLightingShaderMaterial*)lightingShader->material;

			if (material && material->GetShaderType() == BSShaderMaterial::kShaderType_HairTint)
			{
				auto tintedMaterial = (BSTintedShaderMaterial*)material;
				NiColor tintColor = tintedMaterial->tintColor;

				color->color.red = tintColor.r * 255;
				color->color.green = tintColor.g * 255;
				color->color.blue = tintColor.b * 255;
			}
		}
	});
}

void PO3_SKSEFunctions::SetHairColor(Actor* thisActor, BGSColorForm* color)
{
	if (!thisActor || !thisActor->Is3DLoaded())
	{
		return;
	}

	NiColorA val;
	val.r = color->color.red / 255.0;
	val.g = color->color.green / 255.0;
	val.b = color->color.blue / 255.0;
	NiColorA* haircolor = &val;

	NiNode* model = thisActor->GetNiRootNode(0);

	if (model)
	{
		UpdateModelHair(model, &haircolor); //sets hair color
	}
}

void PO3_SKSEFunctions::GetSkinColor(Actor* thisActor, BGSColorForm* color)
{
	if (!thisActor || !thisActor->Is3DLoaded() || !color)
	{
		return;
	}

	g_task->AddTask([thisActor, color]()
	{
		NiGeometry* geometry = GetArmorGeometry(thisActor, BGSBipedObjectForm::kPart_Body, BSShaderMaterial::kShaderType_FaceGenRGBTint);

		if (!geometry)
		{
			geometry = GetArmorGeometry(thisActor, BGSBipedObjectForm::kPart_Hands, BSShaderMaterial::kShaderType_FaceGenRGBTint);
		}

		if (!geometry)
		{
			geometry = GetArmorGeometry(thisActor, BGSBipedObjectForm::kPart_Feet, BSShaderMaterial::kShaderType_FaceGenRGBTint);
		}

		if (!geometry)
		{
			geometry = GetArmorGeometry(thisActor, BGSBipedObjectForm::kPart_Tail, BSShaderMaterial::kShaderType_FaceGenRGBTint);
		}

		if (geometry)
		{
			auto shaderProperty = niptr_cast<BSShaderProperty>(geometry->m_spEffectState);
			if (!shaderProperty)
			{
				return;
			}

			auto lightingShader = ni_cast(shaderProperty, BSLightingShaderProperty);
			if (lightingShader)
			{
				auto material = (BSLightingShaderMaterial*)lightingShader->material;

				if (material && material->GetShaderType() == BSShaderMaterial::kShaderType_FaceGenRGBTint)
				{
					auto tintedMaterial = (BSTintedShaderMaterial*)material;
					NiColor tintColor = tintedMaterial->tintColor;

					color->color.red = tintColor.r * 255;
					color->color.green = tintColor.g * 255;
					color->color.blue = tintColor.b * 255;
				}
			}
		}
		else
		{
			auto actorBase = DYNAMIC_CAST<TESNPC*>(thisActor->baseForm);

			if (actorBase)
			{
				color->color.red = actorBase->color.red;
				color->color.green = actorBase->color.green;
				color->color.blue = actorBase->color.blue;
			}
		}
	});
}

void PO3_SKSEFunctions::SetSkinColor(Actor* thisActor, BGSColorForm* color)
{
	if (!thisActor || !thisActor->Is3DLoaded() || !color)
	{
		return;
	}

	g_task->AddTask([thisActor, color]()
	{
		NiGeometry* geometry = GetHeadPartGeometry(thisActor, BGSHeadPart::kTypeFace);
		SetShaderPropertyRGBTint(geometry);

		NiColorA val;
		val.r = color->color.red / 255.0;
		val.g = color->color.green / 255.0;
		val.b = color->color.blue / 255.0;
		NiColorA* skincolor = &val;

		NiNode* model = thisActor->GetNiRootNode(0);

		if (model)
		{
			UpdateModelSkin(model, &skincolor); //sets body skin color
		}
	});
}

void PO3_SKSEFunctions::MixColorWithSkinTone(Actor* thisActor, BGSColorForm* color, bool manualMode, float percentage)
{
	if (!thisActor || !thisActor->Is3DLoaded() || !color)
	{
		return;
	}

	g_task->AddTask([thisActor, color, percentage, manualMode]()
	{
		auto thisNPC = DYNAMIC_CAST<TESNPC*>(thisActor->baseForm);

		if (!thisNPC)
		{
			return;
		}

		NiGeometry* geometry = GetHeadPartGeometry(thisActor, BGSHeadPart::kTypeFace);
		SetShaderPropertyRGBTint(geometry); //makes face tintable

		float skinLuminance = 0.0;

		if (manualMode)
		{
			skinLuminance = percentage;
		}
		else
		{
			skinLuminance = calculateLuminance(thisNPC->color.red, thisNPC->color.green, thisNPC->color.blue);
		}

		UInt8 colorRed = colorMix(color->color.red, thisNPC->color.red, skinLuminance);
		UInt8 colorGreen = colorMix(color->color.green, thisNPC->color.green, skinLuminance);
		UInt8 colorBlue = colorMix(color->color.blue, thisNPC->color.blue, skinLuminance);

		NiColorA val;
		val.r = colorRed / 255.0;
		val.g = colorGreen / 255.0;
		val.b = colorBlue / 255.0;
		NiColorA* multipliedColor = &val;

		NiNode* model = thisActor->GetNiRootNode(0);

		if (model)
		{
			UpdateModelSkin(model, &multipliedColor); //sets body skin color
		}
	});
}

void PO3_SKSEFunctions::SetSkinAlpha(Actor* thisActor, float alpha)
{
	if (!thisActor || !thisActor->Is3DLoaded())
	{
		return;
	}

	g_task->AddTask([thisActor, alpha]()
	{
		NiGeometry* geometry = GetHeadPartGeometry(thisActor, BGSHeadPart::kTypeFace);
		SetShaderPropertyAlpha(geometry, alpha, true);

		SetArmorSkinAlpha(thisActor, BGSBipedObjectForm::kPart_Body, alpha);
		SetArmorSkinAlpha(thisActor, BGSBipedObjectForm::kPart_Hands, alpha);
		SetArmorSkinAlpha(thisActor, BGSBipedObjectForm::kPart_Feet, alpha);
		SetArmorSkinAlpha(thisActor, BGSBipedObjectForm::kPart_Tail, alpha); //tail
		SetArmorSkinAlpha(thisActor, BGSBipedObjectForm::kPart_Unnamed21, alpha); //decap
	});
}

void PO3_SKSEFunctions::EquipArmorIfSkinVisible(Actor* thisActor, TESObjectARMO* armorToCheck, TESObjectARMO* armorToEquip)
{
	if (!thisActor || !thisActor->Is3DLoaded() || !armorToCheck || !armorToEquip)
	{
		return;
	}

	g_task->AddTask([thisActor, armorToCheck, armorToEquip]()
	{
		for (auto& arma : armorToCheck->armorAddons)
		{
			if (!arma)
			{
				continue;
			}

			NiAVObject* armorNode = VisitArmorAddon(thisActor, armorToCheck, arma);

			if (!armorNode)
			{
				continue;
			}

			NiNode* node = armorNode->GetAsNiNode();

			if (node)
			{
				for (UInt32 i = 0; i < node->GetArrayCount(); i++)
				{
					NiAVObject* object = node->GetAt(i);

					if (object && GetShaderPropertyType(object->GetAsNiGeometry()) == BSShaderMaterial::kShaderType_FaceGenRGBTint)
					{
						thisActor->EquipItem(armorToEquip, 1, false, 0, 0);
						return;
					}
				}
			}
			else
			{
				if (GetShaderPropertyType(armorNode->GetAsNiGeometry()) == BSShaderMaterial::kShaderType_FaceGenRGBTint)
				{
					thisActor->EquipItem(armorToEquip, 1, false, 0, 0);
					return;
				}
			}
		}
	});
}

void PO3_SKSEFunctions::ReplaceArmorTextureSet(Actor* thisActor, TESObjectARMO* thisArmor, BGSTextureSet* sourceTXST, BGSTextureSet* targetTXST, SInt32 textureType)
{
	if (!thisActor || !thisActor->Is3DLoaded() || !thisArmor)
	{
		return;
	}

	g_task->AddTask([thisActor, thisArmor, sourceTXST, targetTXST, textureType]()
	{
		for (auto& armorAddon : thisArmor->armorAddons)
		{
			if (armorAddon)
			{
				NiAVObject* armorNode = VisitArmorAddon(thisActor, thisArmor, armorAddon);

				if (armorNode)
				{
					NiNode* node = armorNode->GetAsNiNode();

					if (node)
					{
						for (UInt32 i = 0; i < node->GetArrayCount(); i++)
						{
							NiAVObject* object = node->GetAt(i);

							if (object)
							{
								ReplaceTextureSet(object->GetAsNiGeometry(), sourceTXST, targetTXST, textureType);
							}
						}
					}
					else
					{
						ReplaceTextureSet(armorNode->GetAsNiGeometry(), sourceTXST, targetTXST, textureType);
					}
				}
			}
		}
	});
}

void PO3_SKSEFunctions::ReplaceSkinTextureSet(Actor* thisActor, BGSTextureSet* maleTXST, BGSTextureSet* femaleTXST, UInt32 slotMask, SInt32 textureType)
{
	if (!thisActor || !thisActor->Is3DLoaded())
	{
		return;
	}

	auto actorBase = DYNAMIC_CAST<TESNPC*>(thisActor->baseForm);

	bool isFemale = false;

	if (actorBase)
	{
		UInt8 gender = actorBase->GetSex();
		isFemale = gender == 1 ? true : false;
	}

	if (isFemale)
	{
		SetArmorSkinTXST(thisActor, femaleTXST, slotMask, textureType);
	}
	else
	{
		SetArmorSkinTXST(thisActor, maleTXST, slotMask, textureType);
	}
}

void PO3_SKSEFunctions::ReplaceFaceTextureSet(Actor* thisActor, BGSTextureSet* maleTXST, BGSTextureSet* femaleTXST, SInt32 textureType)
{
	if (!thisActor || !thisActor->Is3DLoaded())
	{
		return;
	}

	auto actorBase = DYNAMIC_CAST<TESNPC*>(thisActor->baseForm);
	bool isFemale = false;

	if (actorBase)
	{
		UInt8 gender = actorBase->GetSex();
		isFemale = gender == 1 ? true : false;
	}

	g_task->AddTask([thisActor, maleTXST, femaleTXST, textureType, isFemale]()
	{
		NiGeometry* faceGeometry = GetHeadPartGeometry(thisActor, BGSHeadPart::kTypeFace);

		if (isFemale)
		{
			ReplaceSkinTXST(faceGeometry, femaleTXST, textureType);
		}
		else
		{
			ReplaceSkinTXST(faceGeometry, maleTXST, textureType);
		}
	});
}

BGSTextureSet* PO3_SKSEFunctions::GetHeadPartTextureSet(Actor* thisActor, UInt32 type)
{
	if (!thisActor || !thisActor->Is3DLoaded())
	{
		return nullptr;
	}

	auto actorBase = DYNAMIC_CAST<TESNPC*>(thisActor->baseForm);

	if (actorBase)
	{
		BGSHeadPart* headpart = actorBase->GetCurrentHeadPartByType(type);

		if (headpart)
		{
			return headpart->textureSet;
		}
	}

	return nullptr;
}

void PO3_SKSEFunctions::SetHeadPartTextureSet(Actor* thisActor, BGSTextureSet* headpartTXST, UInt32 type)
{
	if (!thisActor || !thisActor->Is3DLoaded() || !headpartTXST)
	{
		return;
	}

	auto actorBase = DYNAMIC_CAST<TESNPC*>(thisActor->baseForm);

	if (actorBase)
	{
		BGSHeadPart* headpart = actorBase->GetCurrentHeadPartByType(type);

		if (headpart)
		{
			headpart->textureSet = headpartTXST;
		}
	}
}

void PO3_SKSEFunctions::SetHeadPartAlpha(Actor* thisActor, UInt32 partType, float alpha)
{
	if (!thisActor || !thisActor->Is3DLoaded())
	{
		return;
	}

	g_task->AddTask([thisActor, partType, alpha]()
	{
		NiGeometry* geometry = GetHeadPartGeometry(thisActor, partType);
		SetShaderPropertyAlpha(geometry, alpha, false);
	});
}

void PO3_SKSEFunctions::ToggleSkinnedDecalNode(Actor* thisActor, bool disable)
{
	if (!thisActor)
	{
		return;
	}

	NiNode* parent = thisActor->GetNiNode();

	if (!parent)
	{
		return;
	}

	g_task->AddTask([parent, disable]()
	{
		NiAVObject* object = parent->GetObjectByName("Skinned Decal Node");

		if (object)
		{
			if (!disable)
			{
				object->m_flags &= ~0x01;
			}
			else
			{
				object->m_flags |= 0x01;
			}
		}
	});
}

void PO3_SKSEFunctions::RemoveFaceGenNode(Actor* thisActor)
{
	if (!thisActor)
	{
		return;
	}

	NiNode* parent = thisActor->GetNiNode();

	if (!parent)
	{
		return;
	}

	g_task->AddTask([parent, thisActor]()
	{
		NiAVObject* object = thisActor->GetFaceGenNiNode();

		if (object)
		{
			parent->RemoveChild(object);
		}
	});
}

bool PO3_SKSEFunctions::IsActorSoulTrapped(Actor* thisActor)
{
	//return CALL_MEMBER_FN(thisActor->processManager, IsSoulTrapped)() //can't use RE function because ST overhaul mods may bypass vanilla SoulTrap()

	if (!thisActor)
	{
		return false;
	}

	auto effects = thisActor->GetActiveEffects();

	if (!effects)
	{
		return false;
	}

	TESObjectREFRPtr refPtr;
	EffectSetting* mgef = nullptr;

	for (auto& effect : *effects)
	{
		if (!effect)
		{
			continue;
		}

		mgef = effect->GetBaseObject();

		if (!mgef)
		{
			continue;
		}

		if (mgef->properties.hitEffectArt != art && mgef->properties.archetype != EffectSetting::Properties::kArchetype_SoulTrap) //only soultrap
		{
			continue;
		}

		//got soul-trap

		LookupREFRByHandle(effect->casterRefhandle, refPtr);

		if (!refPtr)
		{
			return false;
		}

		//found soultrap caster

		bool isNPC = false;

		if (thisActor->HasKeyword(keyword))
		{
			isNPC = true;
		}

		auto exChanges = static_cast<ExtraContainerChanges*>(refPtr->extraData.GetByType(ExtraDataType::ContainerChanges)); //loop through caster inventory
		InventoryChanges* changes = exChanges ? exChanges->changes : nullptr;

		if (changes && changes->entryList)
		{
			for (InventoryEntryData* data : *changes->entryList)
			{
				TESForm* item = data->baseForm;

				if (!item->IsSoulGem())
				{
					continue;
				}

				auto thisSoulGem = DYNAMIC_CAST<TESSoulGem*>(item); //get soulgem

				if (thisSoulGem)
				{
					TESSoulGem::SoulLevel soulGemSize = GetSoulLevel(thisSoulGem, data);

					if (soulGemSize == TESSoulGem::SoulLevel::kNone)
					{
						if (isNPC)
						{
							if (thisSoulGem->flags & TESSoulGem::RecordFlags::kCanHoldNPCSoul)
							{
								return true;
							}
						}
						else
						{
							UInt8 actorSoulSize = thisActor->GetSoulSize();

							if (thisSoulGem->gemSize >= static_cast<TESSoulGem::SoulLevel>(actorSoulSize))
							{
								return true;
							}
						}
					}
				}
			}
		}
	}

	return false;
}

VMArray<TESForm*> PO3_SKSEFunctions::AddAllEquippedItemsToArray(Actor* thisActor)
{
	std::vector<TESForm*> vec;

	if (thisActor)
	{
		auto exChanges = static_cast<ExtraContainerChanges*>(thisActor->extraData.GetByType(ExtraDataType::ContainerChanges)); //loop through caster inventory
		InventoryChanges* changes = exChanges ? exChanges->changes : nullptr;

		if (changes && changes->entryList)
		{
			for (auto& data : *changes->entryList)
			{
				if (data->extraList)
				{
					for (auto& extraList : *data->extraList)
					{
						auto worn = static_cast<ExtraWorn*>(extraList->GetByType(ExtraDataType::Worn));
						auto wornLeft = static_cast<ExtraWornLeft*>(extraList->GetByType(ExtraDataType::WornLeft));

						if (worn || wornLeft)
						{
							vec.push_back(data->baseForm);
						}
					}
				}
			}
		}
	}

	return vec;
}

void PO3_SKSEFunctions::ResetActor3D(Actor* thisActor)
{
	if (!thisActor || !thisActor->Is3DLoaded())
	{
		return;
	}

	NiGeometry* geometry = GetHeadPartGeometry(thisActor, BGSHeadPart::kTypeFace);

	if (!geometry)
	{
		if (!thisActor->IsDecapitated(1) && !thisActor->IsOnMount())
		{
			thisActor->QueueNiNodeUpdate(false);
		}
		return;
	}

	UInt32 type = GetShaderPropertyModdedSkin(geometry);

	if (type == 1)
	{
		g_task->AddTask([thisActor, geometry]()
		{
			SetShaderPropertyAlpha(geometry, 1.0, true);
			SetArmorSkinAlpha(thisActor, BGSBipedObjectForm::kPart_Body, 1.0);
			SetArmorSkinAlpha(thisActor, BGSBipedObjectForm::kPart_Hands, 1.0);
			SetArmorSkinAlpha(thisActor, BGSBipedObjectForm::kPart_Feet, 1.0);
			SetArmorSkinAlpha(thisActor, BGSBipedObjectForm::kPart_Tail, 1.0); //tail
			SetArmorSkinAlpha(thisActor, BGSBipedObjectForm::kPart_Unnamed21, 1.0); //decap
		});
	}
	else if (type == 2)
	{
		if (thisActor == g_thePlayer && !g_thePlayer->IsOnMount())
		{
			thisActor->QueueNiNodeUpdate(false);
		}
		else
		{
			auto thisNPC = DYNAMIC_CAST<TESNPC*>(thisActor->baseForm);

			if (thisNPC)
			{
				NiColorA val;
				val.r = thisNPC->color.red / 255.0;
				val.g = thisNPC->color.green / 255.0;
				val.b = thisNPC->color.blue / 255.0;
				NiColorA* skinColor = &val;

				NiNode* model = thisActor->GetNiRootNode(0);

				if (model)
				{
					UpdateModelSkin(model, &skinColor); //sets body skin color
				}
			}
		}
	}
}

void PO3_SKSEFunctions::DecapitateActor(Actor* thisActor)
{
	if (!thisActor)
	{
		return;
	}

	thisActor->Decapitate();
}

//gets actor's time of death
float PO3_SKSEFunctions::GetTimeDead(Actor* thisActor)
{
	if (thisActor && thisActor->processManager)
	{
		float timeOfDeath = thisActor->processManager->timeOfDeath;

		if (timeOfDeath > 0.0f)
		{
			TESGlobal* g_gameDaysPassed = GameTime::GetSingleton()->gameDaysPassed;

			if (g_gameDaysPassed)
			{
				return (floorf(g_gameDaysPassed->value * 24.0f)) - timeOfDeath;
			}
		}
	}

	return 0.0;
}

//gets actor's time of death
float PO3_SKSEFunctions::GetTimeOfDeath(Actor* thisActor)
{
	if (thisActor && thisActor->processManager)
	{
		float timeOfDeath = thisActor->processManager->timeOfDeath;

		if (timeOfDeath > 0.0)
		{
			return (timeOfDeath / 24.0);
		}
	}

	return 0.0;
}

//gets actor's current package
TESPackage* PO3_SKSEFunctions::GetRunningPackage(Actor* thisActor)
{
	if (thisActor)
	{
		return thisActor->GetCurrentPackageInternal();
	}

	return nullptr;
}

bool PO3_SKSEFunctions::IsActorInWater(Actor* thisActor)
{
	if (thisActor)
	{
		return (thisActor->flags1 & Actor::Flags1::kIsInWater) != 0;
	}

	return false;
}

float PO3_SKSEFunctions::GetActorAlpha(Actor* thisActor)
{
	if (thisActor)
	{
		auto processManager = thisActor->processManager;

		if (processManager)
		{
			auto middleProcess = processManager->middleProcess;

			if (middleProcess)
			{
				return middleProcess->actorAlpha;
			}
		}
	}

	return 1.0;
}

float PO3_SKSEFunctions::GetActorRefraction(Actor* thisActor)
{
	if (thisActor)
	{
		auto processManager = thisActor->processManager;

		if (processManager)
		{
			auto middleProcess = processManager->middleProcess;

			if (middleProcess)
			{
				return middleProcess->actorRefraction;
			}
		}
	}

	return 1.0;
}

void PO3_SKSEFunctions::SetActorRefraction(Actor* thisActor, float refraction)
{
	if (thisActor)
	{
		auto processManager = thisActor->processManager;

		if (processManager)
		{
			refraction = std::clamp(refraction, 0.0f, 1.0f);

			processManager->SetActorRefraction(refraction);

			float invisibility = thisActor->GetActorValueCurrent(54); //invisibility

			if (invisibility < 0.0 || invisibility <= 1.0 && invisibility <= 0.0 || thisActor != g_thePlayer)
			{
				if (refraction <= 0.0)
				{
					thisActor->UpdateRefractionProperty(0, refraction);
					thisActor->UpdateRefractionWithExtraData();
				}
				else
				{
					thisActor->UpdateRefractionProperty(1, refraction);
				}
			}
			else
			{
				thisActor->UpdateRefraction(1.0);

				refraction = 1.0 - refraction / 100.0;
				refraction = (1.0 + (0.01 - 1.0) * ((refraction - 0.0) / (1.0 - 0.0)));

				thisActor->UpdateRefractionProperty(1, refraction);
			}
		}
	}
}

SInt32 PO3_SKSEFunctions::GetDeadState(Actor* thisActor)
{
	if (thisActor)
	{	
		return ((thisActor->flags04 >> 0x15) & 0xF);
	}

	return -1;
}

void PO3_SKSEFunctions::SetShaderType(Actor* thisActor, TESObjectARMO* templateArmor)
{
	/*if (!thisActor || !thisActor->Is3DLoaded() || !templateArmor)
	{
		return;
	g_task->AddTask([thisActor, templateArmor]()
	{
		NiGeometry * templateGeometry = GetTemplateArmorGeometry(thisActor, templateArmor, 11);

		if (!templateGeometry)
		{
			_MESSAGE("no template geometry");
			return;
		}

		NiGeometry * geometry = GetHeadPartGeometry(thisActor, BGSHeadPart::kTypeFace);
		SetShaderPropertyMLP(geometry, templateGeometry);

		SetArmorSkinShaderType(thisActor, templateGeometry, BGSBipedObjectForm::kPart_Body);
		SetArmorSkinShaderType(thisActor, templateGeometry, BGSBipedObjectForm::kPart_Hands);
		SetArmorSkinShaderType(thisActor, templateGeometry, BGSBipedObjectForm::kPart_Feet);
		SetArmorSkinShaderType(thisActor, templateGeometry, BGSBipedObjectForm::kPart_Unnamed21); //decap
		SetArmorSkinShaderType(thisActor, templateGeometry, BGSBipedObjectForm::kPart_Tail); //tail
	})*/
}

//--------------------------------------------------------------------------------------------
// ARRAY
//--------------------------------------------------------------------------------------------

bool PO3_SKSEFunctions::AddStringToArray(BSFixedString thisString, VMArray<BSFixedString> stringArray)
{
	auto length = stringArray.GetSize();

	if (length > 0)
	{
		for (UInt32 i = 0; i < length; i++)
		{
			BSFixedString string = nullptr;
			stringArray.GetAt(i, string);

			if (string.c_str() == nullptr)
			{
				stringArray.SetAt(i, thisString);
				return true;
			}
		}
	}

	return false;
}

bool PO3_SKSEFunctions::AddActorToArray(Actor* thisActor, VMArray<Actor*> actorArray)
{
	auto length = actorArray.GetSize();

	if (length > 0)
	{
		for (UInt32 i = 0; i < length; i++)
		{
			Actor* actor = nullptr;
			actorArray.GetAt(i, actor);

			if (actor == nullptr)
			{
				actorArray.SetAt(i, thisActor);
				return true;
			}
		}
	}

	return false;
}

//count how many instances of string are found in an array
UInt32 PO3_SKSEFunctions::ArrayStringCount(BSFixedString thisString, VMArray<BSFixedString> stringArray)
{
	UInt32 count = 0;
	auto length = stringArray.GetSize();

	if (length > 0)
	{
		for (UInt32 i = 0; i < length; i++)
		{
			BSFixedString string = nullptr;
			stringArray.GetAt(i, string);

			if (string == thisString)
			{
				count++;
			}
		}
	}

	return count;
}

//alphabetically sorts strings inside array
VMArray<BSFixedString> PO3_SKSEFunctions::SortArrayString(VMArray<BSFixedString> stringArray)
{
	VMArray<BSFixedString> result;
	std::vector<std::string> vec;

	auto count = stringArray.GetSize();

	if (!count)
	{
		return result;
	}

	vec.reserve(count);

	for (UInt32 i = 0; i < count; i++)
	{
		BSFixedString string = nullptr;
		stringArray.GetAt(i, string);

		if (string.c_str() != nullptr)
		{
			vec.emplace_back(string.c_str());
		}
	}

	std::sort(vec.begin(), vec.end());

	if (result.Allocate(vec.size()))
	{
		for (size_t i = 0; i < vec.size(); i++)
		{
			BSFixedString tempString = vec[i].c_str();
			result.SetAt(i, tempString);
		}
	}

	return result;
}

//--------------------------------------------------------------------------------------------
// EFFECTSHADER
//--------------------------------------------------------------------------------------------

float PO3_SKSEFunctions::GetEffectShaderFullParticleCount(TESEffectShader* thisEffectShader)
{
	return (thisEffectShader) ? (thisEffectShader->data.particleBirthRatioFull) : 0.0;
}

void PO3_SKSEFunctions::SetEffectShaderFullParticleCount(TESEffectShader* thisEffectShader, float particleCount)
{
	if (thisEffectShader)
	{
		thisEffectShader->data.particleBirthRatioFull = particleCount;
	}
}

float PO3_SKSEFunctions::GetEffectShaderPersistentParticleCount(TESEffectShader* thisEffectShader)
{
	return (thisEffectShader) ? (thisEffectShader->data.particleCountPersistent) : 0.0;
}

void PO3_SKSEFunctions::SetEffectShaderPersistentParticleCount(TESEffectShader* thisEffectShader, float particleCount)
{
	if (thisEffectShader)
	{
		thisEffectShader->data.particleCountPersistent = particleCount;
	}
}

bool PO3_SKSEFunctions::IsEffectShaderFlagSet(TESEffectShader* thisEffectShader, UInt32 flag)
{
	return (thisEffectShader) ? (thisEffectShader->data.flags & flag) == flag : false;
}

void PO3_SKSEFunctions::SetEffectShaderFlag(TESEffectShader* thisEffectShader, UInt32 flag)
{
	if (thisEffectShader)
	{
		thisEffectShader->data.flags |= flag;
	}
}

void PO3_SKSEFunctions::ClearEffectShaderFlag(TESEffectShader* thisEffectShader, UInt32 flag)
{
	if (thisEffectShader)
	{
		thisEffectShader->data.flags &= ~flag;
	}
}

//--------------------------------------------------------------------------------------------
// FORM
//--------------------------------------------------------------------------------------------

// replaces keyword on form 
void PO3_SKSEFunctions::ReplaceKeywordOnForm(TESForm* thisForm, BGSKeyword* KYWDtoRemove, BGSKeyword* KYWDtoAdd)
{
	if (!thisForm || !KYWDtoRemove || !KYWDtoAdd)
	{
		return;
	}

	auto pKeywords = DYNAMIC_CAST<BGSKeywordForm*>(thisForm);

	if (pKeywords)
	{
		UInt32 removeIndex = 0;
		BGSKeyword* thisKYWD = nullptr;
		bool found = false;

		for (UInt32 i = 0; i < pKeywords->GetSize(); i++)
		{
			pKeywords->GetKeywordAt(i, thisKYWD);

			if (thisKYWD)
			{
				if (thisKYWD->keyword.Get() == KYWDtoAdd->keyword.Get())
				{
					return;
				}

				if (thisKYWD->keyword.Get() == KYWDtoRemove->keyword.Get())
				{
					removeIndex = i;
					found = true;
				}
			}
		}

		if (found)
		{
			pKeywords->keywords[removeIndex] = KYWDtoAdd;
		}
	}
}

//--------------------------------------------------------------------------------------------
// GAME
//--------------------------------------------------------------------------------------------

bool PO3_SKSEFunctions::IsPluginInstalled(BSFixedString name)
{
	TESDataHandler* dataHandler = TESDataHandler::GetSingleton();

	const TESFile* modInfo = dataHandler->LookupModByName(name.c_str());

	if (modInfo)
	{
		return true;
	}

	return false;
}

VMArray<TESForm*> PO3_SKSEFunctions::GetAllSpellsInMod(BSFixedString modName, VMArray<BGSKeyword*> keywords, bool isPlayable)
{
	TESDataHandler* dataHandler = TESDataHandler::GetSingleton();
	UInt8 modIndex = dataHandler->GetModIndex(modName.c_str());

	std::vector<TESForm*> vec;

	if (modIndex != 255)
	{
		if (isPlayable)
		{
			for (const auto& book : dataHandler->books)
			{
				if (!book)
				{
					continue;
				}

				if ((book->formID >> 24) != modIndex)
				{
					continue;
				}

				bool isSpellBook = book->data.GetSanitizedType() == TESObjectBOOK::Data::kType_Spell;
				bool accept = false;

				SpellItem* spell = nullptr;

				if (isSpellBook)
				{
					spell = book->data.teaches.spell;
				}

				if (VerifyKeywords(spell, &keywords))
				{
					accept = true;
				}

				if (!accept)
				{
					continue;
				}

				vec.push_back(spell);
			}
		}
		else
		{
			for (const auto& spell : dataHandler->spellItems)
			{
				if (!spell)
				{
					continue;
				}

				if ((spell->formID >> 24) != modIndex)
				{
					continue;
				}

				if (!VerifyKeywords(spell, &keywords))
				{
					continue;
				}

				vec.push_back(spell);
			}
		}
	}

	return vec;
}

VMArray<TESForm*> PO3_SKSEFunctions::GetAllRacesInMod(BSFixedString modName, VMArray<BGSKeyword*> keywords)
{
	TESDataHandler* dataHandler = TESDataHandler::GetSingleton();
	UInt8 modIndex = dataHandler->GetModIndex(modName.c_str());

	std::vector<TESForm*> vec;

	if (modIndex != 255)
	{
		for (const auto& race : dataHandler->races)
		{
			if (!race)
			{
				continue;
			}

			if ((race->formID >> 24) != modIndex)
			{
				continue;
			}

			bool accept = false;

			if (VerifyKeywords(race, &keywords))
			{
				accept = true;
			}

			if (!accept)
			{
				continue;
			}

			vec.push_back(race);
		}
	}

	return vec;
}

void PO3_SKSEFunctions::AddAllGameSpellsToList(BGSListForm* thisList, VMArray<BGSKeyword*> keywords, bool isPlayable)
{
	if (!thisList)
	{
		return;
	}

	TESDataHandler* dataHandler = TESDataHandler::GetSingleton();

	if (isPlayable)
	{
		SpellItem* spell = nullptr;

		for (const auto& book : dataHandler->books)
		{
			if (!book)
			{
				continue;
			}

			bool isSpellBook = book->data.GetSanitizedType() == TESObjectBOOK::Data::kType_Spell;
			bool accept = false;

			if (isSpellBook)
			{
				spell = book->data.teaches.spell;
			}

			if (VerifyKeywords(spell, &keywords))
			{
				accept = true;
			}

			if (!accept)
			{
				continue;
			}


		}
	}
	else
	{
		for (const auto& spell : dataHandler->spellItems)
		{
			if (!spell)
			{
				continue;
			}

			if (!VerifyKeywords(spell, &keywords))
			{
				continue;
			}

			thisList->AddFormToList(spell);
		}
	}

	return;
}

void PO3_SKSEFunctions::AddAllGameRacesToList(BGSListForm* thisList, VMArray<BGSKeyword*> keywords)
{
	if (!thisList)
	{
		return;
	}

	TESDataHandler* dataHandler = TESDataHandler::GetSingleton();

	for (const auto& race : dataHandler->races)
	{
		if (!race)
		{
			continue;
		}

		if (VerifyKeywords(race, &keywords))
		{
			continue;
		}

		thisList->AddFormToList(race);
	}

	return;
}

//gets actors by AI processing level - see https://geck.bethsoft.com/index.php?title=GetActorsByProcessingLevel
VMArray<Actor*> PO3_SKSEFunctions::GetActorsByProcessingLevel(UInt32 level)
{
	std::vector<Actor*> vec;

	auto singleton = Unknown012E32E8::GetSingleton();
	BSTArray<RefHandle>* arr = nullptr;

	switch (level)
	{
	case 0:
		arr = &singleton->actorsHigh;
		break;
	case 1:
		arr = &singleton->actorsMiddleHigh;
		break;
	case 2:
		arr = &singleton->actorsMiddleLow;
		break;
	case 3:
		arr = &singleton->actorsLow;
		break;
	default:
		arr = nullptr;
		break;
	}

	if (arr)
	{
		TESObjectREFRPtr refPtr;

		for (auto& refHandle : *arr)
		{
			LookupREFRByHandle(refHandle, refPtr);

			auto actor = static_cast<Actor*>((TESObjectREFR*)refPtr);

			if (actor)
			{
				vec.push_back(actor);
			}
		}
	}

	return vec;
}

//--------------------------------------------------------------------------------------------
// LIGHT
//--------------------------------------------------------------------------------------------

float PO3_SKSEFunctions::GetLightRadius(TESObjectLIGH* thisLight)
{
	return (thisLight) ? float(thisLight->unk78.radius) : 0.0;
}

void PO3_SKSEFunctions::SetLightRadius(TESObjectLIGH* thisLight, float radius)
{
	if (thisLight)
	{
		thisLight->unk78.radius = (UInt32)radius;
	}
}

float PO3_SKSEFunctions::GetLightFade(TESObjectLIGH* thisLight)
{
	return (thisLight) ? (thisLight->fadeValue) : 0.0;
}

void PO3_SKSEFunctions::SetLightFade(TESObjectLIGH* thisLight, float fadeValue)
{
	if (thisLight)
	{
		thisLight->fadeValue = fadeValue;
	}
}

BGSColorForm* PO3_SKSEFunctions::GetLightColor(TESObjectLIGH* thisLight)
{
	if (thisLight)
	{
		auto colorForm = static_cast<BGSColorForm*>(TESForm::CreateEmptyForm(FormType::ColorForm));

		if (colorForm)
		{
			colorForm->flags &= ~(BGSColorForm::Flag::kPlayable);

			colorForm->color.red = thisLight->unk78.red;
			colorForm->color.green = thisLight->unk78.green;
			colorForm->color.blue = thisLight->unk78.blue;

			return colorForm;
		}
	}

	return nullptr;
}

void PO3_SKSEFunctions::SetLightColor(TESObjectLIGH* thisLight, BGSColorForm* colorForm)
{
	if (thisLight && colorForm)
	{
		thisLight->unk78.red = colorForm->color.red;
		thisLight->unk78.green = colorForm->color.green;
		thisLight->unk78.blue = colorForm->color.blue;
	}
}

UInt32 PO3_SKSEFunctions::GetLightTypeInternal(TESObjectLIGH* thisLight)
{
	if (thisLight)
	{
		const auto flags = thisLight->unk78.flags;

		if ((flags & TESObjectLIGH::kFlag_TypeHemiShadow) == TESObjectLIGH::kFlag_TypeHemiShadow)
		{
			return 1;
		}
		if ((flags & TESObjectLIGH::kFlag_TypeOmni) == TESObjectLIGH::kFlag_TypeOmni)
		{
			return 2;
		}
		if ((flags & TESObjectLIGH::kFlag_TypeOmniShadow) == TESObjectLIGH::kFlag_TypeOmniShadow)
		{
			return 3;
		}
		if ((flags & TESObjectLIGH::kFlag_TypeSpot) == TESObjectLIGH::kFlag_TypeSpot)
		{
			return 4;
		}
		if ((flags & TESObjectLIGH::kFlag_TypeSpotShadow) == TESObjectLIGH::kFlag_TypeSpotShadow)
		{
			return 5;
		}
	}

	return 0;
}

UInt32 PO3_SKSEFunctions::GetLightType(TESObjectLIGH* thisLight)
{
	return GetLightTypeInternal(thisLight);
}

void PO3_SKSEFunctions::SetLightType(TESObjectLIGH* thisLight, UInt32 lightType)
{
	if (!thisLight)
	{
		return;
	}

	auto flags = thisLight->unk78.flags;

	switch (lightType)
	{
	case 1:
		flags = flags & ~TESObjectLIGH::kFlags_Type | TESObjectLIGH::kFlag_TypeHemiShadow;
		break;
	case 2:
		flags = flags & ~TESObjectLIGH::kFlags_Type | TESObjectLIGH::kFlag_TypeOmni;
		break;
	case 3:
		flags = flags & ~TESObjectLIGH::kFlags_Type | TESObjectLIGH::kFlag_TypeOmniShadow;
		break;
	case 4:
		flags = flags & ~TESObjectLIGH::kFlags_Type | TESObjectLIGH::kFlag_TypeSpot;
		break;
	case 5:
		flags = flags & ~TESObjectLIGH::kFlags_Type | TESObjectLIGH::kFlag_TypeSpotShadow;
		break;
	default:
		return;
	}

	thisLight->unk78.flags = flags;
}

float PO3_SKSEFunctions::GetLightFOV(TESObjectLIGH* thisLight)
{
	return (thisLight) ? (thisLight->unk78.baseFOV) : 0.0;
}

void PO3_SKSEFunctions::SetLightFOV(TESObjectLIGH* thisLight, float FOV)
{
	if (thisLight)
	{
		thisLight->unk78.baseFOV = FOV;
	}
}

float PO3_SKSEFunctions::GetLightShadowDepthBias(TESObjectREFR* thisLightObject)
{
	if (!thisLightObject)
	{
		return 1.0;
	}

	auto thisLight = DYNAMIC_CAST<TESObjectLIGH*>(thisLightObject->baseForm);

	if (thisLight)
	{
		auto xLightData = static_cast<ExtraLightData*>(thisLightObject->extraData.GetByType(ExtraDataType::LightData));

		if (xLightData)
		{
			return xLightData->depthBias;
		}
	}

	return 1.0;
}

//creates extralightdata if none exists
void PO3_SKSEFunctions::SetLightShadowDepthBias(TESObjectREFR* thisLightObject, float depthBias)
{
	if (!thisLightObject)
	{
		return;
	}

	auto thisLight = DYNAMIC_CAST<TESObjectLIGH*>(thisLightObject->baseForm);

	if (thisLight)
	{
		auto xLightData = static_cast<ExtraLightData*>(thisLightObject->extraData.GetByType(ExtraDataType::LightData));

		if (xLightData)
		{
			xLightData->depthBias = depthBias;
		}
		else
		{
			ExtraLightData* newLightData = ExtraLightData::Create();
			newLightData->depthBias = depthBias;
			(&thisLightObject->extraData)->Add(ExtraDataType::LightData, newLightData);
		}
	}
}

//--------------------------------------------------------------------------------------------
// LOCATION
//--------------------------------------------------------------------------------------------

BGSLocation* PO3_SKSEFunctions::GetParentLocation(BGSLocation* thisLocation)
{
	return (thisLocation) ? thisLocation->parentLocation : nullptr;
}

void PO3_SKSEFunctions::SetParentLocation(BGSLocation* thisLocation, BGSLocation* newLocation)
{
	if (thisLocation)
	{
		thisLocation->parentLocation = newLocation;
	}
}

//--------------------------------------------------------------------------------------------
// MATHS
//--------------------------------------------------------------------------------------------

// based on mersenne twister
float PO3_SKSEFunctions::GenerateRandomFloat(float afMin, float afMax)
{
	std::random_device rd;

	std::mt19937 engine{ rd() };

	std::uniform_real_distribution<float> dist(afMin, afMax);

	return dist(engine);
}

UInt32 PO3_SKSEFunctions::GenerateRandomInt(UInt32 afMin, UInt32 afMax)
{
	std::random_device rd;

	std::mt19937 engine{ rd() };

	std::uniform_int_distribution<UInt32>dist(afMin, afMax);

	return dist(engine);
}

//--------------------------------------------------------------------------------------------
// MAGICEFFECT
//--------------------------------------------------------------------------------------------

VMArray<EffectSetting*> PO3_SKSEFunctions::GetAllActiveEffectsOnActor(Actor* thisActor, bool showInactive)
{
	std::vector<EffectSetting*> vec;

	if (thisActor)
	{
		auto effects = thisActor->GetActiveEffects();

		if (!effects)
		{
			return vec;
		}

		EffectSetting* mgef = nullptr;

		for (auto& effect : *effects)
		{
			if (!effect)
			{
				continue;
			}

			mgef = effect->GetBaseObject();

			if (!mgef)
			{
				continue;
			}

			if (!showInactive && (effect->flags & ActiveEffect::kFlag_Inactive || mgef->properties.flags & EffectSetting::Properties::kEffectType_HideInUI))
			{
				continue;
			}

			vec.push_back(mgef);
		}
	}

	return vec;
}

// for internal use
BSFixedString PO3_SKSEFunctions::GetEffectArchetypeInternal(EffectSetting* mgef)
{
	BSFixedString archetype = nullptr;

	if (!mgef)
	{
		return archetype;
	}

	switch (mgef->properties.archetype)
	{
	case 0:
		archetype = "ValueMod";
		break;
	case 1:
		archetype = "Script";
		break;
	case 2:
		archetype = "Dispel";
		break;
	case 3:
		archetype = "CureDisease";
		break;
	case 4:
		archetype = "Absorb";
		break;
	case 5:
		archetype = "DualValueMod";
		break;
	case 6:
		archetype = "Calm";
		break;
	case 7:
		archetype = "Demoralise";
		break;
	case 8:
		archetype = "Frenzy";
		break;
	case 9:
		archetype = "Disarm";
		break;
	case 10:
		archetype = "CommandSummoned";
		break;
	case 11:
		archetype = "Invisibility";
		break;
	case 12:
		archetype = "Light";
		break;
	case 15:
		archetype = "Lock";
		break;
	case 16:
		archetype = "Open";
		break;
	case 17:
		archetype = "BoundWeapon";
		break;
	case 18:
		archetype = "SummonCreature";
		break;
	case 19:
		archetype = "DetectLife";
		break;
	case 20:
		archetype = "Telekinesis";
		break;
	case 21:
		archetype = "Paralysis";
		break;
	case 22:
		archetype = "Reanimate";
		break;
	case 23:
		archetype = "SoulTrap";
		break;
	case 24:
		archetype = "TurnUndead";
		break;
	case 25:
		archetype = "Guide";
		break;
	case 26:
		archetype = "WerewolfFeed";
		break;
	case 27:
		archetype = "CureParalysis";
		break;
	case 28:
		archetype = "CureAddiction";
		break;
	case 29:
		archetype = "CurePoison";
		break;
	case 30:
		archetype = "Concussion";
		break;
	case 31:
		archetype = "ValueAndParts";
		break;
	case 32:
		archetype = "AccumulateMagnitude";
		break;
	case 33:
		archetype = "Stagger";
		break;
	case 34:
		archetype = "PeakValueMod";
		break;
	case 35:
		archetype = "Cloak";
		break;
	case 36:
		archetype = "Werewolf";
		break;
	case 37:
		archetype = "SlowTime";
		break;
	case 38:
		archetype = "Rally";
		break;
	case 39:
		archetype = "EnhanceWeapon";
		break;
	case 40:
		archetype = "SpawnHazard";
		break;
	case 41:
		archetype = "Etherealize";
		break;
	case 42:
		archetype = "Banish";
		break;
	case 44:
		archetype = "Disguise";
		break;
	case 45:
		archetype = "GrabActor";
		break;
	case 46:
		archetype = "VampireLord";
		break;
	default:
		break;
	}

	return archetype;
}

bool PO3_SKSEFunctions::HasMagicEffectWithArchetype(Actor* thisActor, BSFixedString archetype)
{
	if (thisActor)
	{
		auto effects = thisActor->GetActiveEffects();

		if (!effects)
		{
			return false;
		}

		EffectSetting* mgef = nullptr;

		for (auto& effect : *effects)
		{
			if (!effect)
			{
				continue;
			}

			mgef = effect->GetBaseObject();

			if (mgef && GetEffectArchetypeInternal(mgef) == archetype)
			{
				return true;
			}
		}
	}

	return false;
}

UInt32 PO3_SKSEFunctions::GetEffectArchetypeAsInt(EffectSetting* mgef)
{
	return (mgef) ? (mgef->properties.archetype) : 0;
}

BSFixedString PO3_SKSEFunctions::GetEffectArchetypeAsString(EffectSetting* mgef)
{
	return GetEffectArchetypeInternal(mgef);
}

BGSSoundDescriptorForm* PO3_SKSEFunctions::GetMagicEffectSound(EffectSetting* mgef, UInt32 type)
{
	if (mgef)
	{
		auto effectSoundArray = &mgef->sounds;

		for (UInt32 i = effectSoundArray->GetSize(); i-- > 0;)
		{
			EffectSetting::SoundInfo effectSound;
			effectSoundArray->GetAt(i, effectSound);

			if (effectSound.value == type)
			{
				return effectSound.sound;
			}
		}
	}

	return nullptr;
}

void PO3_SKSEFunctions::SetMagicEffectSound(EffectSetting* mgef, BGSSoundDescriptorForm* mgefSound, UInt32 type)
{
	if (mgef && mgefSound)
	{
		auto effectSoundArray = &mgef->sounds;

		for (UInt32 i = effectSoundArray->GetSize(); i-- > 0;)
		{
			EffectSetting::SoundInfo effectSound;
			effectSoundArray->GetAt(i, effectSound);

			if (effectSound.value == type)
			{
				effectSound.sound = mgefSound;
				break;
			}
		}
	}

	return;
}

//--------------------------------------------------------------------------------------------
// OBJECTREFERENCES
//--------------------------------------------------------------------------------------------

// [x, y, z]
VMArray<float> PO3_SKSEFunctions::GetPositionAsArray(TESObjectREFR* ref)
{
	VMArray<float> pos(3);

	if (ref)
	{
		pos[0] = ref->pos.x;
		pos[1] = ref->pos.y;
		pos[2] = ref->pos.z;
	}

	return pos;
}

// [angleX, angleY, angleZ]
VMArray<float> PO3_SKSEFunctions::GetRotationAsArray(TESObjectREFR* ref)
{
	VMArray<float> angles(3);

	if (ref)
	{
		angles[0] = ref->rot.x * (180.0 / M_PI);
		angles[1] = ref->rot.y * (180.0 / M_PI);
		angles[2] = ref->rot.z * (180.0 / M_PI);
	}

	return angles;
}

bool PO3_SKSEFunctions::IsLoadDoor(TESObjectREFR* thisDoor)
{
	if (thisDoor)
	{
		//Get the ExtraTeleport
		auto teleport = static_cast<ExtraTeleport*>(thisDoor->extraData.GetByType(ExtraDataType::Teleport));

		if (teleport)
		{
			return true;
		}
	}

	return false;
}

// internal checker
bool PO3_SKSEFunctions::CanItemBeTaken(InventoryEntryData* data, bool noEquipped, bool noFavourited, bool noQuestItem)
{
	if (data->extraList)
	{
		for (auto& extraList : *data->extraList)
		{
			if (noEquipped)
			{
				auto worn = static_cast<ExtraWorn*>(extraList->GetByType(ExtraDataType::Worn));
				auto wornLeft = static_cast<ExtraWornLeft*>(extraList->GetByType(ExtraDataType::WornLeft));

				if (worn || wornLeft)
				{
					return false;
				}
			}

			if (noFavourited)
			{
				auto HotKey = static_cast<ExtraHotkey*>(extraList->GetByType(ExtraDataType::Hotkey));

				if (HotKey)
				{
					return false;
				}
			}

			if (noQuestItem)
			{
				auto aliases = static_cast<ExtraAliasInstanceArray*>(extraList->GetByType(ExtraDataType::AliasInstanceArray));

				if (aliases)
				{
					for (auto& alias : aliases->aliases)
					{
						TESQuest* quest = alias->quest;
						BGSBaseAlias* refAlias = alias->alias;

						if (quest && refAlias && (refAlias->flags & refAlias->kFlag_QuestObject))
						{
							return false;
						}
					}
				}
			}
		}
	}

	return true;
}

void PO3_SKSEFunctions::AddAllInventoryItemsToList(TESObjectREFR* thisRef, BGSListForm* thisList, bool noEquipped, bool noFavourited, bool noQuestItem)
{
	if (thisRef && thisList)
	{
		auto exChanges = static_cast<ExtraContainerChanges*>(thisRef->extraData.GetByType(ExtraDataType::ContainerChanges)); //loop through caster inventory
		InventoryChanges* changes = exChanges ? exChanges->changes : nullptr;

		if (changes && changes->entryList)
		{
			for (auto& data : *changes->entryList)
			{
				if (CanItemBeTaken(data, noEquipped, noFavourited, noQuestItem))
				{
					thisList->AddFormToList(data->baseForm);
				}
			}
		}
	}
}

VMArray<TESForm*> PO3_SKSEFunctions::AddAllInventoryItemsToArray(TESObjectREFR* thisRef, bool noEquipped, bool noFavourited, bool noQuestItem)
{
	std::vector<TESForm*> vec;

	if (thisRef)
	{
		auto exChanges = static_cast<ExtraContainerChanges*>(thisRef->extraData.GetByType(ExtraDataType::ContainerChanges)); //loop through caster inventory
		InventoryChanges* changes = exChanges ? exChanges->changes : nullptr;

		if (changes && changes->entryList)
		{
			for (auto& data : *changes->entryList)
			{
				if (CanItemBeTaken(data, noEquipped, noFavourited, noQuestItem))
				{
					vec.push_back(data->baseForm);
				}
			}
		}
	}

	return vec;
}

// replaces keyword on ref 
void PO3_SKSEFunctions::ReplaceKeywordOnRef(TESObjectREFR* thisRef, BGSKeyword* KYWDtoRemove, BGSKeyword* KYWDtoAdd)
{
	if (!thisRef || !KYWDtoRemove || !KYWDtoAdd)
		return;

	auto pKeywords = DYNAMIC_CAST<BGSKeywordForm*>(thisRef);

	if (pKeywords)
	{
		UInt32 removeIndex = 0;
		BGSKeyword* thisKYWD = nullptr;
		bool found = false;

		for (UInt32 i = 0; i < pKeywords->GetSize(); i++)
		{
			pKeywords->GetKeywordAt(i, thisKYWD);

			if (thisKYWD)
			{
				if (thisKYWD->keyword.Get() == KYWDtoAdd->keyword.Get())
				{
					return;
				}

				if (thisKYWD->keyword.Get() == KYWDtoRemove->keyword.Get())
				{
					removeIndex = i;
					found = true;
				}
			}
		}

		if (found)
		{
			pKeywords->keywords[removeIndex] = KYWDtoAdd;
		}
	}
}

//calculates a 2D vector
void PO3_SKSEFunctions::Apply2DHavokImpulse(TESObjectREFR* source, TESObjectREFR* target, float afZ, float magnitude)
{
	if (!source || !source->Is3DLoaded() || !target || !target->Is3DLoaded())
	{
		return;
	}

	float sourceZ = (source->rot.z * (180.0 / M_PI));
	float angleZ = (sourceZ + GetHeadingAngle(g_skyrimVM->GetState(), 0, source, target)); //source.getanglez() + source.getheadingangle(target)

	g_task->AddTask([target, angleZ, afZ, magnitude]()
	{
		ApplyHavokImpulse(g_skyrimVM->GetState(), 0, target, asinf(angleZ), acosf(angleZ), afZ, magnitude);
	});
}

//calculates a 3D vector and takes into account the elevation between source and target. 
void PO3_SKSEFunctions::Apply3DHavokImpulse(TESObjectREFR* source, TESObjectREFR* target, float magnitude)
{
	if (!source || !source->Is3DLoaded() || !target || !target->Is3DLoaded())
	{
		return;
	}

	float dx = target->pos.x - source->pos.x;
	float dy = target->pos.y - source->pos.y;
	float dz = target->pos.z - source->pos.z;

	float dist = (dx * dx) + (dy * dy) + (dz * dz);

	float x = dx / dist; //x
	float y = dy / dist; //y
	float z = dz / dist; //z

	g_task->AddTask([target, x, y, z, magnitude]()
	{
		ApplyHavokImpulse(g_skyrimVM->GetState(), 0, target, x, y, z, magnitude);
	});
}

//moves object to nearest navemesh location
void PO3_SKSEFunctions::MoveToNearestNavmeshLocation(TESObjectREFR* target)
{
	auto nearestVertex = FindNearestVertex(target);
	if (!nearestVertex)
	{
		return;
	}

	auto handle = target->CreateRefHandle();
	g_task->AddTask([handle, nearestVertex]()
	{
		TESObjectREFRPtr ref;
		TESObjectREFR::LookupByHandle(handle, ref);
		if (!ref)
		{
			return;
		}

		SetPosition(ref, std::move(*nearestVertex));
	});
}

VMArray<TESEffectShader*> PO3_SKSEFunctions::GetAllEffectShaders(TESObjectREFR* thisRef)
{
	std::vector<TESEffectShader*> effectShaders;

	if (thisRef)
	{
		auto singleton = Unknown012E32E8::GetSingleton();
		TESObjectREFRPtr ref;

		singleton->activeEffectShaderLock.Lock();
		for (auto& shaderReferenceEffect : singleton->activeEffectShaders)
		{
			if (!shaderReferenceEffect)
			{
				continue;
			}

			LookupREFRByHandle(shaderReferenceEffect->refHandle, ref);

			if (!ref || thisRef != (TESObjectREFR*)ref)
			{
				continue;
			}

			if (shaderReferenceEffect->shader)
			{
				effectShaders.push_back(shaderReferenceEffect->shader);
			}
		}
		singleton->activeEffectShaderLock.Unlock();
	}

	return effectShaders;
}

UInt32 PO3_SKSEFunctions::HasEffectShader(TESObjectREFR* thisRef, TESEffectShader* effectShader)
{
	UInt32 effectShaderCount = 0;

	if (thisRef && effectShader)
	{
		auto singleton = Unknown012E32E8::GetSingleton();
		TESObjectREFRPtr ref;

		singleton->activeEffectShaderLock.Lock();
		for (auto& shaderReferenceEffect : singleton->activeEffectShaders)
		{
			if (!shaderReferenceEffect)
			{
				continue;
			}

			LookupREFRByHandle(shaderReferenceEffect->refHandle, ref);

			if (!ref || thisRef != (TESObjectREFR*)ref)
			{
				continue;
			}

			auto shader = shaderReferenceEffect->shader;

			if (shader && shader == effectShader)
			{
				effectShaderCount++;
			}
		}
		singleton->activeEffectShaderLock.Unlock();
	}

	return effectShaderCount;
}

VMArray<BGSArtObject*> PO3_SKSEFunctions::GetAllArtObjects(TESObjectREFR* thisRef)
{
	std::vector<BGSArtObject*> artObjects;

	if (thisRef)
	{
		TESObjectREFRPtr ref;
		auto singleton = Unknown012E32E8::GetSingleton();

		singleton->activeEffectShaderLock.Lock();
		for (auto& shaderReferenceEffect : singleton->activeEffectShaders)
		{
			if (!shaderReferenceEffect)
			{
				continue;
			}

			LookupREFRByHandle(shaderReferenceEffect->refHandle, ref);

			if (!ref || thisRef != (TESObjectREFR*)ref)
			{
				continue;
			}

			if (shaderReferenceEffect->artObject)
			{
				artObjects.push_back(shaderReferenceEffect->artObject);
			}
		}
		singleton->activeEffectShaderLock.Unlock();
	}

	return artObjects;
}

UInt32 PO3_SKSEFunctions::HasArtObject(TESObjectREFR* thisRef, BGSArtObject* artObject)
{
	UInt32 artObjectCount = 0;

	if (thisRef && artObject)
	{
		TESObjectREFRPtr ref;
		auto singleton = Unknown012E32E8::GetSingleton();

		singleton->activeEffectShaderLock.Lock();
		for (auto& shaderReferenceEffect : singleton->activeEffectShaders)
		{
			if (!shaderReferenceEffect)
			{
				continue;
			}

			LookupREFRByHandle(shaderReferenceEffect->refHandle, ref);

			if (!ref || thisRef != (TESObjectREFR*)ref)
			{
				continue;
			}

			auto art = shaderReferenceEffect->artObject;

			if (art && art == artObject)
			{
				artObjectCount++;
			}
		}
		singleton->activeEffectShaderLock.Unlock();
	}

	return artObjectCount;
}

void PO3_SKSEFunctions::StopArtObject(TESObjectREFR* thisRef, BGSArtObject* artObject)
{
	if (thisRef)
	{
		Unknown012E32E8::GetSingleton()->StopArtObject(thisRef, artObject);
	}
}

Actor* PO3_SKSEFunctions::GetActorCause(TESObjectREFR* thisRef)
{
	Actor* actor = nullptr;

	if (thisRef)
	{
		TESObjectREFRPtr ref;
		
		LookupREFRByHandle(*thisRef->GetActorCause(), ref);

		actor = static_cast<Actor*>((TESObjectREFR*)ref);

		if (actor)
		{
			return actor;
		}
	}

	return actor;
}

Actor* PO3_SKSEFunctions::GetClosestActorFromRef(TESObjectREFR* thisRef, float radius)
{
	if (thisRef)
	{		
		auto squaredRadius = radius * radius;
		auto closestDistance = FLT_MAX;

		auto originPos = thisRef->pos;
		
		std::map<float, Actor*> map;		
		TESObjectREFRPtr ref;
	
		auto singleton = Unknown012E32E8::GetSingleton();

		for (auto& refHandle : singleton->actorsHigh)
		{
			LookupREFRByHandle(refHandle, ref);

			auto actor = static_cast<Actor*>((TESObjectREFR*)ref);

			if (!actor || actor == thisRef)
			{
				continue;
			}
			
			auto distance = CalcLinearDistance(originPos, actor->pos);

			if (distance > squaredRadius)
			{
				continue;
			}
			
			map.try_emplace(distance, actor);

			if (distance < closestDistance)
			{
				closestDistance = distance;
			}
		}

		if (thisRef != g_thePlayer)
		{
			auto distance = CalcLinearDistance(originPos, g_thePlayer->pos);

			if (distance <= squaredRadius)
			{
				map.try_emplace(distance, g_thePlayer);

				if (distance < closestDistance)
				{
					closestDistance = distance;
				}
			}
		}

		return map.find(closestDistance)->second;
	}

	return nullptr;
}

Actor* PO3_SKSEFunctions::GetRandomActorFromRef(TESObjectREFR* thisRef, float radius)
{
	if (thisRef)
	{
		auto squaredRadius = radius * radius;

		auto originPos = thisRef->pos;

		std::vector<Actor*> vec;
		TESObjectREFRPtr ref;

		auto singleton = Unknown012E32E8::GetSingleton();

		vec.reserve(singleton->numActorsInHighProcess);

		for (auto& refHandle : singleton->actorsHigh)
		{
			LookupREFRByHandle(refHandle, ref);

			auto actor = static_cast<Actor*>((TESObjectREFR*)ref);

			if (!actor || actor == thisRef)
			{
				continue;
			}

			auto distance = CalcLinearDistance(originPos, actor->pos);

			if (distance > squaredRadius)
			{
				continue;
			}

			vec.emplace_back(actor);
		}

		if (thisRef != g_thePlayer)
		{
			auto distance = CalcLinearDistance(originPos, g_thePlayer->pos);

			if (distance <= squaredRadius)
			{
				vec.emplace_back(g_thePlayer);
			}
		}

		std::random_device rd;
		std::mt19937 engine{ rd() };

		std::uniform_int_distribution<UInt32>dist(0, vec.size() - 1);

		return vec.at(dist(engine));
	}

	return nullptr;
}

//--------------------------------------------------------------------------------------------
// PACKAGE
//--------------------------------------------------------------------------------------------

SInt32 PO3_SKSEFunctions::GetPackageType(TESPackage* package)
{
	if (package)
	{
		return package->type;
	}

	return -1;
}

//--------------------------------------------------------------------------------------------
// PROJECTILE
//--------------------------------------------------------------------------------------------

float PO3_SKSEFunctions::GetProjectileSpeed(BGSProjectile* thisProjectile)
{
	return (thisProjectile) ? (thisProjectile->data.speed) : 0.0;
}

void PO3_SKSEFunctions::SetProjectileSpeed(BGSProjectile* thisProjectile, float speed)
{
	if (thisProjectile)
	{
		thisProjectile->data.speed = speed;
	}
}

float PO3_SKSEFunctions::GetProjectileRange(BGSProjectile* thisProjectile)
{
	return (thisProjectile) ? (thisProjectile->data.range) : 0.0;
}

void PO3_SKSEFunctions::SetProjectileRange(BGSProjectile* thisProjectile, float range)
{
	if (thisProjectile)
	{
		thisProjectile->data.range = range;
	}
}

float PO3_SKSEFunctions::GetProjectileGravity(BGSProjectile* thisProjectile)
{
	return (thisProjectile) ? (thisProjectile->data.gravity) : 0.0;
}

void PO3_SKSEFunctions::SetProjectileGravity(BGSProjectile* thisProjectile, float gravity)
{
	if (thisProjectile)
	{
		thisProjectile->data.gravity = gravity;
	}
}

float PO3_SKSEFunctions::GetProjectileImpactForce(BGSProjectile* thisProjectile)
{
	return (thisProjectile) ? (thisProjectile->data.impactForce) : 0.0;
}

void PO3_SKSEFunctions::SetProjectileImpactForce(BGSProjectile* thisProjectile, float impactForce)
{
	if (thisProjectile)
	{
		thisProjectile->data.impactForce = impactForce;
	}
}

UInt32 PO3_SKSEFunctions::GetProjectileType(BGSProjectile* thisProjectile)
{
	if (thisProjectile)
	{
		if (((thisProjectile->data.unk00 >> 16) & 0x7F) == 0x01) //Missile
		{
			return 1;
		}
		else if (((thisProjectile->data.unk00 >> 16) & 0x7F) == 0x02) //Lobber (runes)
		{
			return 2;
		}
		else if (((thisProjectile->data.unk00 >> 16) & 0x7F) == 0x04) //Beam
		{
			return 3;
		}
		else if (((thisProjectile->data.unk00 >> 16) & 0x7F) == 0x08) //Flame
		{
			return 4;
		}
		else if (((thisProjectile->data.unk00 >> 16) & 0x7F) == 0x10) //Cone
		{
			return 5;
		}
		else if (((thisProjectile->data.unk00 >> 16) & 0x7F) == 0x20) //Barrier
		{
			return 6;
		}
		else if (((thisProjectile->data.unk00 >> 16) & 0x7F) == 0x40) //Arrow
		{
			return 7;
		}
	}

	return 0;
}

//--------------------------------------------------------------------------------------------
// SOUND
//--------------------------------------------------------------------------------------------

void PO3_SKSEFunctions::SetSoundDescriptor(TESSound* thisSound, BGSSoundDescriptorForm* thisSoundDescriptor)
{
	if (thisSound && thisSoundDescriptor)
	{
		thisSound->descriptor = thisSoundDescriptor;
	}
}

//--------------------------------------------------------------------------------------------
// SPELL
//--------------------------------------------------------------------------------------------

UInt32 PO3_SKSEFunctions::GetSpellType(SpellItem* thisSpell)
{
	return (thisSpell) ? thisSpell->data.type : 0;
}

//--------------------------------------------------------------------------------------------
// VISUALEFFECT
//--------------------------------------------------------------------------------------------

BGSArtObject* PO3_SKSEFunctions::GetArtObject(BGSReferenceEffect* visualEffect)
{
	return (visualEffect) ? visualEffect->data.effectArt : nullptr;
}

void PO3_SKSEFunctions::SetArtObject(BGSReferenceEffect* visualEffect, BGSArtObject* art)
{
	if (visualEffect && art)
	{
		visualEffect->data.effectArt = art;
	}
}

//--------------------------------------------------------------------------------------------
// WEATHER
//--------------------------------------------------------------------------------------------

//returns wind speed from 0-255 (how it's set up in the weather form)
UInt32 PO3_SKSEFunctions::GetWindSpeedAsInt(TESWeather* thisWeather)
{
	return (thisWeather) ? (thisWeather->general.windSpeed) : 0;
}

//returns wind speed from 0.0-1.0 (how it's set up in the CK)
float PO3_SKSEFunctions::GetWindSpeedAsFloat(TESWeather* thisWeather)
{
	return (thisWeather) ? ((thisWeather->general.windSpeed) / 255.0) : 0.0;
}

SInt32 PO3_SKSEFunctions::GetWeatherType(TESWeather* thisWeather)
{
	TESWeather* currentWeather = nullptr;

	if (thisWeather)
	{
		currentWeather = thisWeather;
	}

	if (!currentWeather)
	{
		currentWeather = Sky::GetSingleton()->curentWeather;
	}

	if (currentWeather)
	{
		const auto flags = currentWeather->general.classification;

		if ((flags & TESWeather::Classification::kWeather_Pleasant) != TESWeather::Classification::kNone)
		{
			return 0;
		}
		if ((flags & TESWeather::Classification::kWeather_Cloudy) != TESWeather::Classification::kNone)
		{
			return 1;
		}
		if ((flags & TESWeather::Classification::kWeather_Rainy) != TESWeather::Classification::kNone)
		{
			return 2;
		}
		if ((flags & TESWeather::Classification::kWeather_Snowy) != TESWeather::Classification::kNone)
		{
			return 3;
		}
	}

	return -1;
}

//--------------------------------------------------------------------------------------------
// REGISTER
//--------------------------------------------------------------------------------------------

// Tell the game about the new functions.
bool PO3_SKSEFunctions::Register(VMState* state)
{
	if (!state)
	{
		_MESSAGE("couldn't get VMState");
		return false;
	}

	REGISTER_PAPYRUS_FUNCTION(PO3_SKSEFunctions, GetHairColor, state);
	REGISTER_PAPYRUS_FUNCTION(PO3_SKSEFunctions, SetHairColor, state);
	REGISTER_PAPYRUS_FUNCTION(PO3_SKSEFunctions, GetSkinColor, state);
	REGISTER_PAPYRUS_FUNCTION(PO3_SKSEFunctions, SetSkinColor, state);
	REGISTER_PAPYRUS_FUNCTION(PO3_SKSEFunctions, MixColorWithSkinTone, state);
	REGISTER_PAPYRUS_FUNCTION(PO3_SKSEFunctions, SetSkinAlpha, state);
	REGISTER_PAPYRUS_FUNCTION(PO3_SKSEFunctions, EquipArmorIfSkinVisible, state);
	REGISTER_PAPYRUS_FUNCTION(PO3_SKSEFunctions, ReplaceArmorTextureSet, state);
	REGISTER_PAPYRUS_FUNCTION(PO3_SKSEFunctions, ReplaceSkinTextureSet, state);
	REGISTER_PAPYRUS_FUNCTION(PO3_SKSEFunctions, ReplaceFaceTextureSet, state);
	REGISTER_PAPYRUS_FUNCTION(PO3_SKSEFunctions, GetHeadPartTextureSet, state);
	REGISTER_PAPYRUS_FUNCTION(PO3_SKSEFunctions, SetHeadPartTextureSet, state);
	REGISTER_PAPYRUS_FUNCTION(PO3_SKSEFunctions, SetHeadPartAlpha, state);
	REGISTER_PAPYRUS_FUNCTION(PO3_SKSEFunctions, ToggleSkinnedDecalNode, state);
	REGISTER_PAPYRUS_FUNCTION(PO3_SKSEFunctions, RemoveFaceGenNode, state);
	REGISTER_PAPYRUS_FUNCTION(PO3_SKSEFunctions, IsActorSoulTrapped, state);
	REGISTER_PAPYRUS_FUNCTION(PO3_SKSEFunctions, ResetActor3D, state);
	REGISTER_PAPYRUS_FUNCTION(PO3_SKSEFunctions, DecapitateActor, state);
	REGISTER_PAPYRUS_FUNCTION(PO3_SKSEFunctions, GetTimeDead, state);
	REGISTER_PAPYRUS_FUNCTION(PO3_SKSEFunctions, GetTimeOfDeath, state);
	REGISTER_PAPYRUS_FUNCTION(PO3_SKSEFunctions, GetRunningPackage, state);
	REGISTER_PAPYRUS_FUNCTION(PO3_SKSEFunctions, IsActorInWater, state);
	REGISTER_PAPYRUS_FUNCTION(PO3_SKSEFunctions, GetActorAlpha, state);
	REGISTER_PAPYRUS_FUNCTION(PO3_SKSEFunctions, GetActorRefraction, state);
	REGISTER_PAPYRUS_FUNCTION(PO3_SKSEFunctions, SetActorRefraction, state);
	REGISTER_PAPYRUS_FUNCTION(PO3_SKSEFunctions, GetDeadState, state);
	//REGISTER_PAPYRUS_FUNCTION(PO3_SKSEFunctions, SetShaderType, state);

	REGISTER_PAPYRUS_FUNCTION(PO3_SKSEFunctions, AddStringToArray, state);
	REGISTER_PAPYRUS_FUNCTION(PO3_SKSEFunctions, AddActorToArray, state);
	REGISTER_PAPYRUS_FUNCTION(PO3_SKSEFunctions, ArrayStringCount, state);
	REGISTER_PAPYRUS_FUNCTION(PO3_SKSEFunctions, SortArrayString, state);

	REGISTER_PAPYRUS_FUNCTION(PO3_SKSEFunctions, GetEffectShaderFullParticleCount, state);
	REGISTER_PAPYRUS_FUNCTION(PO3_SKSEFunctions, SetEffectShaderFullParticleCount, state);
	REGISTER_PAPYRUS_FUNCTION(PO3_SKSEFunctions, GetEffectShaderPersistentParticleCount, state);
	REGISTER_PAPYRUS_FUNCTION(PO3_SKSEFunctions, SetEffectShaderPersistentParticleCount, state);
	REGISTER_PAPYRUS_FUNCTION(PO3_SKSEFunctions, IsEffectShaderFlagSet, state);
	REGISTER_PAPYRUS_FUNCTION(PO3_SKSEFunctions, SetEffectShaderFlag, state);
	REGISTER_PAPYRUS_FUNCTION(PO3_SKSEFunctions, ClearEffectShaderFlag, state);

	REGISTER_PAPYRUS_FUNCTION(PO3_SKSEFunctions, ReplaceKeywordOnForm, state);

	REGISTER_PAPYRUS_FUNCTION(PO3_SKSEFunctions, IsPluginInstalled, state, VMState::kFunctionFlag_NoWait);
	REGISTER_PAPYRUS_FUNCTION(PO3_SKSEFunctions, GetAllSpellsInMod, state);
	REGISTER_PAPYRUS_FUNCTION(PO3_SKSEFunctions, GetAllRacesInMod, state);
	REGISTER_PAPYRUS_FUNCTION(PO3_SKSEFunctions, AddAllGameSpellsToList, state);
	REGISTER_PAPYRUS_FUNCTION(PO3_SKSEFunctions, AddAllGameRacesToList, state);
	REGISTER_PAPYRUS_FUNCTION(PO3_SKSEFunctions, GetActorsByProcessingLevel, state);

	REGISTER_PAPYRUS_FUNCTION(PO3_SKSEFunctions, GetLightRadius, state);
	REGISTER_PAPYRUS_FUNCTION(PO3_SKSEFunctions, SetLightRadius, state);
	REGISTER_PAPYRUS_FUNCTION(PO3_SKSEFunctions, GetLightFade, state);
	REGISTER_PAPYRUS_FUNCTION(PO3_SKSEFunctions, SetLightFade, state);
	REGISTER_PAPYRUS_FUNCTION(PO3_SKSEFunctions, GetLightColor, state);
	REGISTER_PAPYRUS_FUNCTION(PO3_SKSEFunctions, SetLightColor, state);
	REGISTER_PAPYRUS_FUNCTION(PO3_SKSEFunctions, GetLightType, state);
	REGISTER_PAPYRUS_FUNCTION(PO3_SKSEFunctions, SetLightType, state);
	REGISTER_PAPYRUS_FUNCTION(PO3_SKSEFunctions, GetLightFOV, state);
	REGISTER_PAPYRUS_FUNCTION(PO3_SKSEFunctions, SetLightFOV, state);
	REGISTER_PAPYRUS_FUNCTION(PO3_SKSEFunctions, GetLightShadowDepthBias, state);
	REGISTER_PAPYRUS_FUNCTION(PO3_SKSEFunctions, SetLightShadowDepthBias, state);

	REGISTER_PAPYRUS_FUNCTION(PO3_SKSEFunctions, GetParentLocation, state);
	REGISTER_PAPYRUS_FUNCTION(PO3_SKSEFunctions, SetParentLocation, state);

	REGISTER_PAPYRUS_FUNCTION(PO3_SKSEFunctions, GenerateRandomFloat, state, VMState::kFunctionFlag_NoWait);
	REGISTER_PAPYRUS_FUNCTION(PO3_SKSEFunctions, GenerateRandomInt, state, VMState::kFunctionFlag_NoWait);

	REGISTER_PAPYRUS_FUNCTION(PO3_SKSEFunctions, GetAllActiveEffectsOnActor, state);
	REGISTER_PAPYRUS_FUNCTION(PO3_SKSEFunctions, HasMagicEffectWithArchetype, state);
	REGISTER_PAPYRUS_FUNCTION(PO3_SKSEFunctions, GetEffectArchetypeAsInt, state);
	REGISTER_PAPYRUS_FUNCTION(PO3_SKSEFunctions, GetEffectArchetypeAsString, state);
	REGISTER_PAPYRUS_FUNCTION(PO3_SKSEFunctions, GetMagicEffectSound, state);
	REGISTER_PAPYRUS_FUNCTION(PO3_SKSEFunctions, SetMagicEffectSound, state);

	REGISTER_PAPYRUS_FUNCTION(PO3_SKSEFunctions, GetPositionAsArray, state);
	REGISTER_PAPYRUS_FUNCTION(PO3_SKSEFunctions, GetRotationAsArray, state);
	REGISTER_PAPYRUS_FUNCTION(PO3_SKSEFunctions, IsLoadDoor, state);
	REGISTER_PAPYRUS_FUNCTION(PO3_SKSEFunctions, AddAllInventoryItemsToList, state);
	REGISTER_PAPYRUS_FUNCTION(PO3_SKSEFunctions, AddAllInventoryItemsToArray, state);
	REGISTER_PAPYRUS_FUNCTION(PO3_SKSEFunctions, AddAllEquippedItemsToArray, state);
	REGISTER_PAPYRUS_FUNCTION(PO3_SKSEFunctions, ReplaceKeywordOnRef, state);
	REGISTER_PAPYRUS_FUNCTION(PO3_SKSEFunctions, Apply2DHavokImpulse, state);
	REGISTER_PAPYRUS_FUNCTION(PO3_SKSEFunctions, Apply3DHavokImpulse, state);
	REGISTER_PAPYRUS_FUNCTION(PO3_SKSEFunctions, MoveToNearestNavmeshLocation, state);
	REGISTER_PAPYRUS_FUNCTION(PO3_SKSEFunctions, GetAllEffectShaders, state);
	REGISTER_PAPYRUS_FUNCTION(PO3_SKSEFunctions, HasEffectShader, state);
	REGISTER_PAPYRUS_FUNCTION(PO3_SKSEFunctions, GetAllArtObjects, state);
	REGISTER_PAPYRUS_FUNCTION(PO3_SKSEFunctions, HasArtObject, state);
	REGISTER_PAPYRUS_FUNCTION(PO3_SKSEFunctions, StopArtObject, state);
	REGISTER_PAPYRUS_FUNCTION(PO3_SKSEFunctions, GetActorCause, state);
	REGISTER_PAPYRUS_FUNCTION(PO3_SKSEFunctions, GetClosestActorFromRef, state);
	REGISTER_PAPYRUS_FUNCTION(PO3_SKSEFunctions, GetRandomActorFromRef, state);

	REGISTER_PAPYRUS_FUNCTION(PO3_SKSEFunctions, GetPackageType, state);

	REGISTER_PAPYRUS_FUNCTION(PO3_SKSEFunctions, GetProjectileSpeed, state);
	REGISTER_PAPYRUS_FUNCTION(PO3_SKSEFunctions, SetProjectileSpeed, state);
	REGISTER_PAPYRUS_FUNCTION(PO3_SKSEFunctions, GetProjectileRange, state);
	REGISTER_PAPYRUS_FUNCTION(PO3_SKSEFunctions, SetProjectileRange, state);
	REGISTER_PAPYRUS_FUNCTION(PO3_SKSEFunctions, GetProjectileGravity, state);
	REGISTER_PAPYRUS_FUNCTION(PO3_SKSEFunctions, SetProjectileGravity, state);
	REGISTER_PAPYRUS_FUNCTION(PO3_SKSEFunctions, GetProjectileImpactForce, state);
	REGISTER_PAPYRUS_FUNCTION(PO3_SKSEFunctions, SetProjectileImpactForce, state);
	REGISTER_PAPYRUS_FUNCTION(PO3_SKSEFunctions, GetProjectileType, state);

	REGISTER_PAPYRUS_FUNCTION(PO3_SKSEFunctions, SetSoundDescriptor, state);

	REGISTER_PAPYRUS_FUNCTION(PO3_SKSEFunctions, GetSpellType, state);

	REGISTER_PAPYRUS_FUNCTION(PO3_SKSEFunctions, GetArtObject, state);
	REGISTER_PAPYRUS_FUNCTION(PO3_SKSEFunctions, SetArtObject, state);

	REGISTER_PAPYRUS_FUNCTION(PO3_SKSEFunctions, GetWindSpeedAsInt, state);
	REGISTER_PAPYRUS_FUNCTION(PO3_SKSEFunctions, GetWindSpeedAsFloat, state);
	//REGISTER_PAPYRUS_FUNCTION(PO3_SKSEFunctions, GetWeatherType, state);

	_MESSAGE("Registered papyrus functions");

	return true;
}

