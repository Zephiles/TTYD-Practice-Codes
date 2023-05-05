#ifndef TTYD_MARIO_POUCH_H
#define TTYD_MARIO_POUCH_H

#include "ttyd/item_data.h"
#include "ttyd/party.h"

#include <cstdint>

struct PouchPartyData
{
    uint16_t flags;
    int16_t maxHp;
    int16_t baseMaxHp;
    int16_t currentHp;
    int16_t hpLevel;
    int16_t attackLevel;
    int16_t techLevel;
} __attribute__((__packed__));

// Taken from Jdaster64's Infinite Pit project:
// https://github.com/jdaster64/ttyd-infinite-pit/blob/master/ttyd-tools/rel/include/ttyd/mario_pouch.h
struct PouchData
{
    PouchPartyData partyData[8];
    int16_t currentHp;
    int16_t maxHp;
    int16_t currentFp;
    int16_t maxFp;
    int16_t coins;
    int16_t currentSp;
    int16_t maxSp;
    int8_t unk_07e[6];
    float audienceLevel;
    int16_t rank;
    int16_t level;
    uint16_t starPowersObtained; // Bitfield
    int16_t baseMaxHp;
    int16_t baseMaxFp;
    int16_t unallocatedBp;
    int16_t totalBp;
    int16_t starPoints;
    int8_t jumpLevel;
    int8_t hammerLevel;
    int16_t starPieces;
    int16_t shineSprites;
    int16_t powerBounceRecord;
    ItemId keyItems[121];
    ItemId items[20];
    ItemId storedItems[32];
    ItemId badges[200];
    ItemId equippedBadges[200];
    int8_t emailIds[100];
    int8_t pad_57e[2];
    uint32_t emailReceived[4]; // Bitfield
    uint32_t emailRead[4];     // Bitfield
    uint8_t unk_5a0[0x18];
    int8_t merleeCurseUsesRemaining;
    int8_t turnsUntilMerleeActivation;
    int8_t nextMerleeCurseType;
    int8_t superBowserCoins;
    int32_t superBowseScore;
    char yoshiName[16];
    int8_t unk_5d0[4];
} __attribute__((__packed__));

static_assert(sizeof(PouchPartyData) == 0xE);
static_assert(sizeof(PouchData) == 0x5D4);

extern "C"
{
    const char *pouchGetYoshiName();
    void pouchSetYoshiName(const char *name);
    void pouchSetPartyColor(PartyMembers partnerId, uint32_t colorId);
    uint32_t pouchGetPartyColor(PartyMembers partnerId);
    // pouchCheckMail
    // pouchOpenMail
    // pouchReceiveMail
    // pouchReceiveMailCount
    void pouchGetStarStone(uint32_t starId);
    // pouchAddKpaScore
    // pouchGetKpaScore
    // pouchAddKpaCoin
    // pouchGetKpaCoin
    // pouchMajinaiInit
    // pouchArriveBadge
    bool N_pouchUnEquipBadgeID(ItemId badge);
    bool pouchEquipBadgeID(ItemId badge);
    // pouchEquipCheckBadgeIndex
    uint32_t pouchEquipCheckBadge(ItemId badge);
    // pouchUnEquipBadgeIndex
    // pouchEquipBadgeIndex
    // pouchGetStarPoint
    void pouchRevisePartyParam();
    void pouchReviseMarioParam();
    // pouchRemoveKeepItem
    // pouchAddKeepItem
    int16_t pouchGetPartyAttackLv(PartyMembers partnerId);
    uint32_t pouchGetHammerLv();
    uint32_t pouchGetJumpLv();
    // pouchSetAudienceNum
    // pouchGetAudienceNum
    int16_t pouchGetMaxAP();
    void pouchSetAP(int16_t sp);
    // pouchAddAP
    int16_t pouchGetAP();
    // pouchSetMaxFP
    // pouchSetFP
    // pouchGetMaxFP
    // pouchGetFP
    // pouchSetPartyHP
    // pouchGetPartyHP
    // pouchSetMaxHP
    // pouchSetHP
    // pouchGetMaxHP
    // pouchGetHP
    // pouchAddHP
    // pouchAddStarPiece
    int16_t pouchGetStarPiece();
    // pouchSetSuperCoin
    // pouchGetSuperCoin
    // pouchSetCoin
    // pouchAddCoin
    int16_t pouchGetCoin();
    // pouchSortItem
    // comp_kind_r
    // comp_aiueo_r
    // comp_kind
    // comp_aiueo
    bool pouchRemoveItemIndex(ItemId item, uint32_t index);
    // pouchRemoveItem
    uint32_t pouchCheckItem(ItemId item);
    bool pouchGetItem(ItemId item);
    // pouchGetEmptyKeepItemCnt
    // pouchGetEmptyHaveItemCnt
    // pouchGetEquipBadgeCnt
    uint32_t pouchGetHaveBadgeCnt();
    // pouchGetKeepItemCnt
    // pouchGetHaveItemCnt
    // pouchHaveBadge
    // pouchKeepItem
    // pouchHaveItem
    // pouchKeyItem
    // pouchInit
    PouchData *pouchGetPtr();
}

#endif
