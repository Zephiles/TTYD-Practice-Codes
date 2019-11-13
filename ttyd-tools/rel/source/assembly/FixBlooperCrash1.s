.global StartFixBlooperCrash1

StartFixBlooperCrash1:
mr %r3,%r31 # unkValue
# r4 already contains battleUnitPointer
b fixBlooperCrash1