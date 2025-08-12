#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../Headers/second-iteration.h"

#define MEMORY_ALLOCATION_ERROR -1
#define BASE4_ADDRESS_INIT "aaaa"
#define BASE4_WORD_INIT "aaaaa"
#define BINARY_ADDRESS_INIT "0000000000"
#define IMMEDIATE_ADDRESSING_ENCODING "00"
#define DIRECT_ADDRESSING_ENCODING "01"
#define MATRIX_ADDRESSING_ENCODING "10"
#define REGISTER_ADDRESSING_ENCODING "11"
#define AER_EXTERNAL_ENCODING 1
#define AER_RELOCATABLE_ENCODING 2

enum
{
    UNKNOWN_LINE_TYPE,
    EMPTY_LINE,
    COMMENT_LINE,
    NON_ENTRY_DIRECTIVE_LINE,
    ENTRY_LINE,
    OPERATION_LINE
} lineType;

enum
{
    UNKNOWN_ENCODING,
    NON_LABEL_ENCODING,
    DIRECT_ENCODING,
    MATRIX_ENCODING
} opEncodings;

int secondIteration(char *fileName, codeImage *Img, assemblerContext *context)
{
    int state;
    char line[LINE_SIZE] = {'\0'};
    char *arg;
    char *srcFileName;
    FILE *srcFile;
    directive *tempDirective;
    int lineNum = 0;
    int errorFlag = FALSE;
    char *entriesContent;
    size_t entriesContentCapacity = 0;
    char *externalsContent;
    size_t externalsContentCapacity = 0;
    binaryWordNode currentOpWord = **Img;

    srcFileName = malloc(strlen(fileName) + 4);

    if (srcFileName == NULL)
    {
        fprintf(stderr, "Error: failed to allocate memory\n");
        return FALSE;
    }

    strcpy(srcFileName, fileName);
    strcat(srcFileName, ".am");

    /* Open source file for reading */
    srcFile = fopen(srcFileName, "r");
    if (srcFile == NULL)
    {
        fprintf(stderr, "Error: Cannot open file '%s'\n", srcFileName);
        return FALSE;
    }

    while (fgets(line, LINE_SIZE, srcFile) != NULL)
    {
        lineNum++;
        state = UNKNOWN_LINE_TYPE;

        arg = strtok(line, " \t");

        if (arg == NULL)
            state = EMPTY_LINE;

        else if (arg[0] == ';')
            state = COMMENT_LINE;

        else if (searchSymbol(*(context->symbolTable), arg) != NULL)
            arg = strtok(NULL, " \t");

        else if ((tempDirective =
                      searchDirective(*(context->directiveTable), arg)) != NULL)
        {
            if ((*tempDirective).type != ENTRY)
                state = NON_ENTRY_DIRECTIVE_LINE;
            else
                state = ENTRY_LINE;
        }

        else
            state = OPERATION_LINE;

        switch (state)
        {
        case EMPTY_LINE:
        case COMMENT_LINE:
        case NON_ENTRY_DIRECTIVE_LINE:
            continue;

        case ENTRY_LINE:
            if (handleEntry(&arg, context, &entriesContent,
                            &entriesContentCapacity, lineNum) != TRUE)
                errorFlag = TRUE;
            break;

        case OPERATION_LINE:
            break;

        default:
            /* Unexpected line type */
            fprintf(stderr, "Error at line %d: unknown line type\n", lineNum);
            errorFlag = TRUE;
            break;
        }
    }
}

int handleEntry(char **argP, assemblerContext *context,
                char **entriesContentP, size_t *entriesContentCapacityP, int lineNum)
{
    symbol *tempSymbol;

    *argP = strtok(NULL, " \t");

    if (tempSymbol = searchSymbol(*(context->symbolTable), *argP) == NULL)
    {
        fprintf(stderr, "Error at line %d: .entry operand label is not found\n",
                lineNum);
        return FALSE;
    }
    else if ((*tempSymbol).type == EXTERN)
    {
        fprintf(stderr, "Error at line %d: .entry operand label is defeined as external \n",
                lineNum);
        return FALSE;
    }
    else
    {
        insertSymbolToEnteries(tempSymbol, entriesContentP, entriesContentCapacityP,
                               lineNum);
    }
}

insertSymbolToEnteries(symbol *symbolP, char **entriesContentP,
                       size_t *entriesContentCapacityP, int lineNum)
{

    size_t newCapacity;
    char *newContent;
    char *label = (*symbolP).label;
    int address = (*symbolP).address;
    char *base4Address = decimalToBase4(address);

    newCapacity =
        *entriesContentCapacityP + sizeof(label) + sizeof(base4Address) + 1;

    newContent = (char *)malloc(newCapacity);

    if (newContent == NULL)
    {
        fprintf(stderr, "Error at line %d: failed to allocate memory\n", lineNum);
        return MEMORY_ALLOCATION_ERROR;
    }

    *entriesContentP = newContent;
    *entriesContentCapacityP = newCapacity;

    strcat(*entriesContentP, label);
    strcat(*entriesContentP, "\t");
    strcat(*entriesContentP, base4Address);
}

char *intToBase4(int integer)
{
    char *res = BASE4_ADDRESS_INIT;
    int i = 0;

    while (integer)
    {
        res[i] += integer % 4;
        integer /= 4;
        i++;
    }
    return res;
}

int handleOperation(binaryWordNode *opWordP, assemblerContext *context, char *lineCopy, char **externalsContentP,
                    size_t *externalsContentCapacityP, int lineNum)
{
    WordType tempFirstOpWord = (*opWordP).binaryWord;
    unsigned int opCodeEncoding = tempFirstOpWord.opFirst.opcode_bits;
    unsigned int srcOperandAddressEncoding = tempFirstOpWord.opFirst.src_op_bits;
    unsigned int destOperandAddressEncoding = tempFirstOpWord.opFirst.dest_op_bits;
    int state;
    symbol *tempSymbol;
    operationNum operationNum;

    switch (opCodeEncoding)
    {
    case MOV:
    case CMP:
    case ADD:
    case SUB:
    case LEA:
        handleTwoOperandOpEncoding(opWordP, srcOperandAddressEncoding,
                                   destOperandAddressEncoding, context, Img, lineCopy,
                                   externalsContentP, externalsContentCapacityP, lineNum);
        break;

    case NOT:
    case CLR:
    case INC:
    case DEC:
    case JMP:
    case BNE:
    case RED:
    case PRN:
    case JSR:
        break;

    case RTS:
    case STP:
        break;

    default:
        break;
    }
    free(opCodeEncoding);
    free(srcOperandAddressEncoding);
    free(destOperandAddressEncoding);
}

int handleTwoOperandOpEncoding(binaryWordNode *opFirstWordP, unsigned srcOperandAddressEncoding,
                               unsigned destOperandAddressEncoding,
                               assemblerContext *context, codeImage *Img, char *lineCopy, char **extenalsContentP,
                               size_t *externalsContentCapacityP, int lineNum)
{
    binaryWordNode *currentWordP = opFirstWordP;
    char *label1;
    char *label2;
    strtok(lineCopy, " \t");
    label1 = strtok(NULL, "[,");
    label2 = strtok(NULL, " \t\n");
    int res;

    if (strcmp(srcOperandAddressEncoding, DIRECT_ENCODING) ||
        strcmp(srcOperandAddressEncoding, MATRIX_ENCODING))
        res = encodeOpPayloadWord(srcOperandAddressEncoding, label1, context, currentWordP,
                                  extenalsContentP, externalsContentCapacityP, lineNum);
    if (res != TRUE)
    {
        if (res == MEMORY_ALLOCATION_ERROR)
            return MEMORY_ALLOCATION_ERROR;
        return FALSE;
    }
}

int encodeOpPayloadWord(unsigned int OperandAddressEncoding, char *label, assemblerContext *context,
                        binaryWordNode **currentWordP, char **extenalsContentP,
                        size_t *extenalsContentCapacityP, int lineNum)
{
    symbol *tempSymbol;

    if ((tempSymbol = searchSymbol(*(*context).symbolTable, label)) != NULL)
    {
        (*currentWordP)++;
        if ((*tempSymbol).type == TYPE_EXTERNAL)
        {
            writeToExternals((**currentWordP), label, lineNum);
            (*currentWordP)->binaryWord.payload.aer_bits = AER_EXTERNAL_ENCODING;
        }

        else if ((*tempSymbol).type == TYPE_DATA)
            (*currentWordP)->binaryWord.payload.aer_bits = AER_RELOCATABLE_ENCODING;
    }
    else
    {
        fprintf(stderr, "Error at line %d: ''%s'' is not defiend", label, lineNum);
        return FALSE;
    }
}

writeToExternals(binaryWordNode currentWordP, char *label, char **extenalsContentP,
                 size_t *extenalsContentCapacityP, int lineNum)
{
}

handleOneOperandOpEncoding()
{
}
