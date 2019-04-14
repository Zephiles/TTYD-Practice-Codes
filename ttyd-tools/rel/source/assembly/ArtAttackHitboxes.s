.global StartArtAttackHitboxes
.global BranchArtAttackHitboxes

StartArtAttackHitboxes:
bl displayArtAttackHitboxes

# Restore overwritten assembly
lmw %r26,0x18(%sp)

BranchArtAttackHitboxes:
b 0