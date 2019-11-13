#include "assembly.h"

namespace mod {

// Define the restfpr runtime functions
asm (R"(
.global _restfpr_14_x
.global _restfpr_15_x
.global _restfpr_16_x
.global _restfpr_17_x
.global _restfpr_18_x
.global _restfpr_19_x
.global _restfpr_20_x
.global _restfpr_21_x
.global _restfpr_22_x
.global _restfpr_23_x
.global _restfpr_24_x
.global _restfpr_25_x
.global _restfpr_26_x
.global _restfpr_27_x
.global _restfpr_28_x
.global _restfpr_29_x
.global _restfpr_30_x
.global _restfpr_31_x

_restfpr_14_x: lfd %f14,-0x90(%r11)
_restfpr_15_x: lfd %f15,-0x88(%r11)
_restfpr_16_x: lfd %f16,-0x80(%r11)
_restfpr_17_x: lfd %f17,-0x78(%r11)
_restfpr_18_x: lfd %f18,-0x70(%r11)
_restfpr_19_x: lfd %f19,-0x68(%r11)
_restfpr_20_x: lfd %f20,-0x60(%r11)
_restfpr_21_x: lfd %f21,-0x58(%r11)
_restfpr_22_x: lfd %f22,-0x50(%r11)
_restfpr_23_x: lfd %f23,-0x48(%r11)
_restfpr_24_x: lfd %f24,-0x40(%r11)
_restfpr_25_x: lfd %f25,-0x38(%r11)
_restfpr_26_x: lfd %f26,-0x30(%r11)
_restfpr_27_x: lfd %f27,-0x28(%r11)
_restfpr_28_x: lfd %f28,-0x20(%r11)
_restfpr_29_x: lfd %f29,-0x18(%r11)
_restfpr_30_x: lfd %f30,-0x10(%r11)
_restfpr_31_x:
lwz %r0,0x4(%r11)
lfd %f31,-0x8(%r11)
mtlr %r0
mr %sp,%r11
blr
)");

// Define the restgpr runtime functions
asm (R"(
.global _restgpr_14_x
.global _restgpr_15_x
.global _restgpr_16_x
.global _restgpr_17_x
.global _restgpr_18_x
.global _restgpr_19_x
.global _restgpr_20_x
.global _restgpr_21_x
.global _restgpr_22_x
.global _restgpr_23_x
.global _restgpr_24_x
.global _restgpr_25_x
.global _restgpr_26_x
.global _restgpr_27_x
.global _restgpr_28_x
.global _restgpr_29_x
.global _restgpr_30_x
.global _restgpr_31_x

_restgpr_14_x: lwz %r14,-0x48(%r11)
_restgpr_15_x: lwz %r15,-0x44(%r11)
_restgpr_16_x: lwz %r16,-0x40(%r11)
_restgpr_17_x: lwz %r17,-0x3C(%r11)
_restgpr_18_x: lwz %r18,-0x38(%r11)
_restgpr_19_x: lwz %r19,-0x34(%r11)
_restgpr_20_x: lwz %r20,-0x30(%r11)
_restgpr_21_x: lwz %r21,-0x2C(%r11)
_restgpr_22_x: lwz %r22,-0x28(%r11)
_restgpr_23_x: lwz %r23,-0x24(%r11)
_restgpr_24_x: lwz %r24,-0x20(%r11)
_restgpr_25_x: lwz %r25,-0x1C(%r11)
_restgpr_26_x: lwz %r26,-0x18(%r11)
_restgpr_27_x: lwz %r27,-0x14(%r11)
_restgpr_28_x: lwz %r28,-0x10(%r11)
_restgpr_29_x: lwz %r29,-0xC(%r11)
_restgpr_30_x: lwz %r30,-0x8(%r11)
_restgpr_31_x:
lwz %r0,0x4(%r11)
lwz %r31,-0x4(%r11)
mtlr %r0
mr %sp,%r11
blr
)");

void asmReplaceJumpFallAnim()
{
	asm (R"(
	stwu %sp,-0x10(%sp)
	mflr %r3
	stw %r3,0x14(%sp)
	stw %r31,0xC(%sp)
	mr %r31,%r0

	mr %r3,%r25 # jumpFallString
	bl replaceJumpFallAnim
	mr %r4,%r3

	mr %r0,%r31
	lwz %r31,0xC(%sp)
	lwz %r3,0x14(%sp)
	mtlr %r3
	addi %sp,%sp,0x10
	)");
}

void asmAllowRunningFromBattles()
{
	asm (R"(
	stwu %sp,-0x10(%sp)
	mflr %r0
	stw %r0,0x14(%sp)

	# r3 already contains ptr
	bl allowRunningFromBattles
	mr %r0,%r3

	lwz %r3,0x14(%sp)
	mtlr %r3
	addi %sp,%sp,0x10
	)");
}

void asmForceNPCItemDrop()
{
	asm (R"(
	stwu %sp,-0x10(%sp)
	mflr %r0
	stw %r0,0x14(%sp)
	stw %r31,0xC(%sp)
	mr %r31,%r4

	# r3 already contains ptr
	bl forceNPCItemDrop
	mr %r5,%r3

	mr %r4,%r31
	lwz %r31,0xC(%sp)
	lwz %r0,0x14(%sp)
	mtlr %r0
	addi %sp,%sp,0x10
	)");
}

void asmDisplayMegaJumpBadgeBattleMenu()
{
	asm (R"(
	stwu %sp,-0x10(%sp)
	mflr %r3
	stw %r3,0x14(%sp)
	stmw %r30,0x8(%sp)
	mr %r31,%r4
	mr %r30,%r5

	mr %r3,%r0 # checkBit
	bl displayMegaJumpBadgeInMenu

	# Check the returned bool
	cmpwi %r3,0 # False

	mr %r4,%r31
	mr %r5,%r30
	lmw %r30,0x8(%sp)
	lwz %r0,0x14(%sp)
	mtlr %r0
	addi %sp,%sp,0x10
	)");
}

void asmDisplayMegaHammerBadgesBattleMenu()
{
	asm (R"(
	stwu %sp,-0x10(%sp)
	mflr %r3
	stw %r3,0x14(%sp)
	stmw %r30,0x8(%sp)
	mr %r31,%r4
	mr %r30,%r5

	mr %r3,%r0 # checkBit
	bl displayMegaHammerBadgesInMenu

	# Check the returned bool
	cmpwi %r3,0 # False

	mr %r4,%r31
	mr %r5,%r30
	lmw %r30,0x8(%sp)
	lwz %r0,0x14(%sp)
	mtlr %r0
	addi %sp,%sp,0x10
	)");
}

// void asmFixBlooperCrash1()

/* More efficient to define the function in the asm block, due to one 
	of the registers already having the correct value in it */

asm (R"(
.global asmFixBlooperCrash1

asmFixBlooperCrash1:
mr %r3,%r31 # unkValue
# r4 already contains battleUnitPointer
b fixBlooperCrash1
)");

void asmFixBlooperCrash2()
{
	uint32_t Offset;
	
	#ifdef TTYD_US
	Offset = 0x218;
	#elif defined TTYD_JP
	Offset = 0x214;
	#elif defined TTYD_EU
	Offset = 0x218;
	#endif
	
	asm (R"(
	stwu %%sp,-0x10(%%sp)
	mflr %%r0
	stw %%r0,0x14(%%sp)
	stmw %%r30,0x8(%%sp)
	mr %%r31,%%r3
	mr %%r30,%%r4

	mr %%r3,%%r4 # battleUnitPointer
	bl checkIfPointerIsValid

	# Check the returned bool
	cmpwi %%r3,0 # False
	beq- ExitFunction

	mr %%r3,%%r31
	mr %%r4,%%r30
	lmw %%r30,0x8(%%sp)
	lwz %%r0,0x14(%%sp)
	mtlr %%r0
	addi %%sp,%%sp,0x10

	# Restore the overwritten instruction
	lwz %%r5,%[offset](%%r4)
	blr

	ExitFunction:
	lmw %%r28,0x20(%%sp)
	li %%r3,2
	lwz %%r0,0x34(%%sp)
	mtlr %%r0
	addi %%sp,%%sp,0x30
	)"
	: // No outputs
	: [offset] "i" (Offset));
}

void asmPreventTextboxSelection()
{
	char *CurrentText;
	void *StoreAddress;
	int32_t SelectedOption;
	
	asm (R"(
	mr %[currentText],%%r30
	mr %[storeAddress],%%r29
	mr %[selectedOption],%%r0
	)"
	: [currentText] "=r" (CurrentText), 
		[storeAddress] "=r" (StoreAddress), 
		[selectedOption] "=r" (SelectedOption)
	);
	
	/*
	asm ("mr %0,%%r30" : "=r" (CurrentText));
	asm ("mr %0,%%r29" : "=r" (StoreAddress));
	asm ("mr %0,%%r0" : "=r" (SelectedOption));
	*/
	
	preventTextboxOptionSelection(CurrentText, StoreAddress, SelectedOption);
}

void asmDisableDPadOptionsDisplay()
{
	asm (R"(
	stwu %sp,-0x10(%sp)
	mflr %r3
	stw %r3,0x14(%sp)
	stw %r31,0xC(%sp)
	mr %r31,%r4

	mr %r3,%r0 # unkVar
	bl disableDPadOptionsDisplay

	# Check the returned bool
	cmpwi %r3,0 # False

	mr %r4,%r31
	lwz %r31,0xC(%sp)
	lwz %r0,0x14(%sp)
	mtlr %r0
	addi %sp,%sp,0x10
	)");
}

void asmFixEvtMapBlendSetFlagPartnerCrash()
{
	asm (R"(
	stwu %sp,-0x10(%sp)
	mflr %r0
	stw %r0,0x14(%sp)

	# r3 already contains partnerPtr
	bl fixEvtMapBlendSetFlagPartnerCrash

	lwz %r0,0x14(%sp)
	mtlr %r0
	addi %sp,%sp,0x10

	# Restore the overwritten instruction
	cmpwi %r31,1
	)");
}

void asmFixEvtMapBlendSetFlagFollowerCrash()
{
	asm (R"(
	stwu %sp,-0x10(%sp)
	mflr %r0
	stw %r0,0x14(%sp)

	# r3 already contains followerPtr
	bl fixEvtMapBlendSetFlagFollowerCrash

	lwz %r0,0x14(%sp)
	mtlr %r0
	addi %sp,%sp,0x10

	# Restore the overwritten instruction
	cmpwi %r31,1
	)");
}

void asmFallThroughMostObjectsStandard()
{
	asm (R"(
	stwu %sp,-0x10(%sp)
	mflr %r0
	stw %r0,0x14(%sp)

	# r3 already contains ptr
	bl fallThroughMostObjects

	# Check the returned pointer
	cmplwi %r3,0 # nullptr

	lwz %r0,0x14(%sp)
	mtlr %r0
	addi %sp,%sp,0x10
	)");
}

void asmFallThroughMostObjectsBowser()
{
	asm (R"(
	stwu %sp,-0x10(%sp)
	mflr %r0
	stw %r0,0x14(%sp)

	# r3 already contains ptr
	bl fallThroughMostObjects

	# Restore the overwritten instruction
	mr. %r24,%r3

	lwz %r0,0x14(%sp)
	mtlr %r0
	addi %sp,%sp,0x10
	)");
}

}