#ifdef TTYD_JP

.global asmCrashScreenDecrementYPosStart
.global asmCrashScreenDecrementYPosBranchBack

asmCrashScreenDecrementYPosStart:
bl crashScreenDecrementYPos
mr %r25,%r3 # posY

asmCrashScreenDecrementYPosBranchBack:
b 0

#endif
