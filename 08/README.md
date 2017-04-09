## Ch8 Virtual Machine 2: Program control

### goto label
jump to execute the command just after label

    @label
    0;JMP

### if-goto label
cond = pop;     // pop the truth value on the stack
if cond jump to execute the command just after label

    @SP
    A=M-1
    D=M     // get the value from top-1
    @SP
    M=M-1   // SP--
    @label
    D;JLT   // if (-1) then jump

### label label
label declaration command

    (label)

### Functions

#### call f n:
Calling a function f after n arguments have been pushed onto the stack.
The arguments has been pushed into global stack by the caller, and we
can use n to get the base address of `ARG`.

n, i.e. the number arguments and k, i.e. the number of local variables
should be intrepret by high level language, and use it to generate the
`asm` code.

VM code:

    push constant return-address
    push LCL
    push ARG
    push THIS
    push THAT
    ARG = SP-n-5
    LCL = SP
    goto f
    label return-address

ASM code:

    @return-address         // push constant return-address
    D=A
    @SP
    A=M
    M=D
    @SP
    M=M+1

    // push LCL
    @LCL                    // up_push(%s -> LCL)
    A=M
    D=M                     // down_push()
    @SP
    A=M
    M=D
    @SP
    M=M+1

    // push ARG, up_push(%s -> ARG), down_push()
    // push THIS, up_push(%s -> THIS), down_push()
    // push THAT, up_push(%s -> THAT), down_push()

    // ARG = SP-n-5
    @SP
    A=M
    D=M              // D=*SP
    D=D-1 * (n+5)    // do n times
    @ARG
    A=M
    M=D              // M=D, SP-n-5

    // LCL = SP
    @SP
    A=M
    D=M              // D=*SP
    @LCL
    A=M
    M=D              // *LCL=*SP

    // goto f

    // label return-address

#### function f k

VM code:

    label f
    repeat k times:
    push 0

ASM code:

    (f)
    // push constant 0 * k times
    @0
    D=A
    @SP
    A=M
    M=D
    M=M+1

#### return

VM code:

    FRAME = LCL      // FRAME is a temporary variable
    RET = *(FRAME-5)
    *ARG = pop()
