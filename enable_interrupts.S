# enable_interrupts.S
# This file written 2023 by Jonathan Johnson

.global enable_interrupts

.text
enable_interrupts:
	ei 
	jr $ra