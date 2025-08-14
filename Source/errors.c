#include <stdio.h>
#include "../Headers/errors.h"

int fatalError = FALSE;

void setFatalError(int lineNum, errorType type) {
    fatalError = TRUE;
    printf("[XX] Line %-4d | FATAL ERROR: %s\n", lineNum, getErrMessage(type));
}

const char *getErrMessage(errorType type) {
    switch (type) {
    case ERR_MEM_ALLOC:
        return "Failed to allocate memory";
    case ERR_FILE_OPEN:
        return "Cannot open file";
    case ERR_IO:
        return "Generic IO error";

    case ERR_LINE_TOO_LONG:
        return "Line too long";
    case ERR_UNKNOWN_LINE_TYPE:
        return "Unknown line type";
    case ERR_INSERT_EMPTY_LINE_FAIL:
        return "Failed to insert empty line";

    case ERR_MACRO_EXPANSION_FAIL:
        return "Macro expansion failed";
    case ERR_ILLEGAL_MACRO_NAME:
        return "Illegal macro name";

    case ERR_SYMBOL_NOT_FOLLOWED:
        return "Symbol is not followed by a directive or an instruction";
    case ERR_DUPLICATE_SYMBOL:
        return "Duplicate symbol definition";
    case ERR_UNDEFINED_SYMBOL:
        return "Undefined symbol referenced";
    case ERR_SYMBOL_INVALID_LEN:
        return "Invalid symbol length (1-30 chars)";
    case ERR_RESERVED_NAME:
        return "Symbol or identifier uses reserved name";

    case ERR_INVALID_COMMAND:
        return "Command does not exist";

    case ERR_TOO_MANY_OPERANDS:
        return "Too many operands";
    case ERR_MISSING_OPERANDS:
        return "Missing operands";
    case ERR_INVALID_OPERAND:
        return "Invalid operand";

    case ERR_INVALID_ADDRESSING_METHOD:
        return "Invalid addressing method";

    case ERR_INVALID_INTEGER:
        return "One or more of the params is not a legal integer";
    case ERR_NUMBER_OUT_OF_RANGE:
        return "Number out of valid range";
    case ERR_INVALID_NUMBER_FORMAT:
        return "Invalid number format";

    case ERR_DIR_NO_PARAM:
        return "No parameter following the directive";

    case ERR_STR_TOO_MANY_PARAMS:
        return "Only one parameter allowed for '.string' directive";
    case ERR_STRING_TOO_SHORT:
        return "Parameter of '.string' directive is too short";
    case ERR_STRING_MISSING_QUOTES:
        return "Missing \" in the beginning or end of the parameter";

    case ERR_MAT_INVALID_DIM:
        return "Invalid matrix dimensions format";
    case ERR_MAT_TOO_MANY_VALUES:
        return "Too many values for matrix";
    case ERR_MAT_INVALID_VALUE:
        return "One or more of the values is not a legal integer";
    case ERR_MAT_INVALID_REGISTERS:
        return "Invalid row and/or column registers";
    case ERR_MAT_ADDRESS_ILLEGAL_LEN:
        return "Illegal length for matrix addressing (min 9 chars)";

    case ERR_INVALID_REGISTER:
        return "Invalid register name";

    case ERR_ENTRY_SYMBOL_NOT_FOUND:
        return "'.entry' operand symbol was not found";
    case ERR_ENTRY_SYMBOL_DEFINED_EXTERN:
        return "'.entry' operand symbol was defined as external";

    case ERR_CODE_IMAGE_OVERFLOW:
        return "Code image overflow";
    case ERR_ALIGNMENT:
        return "Memory alignment error";
    case ERR_INTERNAL:
        return "Internal assembler error";
    case ERR_SYNTAX:
        return "Syntax error";
    default:
        return "Unknown error";
    }
}