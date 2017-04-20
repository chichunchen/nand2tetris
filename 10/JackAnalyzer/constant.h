#ifndef _CONSTANT_H
#define _CONSTANT_H

extern enum _tokenEnum {
    KEYWORD, SYMBOL,
    IDENTIFIER, INT_CONST,
    STRING_CONST
};

extern enum _keywordEnum {
    CLASS, METHOD, FUNCTION,
    CONSTRUCTOR, _INT,
    _BOOLEAN, _CHAR, _VOID,
    VAR, STATIC, FIELD, LET,
    DO, _IF, _ELSE, _WHILE,
    _RETURN, _TRUE, _FALSE,
    _NULL, _THIS
};

extern const char* keyword[];
extern const char symbol[];
extern const char* op[];
extern const char* unaryOp[];
extern const char *keywordConstant[];

#endif
