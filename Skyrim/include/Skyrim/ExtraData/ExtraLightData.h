#pragma once

#include "BSExtraData.h"

/*==============================================================================
class ExtraLightData +0000 (_vtbl=010794D8)
0000: class ExtraLightData
0000: |   class BSExtraData
==============================================================================*/
// 1C
class ExtraLightData : public BSExtraData
{
public:
	enum { kExtraTypeID = (UInt32)ExtraDataType::LightData };

	ExtraLightData();
	virtual ~ExtraLightData();

	// @override
	virtual UInt32	GetType(void) const override;						// 00421220 { return 0x00000091; }
	virtual bool	IsNotEqual(const BSExtraData *rhs) const override;	// 004211A0

	// @members
	float  modifiedFOV;  // 08
	float  modifiedFade; // 0C
	UInt32 unk10;        // 10 // Not any of the flags that the CK offers for light references.
	float  depthBias;    // 14
	UInt8  unk18;        // 18 // Not any of the flags that the CK offers for light references.
	UInt8  pad19[3];     // 19, 20, 21
	//
	static ExtraLightData* Create();
};
