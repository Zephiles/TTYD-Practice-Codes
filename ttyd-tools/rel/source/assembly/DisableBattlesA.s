.global StartDisableBattles
.global BranchBackDisableBattles

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

StartDisableBattles:
lis %r21,DisableBattlesCombo@ha
lhz %r21,DisableBattlesCombo@l(%r21)

lis %r26,button_input_high
lhz %r26,button_input_low(%r26)
and %r26,%r26,%r21
cmpw %r26,%r21
bne+ Exit

li %r21,0
b 0x8

Exit:
lwz %r21,0x4(%r30)
rlwinm. %r26,%r19,0,27,27

BranchBackDisableBattles:
b 0