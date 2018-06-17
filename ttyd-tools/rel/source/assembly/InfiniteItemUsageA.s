.global StartInfiniteItemUsage
.global BranchInfiniteItemUsage

#ifdef TTYD_US
  button_input_high = 0x803D
  button_input_low = -0x5C68
#elif defined TTYD_JP
  button_input_high = 0x803C
  button_input_low = 0x6818
#elif defined TTYD_EU
  button_input_high = 0x803D
  button_input_low = 0x63F8
#endif

StartInfiniteItemUsage:
lis %r5,InfiniteItemsUsageCombo@ha
lhz %r12,InfiniteItemsUsageCombo@l(%r5)

lis %r5,button_input_high
lhz %r5,button_input_low(%r5)
and %r5,%r5,%r12
cmpw %r5,%r12
beqlr-
stwu %sp,-0x20(%sp)

BranchInfiniteItemUsage:
b 0