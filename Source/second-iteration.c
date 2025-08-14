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
    char *entriesContent;
    size_t entriesContentCapacity = 0;
    char *externalsContent;
    size_t externalsContentCapacity = 0;
    binaryWordNode *currentOpWordP = *codeImage;
    int res;

    printf("=== SECOND ITERATION PHASE BEGIN ===\n");
    printf("Starting second iteration for file: %s\n", fileName);
    printf("Initial Code Counter (ICF): %d\n", ICF);
    printf("Initial Data Counter (DCF): %d\n", DCF);
    printf("Code image contains binary words to be processed\n");

    srcFileName = malloc(strlen(fileName) + 4);

    if (srcFileName == NULL) {
        fprintf(stderr, "Error: failed to allocate memory for source filename\n");
        return FALSE;
    }

    strcpy(srcFileName, fileName);
    strcat(srcFileName, ".am");

    printf("Processing source file: %s\n", srcFileName);

    /* Process each line of source file */
    srcFile = fopen(srcFileName, "r");
    if (srcFile == NULL) {
        fprintf(stderr, "Error: Cannot open file '%s'\n", srcFileName);
        free(srcFileName);
        return FALSE;
    }

    printf("File opened successfully, beginning line-by-line processing...\n\n");

    while (fgets(line, LINE_SIZE, srcFile) != NULL) {
        lineNum++;
        printf("=== Processing Line %d ===\n", lineNum);
        printf("Raw line content: '%s'", line); /* line already contains \n */
        state = UNKNOWN_LINE_TYPE;

        arg = strtok(line, ":\n");
        if (arg != NULL) {
            printf("First token extracted: '%s'\n", arg);
        } else {
            printf("No tokens found in line\n");
        }

        if (arg == NULL) {
            state = EMPTY_LINE;
            printf("Line classification: EMPTY_LINE\n");
        } else if (arg[0] == ';') {
            state = COMMENT_LINE;
            printf("Line classification: COMMENT_LINE\n");
        }

        /* Check if line starts with symbol definition */
        if (state == UNKNOWN_LINE_TYPE && searchSymbol(*(context->symbolTable), arg) != NULL) {
            printf("First token is a known symbol, extracting remainder of line\n");
            arg = strtok(NULL, "\n");
            if (arg != NULL) {
                while (isspace((unsigned char)*arg))
                    arg++;
                printf("Remainder after symbol: '%s'\n", arg);
                lineCopy = (char *)malloc(strlen(arg) + 1);
                if (lineCopy == NULL) {
                    printf("Memory allocation failed for line copy\n");
                    setFatalError(lineNum, ERR_MEM_ALLOC);
                    fclose(srcFile);
                    free(srcFileName);
                    return FALSE;
                }
                strcpy(lineCopy, arg);
            } else {
                printf("No content after symbol definition\n");
                lineCopy = (char *)malloc(1);
                if (lineCopy == NULL) {
                    printf("Memory allocation failed for empty line copy\n");
                    setFatalError(lineNum, ERR_MEM_ALLOC);
                    fclose(srcFile);
                    free(srcFileName);
                    return FALSE;
                }
                lineCopy[0] = '\0';
            }
        } else if (state == UNKNOWN_LINE_TYPE) {
            printf("Line does not start with symbol, using entire line\n");
            lineCopy = (char *)malloc(strlen(line) + 1);
            if (lineCopy == NULL) {
                printf("Memory allocation failed for line copy\n");
                setFatalError(lineNum, ERR_MEM_ALLOC);
                fclose(srcFile);
                free(srcFileName);
                return FALSE;
            }
            strcpy(lineCopy, line);
        }

        if (state == UNKNOWN_LINE_TYPE) {
            isDir = strtok(lineCopy, " \t\n");
            printf("Checking if first word is directive: '%s'\n", isDir);

            if ((tempDirective = searchDirective(*(context->directiveTable), isDir)) != NULL) {
                printf("Found directive of type: %d\n", (*tempDirective).type);
                if ((*tempDirective).type != ENTRY) {
                    state = NON_ENTRY_DIRECTIVE_LINE;
                    printf("Line classification: NON_ENTRY_DIRECTIVE_LINE\n");
                } else {
                    state = ENTRY_LINE;
                    printf("Line classification: ENTRY_LINE\n");
                }
            } else {
                state = OPERATION_LINE;
                printf("Line classification: OPERATION_LINE\n");
            }
        }

        /* Process line based on type */
        switch (state) {
        case EMPTY_LINE:
            printf("Skipping empty line\n");
            if (lineCopy != NULL)
                free(lineCopy);
            continue;

        case COMMENT_LINE:
            printf("Skipping comment line\n");
            if (lineCopy != NULL)
                free(lineCopy);
            continue;

        case NON_ENTRY_DIRECTIVE_LINE:
            printf("Skipping non-entry directive line (already processed in first iteration)\n");
            if (lineCopy != NULL)
                free(lineCopy);
            continue;

        case ENTRY_LINE:
            printf("Processing entry directive...\n");
            res = handleEntryLine(&arg, lineNum, context, entriesTable, &entriesContent, &entriesContentCapacity);
            if (res != TRUE) {
                if (res == MEMORY_ALLOCATION_ERROR) {
                    printf("Memory allocation error in entry handling\n");
                    if (lineCopy != NULL)
                        free(lineCopy);
                    fclose(srcFile);
                    free(srcFileName);
                    return FALSE;
                } else {
                    printf("Error in entry handling, setting error flag\n");
                    errorFlag = TRUE;
                }
            } else {
                printf("Entry line processed successfully\n");
            }
            break;

        case OPERATION_LINE:
            printf("Processing operation line...\n");
            if (currentOpWordP != NULL) {
                printf("Current binary word position - C: %d, L: %d\n", currentOpWordP->C, currentOpWordP->L);
                printf("Current word kind: %d\n", currentOpWordP->kind);
            } else {
                printf("Warning: Current operation word pointer is NULL\n");
            }

            res = handleOperationLine(&currentOpWordP, arg, lineNum, context, entriesTable, &externalsContent,
                                      &externalsContentCapacity);
            if (res != TRUE) {
                if (res == MEMORY_ALLOCATION_ERROR) {
                    printf("Memory allocation error in operation handling\n");
                    if (lineCopy != NULL)
                        free(lineCopy);
                    fclose(srcFile);
                    free(srcFileName);
                    return FALSE;
                } else {
                    printf("Error in operation handling, setting error flag\n");
                    errorFlag = TRUE;
                }
            } else {
                printf("Operation line processed successfully\n");
            }
            break;

        default:
            printf("Unknown line type encountered\n");
            fprintf(stderr, "Error at line %d: unknown line type\n", lineNum);
            errorFlag = TRUE;
            break;
        }

        if (lineCopy != NULL) {
            free(lineCopy);
            lineCopy = NULL;
        }
        printf("Line %d processing complete\n\n", lineNum);
    }

    printf("=== FILE PROCESSING COMPLETE ===\n");
    printf("Total lines processed: %d\n", lineNum);
    printf("Error flag status: %s\n", errorFlag ? "TRUE" : "FALSE");

    /* Generate output files if no errors occurred */
    if (errorFlag == FALSE) {
        printf("No errors detected, generating output files...\n");

        printf("Creating object output file...\n");
        res = createObjectOutputFile(fileName, *codeImage, ICF, DCF, context);
        if (res != TRUE) {
            if (res == MEMORY_ALLOCATION_ERROR) {
                printf("Memory allocation error in object file creation\n");
                fclose(srcFile);
                free(srcFileName);
                return FALSE;
            } else {
                printf("Error in object file creation\n");
                errorFlag = TRUE;
            }
        } else {
            printf("Object file created successfully\n");
        }
    }

    if (errorFlag == FALSE && entriesContentCapacity > 0) {
        printf("Creating entries output file (capacity: %u)...\n", (unsigned int)entriesContentCapacity);
        res = createEntriesOutputFile(fileName, entriesContent, context);
        if (res != TRUE) {
            if (res == MEMORY_ALLOCATION_ERROR) {
                printf("Memory allocation error in entries file creation\n");
                fclose(srcFile);
                free(srcFileName);
                return FALSE;
            } else {
                printf("Error in entries file creation\n");
                errorFlag = TRUE;
            }
        } else {
            printf("Entries file created successfully\n");
        }
    } else if (entriesContentCapacity == 0) {
        printf("No entries to write, skipping entries file creation\n");
    }

    if (errorFlag == FALSE && externalsContentCapacity > 0) {
        printf("Creating externals output file (capacity: %u)...\n", (unsigned int)externalsContentCapacity);
        res = createExternalsOutputFile(fileName, externalsContent, context);
        if (res != TRUE) {
            if (res == MEMORY_ALLOCATION_ERROR) {
                printf("Memory allocation error in externals file creation\n");
                fclose(srcFile);
                free(srcFileName);
                return FALSE;
            } else {
                printf("Error in externals file creation\n");
                errorFlag = TRUE;
            }
        } else {
            printf("Externals file created successfully\n");
        }
    } else if (externalsContentCapacity == 0) {
        printf("No externals to write, skipping externals file creation\n");
    }

    fclose(srcFile);
    free(srcFileName);

    printf("=== SECOND ITERATION PHASE END ===\n");
    printf("Final result: %s\n", errorFlag ? "FAILURE" : "SUCCESS");

    return errorFlag ? FALSE : TRUE;
}

/* Handle .entry directive processing */
int handleEntryLine(char **argP, int lineNum, assemblerContext *context, symbolTable *entriesTable,
                    char **entriesContentP, size_t *entriesContentCapacityP) {
    symbol *tempSymbol;

    printf("--- Entry Line Processing ---\n");

    *argP = strtok(NULL, " \t");
    if (*argP != NULL) {
        printf("Entry operand found: '%s'\n", *argP);
    } else {
        printf("No operand found for entry directive\n");
        fprintf(stderr, "Error at line %d: .entry directive missing operand\n", lineNum);
        return FALSE;
    }

    printf("Searching for symbol '%s' in symbol table...\n", *argP);
    if ((tempSymbol = searchSymbol(*(context->symbolTable), *argP)) != NULL) {
        printf("Symbol found - Label: '%s', Address: %d, Type: %d\n", (*tempSymbol).label, (*tempSymbol).address,
               (*tempSymbol).type);

        if ((*tempSymbol).type == TYPE_EXTERNAL) {
            printf("Error: Entry operand is defined as external\n");
            fprintf(stderr, "Error at line %d: .entry operand label is defined as external\n", lineNum);
            return FALSE;
        } else {
            printf("Valid entry symbol, writing to entries content...\n");
            writeToEntries(tempSymbol, entriesContentP, entriesContentCapacityP, lineNum, context);
            printf("Entry successfully added to entries table\n");
        }
    } else {
        printf("Symbol not found in symbol table\n");
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
    char *base4Address = intToBase4(address, 'a');

    printf("--- Writing Entry to Buffer ---\n");
    printf("Entry details - Label: '%s', Address: %d\n", label, address);
    printf("Converting address %d to base-4...\n", address);

    newCapacity = *entriesContentCapacityP + strlen(label) + strlen(base4Address) + 2;
    printf("New buffer capacity required: %u bytes\n", (unsigned int)newCapacity);

    newContent = (char *)malloc(newCapacity);

    if (newContent == NULL) {
        printf("Memory allocation failed for entries content\n");
        fprintf(stderr, "Error at line %d: failed to allocate memory\n", lineNum);
        return MEMORY_ALLOCATION_ERROR;
    }

    if (*entriesContentP != NULL) {
        strcpy(newContent, *entriesContentP);
        free(*entriesContentP);
    } else {
        newContent[0] = '\0';
    }

    *entriesContentP = newContent;
    *entriesContentCapacityP = newCapacity;

    strcat(*entriesContentP, label);
    strcat(*entriesContentP, "\t");
    strcat(*entriesContentP, base4Address);
    strcat(*entriesContentP, "\n");

    printf("Entries content updated: '%s'\n", *entriesContentP);
    return TRUE;
}

/* Convert integer to base-4 representation */
char *intToBase4(int integer, char type) {
    char *base4;
    size_t length;
    int i = 0;
    printf("word value before converting to base 4: %d", integer);

    if (type == 'a') {
        length = strlen(BASE4_ADDRESS_INIT);
        base4 = calloc(length + 1, sizeof(char));

        if (base4 == NULL)
            return "FAILED";

        strncpy(base4, BASE4_ADDRESS_INIT, length);
        base4[length] = '\0';
    } else if (type == 'c') {
        length = strlen(BASE4_CODE_INIT);
        base4 = calloc(length + 1, sizeof(char));

        if (base4 == NULL)
            return "FAILED";

        strncpy(base4, BASE4_CODE_INIT, length);
        base4[length] = '\0';
    } else {
        printf("Error: invalid type '%c'\n", type);
        return NULL;
    }

    i = length - 1;
    while (integer && i >= 0) {
        base4[i] += integer % 4;
        integer /= 4;
        i--;
    }

    printf("word value after converting to base 4: %s", base4);
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

    printf("--- Operation Line Processing ---\n");
    printf("Current binary word details:\n");
    printf("  Word kind: %d\n", (*opWordP)->kind);
    printf("  Counter (C): %d, Line index (L): %d\n", (*opWordP)->C, (*opWordP)->L);
    printf("  Opcode: %u\n", opCodeEncoding);
    printf("  Source addressing mode: %u\n", srcOperandAddressEncoding);
    printf("  Destination addressing mode: %u\n", destOperandAddressEncoding);
    printf("  AER bits: %u\n", tempFirstOpWord.opFirst.aer_bits);

    /* Route to appropriate handler based on operation type */
    switch (opCodeEncoding) {
    case MOV:
        printf("Processing MOV operation (two operands)\n");
        res = handleTwoOperandOpEncoding(opWordP, lineCopy, srcOperandAddressEncoding, destOperandAddressEncoding,
                                         lineNum, context, entriesTable, externalsContentP, externalsContentCapacityP);
        break;

    case CMP:
        printf("Processing CMP operation (two operands)\n");
        res = handleTwoOperandOpEncoding(opWordP, lineCopy, srcOperandAddressEncoding, destOperandAddressEncoding,
                                         lineNum, context, entriesTable, externalsContentP, externalsContentCapacityP);
        break;

    case ADD:
        printf("Processing ADD operation (two operands)\n");
        res = handleTwoOperandOpEncoding(opWordP, lineCopy, srcOperandAddressEncoding, destOperandAddressEncoding,
                                         lineNum, context, entriesTable, externalsContentP, externalsContentCapacityP);
        break;

    case SUB:
        printf("Processing SUB operation (two operands)\n");
        res = handleTwoOperandOpEncoding(opWordP, lineCopy, srcOperandAddressEncoding, destOperandAddressEncoding,
                                         lineNum, context, entriesTable, externalsContentP, externalsContentCapacityP);
        break;

    case LEA:
        printf("Processing LEA operation (two operands)\n");
        res = handleTwoOperandOpEncoding(opWordP, lineCopy, srcOperandAddressEncoding, destOperandAddressEncoding,
                                         lineNum, context, entriesTable, externalsContentP, externalsContentCapacityP);
        break;

    case NOT:
        printf("Processing NOT operation (one operand)\n");
        res = handleOneOperandOpEncoding(opWordP, lineCopy, destOperandAddressEncoding, lineNum, context, entriesTable,
                                         externalsContentP, externalsContentCapacityP);
        break;

    case CLR:
        printf("Processing CLR operation (one operand)\n");
        res = handleOneOperandOpEncoding(opWordP, lineCopy, destOperandAddressEncoding, lineNum, context, entriesTable,
                                         externalsContentP, externalsContentCapacityP);
        break;

    case INC:
        printf("Processing INC operation (one operand)\n");
        res = handleOneOperandOpEncoding(opWordP, lineCopy, destOperandAddressEncoding, lineNum, context, entriesTable,
                                         externalsContentP, externalsContentCapacityP);
        break;

    case DEC:
        printf("Processing DEC operation (one operand)\n");
        res = handleOneOperandOpEncoding(opWordP, lineCopy, destOperandAddressEncoding, lineNum, context, entriesTable,
                                         externalsContentP, externalsContentCapacityP);
        break;

    case JMP:
        printf("Processing JMP operation (one operand)\n");
        res = handleOneOperandOpEncoding(opWordP, lineCopy, destOperandAddressEncoding, lineNum, context, entriesTable,
                                         externalsContentP, externalsContentCapacityP);
        break;

    case BNE:
        printf("Processing BNE operation (one operand)\n");
        res = handleOneOperandOpEncoding(opWordP, lineCopy, destOperandAddressEncoding, lineNum, context, entriesTable,
                                         externalsContentP, externalsContentCapacityP);
        break;

    case RED:
        printf("Processing RED operation (one operand)\n");
        res = handleOneOperandOpEncoding(opWordP, lineCopy, destOperandAddressEncoding, lineNum, context, entriesTable,
                                         externalsContentP, externalsContentCapacityP);
        break;

    case PRN:
        printf("Processing PRN operation (one operand)\n");
        res = handleOneOperandOpEncoding(opWordP, lineCopy, destOperandAddressEncoding, lineNum, context, entriesTable,
                                         externalsContentP, externalsContentCapacityP);
        break;

    case JSR:
        printf("Processing JSR operation (one operand)\n");
        res = handleOneOperandOpEncoding(opWordP, lineCopy, destOperandAddressEncoding, lineNum, context, entriesTable,
                                         externalsContentP, externalsContentCapacityP);
        break;

    case RTS:
        printf("Processing RTS operation (no operands)\n");
        res = TRUE;
        break;

    case STP:
        printf("Processing STP operation (no operands)\n");
        res = TRUE;
        break;

    default:
        printf("Unknown operation code: %u\n", opCodeEncoding);
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

    printf("--- Two-Operand Encoding ---\n");
    printf("Line content: '%s'\n", lineCopy);
    printf("Source addressing mode: %u, Destination addressing mode: %u\n", srcOperandAddressEncoding,
           destOperandAddressEncoding);

    commaPos = strchr(lineCopy, ',');
    if (commaPos == NULL) {
        printf("Error: No comma found in two-operand instruction\n");
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

    printf("Source operand: '%s'\n", label1 ? label1 : "NULL");
    printf("Destination operand: '%s'\n", label2);

    /* Advance to next word after operation word */
    if ((*opWordP)->next != NULL) {
        *opWordP = (*opWordP)->next;
        printf("Advanced to next word - C: %d, L: %d, Kind: %d\n", (*opWordP)->C, (*opWordP)->L, (*opWordP)->kind);
    }

    /* Encode source operand if needed */
    if (srcOperandAddressEncoding == DIRECT_OPCODE_ENCODING || srcOperandAddressEncoding == MATRIX_OPCODE_ENCODING) {
        printf("Encoding source operand with addressing mode %u\n", srcOperandAddressEncoding);
        res = encodeOpPayloadWord(label1, lineNum, context, entriesTable, *opWordP, externalsContentP,
                                  externalsContentCapacityP);
        if (res != TRUE) {
            if (res == MEMORY_ALLOCATION_ERROR)
                return MEMORY_ALLOCATION_ERROR;
            return FALSE;
        }

        /* Show encoded word details */
        printf("Source operand encoded - Payload: %u, AER: %u\n", (*opWordP)->binaryWord.payload.payload_bits,
               (*opWordP)->binaryWord.payload.aer_bits);
    }

    /* Advance pointer based on source operand type */
    if (srcOperandAddressEncoding == MATRIX_OPCODE_ENCODING) {
        printf("Matrix addressing for source - advancing 2 positions\n");
        if ((*opWordP)->next && (*opWordP)->next->next) {
            *opWordP = (*opWordP)->next->next;
            printf("Advanced to word - C: %d, L: %d, Kind: %d\n", (*opWordP)->C, (*opWordP)->L, (*opWordP)->kind);
        }
    } else {
        printf("Non-Matrix addressing for source - advancing 1 position\n");
        if ((*opWordP)->next) {
            *opWordP = (*opWordP)->next;
            printf("Advanced to word - C: %d, L: %d, Kind: %d\n", (*opWordP)->C, (*opWordP)->L, (*opWordP)->kind);
        }
    }

    /* Encode destination operand if needed */
    if (destOperandAddressEncoding == DIRECT_OPCODE_ENCODING || destOperandAddressEncoding == MATRIX_OPCODE_ENCODING) {
        printf("Encoding destination operand with addressing mode %u\n", destOperandAddressEncoding);
        res = encodeOpPayloadWord(label2, lineNum, context, entriesTable, *opWordP, externalsContentP,
                                  externalsContentCapacityP);
        if (res != TRUE) {
            if (res == MEMORY_ALLOCATION_ERROR)
                return MEMORY_ALLOCATION_ERROR;
            return FALSE;
        }

        /* Show encoded word details */
        printf("Destination operand encoded - Payload: %u, AER: %u\n", (*opWordP)->binaryWord.payload.payload_bits,
               (*opWordP)->binaryWord.payload.aer_bits);
    }

    /* Advance pointer based on destination operand type */
    if (destOperandAddressEncoding == MATRIX_OPCODE_ENCODING) {
        printf("Matrix addressing for destination - advancing 2 positions\n");
        if ((*opWordP)->next && (*opWordP)->next->next) {
            *opWordP = (*opWordP)->next->next;
        }
    } else if (srcOperandAddressEncoding == REGISTER_OPCODE_ENCODING &&
               destOperandAddressEncoding == REGISTER_OPCODE_ENCODING) {
        printf("RegPair addressing - advancing 00000000000 position\n");
    } else {
        printf("Non-Matrix addressing for destination - advancing 1 position\n");
        if ((*opWordP)->next) {
            *opWordP = (*opWordP)->next;
        }
    }

    printf("Two-operand encoding completed successfully\n");
    return TRUE;
}

/* Encode payload word based on symbol type */
int encodeOpPayloadWord(char *label, int lineNum, assemblerContext *context, symbolTable *entriesTable,
                        binaryWordNode *currentWordP, char **externalsContentP, size_t *externalsContentCapacityP) {
    symbol *tempSymbol;

    printf("--- Payload Word Encoding ---\n");
    printf("Encoding payload for label: '%s'\n", label);
    printf("Current word position - C: %d, L: %d, Kind: %d\n", currentWordP->C, currentWordP->L, currentWordP->kind);

    if ((tempSymbol = searchSymbol(*(context->symbolTable), label)) != NULL) {
        printf("Symbol found - Label: '%s', Address: %d, Type: %d\n", (*tempSymbol).label, (*tempSymbol).address,
               (*tempSymbol).type);

        switch ((*tempSymbol).type) {
        case TYPE_EXTERNAL:
            printf("Processing external symbol\n");
            return handleExternal(label, lineNum, entriesTable, currentWordP, externalsContentP,
                                  externalsContentCapacityP, context);

        case TYPE_DATA:
            printf("Processing data symbol\n");
            return handleData(label, *tempSymbol, lineNum, currentWordP, context);

        case TYPE_CODE:
            printf("Processing code symbol\n");
            return handleData(label, *tempSymbol, lineNum, currentWordP, context);

        default:
            printf("Unknown symbol type: %d\n", (*tempSymbol).type);
            fprintf(stderr, "Error at line %d: failed to encode unknown symbol type\n", lineNum);
            return FALSE;
        }
    } else {
        printf("Symbol not found in symbol table\n");
        fprintf(stderr, "Error at line %d: %s is not defined\n", lineNum, label);
        return FALSE;
    }
}

/* Handle external symbol reference */
int handleExternal(char *label, int lineNum, symbolTable *entriesTable, binaryWordNode *currentWordP,
                   char **externalsContentP, size_t *externalsContentCapacityP, assemblerContext *context) {
    int res;
    symbol *tempEntry;

    printf("--- External Symbol Handling ---\n");
    printf("Processing external reference to: '%s'\n", label);

    if ((tempEntry = searchSymbol(*entriesTable, label)) != NULL) {
        printf("External symbol found in entries table - Address: %d\n", (*tempEntry).address);

        currentWordP->binaryWord.payload.payload_bits = (*tempEntry).address;
        currentWordP->binaryWord.payload.aer_bits = AER_EXTERNAL_ENCODING;

        printf("Encoded external word - Payload: %u, AER: %u\n", currentWordP->binaryWord.payload.payload_bits,
               currentWordP->binaryWord.payload.aer_bits);

        res = writeToExternals(*currentWordP, label, lineNum, externalsContentP, externalsContentCapacityP, context);
        if (res != TRUE) {
            if (res == MEMORY_ALLOCATION_ERROR)
                return MEMORY_ALLOCATION_ERROR;
            else
                return FALSE;
        }
        printf("External reference written successfully\n");
    } else {
        printf("External symbol not found in entries table\n");
        fprintf(stderr, "Error at line %d: external referring to undefined label\n", lineNum);
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

    printf("--- Writing External Reference ---\n");
    printf("External reference - Label: '%s', Address: %d\n", label, address);

    newCapacity = *externalsContentCapacityP + strlen(label) + strlen(base4Address) + 2;
    printf("New externals buffer capacity: %u bytes\n", (unsigned int)newCapacity);

    newContent = (char *)malloc(newCapacity);

    if (newContent == NULL) {
        printf("Memory allocation failed for externals content\n");
        fprintf(stderr, "Error at line %d: failed to allocate memory\n", lineNum);
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

    printf("Externals content updated: '%s'\n", *externalsContentP);

    return TRUE;
}

/* Handle data symbol encoding */
int handleData(char *label, symbol tempSymbol, int lineNum, binaryWordNode *currentWordP, assemblerContext *context) {
    printf("--- Data Symbol Handling ---\n");
    printf("Processing data/code symbol - Label: '%s', Address: %d\n", label, tempSymbol.address);

    currentWordP->binaryWord.payload.payload_bits = tempSymbol.address;
    currentWordP->binaryWord.payload.aer_bits = AER_RELOCATABLE_ENCODING;

    printf("Encoded data word - Payload: %u, AER: %u\n", currentWordP->binaryWord.payload.payload_bits,
           currentWordP->binaryWord.payload.aer_bits);

    return TRUE;
}

/* Handle single-operand instruction encoding */
int handleOneOperandOpEncoding(binaryWordNode **opWordP, char *lineCopy, unsigned int destOperandAddressEncoding,
                               int lineNum, assemblerContext *context, symbolTable *entriesTable,
                               char **externalsContentP, size_t *externalsContentCapacityP) {
    int res;
    char *label;

    printf("--- One-Operand Encoding ---\n");
    printf("Line content: '%s'\n", lineCopy);
    printf("Destination addressing mode: %u\n", destOperandAddressEncoding);

    if (opWordP != NULL) {
        printf("Next word position - C: %d, L: %d, Kind: %d\n", (*opWordP)->C, (*opWordP)->L, (*opWordP)->kind);
    } else {
        printf("Warning: No next word available for operand encoding\n");
    }

    /* Advance to next word after operation word */
    if ((*opWordP)->next != NULL) {
        *opWordP = (*opWordP)->next;
        printf("Advanced to next word - C: %d, L: %d, Kind: %d\n", (*opWordP)->C, (*opWordP)->L, (*opWordP)->kind);
    }

    strtok(lineCopy, " \t");
    label = strtok(NULL, "[\n");

    printf("Extracted operand label: '%s'\n", label ? label : "NULL");

    if (destOperandAddressEncoding == DIRECT_OPCODE_ENCODING || destOperandAddressEncoding == MATRIX_OPCODE_ENCODING) {
        printf("Encoding operand with addressing mode %u\n", destOperandAddressEncoding);
        res = encodeOpPayloadWord(label, lineNum, context, entriesTable, *opWordP, externalsContentP,
                                  externalsContentCapacityP);
        if (res != TRUE) {
            if (res == MEMORY_ALLOCATION_ERROR)
                return MEMORY_ALLOCATION_ERROR;
            return FALSE;
        }

        printf("Operand encoded - Payload: %u, AER: %u\n", (*opWordP)->binaryWord.payload.payload_bits,
               (*opWordP)->binaryWord.payload.aer_bits);
    } else {
        printf("No encoding needed for addressing mode %u\n", destOperandAddressEncoding);
    }

    /* Advance pointer based on destination operand type */
    if (destOperandAddressEncoding == MATRIX_OPCODE_ENCODING) {
        printf("Matrix addressing for destination - advancing 2 positions\n");
        if ((*opWordP)->next && (*opWordP)->next->next) {
            *opWordP = (*opWordP)->next->next;
        }
    } else {
        printf("Non-Matrix addressing for destination - advancing 1 position\n");
        if ((*opWordP)->next) {
            *opWordP = (*opWordP)->next;
        }
    }

    printf("One-operand encoding completed successfully\n");
    return TRUE;
}

/* Generate .ent output file */
int createEntriesOutputFile(char *fileName, char *entriesContent, assemblerContext *context) {
    FILE *entriesFile;
    char *entriesFileName;

    printf("--- Creating Entries Output File ---\n");
    printf("Base filename: '%s'\n", fileName);

    /* Create output filename */
    entriesFileName = malloc(strlen(fileName) + 5);
    if (entriesFileName == NULL) {
        printf("Memory allocation failed for entries filename\n");
        fprintf(stdout, "Error: failed to allocate memory\n");
        return MEMORY_ALLOCATION_ERROR;
    }

    strcpy(entriesFileName, fileName);
    strcat(entriesFileName, ".ent");
    printf("Entries output filename: '%s'\n", entriesFileName);

    /* Write entries to file */
    entriesFile = fopen(entriesFileName, "w");
    if (entriesFile == NULL) {
        printf("Failed to create entries file\n");
        fprintf(stdout, "Error: cannot open file '%s'\n", entriesFileName);
        free(entriesFileName);
        return FALSE;
    }

    printf("Writing entries content to file...\n");
    fputs(entriesContent, entriesFile);
    printf("Entries file written successfully\n");

    fclose(entriesFile);
    free(entriesFileName);

    return TRUE;
}

/* Generate .ext output file */
int createExternalsOutputFile(char *fileName, char *externalsContent, assemblerContext *context) {
    FILE *externalsFile;
    char *externalsFileName;

    printf("--- Creating Externals Output File ---\n");
    printf("Base filename: '%s'\n", fileName);

    /* Create output filename */
    externalsFileName = malloc(strlen(fileName) + 5);
    if (externalsFileName == NULL) {
        printf("Memory allocation failed for externals filename\n");
        fprintf(stdout, "Error: failed to allocate memory\n");
        return MEMORY_ALLOCATION_ERROR;
    }

    strcpy(externalsFileName, fileName);
    strcat(externalsFileName, ".ext");
    printf("Externals output filename: '%s'\n", externalsFileName);

    /* Write externals to file */
    externalsFile = fopen(externalsFileName, "w");
    if (externalsFile == NULL) {
        printf("Failed to create externals file\n");
        fprintf(stdout, "Error: cannot open file '%s'\n", externalsFileName);
        free(externalsFileName);
        return FALSE;
    }

    printf("Writing externals content to file...\n");
    fputs(externalsContent, externalsFile);
    printf("Externals file written successfully\n");

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
    int wordValue;

    printf("--- Creating Object Output File ---\n");
    printf("Base filename: '%s'\n", fileName);
    printf("ICF: %d, DCF: %d, Total words: %d\n", ICF, DCF, ICF + DCF);

    /* Create output filename */
    objectFileName = malloc(strlen(fileName) + 5);
    if (objectFileName == NULL) {
        printf("Memory allocation failed for object filename\n");
        fprintf(stdout, "Error: failed to allocate memory\n");
        return MEMORY_ALLOCATION_ERROR;
    }

    strcpy(objectFileName, fileName);
    strcat(objectFileName, ".obj");
    printf("Object output filename: '%s'\n", objectFileName);

    /* Write object file header and data */
    objectFile = fopen(objectFileName, "w");
    if (objectFile == NULL) {
        printf("Failed to create object file\n");
        fprintf(stdout, "Error: cannot open file '%s'\n", objectFileName);
        free(objectFileName);
        return FALSE;
    }

    /* Write header with ICF and DCF */
    printf("Writing object file header...\n");
    sprintf(buffer, "%s\t%s\n", intToBase4(ICF, 'a'), intToBase4(DCF, 'a'));
    fputs(buffer, objectFile);
    printf("Header written: ICF=%s, DCF=%s\n", intToBase4(ICF, 'a'), intToBase4(DCF, 'a'));

    /* Write each binary word */
    printf("Writing binary words to object file...\n");
    while (i < ICF + DCF && currentWordP != NULL) {
        wordValue = BinarywordToInt(*currentWordP);
        printf("Word %d - Address: %d, Kind: %d, Value: %d, Base4: %s\n", i, INITIAL_ADDRESS + i, currentWordP->kind,
               wordValue, intToBase4(wordValue, 'c'));

        sprintf(buffer, "%s\t%s\n", intToBase4(INITIAL_ADDRESS + i, 'a'), intToBase4(wordValue, 'c'));
        fputs(buffer, objectFile);

        currentWordP = currentWordP->next;
        i++;
    }

    printf("Object file completed with %d words written\n", i);

    fclose(objectFile);
    free(objectFileName);
    return TRUE;
}

/* Convert binary word structure to integer */
int BinarywordToInt(binaryWordNode word) {
    unsigned int merge;

    printf("--- Converting Binary Word to Integer ---\n");
    printf("Word kind: %d, C: %d, L: %d\n", word.kind, word.C, word.L);

    /* Merge bit fields based on word type */
    switch (word.kind) {
    case DIR:
        merge = word.binaryWord.dir.data_bits;
        printf("Directive word - Data bits: %u, Result: %u\n", word.binaryWord.dir.data_bits, merge);
        break;

    case OP_FIRST:
        merge = (word.binaryWord.opFirst.opcode_bits << 6) | (word.binaryWord.opFirst.src_op_bits << 4) |
                (word.binaryWord.opFirst.dest_op_bits << 2) | (word.binaryWord.opFirst.aer_bits << 0);
        printf("Operation first word - Opcode: %u, Src: %u, Dest: %u, AER: %u, Result: %u\n",
               word.binaryWord.opFirst.opcode_bits, word.binaryWord.opFirst.src_op_bits,
               word.binaryWord.opFirst.dest_op_bits, word.binaryWord.opFirst.aer_bits, merge);
        break;

    case PAYLOAD:
        merge = (word.binaryWord.payload.payload_bits << 2) | (word.binaryWord.payload.aer_bits << 0);
        printf("Payload word - Payload: %u, AER: %u, Result: %u\n", word.binaryWord.payload.payload_bits,
               word.binaryWord.payload.aer_bits, merge);
        break;

    case REG_PAIR:
        merge = (word.binaryWord.regPair.more_padding << 9) | (word.binaryWord.regPair.reg1_bits << 6) |
                (word.binaryWord.regPair.padding << 5) | (word.binaryWord.regPair.reg2_bits << 2) |
                (word.binaryWord.regPair.aer_bits << 0);
        printf("Register pair word - Reg1: %u, Reg2: %u, AER: %u, Result: %u\n", word.binaryWord.regPair.reg1_bits,
               word.binaryWord.regPair.reg2_bits, word.binaryWord.regPair.aer_bits, merge);
        break;

    default:
        printf("Unknown word kind, returning 0\n");
        merge = 0;
        break;
    }

    return (int)merge;
}