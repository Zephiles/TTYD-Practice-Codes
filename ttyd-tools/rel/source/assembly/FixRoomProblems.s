.global StartFixRoomProblems
.global BranchBackFixRoomProblems

StartFixRoomProblems:
bl fixRoomProblems

# Restore overwritten assembly
li %r3,512

BranchBackFixRoomProblems:
b 0