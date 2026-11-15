// Example call: 7 * 3 = 21
// Result will be inside RES
BSA MUL
DEC 7
DEC 3
STA RES
HLT
RES, DEC 0


// Multiplies 2 numbers
MUL, HEX 0
// Load argument 1
LDA MUL I
STA MA1
// Load argument 2
ISZ MUL
LDA MUL I
STA MA2
// Return 0 if arg2 == 0
SZA
BUN SKP
ISZ MUL
BUN MUL I
// Initialaze counter with negative number
SKP, CMA
INC
STA MA2
// Loop initialization
LDA MA1
// Loop
BUN CHK // Check condition first
LOP, ADD MA1
CHK, ISZ MA2
BUN LOP
// Return result
ISZ MUL
BUN MUL I
// Storage
MA1, DEC 0
MA2, DEC 0
END
