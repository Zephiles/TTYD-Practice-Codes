#pragma once

#include <cstdint>

namespace ttyd::battle_unit {

// There may be more values for ActorGroupBelong
enum class ActorGroupBelong : uint8_t
{
	kParty = 0,
	kEnemy,
	kExtra,
};

// Some of these entries may be different for JP and/or EU
struct AttackParams
{
	const char *nameMsg;
	uint16_t iconId;
	uint16_t unk_0x06;
	uint16_t unk_0x08;
	int16_t itemId;
	char *unk_0x0c;
	uint8_t unk_0x10; // Always 0x64?
	int8_t baseFpCost;
	int8_t baseSpCost;
	int8_t guardTypesAllowed; // 0 = none, 2 = both; ? = guard only
	float unk_0x14;
	uint32_t unk_0x18; // Possibly separate 8-bit values, or flags?

	char *baseDamageFn;
	// Parameters for damage function.
	int32_t baseDamage1;
	int32_t baseDamage2;
	int32_t baseDamage3;
	int32_t baseDamage4;
	int32_t baseDamage5;
	int32_t baseDamage6;
	uint32_t unk_0x38;
	uint32_t unk_0x3c;

	char *baseFpDamageFn;
	// Parameters for FP damage function.
	int32_t baseFpDamage;
	uint32_t unk_0x48;
	uint32_t unk_0x4c;
	uint32_t unk_0x50;
	uint32_t unk_0x54;
	uint32_t unk_0x58;
	uint32_t unk_0x5c;
	uint32_t unk_0x60;

	int8_t numTargets; // 1 for single-target, 2 for multi-target
	uint8_t unk_0x65;
	uint8_t unk_0x66;
	uint8_t unk_0x67;
	uint8_t allowedTargetTypes;
	uint8_t unk_0x69; // Used for enemy-knockback attacks (like Gulp)?
	uint8_t unk_0x6a; // Seems to correlate with allowed targets?
	uint8_t unk_0x6b; // Seems to indicate effects like flip, quake...
	int8_t element;
	uint8_t unk_0x6d;
	uint8_t unk_0x6e; // Always 0x03?
	uint8_t unk_0x6f; // Always 0x02?
	const char *acMsg;
	uint32_t unk_0x74_flags;
	uint32_t unk_0x78_flags;
	uint32_t unk_0x7c_flags; // May be flags; generally similar values.

	int8_t sleepChance;
	int8_t sleepTime;
	int8_t stopChance;
	int8_t stopTime;

	int8_t dizzyChance;
	int8_t dizzyTime;
	int8_t poisonChance;
	int8_t poisonTime;

	int8_t poisonStrength;
	int8_t confuseChance;
	int8_t confuseTime;
	int8_t electricChance;

	int8_t electricTime;
	int8_t dodgyChance;
	int8_t dodgyTime;
	int8_t burnChance;

	int8_t burnTime;
	int8_t freezeChance;
	int8_t freezeTime;
	int8_t sizeChangeChance;

	int8_t sizeChangeTime;
	int8_t sizeChangeStrength;
	int8_t attackChangeChance;
	int8_t attackChangeTime;

	int8_t attackChangeStrength;
	int8_t defenseChangeChance;
	int8_t defenseChangeTime;
	int8_t defenseChangeStrength;

	int8_t allergicChance;
	int8_t allergicTime;
	int8_t ohkoChance;
	int8_t chargeStrength;

	int8_t fastChance;
	int8_t fastTime;
	int8_t slowChance;
	int8_t slowTime;

	int8_t frightChance;
	int8_t galeForceChance;
	int8_t paybackTime;
	int8_t holdFastTime;

	int8_t invisibleChance;
	int8_t invisibleTime;
	int8_t hpRegenTime;
	int8_t hpRegenStrength;

	int8_t fpRegenTime;
	int8_t fpRegenStrength;
	uint8_t unk_0xae;
	uint8_t unk_0xaf;

	char *itemScriptPtr;

	int8_t backgroundA1A2FallWeight;
	int8_t backgroundA1FallWeight;
	int8_t backgroundA2FallWeight;
	int8_t backgroundNoFallWeight;
	int8_t backgroundBFallChance;
	int8_t nozzleChangeDirectionChance;
	int8_t nozzleFireChance;
	int8_t ironFrameFallChance;
	int8_t objectFallChance;
	uint8_t unk_0xbd;
	uint8_t unk_0xbe;
	uint8_t unk_0xbf;
} __attribute__((__packed__));

extern "C" {

// BtlUnit_CheckShadowGuard
// BtlUnit_EnemyItemCanUseCheck
// BtlUnit_HpGaugeMain
// BtlUnit_HpGaugeInit
// BtlUnit_snd_se_pos
// BtlUnit_snd_se
// BtlUnit_ControlPoseSoundMain
// BtlUnit_PoseSoundInit
// BtlUnit_SetCommandAnimPose
// BtlUnit_SetSeMode
// BtlUnit_LoadSeMode
// BtlUnit_ResetMoveStatus
// BtlUnit_GetGuardKouraPtr
// BtlUnit_PayWeaponCost
// BtlUnit_CheckWeaponCost
// BtlUnit_GetWeaponCost
// BtlUnit_SetMaxFp
// BtlUnit_GetMaxFp
// BtlUnit_RecoverFp
// BtlUnit_RecoverHp
// BtlUnit_SetFp
// BtlUnit_GetFp
// BtlUnit_GetCoin
// BtlUnit_GetExp
// BtlUnit_CheckPinchStatus
// BtlUnit_SetParamToPouch
// BtlUnit_ReviseHpFp
// BtlUnit_SetParamFromPouch
// BtlUnit_CanActStatus
// BtlUnit_CanGuardStatus
// BtlUnit_CheckData
// BtlUnit_GetData
// BtlUnit_GetACPossibility
// BtlUnit_SetTotalHitDamage
// BtlUnit_GetTotalHitDamage
// BtlUnit_GetHitDamage
// BtlUnit_GetEnemyBelong
// BtlUnit_GetTalkTogePos
// BtlUnit_ChangeStayAnim
// BtlUnit_ChangeTalkAnim
// BtlUnit_SetBodyAnim
// BtlUnit_SetBodyAnimType
// BtlUnit_SetAnim
// BtlUnit_SetAnimType
// BtlUnit_GetPoseNameFromType
// BtlUnit_OffUnitFlag
// BtlUnit_OnUnitFlag
// BtlUnit_CheckUnitFlag
// BtlUnit_OffStatusFlag
// BtlUnit_OnStatusFlag
// BtlUnit_CheckStatusFlag
bool BtlUnit_CheckStatus(void *battleUnitPtr, uint32_t status);
// BtlUnit_CheckRecoveryStatus
// BtlUnit_ClearStatus
// BtlUnit_SetStatus
// BtlUnit_GetStatus
// _CheckMoveCount
ActorGroupBelong BtlUnit_GetBelong(void *battleUnitPtr);
// BtlUnit_SetJumpSpeed
// BtlUnit_SetPartsMoveSpeed
// BtlUnit_SetMoveSpeed
// BtlUnit_SetPartsFallAccel
// BtlUnit_SetFallAccel
// BtlUnit_SetPartsMoveTargetPos
// BtlUnit_SetMoveTargetPos
// BtlUnit_SetPartsMoveCurrentPos
// BtlUnit_SetMoveCurrentPos
// BtlUnit_SetPartsMoveStartPos
// BtlUnit_SetMoveStartPos
// BtlUnit_AddPartsDispOffset
// BtlUnit_SetPartsDispOffset
// BtlUnit_SetDispOffset
// BtlUnit_AddPartsOffsetPos
// BtlUnit_SetPartsOffsetPos
// BtlUnit_GetPartsOffsetPos
// BtlUnit_SetOffsetPos
// BtlUnit_SetHeight
// BtlUnit_GetHeight
// BtlUnit_GetWidth
// BtlUnit_AddPartsScale
// BtlUnit_SetPartsScale
// BtlUnit_SetPartsBaseScale
// BtlUnit_AddScale
// BtlUnit_SetScale
// BtlUnit_GetScale
// BtlUnit_SetBaseScale
// BtlUnit_AddPartsRotateOffset
// BtlUnit_SetPartsRotateOffset
// BtlUnit_SetRotateOffset
// BtlUnit_GetPartsBaseRotate
// BtlUnit_SetPartsBaseRotate
// BtlUnit_GetBaseRotate
// BtlUnit_SetBaseRotate
// BtlUnit_AddPartsRotate
// BtlUnit_GetPartsRotate
// BtlUnit_SetPartsRotate
// BtlUnit_AddRotate
// BtlUnit_GetRotate
// BtlUnit_SetRotate
// BtlUnit_SetPartsHomePos
// BtlUnit_AddHomePos
// BtlUnit_SetHomePos
// BtlUnit_GetHomePos
// BtlUnit_SetHitCursorOffset
// BtlUnit_SetHitOffset
// BtlUnit_GetHitPos
// BtlUnit_GetPartsWorldPos
// BtlUnit_AddPartsPos
// BtlUnit_SetPartsPos
// BtlUnit_GetPartsPos
// BtlUnit_AddPos
// BtlUnit_SetPos
void BtlUnit_GetPos(void *battleUnitPtr, float *posXOut, float *posYOut, float *posZOut);
int32_t BtlUnit_GetBodyPartsId(void *battleUnitPtr);
void *BtlUnit_GetPartsPtr(void *battleUnitPtr, int32_t bodyPartsId);
// BtlUnit_GetUnitId
// BtlUnit_Spawn
// BtlUnit_Delete
// BtlUnit_Entry
// BtlUnit_Init

}

}
