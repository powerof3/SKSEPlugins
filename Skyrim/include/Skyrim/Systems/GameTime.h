#pragma once

#include "../TESForms/Gameplay/TESGlobal.h"

class GameTime
{
public:
	static GameTime* GetSingleton()
	{
		return *(GameTime**)0x012E35DC; //
	};
	
	static constexpr UInt8 DAYS_IN_MONTH[] = 
{
	31,	// Morning Star
	28,	// Sun's Dawn
	31,	// First Seed
	30,	// Rain's Hand
	31,	// Second Seed
	30,	// Midyear
	31,	// Sun's Height
	31,	// Last Seed
	30,	// Hearthfire
	31,	// Frostfall
	30,	// Sun's Dusk
	31	// Evening Star
	};

	struct Days
	{
		enum Day
		{
			kSundas,
			kMorndas,
			kTirdas,
			kMiddas,
			kTurdas,
			kFredas,
			kLoredas,
			kTotal
		};
	};
	using Day = Days::Day;


	struct Months
	{
		enum Month
		{
			kMorningStar,
			kSunsDawn,
			kFirstSeed,
			kRainsHand,
			kSecondSeed,
			kMidyear,
			kSunsHeight,
			kLastSeed,
			kHearthfire,
			kFrostfall,
			kSunsDusk,
			kEveningStar,
			kTotal
		};
	};
	using Month = Months::Month;

	//members
	UInt32       unk00;				//00
	TESGlobal *  gameYear;			//04 - 201
	TESGlobal *  gameMonth;			//08 - 7
	TESGlobal *  gameDay;			//0C - 17
	TESGlobal *  gameHour;			//010 - 8.0
	TESGlobal *  gameDaysPassed;	//014 - 1.0
	TESGlobal *  timeScale;			//018 - 20.0
	UInt32       unk1C;				//01C - a bunch of classes return this as a virtual function? constructor defaults to 0
	UInt32       unk20;				//020 - constructor defaults to 0;

	DEFINE_MEMBER_FN(Constructor, GameTime&, 0x0068D580);
	DEFINE_MEMBER_FN(GetTimeScale, float, 0x0068CE90)
	DEFINE_MEMBER_FN(GetGameYear, SInt32, 0x0068CEA0); // if no global found the default is 2077 lol
	DEFINE_MEMBER_FN(GetGameMonth, SInt32, 0x0068CEE0);  
	DEFINE_MEMBER_FN(GetGameDay, SInt32, 0x0068CF10); //default is 17
	DEFINE_MEMBER_FN(GetGameHour, float, 0x0068CF40); // default is 12.0 but in CK it's 8.0
	DEFINE_MEMBER_FN(GetGameDaysPassedAsInt, UInt32, 0x0068CF60);
	DEFINE_MEMBER_FN(GetGameDaysPassedAsFloat, float, 0x0068CFA0); 
	DEFINE_MEMBER_FN(GetDayOfTheWeek, UInt32, 0x0068CFC0); //gamedayspassed % 7?
	DEFINE_MEMBER_FN(RoundGameDaysPassed, float, 0x0068D090);
	DEFINE_MEMBER_FN(GetGameDaysPassedIntoGameHours, float, 0x0068D0D0); // gameDaysPassed * 24
};