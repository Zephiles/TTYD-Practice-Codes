.global StartDisableBattles
.global BranchBackDisableBattles

StartDisableBattles:
mr %r3,%r30 # ptr
bl disableBattles
mr %r21,%r3

# Restore overwritten CR value(s)
rlwinm. %r26,%r19,0,27,27

BranchBackDisableBattles:
b 0