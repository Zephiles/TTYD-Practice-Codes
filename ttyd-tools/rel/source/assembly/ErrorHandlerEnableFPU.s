.global StartErrorHandlerEnableFPU

StartErrorHandlerEnableFPU:
mfmsr %r3
ori %r3,%r3,0x2000
mtmsr %r3
isync
blr