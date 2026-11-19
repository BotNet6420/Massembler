// Example Call
// Result will be in RES
INC
BSA RLI
STA RES
HLT
RES, HEX 0
HEP, HEX 0xFFF


// Reads a line from input and returns its string
// Does not include the new-line character
RLI, HEX 0
STA RL7
// Allocate memory
RLB, ISZ RL4 // increment character count
LDA HEP
ADD RL2
STA HEP
INC
STA RL1 // store node pointer
LDA RL3
STA RL1 I // store previous pointer
LDA RL1
INC
STA RL3 // store character address
RLA, SKI // wait for input
BUN RLA
INP
STA RL3 I // store character
LDA RL7 // check echo back
SZA
BUN RLF
BUN RLG
RLF, SKO // echo back
BUN RLF
LDA RL3 I
OUT
RLG, LDA RL3 I
ADD RL5
SZA // skip if new line
BUN RLC
CLA
STA RL3 I // store null-terminator
BUN RLD
RLC, LDA RL1
STA RL3 // update previous node for next iteration
BUN RLB
// Form the string
RLD, LDA RL4
CMA
INC
ADD HEP
STA HEP // allocate memory
ADD RL4
STA RL6 // store string end
// Initialize loop
LDA RL4
CMA
INC
STA RL4 // negative count
// Perform Loop
RLE, LDA RL1
INC
STA RL3 // character address
LDA RL3 I
STA RL6 I // save character
LDA RL1 I
STA RL1 // store next node pointer
LDA RL6
CMA
INC
CMA
STA RL6 // decrement string pointer
ISZ RL4
BUN RLE
// Return the pointer
LDA RL6
INC
BUN RLI I
// Storage
RL1, HEX 0 // Linked List Node pointer: [0] = next pointer, [1] = character
RL2, DEC -2 // Amount to move HEAP for Node
RL3, HEX 0 // Temporary variable
RL4, DEC 0  // Character count
RL5, DEC -10 // Negative of ASCII new-line
RL6, HEX 0 // string pointer
RL7, HEX 0 // 0 = Dont echo back, 1 = echo back
END
