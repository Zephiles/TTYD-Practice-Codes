#pragma once

#include <cstdint>

namespace ttyd::mario_pouch {

extern "C" {

// pouchGetYoshiName
// pouchSetYoshiName
void pouchSetPartyColor(uint32_t partnerId, uint32_t colorId);
uint32_t pouchGetPartyColor(uint32_t partnerId);
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
bool pouchUnEquipBadgeID(int16_t badge);
bool pouchEquipBadgeID(int16_t badge);
// pouchEquipCheckBadgeIndex
uint32_t pouchEquipCheckBadge(int16_t badge);
// pouchUnEquipBadgeIndex
// pouchEquipBadgeIndex
// pouchGetStarPoint
void pouchRevisePartyParam();
void pouchReviseMarioParam();
// pouchRemoveKeepItem
// pouchAddKeepItem
int16_t pouchGetPartyAttackLv(uint32_t partnerId);
uint32_t pouchGetHammerLv();
uint32_t pouchGetJumpLv();
// pouchSetAudienceNum
// pouchGetAudienceNum
int16_t pouchGetMaxAP();
void pouchSetAP(int16_t sp);
// pouchAddAP
// pouchGetAP
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
bool pouchRemoveItemIndex(int16_t item, uint32_t index);
// pouchRemoveItem
uint32_t pouchCheckItem(int16_t item);
bool pouchGetItem(int16_t item);
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
void *pouchGetPtr();

}

}