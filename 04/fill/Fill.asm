// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/04/Fill.asm

// Runs an infinite loop that listens to the keyboard input.
// When a key is pressed (any key), the program blackens the screen,
// i.e. writes "black" in every pixel;
// the screen should remain fully black as long as the key is pressed. 
// When no key is pressed, the program clears the screen, i.e. writes
// "white" in every pixel;
// the screen should remain fully clear as long as no key is pressed.

// Put your code here.

// while(1) {
    // if (KEY is pressed) {
    //     print_screen
    // }
    // else {
    //     clear_screen
    // }
// }

(REPEAT)

@8191
D=A
@i
M=D

(LOOP)
    @i      // if i=8192, 8191... < 0, then goto end
    D=M
    @END
    D; JLT

    @KBD
    D=M
    @PRINT
    D; JNE      // if kbd != 0, then print
    // set 0, since kbd==0
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
    M=0
    @TOLOOP
    0; JMP

    (PRINT)
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
    @TOLOOP
    0; JMP

    (TOLOOP)
    @i
    M=M-1
    @LOOP
    0; JMP
(END)

@REPEAT
0; JMP
