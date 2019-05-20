.global StartFixEvtMapBlendSetFlagCrash
.global BranchBackFixEvtMapBlendSetFlagCrash

StartFixEvtMapBlendSetFlagCrash:
# r3 already contains partnerPtr
bl fixEvtMapBlendSetFlagCrash

# Restore overwritten instruction
cmpwi %r31,1

BranchBackFixEvtMapBlendSetFlagCrash:
b 0