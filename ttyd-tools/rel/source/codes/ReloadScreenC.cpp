#include "mod.h"

#include <ttyd/system.h>
#include <ttyd/seqdrv.h>
#include <ttyd/mariost.h>

#include <cstdio>

extern uint32_t r13;
extern uint16_t ReloadScreenCombo;

namespace mod {

void Mod::reloadScreen()
{
	if ((ttyd::system::keyGetButton(0) & ReloadScreenCombo) == (ReloadScreenCombo))
	{
		uint32_t NextSeq = ttyd::seqdrv::seqGetNextSeq();
		uint32_t Game = static_cast<uint32_t>(ttyd::seqdrv::SeqIndex::kGame);
		uint32_t MapChange = static_cast<uint32_t>(ttyd::seqdrv::SeqIndex::kMapChange);
		
		if ((NextSeq >= Game) && (NextSeq <= MapChange))
		{
			#ifdef TTYD_US
			uint32_t unkDisplayAddress = *reinterpret_cast<uint32_t *>(r13 + 0x1700);
			uint32_t unkCameraDisplayAddress = *reinterpret_cast<uint32_t *>(0x803C95F8);
			uint32_t NextSeqAddress = r13 - 0x7D8C;
			uint32_t SeqAddress = r13 - 0x7D90;
			#elif defined TTYD_JP
			uint32_t unkDisplayAddress = *reinterpret_cast<uint32_t *>(r13 + 0x11A0);
			uint32_t unkCameraDisplayAddress = *reinterpret_cast<uint32_t *>(0x803C5A78);
			uint32_t NextSeqAddress = r13 - 0x7DAC;
			uint32_t SeqAddress = r13 - 0x7DB0;
			#elif defined TTYD_EU
			uint32_t unkDisplayAddress = *reinterpret_cast<uint32_t *>(r13 + 0x17E0);
			uint32_t unkCameraDisplayAddress = *reinterpret_cast<uint32_t *>(0x803D5658);
			uint32_t NextSeqAddress = r13 - 0x7D8C;
			uint32_t SeqAddress = r13 - 0x7D90;
			#endif
			
			ttyd::mariost::marioStSystemLevel(0);
			*reinterpret_cast<uint32_t *>(unkDisplayAddress + 0x20) = 0;
			*reinterpret_cast<float *>(unkDisplayAddress + 0x2C) = 1;
			*reinterpret_cast<uint32_t *>(unkDisplayAddress + 0xC) = 10;
			*reinterpret_cast<uint16_t *>(unkCameraDisplayAddress + 0xA00) = 4;
			*reinterpret_cast<uint32_t *>(NextSeqAddress) = 3;
			*reinterpret_cast<uint32_t *>(SeqAddress) = 3;
		}
	}
}

}