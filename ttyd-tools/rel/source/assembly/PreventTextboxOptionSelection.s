.global StartPreventTextboxSelection
.global BranchBackPreventTextboxSelection

StartPreventTextboxSelection:
mr %r3,%r30 # currentText
mr %r4,%r29 # storeAddress
mr %r5,%r0 # selectedOption
bl preventTextboxOptionSelection

BranchBackPreventTextboxSelection:
b 0