@7
D=A
@SP
A=M
M=D
@SP
M=M+1 // M=257
@8
D=A
@SP
A=M
M=D
@SP
M=M+1 // M=258

// now SP->258
// after add, SP->257, RAM[256]=15
@SP
AM=M-1 // AM=258-1=257
D=M    // D=RAM[257]=8
@SP
M=M-1  // M=256
A=M
M=D+M
@SP
M=M+1
