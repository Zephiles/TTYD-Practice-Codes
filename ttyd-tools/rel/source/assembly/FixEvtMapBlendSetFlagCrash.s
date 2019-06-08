.global StartFixEvtMapBlendSetFlagCrash

StartFixEvtMapBlendSetFlagCrash:
stwu %sp,-0x8(%sp)
mflr %r0
stw %r0,0xC(%sp)

# r3 already contains partnerPtr
bl fixEvtMapBlendSetFlagCrash

lwz %r0,0xC(%sp)
mtlr %r0
addi %sp,%sp,0x8

# Restore the overwritten instruction
cmpwi %r31,1
blr