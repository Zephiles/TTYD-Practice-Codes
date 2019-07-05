.global StartForceNPCItemDrop

StartForceNPCItemDrop:
stwu %sp,-0x10(%sp)
mflr %r0
stw %r0,0x14(%sp)
stw %r31,0xC(%sp)
mr %r31,%r4

# r3 already contains ptr
bl forceNPCItemDrop
mr %r5,%r3

mr %r4,%r31
lwz %r31,0xC(%sp)
lwz %r0,0x14(%sp)
mtlr %r0
addi %sp,%sp,0x10
blr