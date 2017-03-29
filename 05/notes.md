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

### Computer

First problem met: This chip has a circle in its part connection
