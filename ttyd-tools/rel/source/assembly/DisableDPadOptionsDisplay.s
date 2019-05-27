.global StartDisableDPadOptionsDisplay
.global BranchBackDisableDPadOptionsDisplay

StartDisableDPadOptionsDisplay:
stwu %sp,-0xC(%sp)
stw %r31,0x8(%sp)
mr %r31,%r4

mr %r3,%r0 # unkVar
bl disableDPadOptionsDisplay

# Check the returned bool
cmpwi %r3,0 # False

mr %r4,%r31
lwz %r31,0x8(%sp)
addi %sp,%sp,0xC

BranchBackDisableDPadOptionsDisplay:
b 0