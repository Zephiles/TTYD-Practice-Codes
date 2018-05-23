#pragma once

namespace ttyd::swdrv {
	
extern "C" {

// _swByteGet
// _swByteSet
// _swClear
// _swGet
// _swSet
uint32_t swByteGet(uint32_t GSWOffset);
void swByteSet(uint32_t GSWOffset, uint32_t Value);
// swClear
// swGet
// swSet
// swReInit
// swInit

}

}