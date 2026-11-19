// Example Call: 465\n
// Result will be inside RES
BSA STR
HEX 0x34
HEX 0x36
HEX 0x35
STR, HEX 0
LDA STR
BSA ATI
STA RES
HLT
RES, DEC 0


// Converts the given string to an integer
// Stops converting at the first non-numeric character
ATI, HEX 0
// Store original string pointer
STA AT1
STA AT2
CLA
STA AT9 // clear result
// Check every character to be valid
ATL, LDA AT2 I // fetch the character
SZA // end at null-terminator
BUN AT6
BUN ATC
AT6, ADD AT7 // check for less than 0
SPA
BUN ATC
ADD AT4 // return it back to normal
ADD AT8 // check greater than 9
SNA
BUN ATC
ADD AT5 // return it back to normal
ISZ AT2 // increment counter
ISZ AT3 // increment pointer
BUN ATL
// Skip if zero characters
ATC, LDA AT3
SZA
BUN ATR
BUN ATE // Return 0
// Initialize the loop
ATR, CMA
INC
STA AT3 // store loop counter
CLA
STA AT9
// Perform loop
ATV, LDA AT3
LDA AT9
STA AT0
BSA MUL // shift the result by 10 to the left
AT0, DEC 0
DEC 10
STA AT9 // store it
LDA AT1 I // load the character
ADD AT7 // shift it by ASCII 0 to get integer value
ADD AT9 // add it to the total
STA AT9
ISZ AT1 // increment the string pointer
ISZ AT3 // increment loop counter and exit if needed
BUN ATV
// Return the result
ATE, LDA AT9
BUN ATI I
// Storage
AT1, HEX 0 // Original string pointer
AT2, HEX 0 // internal string pointer
AT3, HEX 0 // Digit count
AT4, DEC 48 // ASCII '0'
AT5, DEC 58 // ASCII ':'
AT7, DEC -48 // Negative ASCII '0'
AT8, DEC -58 // NEGATIVE ASCII ':'
AT9, DEC 0 // Final return number


// Helper multiplication function
MUL, HEX 0
LDA MUL I
STA MA1
ISZ MUL
LDA MUL I
STA MA2
SZA
BUN SKP
ISZ MUL
BUN MUL I
SKP, CMA
INC
STA MA2
LDA MA1
BUN CHK
LOP, ADD MA1
CHK, ISZ MA2
BUN LOP
ISZ MUL
BUN MUL I
MA1, DEC 0
MA2, DEC 0

END