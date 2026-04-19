.global asmHandleTubeModeStorage

asmHandleTubeModeStorage:
# Push the stack
stwu %sp,-0x10(%sp)
mflr %r0
stw %r0,0x14(%sp)

# Backup important register values
stw %r3,0x8(%sp)

bl cHandleTubeModeStorage
cmpwi %r3,0 # False

# Restore important register values
lwz %r3,0x8(%sp)

# Pop the stack
lwz %r0,0x14(%sp)
mtlr %r0
addi %sp,%sp,0x10

bne- tubeModeStorageActive

# Restore the overwritten instruction
cmpwi %r3,1
blr

tubeModeStorageActive:
b (marioRollChgChk + 0x78)
