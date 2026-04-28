# At least two separate hooks use this assembly, so this assembly must use the link register to return
.global asmFallThroughMostObjectsStandard

asmFallThroughMostObjectsStandard:
stwu %sp,-0x10(%sp)
mflr %r0
stw %r0,0x14(%sp)

# r3 already contains ptr
bl cFallThroughMostObjects

# Check the returned pointer
cmplwi %r3,0 # nullptr

lwz %r0,0x14(%sp)
mtlr %r0
addi %sp,%sp,0x10
blr
