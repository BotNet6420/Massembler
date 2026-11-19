// Example Call: "Hello Wrold!\n"
BSA STR // Capture the address of the next cell
HEX 0x48 // H
HEX 0x65 // e
HEX 0x6C // l
HEX 0x6C // l
HEX 0x6F // o
HEX 0x20 // [white-space]
HEX 0x57 // W
HEX 0x6F // o
HEX 0x72 // r
HEX 0x6C // l
HEX 0x64 //
HEX 0x21 // !
HEX 0x0A // \n
HEX 0x00 // \0
STR, HEX 0
LDA STR
BSA PRT
HLT


// Prints a null-terminated string to the output
PRT, HEX 0
// Save the address
STA P
// Get the character
P2, LDA P I
// End if null
SZA
BUN P1
BUN PRT I
// Wait for IO
P1, SKO
BUN P1
// Print character
OUT
ISZ P
BUN P2
// Storage
P, HEX 0
END
