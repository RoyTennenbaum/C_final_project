#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "../Headers/second-iteration.h"

int secondIteration(char *fileName, int ICF, int DCF, binaryWordList *codeImage, assemblerContext *context,
                    symbolTable *entriesTable) {
    int state;
    char line[LINE_SIZE] = {'\0'};
    char *arg;
    char *srcFileName;
    FILE *srcFile;
    const directive *tempDirective;
    int lineNum = 0;
    int errorFlag = FALSE;
    char *entriesContent;
    size_t entriesContentCapacity = 0;
    char *externalsContent;
    size_t externalsContentCapacity = 0;
    binaryWordNode *currentOpWordP = *codeImage;
    int res;

    srcFileName = malloc(strlen(fileName) + 4);

    if (srcFileName == NULL) {
        fprintf(stderr, "Error: failed to allocate memory\n");
        return FALSE;
    }

    strcpy(srcFileName, fileName);
    strcat(srcFileName, ".am");

    /* Process each line of source file */
    srcFile = fopen(srcFileName, "r");
    if (srcFile == NULL) {
        fprintf(stderr, "Error: Cannot open file '%s'\n", srcFileName);
        return FALSE;
    }

    while (fgets(line, LINE_SIZE, srcFile) != NULL) {
        lineNum++;
        printf("\nLINE #%d\n", lineNum);
        state = UNKNOWN_LINE_TYPE;

        arg = strtok(line, ":\n");
        printf("The first word is: '%s'\n", arg);

        if (arg == NULL)
            state = EMPTY_LINE;

        else if (arg[0] == ';')
            state = COMMENT_LINE;

        /* Check if line starts with symbol definition */
        if (searchSymbol(*(context->symbolTable), arg) != NULL) {
            arg = strtok(NULL, "\n");
            while (isspace((unsigned char)*arg))
                arg++;
            printf("First word was a symbol. Second word is: '%s'\n", arg);
        }

        else if ((tempDirective = searchDirective(*(context->directiveTable), arg)) != NULL) {
            if ((*tempDirective).type != ENTRY)
                state = NON_ENTRY_DIRECTIVE_LINE;
            else
                state = ENTRY_LINE;
        }

        else
            state = OPERATION_LINE;

        /* Process line based on type */
        switch (state) {
        case EMPTY_LINE:
        case COMMENT_LINE:
        case NON_ENTRY_DIRECTIVE_LINE:
            printf("Line is empty/comment/non-entry-directive\n");
            continue;

        case ENTRY_LINE:
            printf("Line is an entry line\n");
            res = handleEntryLine(&arg, lineNum, context, entriesTable, &entriesContent, &entriesContentCapacity);
            if (res != TRUE) {
                if (res == MEMORY_ALLOCATION_ERROR)
                    return FALSE;
                else
                    errorFlag = TRUE;
            }
            break;

        case OPERATION_LINE:
            printf("Line is an operation line\n");
            res = handleOperationLine(&currentOpWordP, arg, lineNum, context, entriesTable, &externalsContent,
                                      &externalsContentCapacity);
            if (res != TRUE) {
                if (res == MEMORY_ALLOCATION_ERROR)
                    return FALSE;
                else
                    errorFlag = TRUE;
            }
            break;

        default:
            /* Unexpected line type */
            fprintf(stderr, "Error at line %d: unknown line type\n", lineNum);
            errorFlag = TRUE;
            break;
        }
    }

    /* Generate output files if no errors occurred */
    if (errorFlag == FALSE) {
        res = createObjectOutputFile(fileName, *codeImage, ICF, DCF, context);
        if (res != TRUE) {
            if (res == MEMORY_ALLOCATION_ERROR)
                return FALSE;
            else
                errorFlag = TRUE;
        }
    }

    if (errorFlag == FALSE && entriesContentCapacity > 0) {
        res = createEntriesOutputFile(fileName, entriesContent, context);
        if (res != TRUE) {
            if (res == MEMORY_ALLOCATION_ERROR)
                return FALSE;
            else
                errorFlag = TRUE;
        }
    }

    if (errorFlag == FALSE && externalsContentCapacity > 0) {
        res = createExternalsOutputFile(fileName, externalsContent, context);
        if (res != TRUE) {
            if (res == MEMORY_ALLOCATION_ERROR)
                return FALSE;
            else
                errorFlag = TRUE;
        }
    }

    fclose(srcFile);
    free(srcFileName);

    return TRUE;
}

/* Handle .entry directive processing */
int handleEntryLine(char **argP, int lineNum, assemblerContext *context, symbolTable *entriesTable,
                    char **entriesContentP, size_t *entriesContentCapacityP) {
    symbol *tempSymbol;

    *argP = strtok(NULL, " \t");
    printf("Next word: '%s'\n", *argP);

    if ((tempSymbol = searchSymbol(*(context->symbolTable), *argP)) != NULL) {
        if ((*tempSymbol).type == TYPE_EXTERNAL) {
            fprintf(stderr, "Error at line %d: .entry operand label is defeined as external \n", lineNum);
            return FALSE;
        } else {
            writeToEntries(tempSymbol, entriesContentP, entriesContentCapacityP, lineNum, context);
            /* TODO: Insert to entries table */
        }
    } else {
        fprintf(stderr, "Error at line %d: .entry operand label is not found\n", lineNum);
        return FALSE;
    }
    return TRUE;
}

/* Write symbol to entries content buffer */
int writeToEntries(symbol *symbolP, char **entriesContentP, size_t *entriesContentCapacityP, int lineNum,
                   assemblerContext *context) {
    size_t newCapacity;
    char *newContent;
    char *label = (*symbolP).label;
    int address = (*symbolP).address;
    char *base4Address = intToBase4(address);

    newCapacity = *entriesContentCapacityP + strlen(label) + strlen(base4Address) + 1;

    newContent = (char *)malloc(newCapacity);

    if (newContent == NULL) {
        fprintf(stderr, "Error at line %d: failed to allocate memory\n", lineNum);
        return MEMORY_ALLOCATION_ERROR;
    }

    *entriesContentP = newContent;
    *entriesContentCapacityP = newCapacity;

    strcat(*entriesContentP, label);
    strcat(*entriesContentP, "\t");
    strcat(*entriesContentP, base4Address);

    printf("entriesContent: '%s'\n", *entriesContentP);
    return TRUE;
}

/* Convert integer to base-4 representation */
char *intToBase4(int integer) {
    char *base4Address = BASE4_ADDRESS_INIT;
    int i = 0;

    while (integer) {
        base4Address[i] += integer % 4;
        integer /= 4;
        i++;
    }
    return base4Address;
}

/* Process operation line and encode binary words */
int handleOperationLine(binaryWordNode **opWordP, char *lineCopy, int lineNum, assemblerContext *context,
                        symbolTable *entriesTable, char **externalsContentP, size_t *externalsContentCapacityP) {
    WordType tempFirstOpWord = (*opWordP)->binaryWord;
    unsigned int opCodeEncoding = tempFirstOpWord.opFirst.opcode_bits;
    unsigned int srcOperandAddressEncoding = tempFirstOpWord.opFirst.src_op_bits;
    unsigned int destOperandAddressEncoding = tempFirstOpWord.opFirst.dest_op_bits;
    int res;

    /* Route to appropriate handler based on operation type */
    switch (opCodeEncoding) {
    case MOV:
    case CMP:
    case ADD:
    case SUB:
    case LEA:
        res = handleTwoOperandOpEncoding(*opWordP, lineCopy, srcOperandAddressEncoding, destOperandAddressEncoding,
                                         lineNum, context, entriesTable, externalsContentP, externalsContentCapacityP);
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
        res = handleOneOperandOpEncoding(*opWordP, lineCopy, destOperandAddressEncoding, lineNum, context, entriesTable,
                                         externalsContentP, externalsContentCapacityP);
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

    /* Advance to the next binary word in the code image */
    do
        opWordP = &((*opWordP)->next);
    while ((*opWordP)->L != 0);

    return TRUE;
}

/* Handle two-operand instruction encoding */
int handleTwoOperandOpEncoding(binaryWordNode *opFirstWordP, char *lineCopy, unsigned int srcOperandAddressEncoding,
                               unsigned int destOperandAddressEncoding, int lineNum, assemblerContext *context,
                               symbolTable *entriesTable, char **externalsContentP, size_t *externalsContentCapacityP) {
    binaryWordNode *currentWordP = opFirstWordP->next;
    char *label1;
    char *label2;
    int res;

    strtok(lineCopy, " \t");
    label1 = strtok(NULL, "[ , \t");
    label2 = strtok(NULL, " \t\n");

    printf("lineCopy: '%s'\n", lineCopy);
    printf("label1: '%s'\n", label1);
    printf("label2: '%s'\n", label2);

    /* Encode source operand if needed */
    if (srcOperandAddressEncoding == DIRECT_ENCODING || srcOperandAddressEncoding == MATRIX_ENCODING)
        res = encodeOpPayloadWord(label1, lineNum, context, entriesTable, currentWordP, externalsContentP,
                                  externalsContentCapacityP);
    if (res != TRUE) {
        if (res == MEMORY_ALLOCATION_ERROR)
            return MEMORY_ALLOCATION_ERROR;
        return FALSE;
    }

    /* Advance pointer based on operand type */
    if (srcOperandAddressEncoding == MATRIX_ENCODING)
        currentWordP = (opFirstWordP->next)->next;
    else
        currentWordP = opFirstWordP->next;

    /* Encode destination operand if needed */
    if (destOperandAddressEncoding == DIRECT_ENCODING || destOperandAddressEncoding == MATRIX_ENCODING)
        res = encodeOpPayloadWord(label2, lineNum, context, entriesTable, currentWordP, externalsContentP,
                                  externalsContentCapacityP);
    if (res != TRUE) {
        if (res == MEMORY_ALLOCATION_ERROR)
            return MEMORY_ALLOCATION_ERROR;
        return FALSE;
    }
    return TRUE;
}

/* Encode payload word based on symbol type */
int encodeOpPayloadWord(char *label, int lineNum, assemblerContext *context, symbolTable *entriesTable,
                        binaryWordNode *currentWordP, char **externalsContentP, size_t *externalsContentCapacityP) {
    symbol *tempSymbol;

    if ((tempSymbol = searchSymbol(*(context->symbolTable), label)) != NULL) {
        switch ((*tempSymbol).type) {
        case TYPE_EXTERNAL:
            handleExternal(label, lineNum, entriesTable, currentWordP, externalsContentP, externalsContentCapacityP,
                           context);
            break;
        case TYPE_DATA:
            handleData(label, *tempSymbol, lineNum, currentWordP, context);
            break;

        default:
            fprintf(stderr, "Error at line %d: failed to encode", lineNum);
            return FALSE;
            break;
        }
    } else {
        fprintf(stderr, "Error at line %d: ''%s'' is not defiend", lineNum, label);
        return FALSE;
    }
    return TRUE;
}

/* Handle external symbol reference */
int handleExternal(char *label, int lineNum, symbolTable *entriesTable, binaryWordNode *currentWordP,
                   char **externalsContentP, size_t *externalsContentCapacityP, assemblerContext *context) {
    int res;
    symbol *tempEntry;
    if ((tempEntry = searchSymbol(*entriesTable, label)) != NULL) {
        currentWordP->binaryWord.payload.payload_bits = (*tempEntry).address;
        currentWordP->binaryWord.payload.aer_bits = AER_EXTERNAL_ENCODING;

        res = writeToExternals(*currentWordP, label, lineNum, externalsContentP, externalsContentCapacityP, context);
        if (res != TRUE) {
            if (res == MEMORY_ALLOCATION_ERROR)
                return MEMORY_ALLOCATION_ERROR;
            else
                return FALSE;
        }
    } else {
        fprintf(stderr, "Error at line %d: external refering to undefiend label", lineNum);
        return FALSE;
    }
    return TRUE;
}

/* Write external reference to output buffer */
int writeToExternals(binaryWordNode currentWordP, char *label, int lineNum, char **externalsContentP,
                     size_t *externalsContentCapacityP, assemblerContext *context) {
    size_t newCapacity;
    char *newContent;
    int address = currentWordP.C + currentWordP.L;
    char *base4Address = intToBase4(address);

    newCapacity = *externalsContentCapacityP + sizeof(label) + sizeof(base4Address) + 1;

    newContent = (char *)malloc(newCapacity);

    if (newContent == NULL) {
        fprintf(stderr, "Error at line %d: failed to allocate memory\n", lineNum);
        return MEMORY_ALLOCATION_ERROR;
    }

    *externalsContentP = newContent;
    *externalsContentCapacityP = newCapacity;

    strcat(*externalsContentP, label);
    strcat(*externalsContentP, "\t");
    strcat(*externalsContentP, base4Address);

    printf("externalsContentP: '%s'\n", *externalsContentP);

    return TRUE;
}

/* Handle data symbol encoding */
int handleData(char *label, symbol tempSymbol, int lineNum, binaryWordNode *currentWordP, assemblerContext *context) {
    currentWordP->binaryWord.payload.payload_bits = tempSymbol.address;
    currentWordP->binaryWord.payload.aer_bits = AER_RELOCATABLE_ENCODING;

    return TRUE;
}

/* Handle single-operand instruction encoding */
int handleOneOperandOpEncoding(binaryWordNode *opFirstWordP, char *lineCopy, unsigned int destOperandAddressEncoding,
                               int lineNum, assemblerContext *context, symbolTable *entriesTable,
                               char **externalsContentP, size_t *externalsContentCapacityP) {
    int res;
    binaryWordNode *currentWordP = opFirstWordP->next;
    char *label;

    strtok(lineCopy, " \t");
    label = strtok(NULL, "[\n");

    printf("lineCopy: '%s'\n", lineCopy);
    printf("label: '%s'\n", label);

    if (destOperandAddressEncoding == DIRECT_ENCODING || destOperandAddressEncoding == MATRIX_ENCODING)
        res = encodeOpPayloadWord(label, lineNum, context, entriesTable, currentWordP, externalsContentP,
                                  externalsContentCapacityP);
    if (res != TRUE) {
        if (res == MEMORY_ALLOCATION_ERROR)
            return MEMORY_ALLOCATION_ERROR;
        return FALSE;
    }
    return TRUE;
}

/* Generate .ent output file */
int createEntriesOutputFile(char *fileName, char *entriesContent, assemblerContext *context) {
    FILE *entriesFile;
    char *entriesFileName;

    /* Create output filename */
    entriesFileName = malloc(strlen(fileName) + 5);
    if (entriesFileName == NULL) {
        fprintf(stdout, "Error: failed to allocate memory\n");
        return MEMORY_ALLOCATION_ERROR;
    }

    strcpy(entriesFileName, fileName);
    strcat(entriesFileName, ".ent");

    /* Write entries to file */
    entriesFile = fopen(entriesFileName, "w");
    if (entriesFile == NULL) {
        fprintf(stdout, "Error: cannot open file '%s'\n", entriesFileName);
        free(entriesFileName);
        return FALSE;
    }

    fputs(entriesContent, entriesFile);
    fclose(entriesFile);
    free(entriesFileName);

    return TRUE;
}

/* Generate .ext output file */
int createExternalsOutputFile(char *fileName, char *externalsContent, assemblerContext *context) {
    FILE *externalsFile;
    char *externalsFileName;

    /* Create output filename */
    externalsFileName = malloc(strlen(fileName) + 5);
    if (externalsFileName == NULL) {
        fprintf(stdout, "Error: failed to allocate memory\n");
        return MEMORY_ALLOCATION_ERROR;
    }

    strcpy(externalsFileName, fileName);
    strcat(externalsFileName, ".ext");

    /* Write externals to file */
    externalsFile = fopen(externalsFileName, "w");
    if (externalsFile == NULL) {
        fprintf(stdout, "Error: cannot open file '%s'\n", externalsFileName);
        free(externalsFileName);
        return FALSE;
    }

    fputs(externalsContent, externalsFile);
    fclose(externalsFile);
    free(externalsFileName);

    return TRUE;
}

/* Generate .obj output file with binary image */
int createObjectOutputFile(char *fileName, binaryWordList codeImage, int ICF, int DCF, assemblerContext *context) {
    FILE *objectFile;
    char *objectFileName;
    char buffer[OBJECT_FILE_LINE_SIZE] = {'\0'};
    binaryWordNode *currentWordP = codeImage;
    int i = 0;

    /* Create output filename */
    objectFileName = malloc(strlen(fileName) + 5);
    if (objectFileName == NULL) {
        fprintf(stdout, "Error: failed to allocate memory\n");
        return MEMORY_ALLOCATION_ERROR;
    }

    strcpy(objectFileName, fileName);
    strcat(objectFileName, ".obj");

    /* Write object file header and data */
    objectFile = fopen(objectFileName, "w");
    if (objectFile == NULL) {
        fprintf(stdout, "Error: cannot open file '%s'\n", objectFileName);
        free(objectFileName);
        return FALSE;
    }

    /* Write header with ICF and DCF */
    sprintf(buffer, "%s\t%s\n", intToBase4(ICF), intToBase4(DCF));
    fputs(buffer, objectFile);

    /* Write each binary word */
    while (i < ICF + DCF) {
        sprintf(buffer, "%s\t%s\n", intToBase4(INITIAL_ADDRESS + i), intToBase4(BinarywordToInt((*currentWordP))));

        fputs(buffer, objectFile);
        currentWordP++;
        i++;
    }

    fclose(objectFile);
    free(objectFileName);
    return TRUE;
}

/* Convert binary word structure to integer */
int BinarywordToInt(binaryWordNode word) {
    unsigned int merge;

    /* Merge bit fields based on word type */
    switch (word.kind) {
    case DIR:
        merge = word.binaryWord.dir.data_bits;
        break;

    case OPFIRST:
        merge = (word.binaryWord.opFirst.opcode_bits << 6) | (word.binaryWord.opFirst.src_op_bits << 4) |
                (word.binaryWord.opFirst.dest_op_bits << 2) | (word.binaryWord.opFirst.aer_bits << 0);
        break;

    case PAYLOAD:
        merge = (word.binaryWord.payload.payload_bits << 2) | (word.binaryWord.payload.aer_bits << 0);
        break;

    case REGPAIR:
        merge = (word.binaryWord.regPair.more_padding << 9) | (word.binaryWord.regPair.reg1_bits << 6) |
                (word.binaryWord.regPair.padding << 5) | (word.binaryWord.regPair.reg2_bits << 4) |
                (word.binaryWord.regPair.aer_bits << 0);
        break;

    default:
        break;
    }
    return merge;
}