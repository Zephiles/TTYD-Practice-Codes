.global asmFixEvtMapBlendSetFlagPartnerCrashStart
.global asmFixEvtMapBlendSetFlagPartnerCrashBranchBack

asmFixEvtMapBlendSetFlagPartnerCrashStart:
# r3 already contains partnerPtr
bl cFixEvtMapBlendSetFlagPartnerCrash

# Restore the overwritten instruction
cmpwi %r31,1

asmFixEvtMapBlendSetFlagPartnerCrashBranchBack:
b 0
