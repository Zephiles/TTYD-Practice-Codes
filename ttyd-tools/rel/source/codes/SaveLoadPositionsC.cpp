#include "mod.h"

#include <ttyd/system.h>
#include <ttyd/mario.h>
#include <ttyd/party.h>
#include <ttyd/mario_party.h>

#include <cstdio>

extern uint16_t SavePositions;
extern uint16_t LoadPositions;

namespace mod {

void Mod::saveLoadPositions()
{
  uint32_t ButtonInput = ttyd::system::keyGetButton(0);
  ttyd::mario::Player *player = ttyd::mario::marioGetPtr();
  uint32_t PartnerPointer = ttyd::party::partyGetPtr(ttyd::mario_party::marioGetPartyId());
  
  if ((ButtonInput & SavePositions) == (SavePositions))
  {
    // Save Positions
    MarioSavePositions[0] = player->playerPosition[0]; // Mario Coordinate X
    MarioSavePositions[1] = player->playerPosition[1]; // Mario Coordinate Y
    MarioSavePositions[2] = player->playerPosition[2]; // Mario Coordinate Z
    MarioSavePositions[3] = player->unk_1a0;
    
    if (PartnerPointer != 0)
    {
      // A Partner is currently out
      PartnerSavePositions[0] = *reinterpret_cast<float *>(PartnerPointer + 0x58); // Partner Coordinate X
      PartnerSavePositions[1] = *reinterpret_cast<float *>(PartnerPointer + 0x5C); // Partner Coordinate Y
      PartnerSavePositions[2] = *reinterpret_cast<float *>(PartnerPointer + 0x60); // Partner Coordinate Z
      PartnerSavePositions[3] = *reinterpret_cast<float *>(PartnerPointer + 0x100);
    }
  }
  else if ((ButtonInput & LoadPositions) == (LoadPositions))
  {
    // Load Positions
    player->playerPosition[0] = MarioSavePositions[0]; // Mario Coordinate X
    player->playerPosition[1] = MarioSavePositions[1]; // Mario Coordinate Y
    player->playerPosition[2] = MarioSavePositions[2]; // Mario Coordinate Z
    player->unk_1a0 = MarioSavePositions[3];
    
    if (PartnerPointer != 0)
    {
      // A Partner is currently out
      *reinterpret_cast<float *>(PartnerPointer + 0x58) = PartnerSavePositions[0]; // Partner Coordinate X
      *reinterpret_cast<float *>(PartnerPointer + 0x5C) = PartnerSavePositions[1]; // Partner Coordinate Y
      *reinterpret_cast<float *>(PartnerPointer + 0x60) = PartnerSavePositions[2]; // Partner Coordinate Z
      *reinterpret_cast<float *>(PartnerPointer + 0x100) = PartnerSavePositions[3];
    }
  }
}

}