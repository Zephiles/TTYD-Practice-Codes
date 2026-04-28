.global asmArbitraryMemoryWriteGetProperPointerStart
.global asmArbitraryMemoryWriteGetProperPointerBranchBack

asmArbitraryMemoryWriteGetProperPointerStart:
# r3 already contains pointerRaw
mr %r4,%r0 # multipliedIndex
bl cArbitraryMemoryWriteGetProperPointer
mr %r31,%r3

asmArbitraryMemoryWriteGetProperPointerBranchBack:
b 0
