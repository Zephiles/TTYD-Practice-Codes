.global StartForceNPCItemDrop
.global BranchBackForceNPCItemDrop

StartForceNPCItemDrop:
stwu %sp,-0xC(%sp)
stw %r31,0x8(%sp)
mr %r31,%r4

# r3 already contains ptr
bl forceNPCItemDrop
mr %r5,%r3

mr %r4,%r31
lwz %r31,0x8(%sp)
addi %sp,%sp,0xC

BranchBackForceNPCItemDrop:
b 0