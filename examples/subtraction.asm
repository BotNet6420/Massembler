// Example call: 10 - 7 = 3
// Result will be inside RES
BSA SUB
DEC 10
DEC 7
STA RES
HLT
RES, DEC 0


// Subtracts 2 numbers
SUB, HEX 0
// Load argument 1
LDA SUB I
STA SA1
// Load argument 2
ISZ SUB
LDA SUB I
CMA // make it negative
INC
STA SA2
// Subtract
LDA SA1
ADD SA2
// Return
ISZ SUB
BUN SUB I
// Storage
SA1, DEC 0
SA2, DEC 0
END