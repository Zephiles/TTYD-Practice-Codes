.global setTime

setTime:
# Set the lower 32 bits to 0 to prevent overflowing into the upper 32 bits
li %r0,0
mtspr 284,%r0

# Set the new time
mtspr 285,%r3
mtspr 284,%r4
blr