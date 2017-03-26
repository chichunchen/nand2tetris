// Practice

// sum = 0
@sum
M=0

// j = j+1
@j
D=M
@1
D=D+1
@j
M=D

// q = sum + 12 -j
@sum
D=M         // D=sum
@12
D=D+A       // D=sum+12
@j
D=D-M       // D=sum-*j

// arr[3]=-1
@3
D=A
@arr
A=D+A
M=-1

// arr[j]=0
@j
D=A
@arr
A=D+A
M=0

// arr[j]=17
@j
D=A
@arr
A=D+A
M=D
