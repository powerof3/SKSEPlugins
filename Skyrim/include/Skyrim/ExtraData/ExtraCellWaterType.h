#pragma once

#include "BSExtraData.h"

/*==============================================================================
class ExtraCellWaterType +0000 (_vtbl=010795E8)
0000: class ExtraCellWaterType
0000: |   class BSExtraData
==============================================================================*/
// 0C
class ExtraCellWaterType : public BSExtraData
{
public:
	enum { kExtraTypeID = (UInt32)ExtraDataType::CellWaterType };

	// @override
	virtual UInt32	GetType(void) const override;				// 00902DF0 { return 0x00000003; }
	virtual bool	IsNotEqual(const BSExtraData *rhs) const;	// 00421230 { return unk08 != rhs->unk08; }

	// @members
	UInt32	unk08;			// 08
};
