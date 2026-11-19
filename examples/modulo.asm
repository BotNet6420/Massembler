// Example Call: 30 / 6 = 5
// Result will be inside RES
BSA MOD
DEC 30
DEC 6
STA RES
HLT
RES, DEC 0


// Divide argument 1 by argument 2 and return the remainder
MOD, HEX 0
// Load arg1
LDA MOD I
SZA // return 0 if ar1 == 0
BUN LM2
ISZ MOD
ISZ MOD
BUN MOD I
// Load arg2
LM2, STA MD1
ISZ MOD
LDA MOD I
SZA  // return 0 if arg2 == 0
BUN MKP
ISZ MOD
BUN MOD I
// Make agr2 negative
MKP, CMA
INC
STA MD2
// Perform division
LDA MD1
MOP, ADD MD2
SNA
BUN MOP
// Add arg2 once
STA MD1
LDA MD2
CMA
INC
ADD MD1
// Return the result
ISZ MOD
BUN MOD I
// Storage
MD1, DEC 0
MD2, DEC 0
END
