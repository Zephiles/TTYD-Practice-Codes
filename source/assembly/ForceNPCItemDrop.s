.global asmForceNpcItemDrop

asmForceNpcItemDrop:
stwu %sp,-0x10(%sp)
mflr %r0
stw %r0,0x14(%sp)

# Backup important register values
stw %r4,0x8(%sp)

# r3 already contains ptr
bl cForceNpcItemDrop
mr %r5,%r3

# Restore important register values
lwz %r4,0x8(%sp)

lwz %r0,0x14(%sp)
mtlr %r0
addi %sp,%sp,0x10
blr
