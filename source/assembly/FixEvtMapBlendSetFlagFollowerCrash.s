.global asmFixEvtMapBlendSetFlagFollowerCrashStart
.global asmFixEvtMapBlendSetFlagFollowerCrashBranchBack

asmFixEvtMapBlendSetFlagFollowerCrashStart:
# r3 already contains followerPtr
li %r4,0 # False
bl cFixEvtMapSetFlagCrash

# Restore the overwritten instruction
cmpwi %r31,1

asmFixEvtMapBlendSetFlagFollowerCrashBranchBack:
b 0
