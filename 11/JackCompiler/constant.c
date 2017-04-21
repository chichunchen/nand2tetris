#include "constant.h"

const char* keyword[] = {
    "class", "constructor", "function",
    "method", "field", "static", "var",
    "int", "char", "boolean", "void", "true",
    "false", "null", "this", "let", "do",
    "if", "else", "while", "return"
};

const char symbol[] = {
    '{', '}', '(', ')', '[', ']', '.',
    ',', ';', '+', '-', '*', '/', '&',
    '|', '<', '>', '=', '~'
};

const char* op[] = {
    "+", "-", "*", "/", "&amp;", "|", "&lt;", "&gt;", "=",
};

const char* unaryOp[] = {
    "-", "~"
};

const char *keywordConstant[] = {
    "true", "false", "null", "this"
};

const char *types[] = {
    "int", "char", "boolean", "void", "String", "Array"
};
