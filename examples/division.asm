// Example Call: 30 / 6 = 5
// Result will be inside RES
BSA DIV
DEC 30
DEC 6
STA RES
HLT
RES, DEC 0


// Divide argument 1 by argument 2
DIV, HEX 0
// Load arg1
LDA DIV I
SZA // return 0 if ar1 == 0
BUN LD2
ISZ DIV
ISZ DIV
BUN DIV I
// Load arg2
LD2, STA DA1
ISZ DIV
LDA DIV I
SZA  // return 0 if arg2 == 0
BUN SKP
ISZ DIV
BUN DIV I
// Make agr2 negative
SKP, CMA
INC
STA DA2
// Perform division
CLA
STA QOT
LDA DA1
LOP, ADD DA2
SNA
ISZ QOT
SNA
BUN LOP
// Return result
LDA QOT
ISZ DIV
BUN DIV I
// Storage
DA1, DEC 0
DA2, DEC 0
QOT, DEC 0
END
