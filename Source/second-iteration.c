#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "../Headers/second-iteration.h"

int secondIteration(char *fileName, int ICF, int DCF, binaryWordList *codeImage, assemblerContext *context,
                    symbolTable *entriesTable) {
    int state;
    char line[LINE_SIZE] = {'\0'};
    char *lineCopy;
    char *arg;
    char *isDir;
    char *srcFileName;
    FILE *srcFile;
    const directive *tempDirective;
    int lineNum = 0;
    int errorFlag = FALSE;
    char *entriesContent = "";
    size_t entriesContentCapacity = 0;
    char *externalsContent;
    size_t externalsContentCapacity = 0;
    binaryWordNode *currentOpWordP = *codeImage;
    int res;

    srcFileName = malloc(strlen(fileName) + 4);

    if (srcFileName == NULL) {
        return FALSE;
    }

    strcpy(srcFileName, fileName);
    strcat(srcFileName, ".am");

    /* Process each line of source file */
    srcFile = fopen(srcFileName, "r");
    if (srcFile == NULL) {
        free(srcFileName);
        return FALSE;
    }

    while (fgets(line, LINE_SIZE, srcFile) != NULL) {
        lineNum++;
        printf("LINE #%d\n", lineNum);
        lineCopy = NULL;

        state = UNKNOWN_LINE_TYPE;

        arg = strtok(line, ":\n");

        if (arg == NULL) {
            state = EMPTY_LINE;
        } else if (arg[0] == ';') {
            state = COMMENT_LINE;
        }

        /* Check if line starts with symbol definition */
        if (state == UNKNOWN_LINE_TYPE && searchSymbol(*(context->symbolTable), arg) != NULL) {
            arg = strtok(NULL, "\n");
            if (arg != NULL) {
                while (isspace((unsigned char)*arg))
                    arg++;
                lineCopy = (char *)malloc(strlen(arg) + 1);
                if (lineCopy == NULL) {
                    setFatalError(lineNum, ERR_MEM_ALLOC);
                    fclose(srcFile);
                    free(srcFileName);
                    return FALSE;
                }
                strcpy(lineCopy, arg);
            } else {
                lineCopy = (char *)malloc(1);
                if (lineCopy == NULL) {
                    setFatalError(lineNum, ERR_MEM_ALLOC);
                    fclose(srcFile);
                    free(srcFileName);
                    return FALSE;
                }
                lineCopy[0] = '\0';
            }
        } else if (state == UNKNOWN_LINE_TYPE) {
            lineCopy = (char *)malloc(strlen(line) + 1);
            if (lineCopy == NULL) {
                setFatalError(lineNum, ERR_MEM_ALLOC);
                fclose(srcFile);
                free(srcFileName);
                return FALSE;
            }
            strcpy(lineCopy, line);
        }

        if (state == UNKNOWN_LINE_TYPE) {
            isDir = strtok(lineCopy, " \t\n");

            if ((tempDirective = searchDirective(*(context->directiveTable), isDir)) != NULL) {
                if ((*tempDirective).type != ENTRY) {
                    state = NON_ENTRY_DIRECTIVE_LINE;
                } else {
                    state = ENTRY_LINE;
                }
            } else {
                state = OPERATION_LINE;
            }
        }

        /* Process line based on type */
        switch (state) {
        case EMPTY_LINE:
            if (lineCopy != NULL)
                free(lineCopy);
            continue;

        case COMMENT_LINE:
            if (lineCopy != NULL)
                free(lineCopy);
            continue;

        case NON_ENTRY_DIRECTIVE_LINE:
            if (lineCopy != NULL)
                free(lineCopy);
            continue;

        case ENTRY_LINE:
            res = handleEntryLine(&arg, lineNum, context, entriesTable, &entriesContent, &entriesContentCapacity);
            if (res != TRUE) {
                if (res == MEMORY_ALLOCATION_ERROR) {
                    if (lineCopy != NULL)
                        free(lineCopy);
                    fclose(srcFile);
                    free(srcFileName);
                    return FALSE;
                } else {
                    errorFlag = TRUE;
                }
            }
            break;

        case OPERATION_LINE:
            if (currentOpWordP != NULL) {
            } else {
            }

            res = handleOperationLine(&currentOpWordP, arg, lineNum, context, entriesTable, &externalsContent,
                                      &externalsContentCapacity);
            if (res != TRUE) {
                if (res == MEMORY_ALLOCATION_ERROR) {
                    if (lineCopy != NULL)
                        free(lineCopy);
                    fclose(srcFile);
                    free(srcFileName);
                    return FALSE;
                } else {
                    errorFlag = TRUE;
                }
            } else {
            }
            break;

        default:
            errorFlag = TRUE;
            break;
        }

        if (lineCopy != NULL) {
            free(lineCopy);
            lineCopy = NULL;
        }
    }

    /* Generate output files if no errors occurred */
    if (errorFlag == FALSE) {

        res = createObjectOutputFile(fileName, *codeImage, ICF, DCF, context);
        if (res != TRUE) {
            if (res == MEMORY_ALLOCATION_ERROR) {
                fclose(srcFile);
                free(srcFileName);
                return FALSE;
            } else {
                errorFlag = TRUE;
            }
        } else {
        }
    }

    if (errorFlag == FALSE && entriesContentCapacity > 0) {
        res = createEntriesOutputFile(fileName, entriesContent, context);
        if (res != TRUE) {
            if (res == MEMORY_ALLOCATION_ERROR) {
                fclose(srcFile);
                free(srcFileName);
                return FALSE;
            } else {
                errorFlag = TRUE;
            }
        } else {
        }
    } else if (entriesContentCapacity == 0) {
    }

    if (errorFlag == FALSE && externalsContentCapacity > 0) {
        res = createExternalsOutputFile(fileName, externalsContent, context);
        if (res != TRUE) {
            if (res == MEMORY_ALLOCATION_ERROR) {
                fclose(srcFile);
                free(srcFileName);
                return FALSE;
            } else {
                errorFlag = TRUE;
            }
        } else {
        }
    } else if (externalsContentCapacity == 0) {
    }

    fclose(srcFile);
    free(srcFileName);
    if (entriesContent != NULL) {
        free(entriesContent);
    }
    return errorFlag ? FALSE : TRUE;
}

/* Handle .entry directive processing */
int handleEntryLine(char **argP, int lineNum, assemblerContext *context, symbolTable *entriesTable,
                    char **entriesContentP, size_t *entriesContentCapacityP) {
    symbol *tempSymbol;
    printf("PRINT ALL VARS: %s, %s, %d", *argP, *entriesContentP, (int)*entriesContentCapacityP);

    *argP = strtok(NULL, " \t");
    if (*argP == NULL) {
        return FALSE;
    }

    if ((tempSymbol = searchSymbol(*(context->symbolTable), *argP)) != NULL) {

        if ((*tempSymbol).type == TYPE_EXTERNAL) {
            return FALSE;
        } else {
            printf("PRINT BEFORE INSERT SYMBOL: %s, %d\n", *argP, tempSymbol->address);
            insertSymbol(entriesTable, *argP, tempSymbol->address, TYPE_ENTRY, lineNum);
            writeToEntries(tempSymbol, entriesContentP, entriesContentCapacityP, lineNum, context);
        }
    } else {
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
    char *base4Address = intToBase4(address, 'a');

    newCapacity = *entriesContentCapacityP + strlen(label) + strlen(base4Address) + 2;
    printf("BADIDAAAAAAAAAAAAA: %d", (int)newCapacity);

    newContent = calloc(newCapacity, sizeof(char));

    if (newContent == NULL) {
        return MEMORY_ALLOCATION_ERROR;
    }

    if (*entriesContentP != NULL) {
        strcpy(newContent, *entriesContentP);
        free(*entriesContentP);
    }

    *entriesContentP = newContent;
    *entriesContentCapacityP = newCapacity;

    strcat(*entriesContentP, label);
    strcat(*entriesContentP, "\t");
    strcat(*entriesContentP, base4Address);
    strcat(*entriesContentP, "\n");

    free(base4Address);
    return TRUE;
}

/* Convert integer to base-4 representation */
char *intToBase4(int integer, char type) {
    char *base4;
    size_t length;
    int i = 0;

    if (type == 'a') {
        length = strlen(BASE4_ADDRESS_INIT);
        base4 = calloc(length + 1, sizeof(char));

        if (base4 == NULL)
            return NULL;

        strncpy(base4, BASE4_ADDRESS_INIT, length);
        base4[length] = '\0';
    } else if (type == 'c') {
        length = strlen(BASE4_CODE_INIT);
        base4 = calloc(length + 1, sizeof(char));

        if (base4 == NULL)
            return NULL;

        strncpy(base4, BASE4_CODE_INIT, length);
        base4[length] = '\0';
    } else {
        return NULL;
    }

    i = length - 1;
    while (integer && i >= 0) {
        base4[i] += integer % 4;
        integer /= 4;
        i--;
    }

    return base4;
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
        res = handleTwoOperandOpEncoding(opWordP, lineCopy, srcOperandAddressEncoding, destOperandAddressEncoding,
                                         lineNum, context, entriesTable, externalsContentP, externalsContentCapacityP);
        break;

    case NOT:
        break;

    case CLR:
    case INC:
    case DEC:
    case JMP:
    case BNE:
    case RED:
    case PRN:
    case JSR:
        res = handleOneOperandOpEncoding(opWordP, lineCopy, destOperandAddressEncoding, lineNum, context, entriesTable,
                                         externalsContentP, externalsContentCapacityP);
        break;

    case RTS:
    case STP:
        res = TRUE;
        break;

    default:
        /*unknown encoding*/
        res = FALSE;
        break;
    }

    if (res != TRUE) {
        if (res == MEMORY_ALLOCATION_ERROR)
            return MEMORY_ALLOCATION_ERROR;
        else
            return FALSE;
    }

    return TRUE;
}

/* Handle two-operand instruction encoding */
int handleTwoOperandOpEncoding(binaryWordNode **opWordP, char *lineCopy, unsigned int srcOperandAddressEncoding,
                               unsigned int destOperandAddressEncoding, int lineNum, assemblerContext *context,
                               symbolTable *entriesTable, char **externalsContentP, size_t *externalsContentCapacityP) {
    char *label1;
    char *label2;
    char *commaPos;
    int res;

    commaPos = strchr(lineCopy, ',');
    if (commaPos == NULL) {

        return FALSE;
    }

    *commaPos = '\0';

    label1 = strtok(lineCopy, " \t");
    if (label1 != NULL)
        label1 = strtok(NULL, " \t,[");

    label2 = commaPos + 1;

    while (isspace(*label2))
        label2++;

    strtok(label2, " \t[");

    /* Advance to next word after operation word */
    if ((*opWordP)->next != NULL) {
        *opWordP = (*opWordP)->next;
    }

    /* Encode source operand if needed */
    if (srcOperandAddressEncoding == DIRECT_OPCODE_ENCODING || srcOperandAddressEncoding == MATRIX_OPCODE_ENCODING) {

        res = encodeOpPayloadWord(label1, lineNum, context, entriesTable, *opWordP, externalsContentP,
                                  externalsContentCapacityP, srcOperandAddressEncoding);
        if (res != TRUE) {
            if (res == MEMORY_ALLOCATION_ERROR)
                return MEMORY_ALLOCATION_ERROR;
            return FALSE;
        }
    }

    /* Advance pointer based on source operand type */
    if (srcOperandAddressEncoding == MATRIX_OPCODE_ENCODING) {
        if ((*opWordP)->next && (*opWordP)->next->next) {
            *opWordP = (*opWordP)->next->next;
        }
    } else {
        if ((*opWordP)->next) {
            *opWordP = (*opWordP)->next;
        }
    }

    /* Encode destination operand if needed */
    if (destOperandAddressEncoding == DIRECT_OPCODE_ENCODING || destOperandAddressEncoding == MATRIX_OPCODE_ENCODING) {
        res = encodeOpPayloadWord(label2, lineNum, context, entriesTable, *opWordP, externalsContentP,
                                  externalsContentCapacityP, destOperandAddressEncoding);
        if (res != TRUE) {
            if (res == MEMORY_ALLOCATION_ERROR)
                return MEMORY_ALLOCATION_ERROR;
            return FALSE;
        }
    }

    /* Advance pointer based on destination operand type */
    if (destOperandAddressEncoding == MATRIX_OPCODE_ENCODING) {
        if ((*opWordP)->next && (*opWordP)->next->next) {
            *opWordP = (*opWordP)->next->next;
        }
    } else if (srcOperandAddressEncoding == REGISTER_OPCODE_ENCODING &&
               destOperandAddressEncoding == REGISTER_OPCODE_ENCODING) {
    } else {
        if ((*opWordP)->next) {
            *opWordP = (*opWordP)->next;
        }
    }

    return TRUE;
}

/* Encode payload word based on symbol type */
int encodeOpPayloadWord(char *label, int lineNum, assemblerContext *context, symbolTable *entriesTable,
                        binaryWordNode *currentWordP, char **externalsContentP, size_t *externalsContentCapacityP,
                        unsigned int operandAddressEncoding) {
    symbol *tempSymbol;

    if ((tempSymbol = searchSymbol(*(context->symbolTable), label)) != NULL) {
        switch ((*tempSymbol).type) {
        case TYPE_EXTERNAL:
            return handleExternal(label, lineNum, entriesTable, currentWordP, externalsContentP,
                                  externalsContentCapacityP, context);

        case TYPE_DATA:
            return handleData(label, *tempSymbol, lineNum, currentWordP, context);

        case TYPE_CODE:
            if (operandAddressEncoding == DIRECT_OPCODE_ENCODING) {
                insertError((*context).errorList, ERR_INVALID_SYMBOL_TYPE, lineNum);
            } else
                return handleData(label, *tempSymbol, lineNum, currentWordP, context);

        default:
            return FALSE;
        }
    } else {
        insertError((*context).errorList, ERR_UNDEFINED_SYMBOL, lineNum);
        return FALSE;
    }
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
    char *base4Address = intToBase4(address, 'a');

    newCapacity = *externalsContentCapacityP + strlen(label) + strlen(base4Address) + 2;
    newContent = (char *)malloc(newCapacity);

    if (newContent == NULL) {
        return MEMORY_ALLOCATION_ERROR;
    }

    if (*externalsContentP != NULL) {
        strcpy(newContent, *externalsContentP);
        free(*externalsContentP);
    } else {
        newContent[0] = '\0';
    }

    *externalsContentP = newContent;
    *externalsContentCapacityP = newCapacity;

    strcat(*externalsContentP, label);
    strcat(*externalsContentP, "\t");
    strcat(*externalsContentP, base4Address);
    strcat(*externalsContentP, "\n");

    free(base4Address);
    return TRUE;
}

/* Handle data symbol encoding */
int handleData(char *label, symbol tempSymbol, int lineNum, binaryWordNode *currentWordP, assemblerContext *context) {

    currentWordP->binaryWord.payload.payload_bits = tempSymbol.address;
    currentWordP->binaryWord.payload.aer_bits = AER_RELOCATABLE_ENCODING;

    return TRUE;
}

/* Handle single-operand instruction encoding */
int handleOneOperandOpEncoding(binaryWordNode **opWordP, char *lineCopy, unsigned int destOperandAddressEncoding,
                               int lineNum, assemblerContext *context, symbolTable *entriesTable,
                               char **externalsContentP, size_t *externalsContentCapacityP) {
    int res;
    char *label;

    /* Advance to next word after operation word */
    if ((*opWordP)->next != NULL) {
        *opWordP = (*opWordP)->next;
    }

    strtok(lineCopy, " \t");
    label = strtok(NULL, "[\n");

    if (destOperandAddressEncoding == DIRECT_OPCODE_ENCODING || destOperandAddressEncoding == MATRIX_OPCODE_ENCODING) {
        res = encodeOpPayloadWord(label, lineNum, context, entriesTable, *opWordP, externalsContentP,
                                  externalsContentCapacityP, destOperandAddressEncoding);
        if (res != TRUE) {
            if (res == MEMORY_ALLOCATION_ERROR)
                return MEMORY_ALLOCATION_ERROR;
            return FALSE;
        }
    }

    /* Advance pointer based on destination operand type */
    if (destOperandAddressEncoding == MATRIX_OPCODE_ENCODING) {

        if ((*opWordP)->next && (*opWordP)->next->next) {
            *opWordP = (*opWordP)->next->next;
        }
    } else {
        if ((*opWordP)->next) {
            *opWordP = (*opWordP)->next;
        }
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
        return MEMORY_ALLOCATION_ERROR;
    }

    strcpy(entriesFileName, fileName);
    strcat(entriesFileName, ".ent");

    /* Write entries to file */
    entriesFile = fopen(entriesFileName, "w");
    if (entriesFile == NULL) {
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
        return MEMORY_ALLOCATION_ERROR;
    }

    strcpy(externalsFileName, fileName);
    strcat(externalsFileName, ".ext");

    /* Write externals to file */
    externalsFile = fopen(externalsFileName, "w");
    if (externalsFile == NULL) {
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
    char *icfStr = intToBase4(ICF, 'a'), *dcfStr = intToBase4(DCF, 'a'), *addressStr, *wordStr;
    binaryWordNode *currentWordP = codeImage;
    int i = 0;
    int wordValue;

    /* Create output filename */
    objectFileName = malloc(strlen(fileName) + 5);
    if (objectFileName == NULL) {
        return MEMORY_ALLOCATION_ERROR;
    }

    strcpy(objectFileName, fileName);
    strcat(objectFileName, ".obj");

    /* Write object file header and data */
    objectFile = fopen(objectFileName, "w");
    if (objectFile == NULL) {
        free(objectFileName);
        return FALSE;
    }

    /* Write header with ICF and DCF */
    if (icfStr == NULL || dcfStr == NULL) {
        fclose(objectFile);
        free(objectFileName);
        return MEMORY_ALLOCATION_ERROR;
    }

    sprintf(buffer, "%s\t%s\n", icfStr, dcfStr);
    fputs(buffer, objectFile);

    free(icfStr);
    free(dcfStr);

    /* Write each binary word */
    while (i < ICF + DCF && currentWordP != NULL) {
        wordValue = BinarywordToInt(*currentWordP);

        addressStr = intToBase4(INITIAL_ADDRESS + i, 'a');
        wordStr = intToBase4(wordValue, 'c');
        if (addressStr == NULL || wordStr == NULL) {
            fclose(objectFile);
            free(objectFileName);
            return MEMORY_ALLOCATION_ERROR;
        }

        sprintf(buffer, "%s\t%s\n", addressStr, wordStr);
        fputs(buffer, objectFile);

        free(addressStr);
        free(wordStr);

        currentWordP = currentWordP->next;
        i++;
    }

    fclose(objectFile);
    free(objectFileName);
    return TRUE;
}

/* Convert binary word structure to integer */
unsigned int BinarywordToInt(binaryWordNode word) {
    unsigned int merge;

    /* Merge bit fields based on word type */
    switch (word.kind) {
    case DIR:
        merge = word.binaryWord.dir.data_bits;
        break;

    case OP_FIRST:
        merge = (word.binaryWord.opFirst.opcode_bits << 6) | (word.binaryWord.opFirst.src_op_bits << 4) |
                (word.binaryWord.opFirst.dest_op_bits << 2) | (word.binaryWord.opFirst.aer_bits << 0);
        break;

    case PAYLOAD:
        merge = (word.binaryWord.payload.payload_bits << 2) | (word.binaryWord.payload.aer_bits << 0);
        break;

    case REG_PAIR:
        merge = (word.binaryWord.regPair.more_padding << 9) | (word.binaryWord.regPair.reg1_bits << 6) |
                (word.binaryWord.regPair.padding << 5) | (word.binaryWord.regPair.reg2_bits << 2) |
                (word.binaryWord.regPair.aer_bits << 0);
        break;

    default:
        merge = 0;
        break;
    }

    return merge;
}