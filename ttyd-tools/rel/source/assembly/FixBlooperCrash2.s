.global StartFixBlooperCrash2

#ifdef TTYD_US
offset = 0x218
#elif defined TTYD_JP
offset = 0x214
#elif defined TTYD_EU
offset = 0x218
#endif

StartFixBlooperCrash2:
stwu %sp,-0x10(%sp)
mflr %r0
stw %r0,0x14(%sp)
stmw %r30,0x8(%sp)
mr %r31,%r3
mr %r30,%r4

mr %r3,%r4 # battleUnitPointer
bl checkIfPointerIsValid

# Check the returned bool
cmpwi %r3,0 # False
beq- ExitFunction

mr %r3,%r31
mr %r4,%r30
lmw %r30,0x8(%sp)
lwz %r0,0x14(%sp)
mtlr %r0
addi %sp,%sp,0x10

# Restore the overwritten instruction
lwz %r5,offset(%r4)
blr

ExitFunction:
lmw %r28,0x20(%sp)
li %r3,2
lwz %r0,0x34(%sp)
mtlr %r0
addi %sp,%sp,0x30
blr