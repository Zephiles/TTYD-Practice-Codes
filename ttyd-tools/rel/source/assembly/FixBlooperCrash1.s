.global StartFixBlooperCrash1
.global BranchBackFixBlooperCrash1

StartFixBlooperCrash1:
mr %r3,%r31 # unkValue
# r4 already contains battleUnitPointer
bl fixBlooperCrash1

BranchBackFixBlooperCrash1:
b 0