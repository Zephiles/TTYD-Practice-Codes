.global StartErrorHandlerInterrupts

StartErrorHandlerInterrupts:
mfmsr %r3
ori %r3,%r3,0x8002
mtmsr %r3
isync
blr