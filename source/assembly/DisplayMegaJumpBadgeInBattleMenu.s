.global asmDisplayMegaJumpBadgeInBattleMenu

asmDisplayMegaJumpBadgeInBattleMenu:
stwu %sp,-0x10(%sp)
mflr %r3
stw %r3,0x14(%sp)

# Backup important register values
stw %r5,0x10(%sp)
stw %r4,0x8(%sp)

li %r3,1 # True
mr %r4,%r0 # bitfield
bl displayMegaBadgesInBattleMenu

# Check the returned bool
cmpwi %r3,0 # False

# Restore important register values
lwz %r4,0x8(%sp)
lwz %r5,0x10(%sp)

lwz %r0,0x14(%sp)
mtlr %r0
addi %sp,%sp,0x10
blr
