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
