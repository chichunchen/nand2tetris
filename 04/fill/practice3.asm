// goto 50
//@50
//D;JMP
//*/

// if D==0, goto 112
// @112
// D; JEQ

// if D < 9, goto 507
// @9
// D=D-A
// @507
// D; JLT

// if RAM[12] > 0, goto 50
// @12
// D=M
// MD=A
// @50
// D; JGT

// if sum >= 0, goto end
// (END)
// @sum
// D=M
// @END
// D; JGE

// if x[i]<=0 goto NEXT.
// (NEXT)
// @i
// D=A
// @x
// A=D+A
// MD=-1        // let x[i]=-1
// D; JLE

// add 1 to 10
// sum = 0
// i = 10
// while(i > 0)
//     sum += i

// @sum
// M=0     // sum = 0
// @10
// D=A
// @i      // i = 100
// M=D
// (LOOP)
//     @i
//     D=M
//     @END
//     D; JLE
//     @sum
//     M=M+D
//     @i
//     M=M-1
//     @LOOP
//     0; JMP
// (END)

// i=8192
// while(i < 0)
//     RAM[SCREEN+i]=-1
//     i--
@8191
D=A
@i
M=D
(LOOP)
    @i      // if i=8192, 8191... < 0, then goto end
    D=M
    @END
    D; JLT

    // content
    @i
    D=M     // D=8192
    @temp   // @17
    M=D     // temp=i=8192
    @SCREEN
    D=A     // D=16384
    @temp
    D=M+D   // D=16384+8192
    @pixel
    A=D
    M=-1

    @i
    M=M-1
    @LOOP
    0; JMP
(END)
