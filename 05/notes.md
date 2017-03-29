## Chapter 5: Computer Architecture

### CPU

1. Figure out all of the control bits in Figure 5.9 in the textbook.

For example: dest = comp         ; jump

    1110 0 1 1 1 1 0  1 0 0  1 1 1
    o  a c1c2c3c4c5c6 d3d2d1 j3j2j1

o: opcode, which decides whether it's an a-instruction or c-instruction
a: whether the writeM bit true/false
c1~c6: comp
d3: A
d2: D
d1: M
j3: out < 0
j2: out == 0
j1: out > 0

2. The control bits are also affected by whether the instruction is a or c

### Memory

Use a DMux4Way Chip to decide the place of the load bit (place means the
memory range in Register16K)
Finally, use a Mux16 chip to decide the output

### Computer

First problem met: This chip has a circle in its part connection
There is an example from Nand2Tetris forum from mentor Mark:

    The "circle in its parts" message is complaining about `unclocked feedback`.
    Chips like "Bit" need feedback to store their state, but that feedback is controlled
    through a DFF, so the circuit doesn't run wild.

    An example of unclocked feedback would be this chip:

    CHIP Foo {
        INPUT in;
        OUTPUT out;
    PARTS:
        And (a=in, b=fb, out=aout, out=out);
        Not (in=aout, out=fb);
    }

    Built with real parts, this circuit would oscillate between 0 and 1 output as fast as possible,
    whenever in is 1.

    To make this a legal circuit for the Hardware Simulator, there needs to be a clocked delay somewhere. For instance
        DFF (in=fb, out=dffout);
        And (a=in, fb=dffout, ...
    In the CPU, the most common unclocked feedback is ALU output somehow getting back to ALU x or y.

    If you want to, feel free to email me your CPU and I'll let you know what I see.

It seems like my first implementation version of Memory.hdl which didn't
use the DMUX4WAY and Mux16 chips, instead, just use several combinatorial chips
to combine the logic makes the output can sometimes only come from those
combinatorial chips, and the result of it is the state of the writeM from
CPU to the load bit in Memory becomes an unclocked feedback.
While using the DMUX4WAY and Mux16 chips, we always get the result that flow
through the RAM chip, therefore, the issue solved inherently.
