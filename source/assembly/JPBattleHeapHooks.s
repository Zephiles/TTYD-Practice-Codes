.global asmAllocFromBattleHeap
.global asmFreeToBattleHeap

asmAllocFromBattleHeap:
# The code that is being jumped to requires the size to be in r4
mr %r4,%r3

# The code that is being jumped to requires the heap array to be in r5
lis %r5,gBattleHeapInfo@ha
lwz %r5,gBattleHeapInfo@l(%r5)
b (OSAllocFromHeap + 0xC)

asmFreeToBattleHeap:
# Push stack
mflr %r0
stw %r0,0x4(%sp)
stwu %sp,-0x18(%sp)
stw %r31,0x14(%sp)

# Restore skipped instructions
subi %r6,%r3,32
lwz %r3,0x4(%r6)

# The code that is being jumped to requires the heap array to be in r31
lis %r31,gBattleHeapInfo@ha
lwz %r31,gBattleHeapInfo@l(%r31)
b (OSFreeToHeap + 0x24)
