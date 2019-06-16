#pragma once

#include <cstdint>

namespace gc::ppc {

typedef union WriteGatherPipe
{
	int8_t 		s8;
	int16_t 	s16;
	int32_t 	s32;
	int64_t 	s64;
	uint8_t 	u8;
	uint16_t 	u16;
	uint32_t 	u32;
	uint64_t 	u64;
	float 		f32;
	double 		f64;
} WriteGatherPipe;

extern "C" {

extern volatile WriteGatherPipe writeGatherPipe;

}

}