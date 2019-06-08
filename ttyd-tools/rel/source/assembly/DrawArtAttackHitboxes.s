.global StartDrawArtAttackHitboxes

StartDrawArtAttackHitboxes:
stwu %sp,-0x8(%sp)
mflr %r0
stw %r0,0xC(%sp)

bl drawArtAttackHitboxes

lwz %r0,0xC(%sp)
mtlr %r0
addi %sp,%sp,0x8

# Restore the overwritten instruction
lmw %r26,0x18(%sp)
blr