.global StartAutoActionCommands
.global BranchBackAutoActionCommands

StartAutoActionCommands:
stwu %sp,-0xC(%sp)
stw %r31,0x8(%sp)
mflr %r31

mr %r3,%r4 # ptr
bl autoActionCommands
mr %r0,%r3

mtlr %r31
lwz %r31,0x8(%sp)
addi %sp,%sp,0xC

BranchBackAutoActionCommands:
b 0