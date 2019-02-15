.global StartAllowRunningFromBattles
.global BranchBackAllowRunningFromBattles

StartAllowRunningFromBattles:
# r3 already contains ptr
bl allowRunningFromBattles
mr %r0,%r3

BranchBackAllowRunningFromBattles:
b 0