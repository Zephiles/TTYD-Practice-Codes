.global StartPhantomEmberDrop
.global BranchBackPhantomEmberDrop

#ifdef TTYD_US
  map_las_25_low = 0x0A84
  next_map_offset = 0x16A8
#elif defined TTYD_JP
  map_las_25_low = 0x2804
  next_map_offset = 0x1148
#elif defined TTYD_EU
  map_las_25_low = 0xC604
  next_map_offset = 0x1788
#endif

StartPhantomEmberDrop:
stwu %sp,-0x10(%sp)
stmw %r30,0x8(%sp)
mr %r30,%r3
mr %r31,%r4

lis %r3,0x802C
ori %r3,%r3,map_las_25_low
addi %r4,%r13,next_map_offset
bl strcmp
cmpwi %r3,0
mr %r3,%r30
bne+ Exit

lwz %r5,-0x6F50(%r13)
lwz %r5,0x174(%r5) # SequencePosition
cmpwi %r5,389
bgt- Exit

li %r5,138 # SleepySheep
b 0x8

Exit:
lwz %r5,0x23C(%r3)
mr %r4,%r31
lmw %r30,0x8(%sp)
addi %sp,%sp,0x10

BranchBackPhantomEmberDrop:
b 0