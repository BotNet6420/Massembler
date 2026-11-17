// Example Call: 137
// Result will be inside RES
LDA NUM
BSA ITA
HLT
NUM, DEC 137
HEP, HEX 0xFFF // Heap pointer


// Converts the given integer to a string
// Returns the pointer to the start of the string
ITA, HEX 0
// Store the number
STA IT1
// Figure out the amount of digits
FIG, STA IT2
BSA DIV
IT2, DEC 0
DEC 10
ISZ IT3
SZA
BUN FIG
// Allocate memory
LDA IT3
CMA
STA IT3
LDA HEP
STA IT4 // will start writing the string backwards
ADD IT3
STA HEP
// Add \0
CLA
STA IT4 I
LDA IT4
ADD IT5
STA IT4
// initialize loop
ISZ IT3 // counter
LDA IT1 // initial division call
STA IT6
// Get the last digit, shift it to ASCII, store it, shift number to the right, repeat
// Get the last digit
ITL, BSA MOD
IT1, DEC 0 // Original number
DEC 10
ADD IT0 // shift
// Store in the string
STA IT4 I
LDA IT4 // Decrement string pointer
ADD IT5
STA IT4
// Shift number to the right
BSA DIV // divide by 10
IT6, DEC 0
DEC 10
STA IT1 // store the shifted number
STA IT6
ISZ IT3 // loop counter
BUN ITL
// Return the result
LDA IT4 // It's pointing 1 too low because of the loop
INC
BUN ITA I
// Storage
IT0, HEX 0x30 // ASCII of '0'
IT3, DEC 0 // Count of digits, and loop counter
IT4, HEX 0 // result string pointer
IT5, DEC -1 // For convenience

// Helper Division function
DIV, HEX 0
LDA DIV I
BUN LD2
ISZ DIV
ISZ DIV
BUN DIV I
LD2, STA DA1
ISZ DIV
LDA DIV I
BUN SKP
ISZ DIV
BUN DIV I
SKP, CMA
INC
STA DA2
CLA
STA QOT
LDA DA1
LOP, ADD DA2
SNA
ISZ QOT
SNA
BUN LOP
LDA QOT
ISZ DIV
BUN DIV I
DA1, DEC 0
DA2, DEC 0
QOT, DEC 0
// Helper Modulo Function
MOD, HEX 0
LDA MOD I
BUN LM2
ISZ MOD
ISZ MOD
BUN MOD I
LM2, STA MD1
ISZ MOD
LDA MOD I
BUN MKP
ISZ MOD
BUN MOD I
MKP, CMA
INC
STA MD2
LDA MD1
MOP, ADD MD2
SNA
BUN MOP
STA MD1
LDA MD2
CMA
INC
ADD MD1
ISZ MOD
BUN MOD I
MD1, DEC 0
MD2, DEC 0

END