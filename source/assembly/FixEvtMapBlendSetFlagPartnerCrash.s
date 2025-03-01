.global asmFixEvtMapBlendSetFlagPartnerCrashStart
.global asmFixEvtMapBlendSetFlagPartnerCrashBranchBack

asmFixEvtMapBlendSetFlagPartnerCrashStart:
# r3 already contains partnerPtr
li %r4,1 # True
bl cFixEvtMapSetFlagCrash

# Restore the overwritten instruction
cmpwi %r31,1

asmFixEvtMapBlendSetFlagPartnerCrashBranchBack:
b 0
