.global asmFontDrawMessageMtxHandleCommandStart
.global asmFontDrawMessageMtxHandleCommandBranchBack

asmFontDrawMessageMtxHandleCommandStart:
# Restore the original instruction
mr %r24,%r0

# r3 already contains command
addi %r4,%sp,0xF0 # commandValueString
bl cFontDrawMessageMtxHandleCommand

# Check the returned value
cmpwi %r3,0 # False

# r3 is needed if False is returned, so restore the original value for it
addi %r3,%sp,0x170
bne- returnedTrue

asmFontDrawMessageMtxHandleCommandBranchBack:
b 0

returnedTrue:
b (FontDrawMessageMtx + 0x45C) # 0x45C offset is the same for all retail versions
