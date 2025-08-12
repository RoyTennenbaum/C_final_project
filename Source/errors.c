#include "../Headers/errors.h"

const char *getErrMessage(errorType type) {
    switch (type) {
    case ERR_MEM_ALLOC:
        return "Error: failed to allocate memory";
    case ERR_FILE_OPEN:
        return "Error: cannot open file '%s'";
    case ERR_IO:
        return "Error: IO error";
    case ERR_LINE_TOO_LONG:
        return "Error at line %d: line is too long";
    case ERR_UNKNOWN_LINE_TYPE:
        return "Error at line %d: unknown line type";
    case ERR_INSERT_EMPTY_LINE_FAIL:
        return "Error at line %d: failed to insert empty line";
    case ERR_MACRO_EXPANSION_FAIL:
        return "Error at line %d: macro expansion failed";
    case ERR_INVALID_MACRO_NAME:
        return "Error at line %d: invalid macro name '%s'";
    case ERR_SYMBOL_NOT_FOLLOWED:
        return "Error: symbol is not followed by a directive or an instruction";
    case ERR_DUPLICATE_SYMBOL:
        return "Error: duplicate symbol definition";
    case ERR_UNDEFINED_SYMBOL:
        return "Error: undefined symbol referenced";
    case ERR_SYMBOL_TOO_LONG:
        return "Error: symbol exceeds maximum length";
    case ERR_RESERVED_NAME:
        return "Error: symbol or identifier uses reserved name";
    case ERR_INVALID_COMMAND:
        return "Error: command does not exist";
    case ERR_INVALID_OPERAND_COUNT:
        return "Error: invalid number of operands";
    case ERR_TOO_MANY_OPERANDS:
        return "Error: too many operands";
    case ERR_MISSING_OPERANDS:
        return "Error: missing operands";
    case ERR_INVALID_ADDRESSING:
        return "Error: invalid addressing method(s)";
    case ERR_INVALID_INTEGER:
        return "Error: one or more of the params is not a legal integer";
    case ERR_NUMBER_OUT_OF_RANGE:
        return "Error: number out of range";
    case ERR_INVALID_NUMBER_FORMAT:
        return "Error: invalid number format";
    case ERR_STRING_NO_PARAM:
        return "Error: no parameter for '.string' directive";
    case ERR_STRING_TOO_MANY_PARAMS:
        return "Error: only one parameter is allowed for '.string' directive";
    case ERR_STRING_TOO_SHORT:
        return "Error: parameter of '.string' directive is too short";
    case ERR_STRING_MISSING_QUOTES:
        return "Error: no \" in the beginning or end of '.string' parameter";
    case ERR_MAT_NO_PARAMS:
        return "Error: no parameters for '.mat' directive";
    case ERR_MAT_INVALID_DIMS:
        return "Error: invalid '.mat' dimensions format";
    case ERR_MAT_TOO_MANY_VALUES:
        return "Error: too many values for matrix";
    case ERR_MAT_INVALID_VALUE:
        return "Error: one or more of the values is not a legal integer";
    case ERR_MAT_INVALID_REGISTERS:
        return "Error: invalid row and column registers";
    case ERR_MATRIX_ADDR_ILLEGAL_LEN:
        return "Error: illegal length for matrix addressing";
    case ERR_ENTRY_SYMBOL_NOT_FOUND:
        return "Error at line %d: .entry operand symbol is not found";
    case ERR_ENTRY_SYMBOL_DEFINED_EXTERN:
        return "Error at line %d: .entry operand symbol is defined as external";
    case ERR_CODE_IMAGE_OVERFLOW:
        return "Error: code or data image overflow";
    case ERR_ALIGNMENT:
        return "Error: memory alignment error";
    case ERR_INTERNAL:
        return "Internal assembler error";
    case ERR_SYNTAX:
        return "Syntax error";
    default:
        return "Unknown error";
    }
}