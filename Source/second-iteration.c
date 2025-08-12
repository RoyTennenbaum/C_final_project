#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../Headers/second-iteration.h"

int secondIteration(char *fileName, int ICF, int DCF, binaryWordList *codeImage,
                    assemblerContext *context, symbolTable *entriesTable)
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
    binaryWordNode currentOpWord = **codeImage;

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
            if (handleEntry(&arg, context, &entriesContent, entriesTable, &entriesContentCapacity, lineNum) != TRUE)
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

int handleEntry(char **argP, assemblerContext *context, symbolTable *entriesTable,
                char **entriesContentP, size_t *entriesContentCapacityP, int lineNum)
{
    symbol *tempSymbol;

    *argP = strtok(NULL, " \t");

    if (tempSymbol = searchSymbol(*(context->symbolTable), *argP) != NULL)
    {
        if ((*tempSymbol).type == EXTERN)
        {
            fprintf(stderr, "Error at line %d: .entry operand label is defeined as external \n",
                    lineNum);
            return FALSE;
        }
        else
        {
            writeToEntries(tempSymbol, entriesContentP, entriesContentCapacityP, lineNum);
            /*TO-DO: Insert to entries table*/
        }
    }
    else
    {
        fprintf(stderr, "Error at line %d: .entry operand label is not found\n",
                lineNum);
        return FALSE;
    }
}

writeToEntries(symbol *symbolP, char **entriesContentP,
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
    char *base4Address = BASE4_ADDRESS_INIT;
    int i = 0;

    while (integer)
    {
        base4Address[i] += integer % 4;
        integer /= 4;
        i++;
    }
    return base4Address;
}

int handleOperation(binaryWordNode *opWordP, assemblerContext *context, symbolTable *entriesTable, const char *line, char **externalsContentP,
                    size_t *externalsContentCapacityP, int lineNum)
{
    char lineCopy[80] = {'/0'};
    strcpy(lineCopy, line);
    WordType tempFirstOpWord = (*opWordP).binaryWord;
    unsigned int opCodeEncoding = tempFirstOpWord.opFirst.opcode_bits;
    unsigned int srcOperandAddressEncoding = tempFirstOpWord.opFirst.src_op_bits;
    unsigned int destOperandAddressEncoding = tempFirstOpWord.opFirst.dest_op_bits;
    int state;
    symbol *tempSymbol;
    operationNum operationNum;
    int res;

    switch (opCodeEncoding)
    {
    case MOV:
    case CMP:
    case ADD:
    case SUB:
    case LEA:
        res = handleTwoOperandOpEncoding(opWordP, srcOperandAddressEncoding,
                                         destOperandAddressEncoding, context, entriesTable, lineCopy,
                                         externalsContentP, externalsContentCapacityP, lineNum);
        if (res == MEMORY_ALLOCATION_ERROR)
            return MEMORY_ALLOCATION_ERROR;
        else if (res != TRUE)
            return FALSE;
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
        res = handleOneOperandOpEncoding(opWordP, context, entriesTable, lineCopy,
                                         externalsContentP, externalsContentCapacityP, lineNum);
        if (res == MEMORY_ALLOCATION_ERROR)
            return MEMORY_ALLOCATION_ERROR;
        else if (res != TRUE)
            return FALSE;
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

int handleTwoOperandOpEncoding(binaryWordNode *opFirstWordP,
                               unsigned int srcOperandAddressEncoding,
                               unsigned int destOperandAddressEncoding,
                               const assemblerContext *context, symbolTable *entriesTable, char *lineCopy,
                               char **extenalsContentP, size_t *externalsContentCapacityP,
                               int lineNum)

{

    binaryWordNode *currentWordP = opFirstWordP + 1;
    char *label1;
    char *label2;
    strtok(lineCopy, " \t");
    label1 = strtok(NULL, "[ , \t");
    label2 = strtok(NULL, " \t\n");
    int res;

    if (srcOperandAddressEncoding == DIRECT_ENCODING ||
        srcOperandAddressEncoding == MATRIX_ENCODING)
        res = encodeOpPayloadWord(label1, context, entriesTable, currentWordP,
                                  extenalsContentP, externalsContentCapacityP, lineNum);
    if (res != TRUE)
    {
        if (res == MEMORY_ALLOCATION_ERROR)
            return MEMORY_ALLOCATION_ERROR;
        return FALSE;
    }

    if (srcOperandAddressEncoding == MATRIX_ENCODING)
        currentWordP += 2;
    else
        currentWordP++;

    if (destOperandAddressEncoding == DIRECT_ENCODING ||
        destOperandAddressEncoding == MATRIX_ENCODING)
        res = encodeOpPayloadWord(label2, context, entriesTable, currentWordP,
                                  extenalsContentP, externalsContentCapacityP,
                                  lineNum);
    if (res != TRUE)
    {
        if (res == MEMORY_ALLOCATION_ERROR)
            return MEMORY_ALLOCATION_ERROR;
        return FALSE;
    }
}

int encodeOpPayloadWord(const char *label, const assemblerContext *context,
                        symbolTable *entriesTable, binaryWordNode **currentWordP,
                        char **extenalsContentP, size_t *extenalsContentCapacityP,
                        int lineNum)
{
    symbol *tempSymbol;

    if ((tempSymbol = searchSymbol(*(*context).symbolTable, label)) != NULL)
    {
        switch ((*tempSymbol).type)
        {
        case TYPE_EXTERNAL:
            handleExternal(label, entriesTable, currentWordP, extenalsContentP,
                           extenalsContentCapacityP, lineNum);
            break;
        case TYPE_DATA:
            handleData(label, currentWordP, *tempSymbol, lineNum);
            break;

        default:
            fprintf(stderr, "Error at line %d: failed to encode", lineNum);
            return FALSE;
            break;
        }

        /*if ((*tempSymbol).type == TYPE_EXTERNAL)
        {
            writeToExternals((**currentWordP), label, &extenalsContentP,
                             &extenalsContentCapacityP, lineNum);
            (*currentWordP)->binaryWord.payload.aer_bits = AER_EXTERNAL_ENCODING;
        }

        else if ((*tempSymbol).type == TYPE_DATA)
            (*currentWordP)->binaryWord.payload.aer_bits = AER_RELOCATABLE_ENCODING;*/
    }
    else
    {
        fprintf(stderr, "Error at line %d: ''%s'' is not defiend", label, lineNum);
        return FALSE;
    }
}

int handleExternal(const char *label, symbolTable *entriesTable, binaryWordNode **currentWordP,
                   char **extenalsContentP, size_t *extenalsContentCapacityP, int lineNum)
{
    symbol *tempEntry;
    if ((tempEntry = searchSymbol(*entriesTable, label)) != NULL)
    {
        (*currentWordP)->binaryWord.payload.payload_bits = (*tempEntry).address;
        (*currentWordP)->binaryWord.payload.aer_bits = AER_EXTERNAL_ENCODING;

        writeToExternals((**currentWordP), label, &extenalsContentP,
                         &extenalsContentCapacityP, lineNum);
    }
    else
    {
        fprintf(stderr, "Error at line %d: external refering to undefiend label", lineNum);
        return FALSE;
    }
}

int writeToExternals(binaryWordNode currentWordP, const char *label, char **extenalsContentP,
                     size_t *extenalsContentCapacityP, int lineNum)
{
    size_t newCapacity;
    char *newContent;
    int address = currentWordP.C + currentWordP.L;
    char *base4Address = decimalToBase4(address);

    newCapacity =
        *extenalsContentCapacityP + sizeof(label) + sizeof(base4Address) + 1;

    newContent = (char *)malloc(newCapacity);

    if (newContent == NULL)
    {
        fprintf(stderr, "Error at line %d: failed to allocate memory\n", lineNum);
        return MEMORY_ALLOCATION_ERROR;
    }

    *extenalsContentP = newContent;
    *extenalsContentCapacityP = newCapacity;

    strcat(*extenalsContentP, label);
    strcat(*extenalsContentP, "\t");
    strcat(*extenalsContentP, base4Address);

    return TRUE;
}
handleData(const char *label, binaryWordNode **currentWordP, symbol tempSymbol, int lineNum)
{
    (*currentWordP)->binaryWord.payload.payload_bits = tempSymbol.address;
    (*currentWordP)->binaryWord.payload.aer_bits = AER_RELOCATABLE_ENCODING;
}

int handleOneOperandOpEncoding(binaryWordNode *opFirstWordP,
                               unsigned int destOperandAddressEncoding,
                               const assemblerContext *context, char *lineCopy,
                               char **extenalsContentP, size_t *externalsContentCapacityP,
                               int lineNum)
{
    binaryWordNode *currentWordP = opFirstWordP + 1;
    char *label;
    strtok(lineCopy, " \t");
    label = strtok(NULL, "[\n");
    int res;

    if (destOperandAddressEncoding == DIRECT_ENCODING ||
        destOperandAddressEncoding == MATRIX_ENCODING)
        res = encodeOpPayloadWord(destOperandAddressEncoding, label, context, currentWordP,
                                  extenalsContentP, externalsContentCapacityP, lineNum);
    if (res != TRUE)
    {
        if (res == MEMORY_ALLOCATION_ERROR)
            return MEMORY_ALLOCATION_ERROR;
        return FALSE;
    }
    return TRUE;
}
