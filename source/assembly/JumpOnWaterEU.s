#ifdef TTYD_EU

.global asmJumpOnWaterStart
.global asmJumpOnWaterBranchBack

asmJumpOnWaterStart:
# r3 already contains ptr
bl cJumpOnWater

# Restore the overwritten instruction
cmplwi %r3,0

asmJumpOnWaterBranchBack:
b 0

#endif
