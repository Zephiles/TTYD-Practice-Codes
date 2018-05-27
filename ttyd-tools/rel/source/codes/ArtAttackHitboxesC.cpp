#include "mod.h"
#include "patch.h"

#include <cstdio>

namespace mod {

extern "C" {
void StartArtAttackHitboxes();
void BranchArtAttackHitboxes();
}

void Mod::artAttackHitboxes()
{
  #ifdef TTYD_US
    uint32_t HookAddress = 0x80231928;
    uint32_t stwu_address = 0x80231704;
  #elif defined TTYD_JP
    uint32_t HookAddress = 0x8022C278;
    uint32_t stwu_address = 0x8022C054;
  #elif defined TTYD_EU
    uint32_t HookAddress = 0x802353B8;
    uint32_t stwu_address = 0x80235194;
  #endif
  
  uint32_t stw_r0_address = stwu_address + 0x8;
  uint32_t stmw_address = stw_r0_address + 0x4;
  uint32_t addi_r3_address = stmw_address + 0x178;
  uint32_t stw_r6_address = addi_r3_address + 0x8;
  uint32_t stb_r0_address = stw_r6_address + 0x18;
  uint32_t lwz_r0_address = stb_r0_address + 0x4;
  uint32_t stw_r0_2_address = lwz_r0_address + 0x4;
  uint32_t stw_r3_address = stw_r0_2_address + 0x8;
  uint32_t addi_r4_address = stw_r3_address + 0x4;
  uint32_t lwz_r0_2_address = addi_r4_address + 0x70;
  uint32_t addi_sp_address = lwz_r0_2_address + 0x8;

  // Art Attack assembly overwrites
  *reinterpret_cast<uint8_t *>(stwu_address + 0x3) = 0x60;
  *reinterpret_cast<uint8_t *>(stw_r0_address + 0x3) = 0xA4;
  *reinterpret_cast<uint8_t *>(stmw_address + 0x3) = 0x48;
  *reinterpret_cast<uint8_t *>(addi_r3_address + 0x3) = 0xC;
  *reinterpret_cast<uint8_t *>(stw_r6_address + 0x3) = 0x14;
  *reinterpret_cast<uint8_t *>(stb_r0_address + 0x3) = 0x14;
  *reinterpret_cast<uint8_t *>(lwz_r0_address + 0x3) = 0x14;
  *reinterpret_cast<uint8_t *>(stw_r0_2_address + 0x3) = 0xC;
  *reinterpret_cast<uint8_t *>(stw_r3_address + 0x3) = 0x10;
  *reinterpret_cast<uint8_t *>(addi_r4_address + 0x3) = 0x10;
  *reinterpret_cast<uint8_t *>(lwz_r0_2_address + 0x3) = 0xA4;
  *reinterpret_cast<uint8_t *>(addi_sp_address + 0x3) = 0xA0;

  patch::writeBranch(reinterpret_cast<void *>(HookAddress), reinterpret_cast<void *>(StartArtAttackHitboxes));
  patch::writeBranch(reinterpret_cast<void *>(BranchArtAttackHitboxes), reinterpret_cast<void *>(HookAddress + 0x4));
}

}