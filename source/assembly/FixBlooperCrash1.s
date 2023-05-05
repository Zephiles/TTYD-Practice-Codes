.global asmFixBlooperCrash1

asmFixBlooperCrash1:
mr %r3,%r31 # unkValue
# r4 already contains battleUnitPointer
b cFixBlooperCrash1
