make

./a.out pong/Pong.asm
file1=pong/pong.hack
file2=pong/test.hack
if diff -q $file1 $file2; then
    echo $file1 "done"
else
    echo "fail"
fi

./a.out add/Add.asm
file1=add/Add.hack
file2=add/test.hack
if diff -q $file1 $file2; then
    echo $file1 "done"
else
    echo "fail"
fi

./a.out max/Max.asm
file1=max/Max.hack
file2=max/test.hack
if diff -q $file1 $file2; then
    echo $file1 "done"
else
    echo "fail"
fi

./a.out rect/Rect.asm
file1=rect/Rect.hack
file2=rect/test.hack
if diff -q $file1 $file2; then
    echo $file1 "done"
else
    echo "fail"
fi
