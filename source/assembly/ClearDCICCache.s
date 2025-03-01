.global asmClearDCICCache

asmClearDCICCache:
# Make sure the size is valid
cmplwi %r4,0
blelr-

# Get the loop count
rlwinm %r5,%r3,0,27,31
add %r4,%r4,%r5
addi %r4,%r4,31
rlwinm %r4,%r4,27,5,31
mtctr %r4

asmClearDCICCacheLoopStart:
# Flush the data cache and invalidate the instruction cache
dcbf 0,%r3
icbi 0,%r3
addi %r3,%r3,32
bdnz+ asmClearDCICCacheLoopStart

# Done, so sync the caches and exit
sync
isync
blr
