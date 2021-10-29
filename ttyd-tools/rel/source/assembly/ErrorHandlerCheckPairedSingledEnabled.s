.global asmErrorHandlerCheckPairedSinglesEnabled

asmErrorHandlerCheckPairedSinglesEnabled:
mfspr %r3,920 # 920 = HID2
rlwinm %r3,%r3,3,31,31
blr