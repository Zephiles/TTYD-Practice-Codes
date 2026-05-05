########################################################################################
# Fix Blooper crash

.global asmFixBlooperCrash1
.global asmFixBlooperCrash2

asmFixBlooperCrash1:
mr %r3,%r31 # unkValue
# r4 already contains battleUnitPointer
b cFixBlooperCrash1



asmFixBlooperCrash2:
# Check if BattleGetUnitPtr returned a proper pointer
cmpwi %r3,0
beq- exitFunction

# Restore the overwritten instruction
rlwinm %r0,%r31,2,0,29
blr

exitFunction:
b (btlevtcmd_GetUnitWork + 0x6C)
########################################################################################



########################################################################################
# Fix `evt_map_blend_set_flag` crashes

.global asmFixEvtMapBlendSetFlagPartnerCrashStart
.global asmFixEvtMapBlendSetFlagPartnerCrashBranchBack
.global asmFixEvtMapBlendSetFlagFollowerCrashStart
.global asmFixEvtMapBlendSetFlagFollowerCrashBranchBack

asmFixEvtMapBlendSetFlagPartnerCrashStart:
# r3 already contains partnerPtr
li %r4,1 # True
bl cFixEvtMapSetFlagCrash

# Restore the overwritten instruction
cmpwi %r31,1

asmFixEvtMapBlendSetFlagPartnerCrashBranchBack:
b 0



asmFixEvtMapBlendSetFlagFollowerCrashStart:
# r3 already contains followerPtr
li %r4,0 # False
bl cFixEvtMapSetFlagCrash

# Restore the overwritten instruction
cmpwi %r31,1

asmFixEvtMapBlendSetFlagFollowerCrashBranchBack:
b 0
########################################################################################



########################################################################################
# Prevent diary textbox selection

.global asmPreventDiaryTextboxSelection

asmPreventDiaryTextboxSelection:
mr %r3,%r30 # currentText
addi %r4,%r29,0x9C # storeAddress
mr %r5,%r0 # selectedOption
b cPreventDiaryTextboxOptionSelection
########################################################################################



########################################################################################
# Always show the `Mega Jump` and `Mega Hammer` moves in the battle menu

.global asmDisplayMegaJumpBadgeInBattleMenu
.global asmDisplayMegaHammerBadgesInBattleMenu

asmDisplayMegaJumpBadgeInBattleMenu:
li %r3,1 # True
b displayMegaBadgesInBattleMenuMain



asmDisplayMegaHammerBadgesInBattleMenu:
li %r3,0 # False
b displayMegaBadgesInBattleMenuMain



displayMegaBadgesInBattleMenuMain:
stwu %sp,-0x10(%sp)
mflr %r6
stw %r6,0x14(%sp)

# Backup important register values
stw %r5,0x10(%sp)
stw %r4,0x8(%sp)

# r3 already contains checkJumpBadge
mr %r4,%r0 # bitfield
bl cDisplayMegaBadgesInBattleMenu

# Check the returned bool
cmpwi %r3,0 # False

# Restore important register values
lwz %r4,0x8(%sp)
lwz %r5,0x10(%sp)

lwz %r0,0x14(%sp)
mtlr %r0
addi %sp,%sp,0x10
blr
########################################################################################



########################################################################################
# Allow additional text commands to be used with `FontDrawMessageMtx`

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
# Jump past the code that checks for vanilla commands
b (FontDrawMessageMtx + 0x45C)
########################################################################################



########################################################################################
# Allow running from any battles

.global asmAllowRunningFromBattlesStart
.global asmAllowRunningFromBattlesBranchBack

asmAllowRunningFromBattlesStart:
# r3 already contains ptr
bl cAllowRunningFromBattles
mr %r0,%r3

asmAllowRunningFromBattlesBranchBack:
b 0
########################################################################################



########################################################################################
# Force an enemy to drop a specific item

.global asmForceNpcItemDrop

asmForceNpcItemDrop:
stwu %sp,-0x10(%sp)
mflr %r0
stw %r0,0x14(%sp)

# Backup important register values
stw %r4,0x8(%sp)

# r3 already contains ptr
bl cForceNpcItemDrop
mr %r5,%r3

# Restore important register values
lwz %r4,0x8(%sp)

lwz %r0,0x14(%sp)
mtlr %r0
addi %sp,%sp,0x10
blr
########################################################################################



########################################################################################
# Cheat for falling through most objects

.global asmFallThroughMostObjectsStandard
.global asmFallThroughMostObjectsBowser

# At least two separate hooks use `asmFallThroughMostObjectsStandard`, so it's assembly must use the link register to return
asmFallThroughMostObjectsStandard:
stwu %sp,-0x10(%sp)
mflr %r0
stw %r0,0x14(%sp)

# r3 already contains ptr
bl cFallThroughMostObjects

# Check the returned pointer
cmplwi %r3,0 # nullptr

lwz %r0,0x14(%sp)
mtlr %r0
addi %sp,%sp,0x10
blr



asmFallThroughMostObjectsBowser:
stwu %sp,-0x10(%sp)
mflr %r0
stw %r0,0x14(%sp)

# r3 already contains ptr
bl cFallThroughMostObjects

# Restore the overwritten instruction
mr. %r24,%r3

lwz %r0,0x14(%sp)
mtlr %r0
addi %sp,%sp,0x10
blr
########################################################################################



########################################################################################
# Replace jump fall animation

.global asmReplaceJumpFallAnim

asmReplaceJumpFallAnim:
stwu %sp,-0x10(%sp)
mflr %r3
stw %r3,0x14(%sp)

# Backup important register values
stw %r0,0x8(%sp)

mr %r3,%r25 # jumpFallString
bl cReplaceJumpFallAnim
mr %r4,%r3

# Restore important register values
lwz %r0,0x8(%sp)

lwz %r3,0x14(%sp)
mtlr %r3
addi %sp,%sp,0x10
blr
########################################################################################



########################################################################################
# Display for hiding the D-Pad Options at the bottom-left of the screen

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
########################################################################################



########################################################################################
# Disable the random fail chance when doing an AMW

.global asmArbitraryMemoryWriteDisableRandomFail

asmArbitraryMemoryWriteDisableRandomFail:
stwu %sp,-0x10(%sp)
mflr %r3
stw %r3,0x14(%sp)

# Backup important register values
stw %r4,0x8(%sp)

mr %r3,%r0 # flags
bl cArbitraryMemoryWriteDisableRandomFail

# Restore the overwritten instruction
rlwinm. %r0,%r3,0,31,31

# Restore important register values
lwz %r4,0x8(%sp)

lwz %r3,0x14(%sp)
mtlr %r3
addi %sp,%sp,0x10
blr
########################################################################################



########################################################################################
# Get the proper vanilla pointer when doing an AMW

.global asmArbitraryMemoryWriteGetProperPointerStart
.global asmArbitraryMemoryWriteGetProperPointerBranchBack

asmArbitraryMemoryWriteGetProperPointerStart:
# r3 already contains pointerRaw
mr %r4,%r0 # multipliedIndex
bl cArbitraryMemoryWriteGetProperPointer
mr %r31,%r3

asmArbitraryMemoryWriteGetProperPointerBranchBack:
b 0
########################################################################################



########################################################################################
# Cheat for handling `Tube Mode` storage

.global asmHandleTubeModeStorage

asmHandleTubeModeStorage:
stwu %sp,-0x10(%sp)
mflr %r0
stw %r0,0x14(%sp)

# Backup important register values
stw %r3,0x8(%sp)

bl cHandleTubeModeStorage
cmpwi %r3,0 # False

# Restore important register values
lwz %r3,0x8(%sp)

lwz %r0,0x14(%sp)
mtlr %r0
addi %sp,%sp,0x10

bne- tubeModeStorageActive

# Restore the overwritten instruction
cmpwi %r3,1
blr

tubeModeStorageActive:
# Go past some additional checks that would check if Mario should enter Tube Mode at this time
b (marioRollChgChk + 0x78)
########################################################################################



########################################################################################
# Allow jumping on water when the Bridge Skip display is enabled

#ifdef TTYD_EU

.global asmJumpOnWaterStart
.global asmJumpOnWaterBranchBack

asmJumpOnWaterStart:
# r3 already contains ptr
bl cJumpOnWater

# Restore the overwritten instruction
cmplwi %r3,0

asmJumpOnWaterBranchBack:
b 0

#endif
########################################################################################



########################################################################################
# Make the crash screen to scroll

#ifdef TTYD_JP

.global asmCrashScreenDecrementYPosStart
.global asmCrashScreenDecrementYPosBranchBack

asmCrashScreenDecrementYPosStart:
bl cCrashScreenDecrementYPos
mr %r25,%r3 # posY

asmCrashScreenDecrementYPosBranchBack:
b 0

#endif
########################################################################################



########################################################################################
# Update the time register with a new time

.global asmSetTime

asmSetTime:
# Set the lower 32 bits to 0 to prevent overflowing into the upper 32 bits
li %r0,0
mtspr 284,%r0

# Set the new time
mtspr 285,%r3
mtspr 284,%r4
blr
########################################################################################
