# Example Programs
Some example programs and useful subroutines written for the Basic Computer. 

## Subroutine calling convention
- Put the arguments for the subroutine immediately after the `BSA` instruction.
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
