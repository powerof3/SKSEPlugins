#pragma once

#include "ReferenceEffect.h"
#include "Skyrim/Forms/TESEffectShader.h"

class ShaderReferenceEffect : public ReferenceEffect
{
public:
	struct UnknownTypeInformation
	{
		char* type;
		UnknownTypeInformation* next;
	};
	virtual ~ShaderReferenceEffect();
	//
	virtual void Unk_01(void);
	virtual UnknownTypeInformation* GetTypeInformation(); // 02
	//
	UInt32 unk00[(0x18 - 0x00) / sizeof(UInt32)];
	RefHandle refHandle; // 20
	UInt32 unk24;
	UInt8  unk28;
	UInt8  unk29; // pad
	UInt8  unk2A; // pad
	UInt8  unk2B; // pad
	UInt32 unk2C[(0x64 - 0x2C) / sizeof(UInt32)];
	BGSArtObject * artObject; //64
	UInt32 unk65[(0x90 - 0x68) / sizeof(UInt32)];
	TESEffectShader* shader; // 90
};
static_assert(offsetof(ShaderReferenceEffect,refHandle) >= 0x20, "refHandle is too early!");
static_assert(offsetof(ShaderReferenceEffect, refHandle) <= 0x20, "refHandle is too late!");
static_assert(offsetof(ShaderReferenceEffect, artObject) >= 0x64, "art is too early!");
static_assert(offsetof(ShaderReferenceEffect, artObject) <= 0x64, "art is too late!");
static_assert(offsetof(ShaderReferenceEffect, shader) >= 0x90, "shader is too early!");
static_assert(offsetof(ShaderReferenceEffect, shader) <= 0x90, "shader is too late!");