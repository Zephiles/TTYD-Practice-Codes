.global asmDisableDPadOptionsDisplay

asmDisableDPadOptionsDisplay:
stwu %sp,-0x10(%sp)
mflr %r3
stw %r3,0x14(%sp)

# Backup important register values
stw %r4,0x8(%sp)

mr %r3,%r0 # unkVar
bl cDisableDPadOptionsDisplay

# Check the returned bool
cmpwi %r3,0 # False

# Restore important register values
lwz %r4,0x8(%sp)

lwz %r0,0x14(%sp)
mtlr %r0
addi %sp,%sp,0x10
blr
