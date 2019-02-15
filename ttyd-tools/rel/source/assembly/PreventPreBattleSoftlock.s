.global StartPreventPreBattleSoftlock
.global BranchBackPreventPreBattleSoftlock

StartPreventPreBattleSoftlock:
mr %r3,%r24 # fbatPointer
bl preventPreBattleSoftlock

BranchBackPreventPreBattleSoftlock:
b 0