.global asmPreventDiaryTextboxSelection

asmPreventDiaryTextboxSelection:
mr %r3,%r30 # currentText
addi %r4,%r29,0x9C # storeAddress
mr %r5,%r0 # selectedOption
b cPreventDiaryTextboxOptionSelection
