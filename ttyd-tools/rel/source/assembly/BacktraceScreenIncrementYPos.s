.global asmBacktraceScreenIncrementYPos

asmBacktraceScreenIncrementYPos:
stwu %sp,-0x10(%sp)
mflr %r0
stw %r0,0x14(%sp)

bl backtraceScreenIncrementYPos
mr %r25,%r3 # posY

lwz %r0,0x14(%sp)
mtlr %r0
addi %sp,%sp,0x10
blr