#ifdef TTYD_JP

.global asmCrashScreenDecrementYPosStart
.global asmCrashScreenDecrementYPosBranchBack

asmCrashScreenDecrementYPosStart:
bl cCrashScreenDecrementYPos
mr %r25,%r3 # posY

asmCrashScreenDecrementYPosBranchBack:
b 0

#endif
