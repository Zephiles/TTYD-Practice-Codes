.global StartDrawArtAttackHitboxes

StartDrawArtAttackHitboxes:
stwu %sp,-0x10(%sp)
mflr %r0
stw %r0,0x14(%sp)

bl drawArtAttackHitboxes

lwz %r0,0x14(%sp)
mtlr %r0
addi %sp,%sp,0x10

# Restore the overwritten instruction
lmw %r26,0x18(%sp)
blr