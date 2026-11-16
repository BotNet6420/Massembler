# Example Programs
Some example programs and useful subroutines written for the Basic Computer. 

## Subroutine calling convention
- Put single arguments inside AC and then `BSA`
- For more than 1 argument, put them immediately after the `BSA` instruction.
- The returned result will be inside AC

## Examples List

### [Subtraction](subtraction.asm)
Subtracts the first argument from the second and returns the result.

#### Arguments List
1. `SA1`: the number to subtract from
2. `SA2`: the number to subtract

#### Return Value
The result of the subtraction.

### [Multiplication](multiplication.asm)
Multiplies the first argument by the second and returns the result.

#### Arguments List
1. `MA1`: The number to multiply
2. `MA2`: The number to multiply by

#### Other Labels
1. `SKP`
2. `CHK`
3. `LOP`

#### Return Value
The result of the multiplication.

### [Division](division.asm)
Divides argument 2 by argument 1 and returns the result.

#### Arguments List
1. `DA1`: Numerator, must be Positive
2. `DA2`: Denominator, must be positive

#### Other Labels
1. `LD2`
2. `SKP`
3. `LOP`
4. `QOT`

#### Return Value
The result of integer division.

### [Modulo](modulo.asm)
Divides argument 2 by argument 1 and returns the remainder.

#### Arguments List
1. `MD1`: Numerator, must be Positive
2. `MD2`: Denominator, must be positive

#### Other Labels
1. `LM2`
2. `MKP`
3. `MOP`

#### Return Value
The remainder of integer division.

### [Print String](print_string.asm)
Prints a null terminated string to the output.
Blocks execution while doing so.

#### Arguments List
1. Inside AC: Address to the first character of a null-terminated string

#### Other Labels
1. `P`
2. `P1`
3. `P1`

#### Returns
Nothing.

### [Integer to Alphabet](integer_to_alphabet.asm)
Converts the given number into a null-terminated string and returns the address of it.
- Depends on [Modulo](#Modulo)
- Depends on [Division](#Division)
- Needs a heap pointer

#### Arguments List
1. Inside AC: The number to convert, must be positive

#### Other labels
1. `FIG`
2. `ITL`
3. `IT0`
4. `IT1`
5. `IT2`
6. `IT3`
7. `IT4`
8. `IT5`
9. `IT6`

#### Returns
A pointer to the start of a null-terminated string.

### [Alphabet to Integer](alphabet_to_integer.asm)
Converts the given null-terminated string into a positive integer.
Stops converting at the first non-digit ASCII character.
- Depends on [Multiplication](#multiplication)

#### Arguments List
1. Inside AC: The pointer to the null-terminated string to convert

#### Other Labels
1. `AT0`
2. `AT1`
3. `AT2`
4. `AT3`
5. `AT4`
6. `AT5`
7. `AT6`
8. `AT7`
9. `AT8`
10. `AT9`
11. `ATE`
12. `ATC`
13. `ATL`
14. `ATR`
15. `ATV`

#### Returns
The integer conversion of the given string.
