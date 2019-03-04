#include "commonfunctions.h"
#include "global.h"

#include <ttyd/system.h>
#include <ttyd/seqdrv.h>
#include <ttyd/battle.h>
#include <ttyd/swdrv.h>
#include <ttyd/string.h>
#include <ttyd/mariost.h>
#include <ttyd/__mem.h>
#include <ttyd/party.h>
#include <ttyd/mario_party.h>
#include <ttyd/mario_pouch.h>

#include <cstdio>

namespace mod {

bool checkButtonCombo(uint16_t combo)
{
	uint32_t ButtonInput = ttyd::system::keyGetButton(0);
	if ((ButtonInput & combo) == combo)
	{
		uint32_t ButtonInputTrg = ttyd::system::keyGetButtonTrg(0);
		if (ButtonInputTrg & combo)
		{
			return true;
		}
	}
	return false;
}

bool checkButtonComboEveryFrame(uint16_t combo)
{
	uint32_t ButtonInput = ttyd::system::keyGetButton(0);
	if ((ButtonInput & combo) == combo)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool checkForSpecificSeq(ttyd::seqdrv::SeqIndex wantedSeq)
{
	ttyd::seqdrv::SeqIndex NextSeq = ttyd::seqdrv::seqGetNextSeq();
	if (NextSeq == wantedSeq)
	{
		ttyd::seqdrv::SeqIndex CurrentSeq = ttyd::seqdrv::seqGetSeq();
		if (CurrentSeq == wantedSeq)
		{
			return true;
		}
	}
	return false;
}

bool checkIfInGame()
{
	ttyd::seqdrv::SeqIndex NextSeq = ttyd::seqdrv::seqGetNextSeq();
	ttyd::seqdrv::SeqIndex Game = ttyd::seqdrv::SeqIndex::kGame;
	
	if (NextSeq == Game)
	{
		ttyd::seqdrv::SeqIndex CurrentSeq = ttyd::seqdrv::seqGetSeq();
		if (CurrentSeq == Game)
		{
			uint32_t Current_REL_Loaded = *reinterpret_cast<uint32_t *>(
				*reinterpret_cast<uint32_t *>(GlobalWorkPointer) + 0x15C);
			
			if (Current_REL_Loaded != 0)
			{
				#ifdef TTYD_US
				const uint32_t DMO = 0x4;
				#elif defined TTYD_JP
				const uint32_t DMO = 0x5;
				#elif defined TTYD_EU
				const uint32_t DMO = 0x5;
				#endif
				
				uint32_t Current_REL_Id = *reinterpret_cast<uint32_t *>(Current_REL_Loaded);
				if (Current_REL_Id != DMO)
				{
					// Currently not in the intro
					return true;
				}
			}
		}
	}
	return false;
}

void *getBattlePointer()
{
	uint32_t *BattlePointer = *reinterpret_cast<uint32_t **>(BattleAddressesStart);
	return BattlePointer;
}

void *getMarioBattlePointer()
{
	void *BattlePointer = getBattlePointer();
	if (BattlePointer == nullptr)
	{
		return nullptr;
	}
	
	return ttyd::battle::BattleGetMarioPtr(BattlePointer);
}

void *getPartnerBattlePointer()
{
	void *MarioBattlePointer = getMarioBattlePointer();
	if (MarioBattlePointer == nullptr)
	{
		return nullptr;
	}
	
	uint32_t *BattlePointer = *reinterpret_cast<uint32_t **>(BattleAddressesStart);
	return ttyd::battle::BattleGetPartnerPtr(BattlePointer, MarioBattlePointer);
}

void *getActorPointer(uint32_t slot)
{
	void *BattlePointer = getBattlePointer();
	if (BattlePointer == nullptr)
	{
		return nullptr;
	}
	
	return ttyd::battle::BattleGetUnitPtr(BattlePointer, slot);
}

uint32_t secondsToFrames(uint32_t seconds)
{
	return ttyd::system::sysMsec2Frame(seconds * 1000);
}

uint32_t getSequencePosition()
{
	return ttyd::swdrv::swByteGet(0);
}

void setSequencePosition(uint32_t value)
{
	ttyd::swdrv::swByteSet(0, value);
}

void setNextMap(const char *map)
{
	ttyd::string::strcpy(NextMap, map);
	ttyd::string::strncpy(NextArea, map, 3);
}

void setNextBero(const char *bero)
{
	ttyd::string::strcpy(NextBero, bero);
}

bool compareStrings(const char *str1, const char *str2)
{
	return ttyd::string::strcmp(str1, str2) == 0;
}

bool compareStringsSize(const char *str1, const char *str2, uint32_t size)
{
	return ttyd::string::strncmp(str1, str2, size) == 0;
}

bool compareStringToNextMap(const char *str)
{
	return ttyd::string::strcmp(NextMap, str) == 0;
}

uint32_t getSystemLevel()
{
	return ttyd::mariost::marioStGetSystemLevel();
}

void setSystemLevel(uint32_t value)
{
	ttyd::mariost::marioStSystemLevel(value);
}

void setSeq(const char *map, const char *bero)
{
	ttyd::seqdrv::seqSetSeq(ttyd::seqdrv::SeqIndex::kMapChange, map, bero);
}

void setSpecificSeq(ttyd::seqdrv::SeqIndex seq, const char *map, const char *bero)
{
	ttyd::seqdrv::seqSetSeq(seq, map, bero);
}

void *clearMemory(void *destination, uint32_t size)
{
	return ttyd::__mem::memset(destination, 0, size);
}

int32_t getPartnerID()
{
	return ttyd::mario_party::marioGetPartyId();
}

int32_t getFollowerID()
{
	return ttyd::mario_party::marioGetExtraPartyId();
}

void *getPartnerPointer()
{
	int32_t PartyID = getPartnerID();
	if (PartyID < 0)
	{
		return nullptr;
	}
	
	return ttyd::party::partyGetPtr(PartyID);
}

void *getFollowerPointer()
{
	int32_t PartyID = getFollowerID();
	if (PartyID < 0)
	{
		return nullptr;
	}
	
	return ttyd::party::partyGetPtr(PartyID);
}

void removePartnerFromOverworld()
{
	int32_t PartyID = getPartnerID();
	if (PartyID >= 0)
	{
		ttyd::party::partyKill2(PartyID);
	}
}

void removeFollowerFromOverworld()
{
	int32_t PartyID = getFollowerID();
	if (PartyID >= 0)
	{
		ttyd::party::partyKill2(PartyID);
	}
}

bool checkIfBadgeEquipped(int16_t badge)
{
	return ttyd::mario_pouch::pouchEquipCheckBadge(badge) > 0;
}

void recheckJumpAndHammerLevels()
{
	ttyd::mario_pouch::pouchGetJumpLv();
	ttyd::mario_pouch::pouchGetHammerLv();
}

uint32_t getCurrentPitFloor()
{
	return ttyd::swdrv::swByteGet(1321) + 1; // GSW(1321);
}

void clearGSWFsRange(uint32_t lowerBound, uint32_t upperBound)
{
	for (uint32_t i = lowerBound; i <= upperBound; i++)
	{
		ttyd::swdrv::swClear(i);
	}
}

}