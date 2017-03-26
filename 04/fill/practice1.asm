// Practice

// Put your code here.
@SCREEN     // A=16384
D=A         // D=16384

// set A and D to A + 1
@1          // A=1
D=D+A       // D=16385

// set D to 19
@19
D=A

// set Both A and D to A + D
@19         // A=19
D=D+A       // D=38

// set RAM[5034] to D-1
@5034
D=M         // D=M[5034], which is zero
@1
M=D-A       // -1

// set RAM[53] to 171
@53
D=M         // D=M[53]
@171
M=A

// Add 1 to RAM[7], and store the result in D
@7
D=M
@1
D=D+A
