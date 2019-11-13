.global StartDisplayMegaHammerBadgesBattleMenu

StartDisplayMegaHammerBadgesBattleMenu:
stwu %sp,-0x10(%sp)
mflr %r3
stw %r3,0x14(%sp)
stmw %r30,0x8(%sp)
mr %r31,%r4
mr %r30,%r5

mr %r3,%r0 # checkBit
bl displayMegaHammerBadgesInMenu

# Check the returned bool
cmpwi %r3,0 # False

mr %r4,%r31
mr %r5,%r30
lmw %r30,0x8(%sp)
lwz %r0,0x14(%sp)
mtlr %r0
addi %sp,%sp,0x10
blr