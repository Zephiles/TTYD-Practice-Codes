.global StartActionCommands
.global BranchBackActionCommands

#ifdef TTYD_US
button_input_high = 0x803D
button_input_low = -0x5C68
store_byte = 0x307
#elif defined TTYD_JP
button_input_high = 0x803C
button_input_low = 0x6818
store_byte = 0x303
#elif defined TTYD_EU
button_input_high = 0x803D
button_input_low = 0x63F8
store_byte = 0x30B
#endif

StartActionCommands:
lwz %r3,0x1C90(%r4)
cmplwi %r3,0
beq- Exit

lis %r5,AutoActionCommandsCombo@ha
lhz %r0,AutoActionCommandsCombo@l(%r5)

lis %r5,button_input_high
lhz %r5,button_input_low(%r5)
and %r5,%r5,%r0
cmpw %r5,%r0
li %r0,0
bne+ 0x8

li %r0,1
stb %r0,store_byte(%r3)

Exit:
lwz %r0,0xEF4(%r4)

BranchBackActionCommands:
b 0