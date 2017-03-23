## Chapter 2 Boolean Arithmetic

### FullAdder

Use 2 Halfadder and one Or gate. First, use a halfadder to
sum up {a,b} and then get {sum=w1,carry=w2}. And then, use
the second halfadder to sum up {w1,c}. The sum we get now
is the final sum, since the information we got lose in
this moment is the carry bit in the first halfadder, however,
the carry bit have no effect on the sum bit. The last thing
to do is use the Or to make the carry bit right.
