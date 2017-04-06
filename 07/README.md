## Ch7 VMtranslator

### lt:

    // if (x-y) lt 0, then -1, else 0
    @SP
    AM=M-1
    D=M     // D=arg2
    @SP
    AM=M-1
    D=M-D   // D=arg1-arg2, if (arg1-arg2) < 0, then true
    @true$lt$1
    D;JLT
    @SP     // if not lt
    A=M
    M=0
    @end$lt$1
    0;JMP
    (true$lt$1)
    @SP
    A=M
    M=-1
    (end$lt$1)
    @SP
    M=M+1

### eq:

    @SP
    AM=M-1
    D=M
    @SP
    AM=M-1
    D=M-D
    @true$eq$1      // change true eq
    D;JEQ           // JEQ
    @SP
    A=M
    M=0
    @end$eq$1       // change end eq
    0;JMP
    (true$eq$1)     // change true eq
    @SP
    A=M
    M=-1
    (end$eq$1)      // change end eq
    @SP
    M=M+1

### gt is the same as eq and lt

### neg:

    @SP
    A=M-1
    M=-M

### not:

    @SP
    A=M-1
    M=!M

### and:

    @SP
    AM=M-1
    D=M
    @SP
    AM=M-1
    M=M&D   // or: M=M|D


### push constant x:

    @x
    D=A
    // put x to the top of stack
    @SP
    A=M
    M=D
    // top++
    @SP
    M=M+1

### static

When a new symbol is encountered for the first time in an assembly program,
the assembler allocates a new RAM address to it, starting at address `16`.

What we have to do is to print out the assembly code below, the assembler
will translate the symbol to address.

filename: Xxx.vm
push static 3

    @Xxx.3
    D=M
    @SP
    A=M
    M=D
    @SP
    M=M+1

pop static 8

	@SP
	A=M-1
	D=M
	@Xxx.8
	M=D

### Read multiple flies
First use `stat` to check whether it's a directory or a regular file.
Once it's a directory, use `opendir and readdir` to get the name of the file.

The following example came from TLPI listing 18-2: Scanning a direcory.

Ex:
    DIR *dirp;
    struct dirent *dp;
    Boolean isCurrent;

    isCurrent = strcmp(dirpath, ".") == 0;

    dirp = opendir(dirpath);
    if (dirp == NULL) {
        errMsg("opendir failed on '%s'", dirpath);
        return;
    }
    /* For each entry in this directory, print directory + filename */

    for(;;) {
        errno = 0;
        dp = readdir(dirp);         // each call to readdir reads the next directory entry
                                    // from the directory stream referred to by dirp and
                                    // returns a pointer to a statically allocated structure
                                    // of type dirent
        if (dp == NULL)
            break;
        if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0)
            continue;
        if (!isCurrent)
            printf("%s/", dirpath);
        printf("%s\n", dp->d_name);
    }
    if (errno != 0)
        errExit("readdir");
    if (closedir(dirp) == -1)
        errMsg("closedir");

