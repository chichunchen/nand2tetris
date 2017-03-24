## Chapter 2 Boolean Arithmetic

### FullAdder

Use 2 Halfadder and one Or gate. First, use a halfadder to
sum up {a,b} and then get {sum=w1,carry=w2}. And then, use
the second halfadder to sum up {w1,c}. The sum we get now
is the final sum, since the information we got lose in
this moment is the carry bit in the first halfadder, however,
the carry bit have no effect on the sum bit. The last thing
to do is use the Or to make the carry bit right.

### ALU

1. Use Mux16 to do the if ... else ... thing, and then it is
easy to deal with zx, nx, zy, ny, f, no bit.
2. To deal with bit ng and zr, it is important to understand
the Example in Appendix A.5.3. In `hdl` we cannot use the
subscript syntax on internal part pins, since the widths of
internal pins are deduced implicitly, from their connections.

For example, the following code have the error: `sub bus of
internal pin may not be used.`

    Mux(a=false, b=true, sel=tout[15], out=ng);

To make it write, we have to understand A.5.3, which use an
example to clarify this kind of problem:

    CHIP Foo {
        IN in[8];
        OUT out[8];
    }

Suppose now that Foo is invoked by another chip using
the part statement:

    Foo(in[2..4]=v, in[6..7]=true, out[0..3]=x, out[2..6]=y)

Where v is a previously declared 3-bit internal pin.

in: 7 6 5 4    3    2    1 0 (bits)
    1 1 ? v[2] v[1] v[0] ? ?

in: 7 6 5 4 3 2 1 0 (bits)
    1 1 0 1 0 0 1 1

x: 3 2 1 0
   0 0 1 1

y: 4 3 2 1 0
   1 0 1 0 0

In this case, we understand that in the invokion of Foo,
we make the partial output to x and y to avoid to use the
sub bus of internal pin.
