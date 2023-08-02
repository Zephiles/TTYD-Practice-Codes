.global asmAllowRunningFromBattlesStart
.global asmAllowRunningFromBattlesBranchBack

asmAllowRunningFromBattlesStart:
# r3 already contains ptr
bl allowRunningFromBattles
mr %r0,%r3

asmAllowRunningFromBattlesBranchBack:
b 0
