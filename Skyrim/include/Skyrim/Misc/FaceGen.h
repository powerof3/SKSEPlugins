#pragma once

#include "Skyrim/BSCore/BSFixedString.h"

class BSFaceGenNiNode;
class BGSHeadPart;
class TESNPC;

class FaceGen
{
public:
	static FaceGen* GetSingleton()
	{
		return *(FaceGen**)0x012E3538; // or should it be *(ClassName*)0x012E32E8 ?
	};

	struct Action
	{
		BSFixedString name;
		UInt32	unk04;
		float	delta;
	};

	class MorphDatabase
	{
	public:
		//DEFINE_MEMBER_FN(GetFaceGenModelMapEntry, bool, 0x005A6230, const char * meshPath, BSFaceGenModelMap ** entry);
		//DEFINE_MEMBER_FN(SetFaceGenModelMapEntry, void, 0x005A6540, const char * meshPath, BSFaceGenModel * model);

		UInt32 unk00;	// 00 - Doesn't seem to be anything here?
	};

	UInt32			unk00;						// 00
	UInt32			unk04;						// 04
	UInt32			unk08;						// 08
	UInt32			unk0C;						// 0C
	MorphDatabase	morphDatabase;				// 10
	UInt32			unk14[(0x3C - 0x14) >> 2];	// 14
	UInt8			isReset;					// 3C
	UInt8			pad3D[3];					// 3D

	DEFINE_MEMBER_FN(RegenerateHead, void, 0x005A4B80, BSFaceGenNiNode * headNode, BGSHeadPart * head, TESNPC * npc);
	DEFINE_MEMBER_FN(ApplyMorph, void, 0x005A4070, BSFaceGenNiNode * faceGenNode, BGSHeadPart * headPart, BSFixedString * morphName, float relative);
};
STATIC_ASSERT(offsetof(FaceGen, isReset) == 0x3C);