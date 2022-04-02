.global asmFontDrawMessageMtxHandleCommand

asmFontDrawMessageMtxHandleCommand:
# Restore the original instruction
mr %r24,%r0

stwu %sp,-0x10(%sp)
mflr %r0
stw %r0,0x14(%sp)

addi %r3,%sp,0x180 # command # -> 0x170 + 0x10
addi %r4,%sp,0x100 # commandValueString # -> 0xF0 + 0x10
bl fontDrawMessageMtxHandleCommand

lwz %r0,0x14(%sp)
mtlr %r0
addi %sp,%sp,0x10

# Check the returned value
cmpwi %r3,0 # False

# r3 is needed if False is returned, so restore the original value for it
addi %r3,%sp,0x170
beqlr+

b (FontDrawMessageMtx + 0x45C) # 0x45C offset is the same for all retail versions