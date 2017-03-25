## Chapter 3 Sequential Logic

### Bit

The textbook has given the architecture of Bit, which is
using the Mux to choose whether the bit to load the input or
just use the old value. To follow the instruction, we need to
aware that we are writing hardware declaration language, so
we need to think about the connection of the wire and don't
think the in or out as the variable we use in software program.

### RAM8

Firstly, select which address we want to load (or not load,
if load is 0), hence, we use DMux8Way to choose which register
to load.
After choosing which register to load, we setup 8 register
and then load the register.
Finally, we have to choose the address selected register's
output to out.

### PC

I solve this multi-conditional problem by deal with the lowest
priority bit first, for example, the reset bit has the highest
priority, therefore, I put it at the last so that it can serve
as the last filter to make the register load 0 to it.
If the load and reset bit is set to 0, then the register might
either add the out from the last time unit or not, and what we
have to do is connect the out from the register to the increment
gate or connect it direct to the current input.
