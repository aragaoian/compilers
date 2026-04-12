#ifndef CONSTANTS_H
#define CONSTANTS_H

enum TokenId 
{
    EPSILON  = 0,
    DOLLAR   = 1,
    t_ID = 2,
    t_PRINT = 3,
    t_ONELINE_COMMENT = 4,
    t_MULTILINE_COMMENT = 5,
    t_IF = 6,
    t_THEN = 7,
    t_ELSE = 8,
    t_TRY = 9,
    t_CATCH = 10,
    t_FINALLY = 11,
    t_END = 12,
    t_WHILE = 13,
    t_FOR = 14,
    t_CONST = 15,
    t_BREAK = 16,
    t_CONTINUE = 17,
    t_RETURN = 18,
    t_DO = 19,
    t_NEW = 20,
    t_DEFAULT = 21,
    t_CIN = 22,
    t_COUT = 23,
    t_VOID = 24,
    t_INT_KEY = 25,
    t_INT_VALUE_DECIMAL = 26,
    t_INT_VALUE_BINARY = 27,
    t_INT_VALUE_HEXADECIMAL = 28,
    t_FLOAT_KEY = 29,
    t_FLOAT_VALUE = 30,
    t_BOOLEAN_KEY = 31,
    t_TRUE = 32,
    t_FALSE = 33,
    t_CHAR_KEY = 34,
    t_CHAR_VALUE = 35,
    t_STRING_KEY = 36,
    t_STRING_VALUE = 37,
    t_LOG = 38,
    t_SQRT = 39,
    t_EXP = 40,
    t_DIV = 41,
    t_MUL = 42,
    t_SUM = 43,
    t_SUB = 44,
    t_EQUAL = 45,
    t_MOD = 46,
    t_GREATER_EQ = 47,
    t_LESSER_EQ = 48,
    t_EQUAL_CONDITION = 49,
    t_DIFFERENT_CONDITION = 50,
    t_GREATER = 51,
    t_LESSER = 52,
    t_RIGHT_SHIFT = 53,
    t_LEFT_SHIFT = 54,
    t_BITWISE_AND = 55,
    t_BITWISE_OR = 56,
    t_BITWISE_NOT = 57,
    t_BITWISE_XOR = 58,
    t_AND = 59,
    t_OR = 60,
    t_NOT = 61,
    t_TOKEN_62 = 62, //"("
    t_TOKEN_63 = 63, //")"
    t_TOKEN_64 = 64, //"["
    t_TOKEN_65 = 65, //"]"
    t_TOKEN_66 = 66, //"{"
    t_TOKEN_67 = 67, //"}"
    t_TOKEN_68 = 68, //","
    t_TOKEN_69 = 69, //"."
    t_TOKEN_70 = 70, //";"
    t_TOKEN_71 = 71, //":
};

const int STATES_COUNT = 51;

extern int SCANNER_TABLE[STATES_COUNT][256];

extern int TOKEN_STATE[STATES_COUNT];

extern int SPECIAL_CASES_INDEXES[73];

extern const char *SPECIAL_CASES_KEYS[30];

extern int SPECIAL_CASES_VALUES[30];

extern const char *SCANNER_ERROR[STATES_COUNT];

const int FIRST_SEMANTIC_ACTION = 147;

const int SHIFT  = 0;
const int REDUCE = 1;
const int ACTION = 2;
const int ACCEPT = 3;
const int GO_TO  = 4;
const int ERROR  = 5;

extern const int PARSER_TABLE[254][147][2];

extern const int PRODUCTIONS[156][2];

extern const char *PARSER_ERROR[254];

#endif
