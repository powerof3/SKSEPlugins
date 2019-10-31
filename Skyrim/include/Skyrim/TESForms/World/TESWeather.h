#pragma once

#include "../../FormComponents/TESForm.h"
#include "../../FormComponents/TESTexture.h"
#include "../../FormComponents/TESTexture1024.h"
#include "../../FormComponents/TESModel.h"
#include "../../FormComponents/TESAIForm.h"
#include "../../BSCore/BSTArray.h"
#include "BGSLightingTemplate.h"

class TESImageSpace;
class BGSShaderParticleGeometryData;
class BGSReferenceEffect;


/*==============================================================================
class TESWeather +0000 (_vtbl=01089D24)
0000: class TESWeather
0000: |   class TESForm
0000: |   |   class BaseFormComponent
==============================================================================*/
// 760
class TESWeather : public TESForm
{
public:
	enum { kTypeID = (UInt32)FormType::Weather };

	enum { kNumColorTypes = 17 };
	enum { kNumTimeOfDay = 4 };

	enum ColorTypes {
		kColorType_SkyUpper = 0,
		kColorType_FogNear = 1,
		kColorType_Unk = 2,
		kColorType_Ambient = 3,
		kColorType_Sunlight = 4,
		kColorType_Sun = 5,
		kColorType_Stars = 6,
		kColorType_SkyLower = 7,
		kColorType_Horizon = 8,
		kColorType_EffectLighting = 9,
		kColorType_CloudLODDiffuse = 10,
		kColorType_CloudLODAmbient = 11,
		kColorType_FogFar = 12,
		kColorType_SkyStatics = 13,
		kColorType_WaterMultiplier = 14,
		kColorType_SunGlare = 15,
		kColorType_MoonGlare = 16
	};

	enum TimeOfDay {
		kTime_Sunrise = 0,
		kTime_Day = 1,
		kTime_Sunset = 2,
		kTime_Night = 3
	};

	enum Classification : UInt8 {
		kNone = 0,
		kWeather_Pleasant = 1,
		kWeather_Cloudy = 2,
		kWeather_Rainy = 4,
		kWeather_Snowy = 8,
		kSkyStatic_AlwaysVisible = 16,
		kSkyStatic_FollowsSunPosition = 32
	};

	// 110
	struct ColorType
	{
		UInt32	time[kNumTimeOfDay];
	};

	// 13
	struct General
	{
		struct Color3
		{
			UInt8	red;	// 0
			UInt8	green;	// 1
			UInt8	blue;	// 2
		};
		STATIC_ASSERT(sizeof(Color3) == 0x3);
			
		UInt8	windSpeed;						// 00
		UInt8	unk01;							// 01
		UInt8	unk02;							// 02
		UInt8	transDelta;						// 03
		UInt8	sunGlare;						// 04
		UInt8	sunDamage;						// 05
		UInt8	precipitationBeginFadeIn;		// 06
		UInt8	precipitationEndFadeOut;		// 07
		UInt8	thunderLightningBeginFadeIn;	// 08
		UInt8	thunderLightningEndFadeOut;		// 09
		UInt8	thunderLightningFrequency;		// 0A
		UInt8	classification;					// 0B
		Color3	lightningColor;					// 0C
		UInt8	visualEffectBegin;				// 0F
		UInt8	visualEffectEnd;				// 10
		UInt8	windDirection;					// 11
		UInt8	windDirectionRange;				// 12
		UInt8	unk13;							// 13
	};

	// 20
	struct FogDistance
	{
		float	nearDay;
		float	farDay;				// 10
		float	nearNight;			// 14
		float	farNight;			// 18
		float	powerDay;			// 1C
		float	powerNight;
		float	maxDay;
		float	maxNight;
	};

	struct CloudAlpha
	{
		float	time[kNumTimeOfDay];
	};

	// @members
	TESTexture1024						texture[0x20];						// 014
	UInt8								unk114[0x20];						// 114 - cleared to 0x7F
	UInt8								unk134[0x20];						// 134 - cleared to 0x7F
	UInt8								unk154[0x200];						// 154
	CloudAlpha							cloudAlpha[0x20];					// 354
	UInt32								unk554;								// 554
	General								general;							// 558
	//UInt8								pad56B;								// 56B
	FogDistance							fogDistance;						// 56C
	ColorType							colorTypes[kNumColorTypes];			// 58C
	TESAIForm::Data						unk69C;								// 69C
	BSTArray<void *>					unk6A4;								// 6A4
	UInt32								pad6B0;								// 6B0 - not init'd
	TESImageSpace						* imageSpaces[kNumTimeOfDay];		// 6B4
	BGSLightingTemplate::Data::Color	directionalAmbient[kNumTimeOfDay];	// 6C4
	TESModel							unk744;								// 744
	BGSShaderParticleGeometryData		* particleShader;					// 758
	BGSReferenceEffect					* referenceEffect;					// 75C - NNAM visual effect
};
STATIC_ASSERT(sizeof(TESWeather) == 0x760);
