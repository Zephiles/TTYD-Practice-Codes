.global StartFixEvtMapBlendSetFlagFollowerCrash

StartFixEvtMapBlendSetFlagFollowerCrash:
stwu %sp,-0x10(%sp)
mflr %r0
stw %r0,0x14(%sp)

# r3 already contains followerPtr
bl fixEvtMapBlendSetFlagFollowerCrash

lwz %r0,0x14(%sp)
mtlr %r0
addi %sp,%sp,0x10

# Restore the overwritten instruction
cmpwi %r31,1
blr