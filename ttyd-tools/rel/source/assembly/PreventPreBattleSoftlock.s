.global StartPreventPreBattleSoftlock

StartPreventPreBattleSoftlock:
mr %r3,%r24 # fbatPointer
b preventPreBattleSoftlock