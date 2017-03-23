## Project 01

### Mul

    |   a   |   b   |  sel  |  out  |
    |   0   |   0   |   0   |   0   |
    |   0   |   0   |   1   |   0   |
    |   0   |   1   |   0   |   0   |
    |   0   |   1   |   1   |   1   |
    |   1   |   0   |   0   |   1   |
    |   1   |   0   |   1   |   0   |
    |   1   |   1   |   0   |   1   |
    |   1   |   1   |   1   |   1   |

While out == 1, we have
row 4, 5, 7, 8, using canonical representation, we got

    (a-b-sel) + (ab-sel) + (-absel) + (absel)
    -> a-sel(b dot notb) + selb(a dot nota)
    -> (a dot notsel) + (b dot sel)

### DMul

If sel == 0, then {a=in, b=0}
else if sel == 1, {a=0, b=in}

    |  in   |  sel  |   a   |   b   |
    |   0   |   0   |   0   |   0   |
    |   0   |   1   |   0   |   0   |
    |   1   |   0   |   1   |   0   |
    |   1   |   1   |   0   |   1   |

In canonical representation, if in==1 && sel==0, then a equals 1,
otherwise a equals to zero. Therefore, we have `in dot notsel` for a. Same for b, if in==1 && sel==1, then b equals to
1, we have `in dot sel`.

### Mux4Way16

In the textbook "Element of Computing Systems", it give
us the hint "use fork" to work with. Firstly, we use two Mux which
mask use of sel[1] or sel[0] to distinguish 2 elements from 4 elements,
and then we use Mux again to get the final element.

### DMux4Way

Same as Mux4way16, I use sel[1] to distinguish which set to use, i.e {a,b} or
{c,d}, and then use sel[0] to choose the element from the set.
