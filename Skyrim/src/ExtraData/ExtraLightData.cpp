#include "Skyrim.h"
#include "Skyrim/ExtraData/ExtraLightData.h"

static const UInt32 s_ExtraLightDataVtbl = 0x010794D8;

ExtraLightData*   ExtraLightData::Create()
{
	auto data = (ExtraLightData*)BSExtraData::Create(sizeof(ExtraLightData), s_ExtraLightDataVtbl);
	{
		data->modifiedFOV = 0;
		data->modifiedFade = 0;
		data->unk10 = 0;
		data->depthBias = 0;
		data->unk18 = 0;
		data->pad19[0] = 0;
		data->pad19[1] = 0;
		data->pad19[2] = 0;
	}
	return data;
};
