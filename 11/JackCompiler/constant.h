#ifndef _CONSTANT_H
#define _CONSTANT_H

enum {
    KEYWORD, SYMBOL,
    IDENTIFIER, INT_CONST,
    STRING_CONST
};

enum reserve {
    _CLASS, _METHOD, _FUNCTION,
    _CONSTRUCTOR, _INT,
    _BOOLEAN, _CHAR, _VOID,
    _VAR, _STATIC, _FIELD, _LET,
    _DO, _IF, _ELSE, _WHILE,
    _RETURN, _TRUE, _FALSE,
    _NULL, _THIS
};

enum kind {
    STATIC, FIELD, ARG, VAR, NONE = -1
};

enum table_choice {
    CLASS, SUBROUTINE
};

enum vm_type {
    __CONST, __ARG, __LOCAL, __STATIC, __THIS, __THAT, __POINTER, __TEMP
};

extern const char* keyword[];
extern const char symbol[];
extern const char* op[];
extern const char* unaryOp[];
extern const char *keywordConstant[];
extern const char *types[];

#endif
