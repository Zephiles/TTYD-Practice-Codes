.global asmFontDrawMessageMtxHandleCol

#ifdef TTYD_US
AddressToJumpTo = 0x80075D84
#elif defined TTYD_JP
AddressToJumpTo = 0x80074AC0
#elif defined TTYD_EU
AddressToJumpTo = 0x80077040
#endif

asmFontDrawMessageMtxHandleCol:
# Restore the original instruction
mr %r24,%r0

stwu %sp,-0x10(%sp)
mflr %r0
stw %r0,0x14(%sp)

addi %r3,%sp,0x180 # command
addi %r4,%sp,0x100 # colorString
bl fontDrawMessageMtxHandleCol

# Check the returned value
cmpwi %r3,0 # False

lwz %r0,0x14(%sp)
mtlr %r0
addi %sp,%sp,0x10
bne- jumpPastOtherCommands

# r3 is needed, so restore the original value for that
addi %r3,%sp,0x170
blr

jumpPastOtherCommands:
lis %r3,AddressToJumpTo@h
ori %r3,%r3,AddressToJumpTo@l
mtctr %r3
bctr