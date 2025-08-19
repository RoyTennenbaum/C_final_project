#include "../Headers/second-iteration.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int secondIteration(char *fileName, int ICF, int DCF, binaryWordList *codeImage,
                    assemblerContext *context, symbolTable *entriesTable) {
  FILE *srcFile;
  char *srcFileName;
  char line[LINE_SIZE] = {0};
  char isDir[LINE_SIZE] = {0};
  char *arg;
  char *entriesContent = NULL;
  char *externalsContent = NULL;
  size_t entriesContentCapacity = 0;
  size_t externalsContentCapacity = 0;
  int lineNum = 0;
  int state;
  int errorFlag = FALSE;
  const directive *tempDirective;
  binaryWordNode *currentWordP = *codeImage;

  srcFileName = calloc(strlen(fileName) + 4, sizeof(char));

  if (srcFileName == NULL) {
    setFatalError(lineNum, ERR_MEM_ALLOC);
    return FALSE;
  }

  strcpy(srcFileName, fileName);
  strcat(srcFileName, ".am");

  srcFile = fopen(srcFileName, "r");
  if (srcFile == NULL) {
    setFatalError(lineNum, ERR_FILE_OPEN);
    free(srcFileName);
    return FALSE;
  }

  while (fgets(line, LINE_SIZE, srcFile) != NULL) {
    lineNum++;
    state = UNKNOWN_LINE_TYPE;
    arg = strtok(line, ":\n"); /*extract label if exists*/

    if (arg == NULL) {
      state = EMPTY_LINE;
    }

    else if (arg[0] == ';') {
      state = COMMENT_LINE;
    }

    else if (searchSymbol(*(context->symbolTable), arg) != NULL) {
      arg = strtok(NULL, "\n"); /*skip the label*/
      while (isspace(*arg)) arg++;
    }
    /*by now arg points to the line without the label (no '\n' at the end)*/

    if (state == UNKNOWN_LINE_TYPE) {
      /* extract first word to check if it's a directive */
      strcpy(isDir, arg);
      strtok(isDir, " \t\n");

      if ((tempDirective = searchDirective(*(context->directiveTable), isDir)) != NULL) {
        if ((*tempDirective).type == ENTRY) {
          state = ENTRY_LINE;
        } else {
          state = NON_ENTRY_DIRECTIVE_LINE;
        }
      } else {
        state = OPERATION_LINE;
      }
    }

    switch (state) {
      case EMPTY_LINE:
      case COMMENT_LINE:
      case NON_ENTRY_DIRECTIVE_LINE:
        continue;

      case ENTRY_LINE:
        handleEntryLine(arg, &entriesContent, &entriesContentCapacity, entriesTable, lineNum,
                        &errorFlag, context);
        if (fatalError) {
          fclose(srcFile);
          free(srcFileName);
          if (entriesContent != NULL) free(entriesContent);
          if (externalsContent != NULL) free(externalsContent);
          return FALSE;
        }
        break;

      case OPERATION_LINE:
        handleOperationLine(arg, &currentWordP, &externalsContent, &externalsContentCapacity,
                            lineNum, &errorFlag, context, entriesTable);
        if (fatalError) {
          fclose(srcFile);
          free(srcFileName);
          if (entriesContent != NULL) free(entriesContent);
          if (externalsContent != NULL) free(externalsContent);
          return FALSE;
        }
        break;

      default:
        setFatalError(lineNum, UNKNOWN_LINE_TYPE);
        return FALSE;
        break;
    }
    printf("\n");
  }

  if (errorFlag == FALSE) {
    printf(
        "\n========================================"
        "\n"
        "Generating output files"
        "\n"
        "========================================\n");
    createObjectOutputFile(fileName, codeImage, ICF, DCF);
    if (fatalError) {
      fclose(srcFile);
      free(srcFileName);
      if (entriesContent != NULL) free(entriesContent);
      if (externalsContent != NULL) free(externalsContent);
    }
  }

  if (errorFlag == FALSE && entriesContent != NULL) {
    createEntriesOutputFile(fileName, entriesContent);
    if (fatalError) {
      fclose(srcFile);
      free(srcFileName);
      if (entriesContent != NULL) free(entriesContent);
      if (externalsContent != NULL) free(externalsContent);
    }
  }

  if (errorFlag == FALSE && externalsContent != NULL) {
    createExternalsOutputFile(fileName, externalsContent);
    if (fatalError) {
      fclose(srcFile);
      free(srcFileName);
      if (entriesContent != NULL) free(entriesContent);
      if (externalsContent != NULL) free(externalsContent);
    }
  }

  fclose(srcFile);
  free(srcFileName);
  if (entriesContent != NULL) free(entriesContent);
  if (externalsContent != NULL) free(externalsContent);

  return errorFlag ? FALSE : TRUE;
}

void handleEntryLine(char *line, char **entriesContentP, size_t *entriesContentCapacityP,
                     symbolTable *entriesTable, const int lineNum, int *errorFlagP,
                     assemblerContext *context) {
  symbol *tempSymbol;
  char *arg;

  strtok(line, " \t");
  arg = strtok(NULL, " \t");

  if ((tempSymbol = searchSymbol(*(context->symbolTable), arg)) != NULL) {
    if ((*tempSymbol).type == TYPE_EXTERNAL) {
      insertError((context->errorList), ERR_ENTRY_SYMBOL_DEFINED_EXTERN, lineNum);
      *errorFlagP = TRUE;
      return;
    } else {
      insertSymbol(entriesTable, arg, (*tempSymbol).address, TYPE_ENTRY, lineNum);
      writeToEntries(*tempSymbol, entriesContentP, entriesContentCapacityP, lineNum,
                     errorFlagP, context);
    }
  } else {
    insertError((context->errorList), ERR_UNDEFINED_SYMBOL, lineNum);
    *errorFlagP = TRUE;
    return;
  }
}

void writeToEntries(const symbol symbol, char **entriesContentP,
                    size_t *entriesContentCapacityP, const int lineNum, int *errorFlagP,
                    assemblerContext *context) {
  char *addressStr;
  int extraChars = (*entriesContentP == NULL) ? 3 : 2;
  size_t newCapacity;
  char *buffer;

  addressStr = intToBase4(symbol.address, ADDRESS_REP, lineNum);

  if (fatalError) return;

  newCapacity = *entriesContentCapacityP + strlen(addressStr) + BASE4_ADDRESS_PADDING +
                strlen(symbol.label) + extraChars;

  buffer = realloc(*entriesContentP, newCapacity);

  if (buffer == NULL) {
    setFatalError(lineNum, ERR_MEM_ALLOC);
    if (addressStr != NULL) free(addressStr);
    return;
  }

  if (*entriesContentP == NULL) {
    buffer[0] = '\0';
  }

  *entriesContentP = buffer;
  *entriesContentCapacityP = newCapacity;

  strcat(*entriesContentP, "\n");
  strcat(*entriesContentP, addressStr);
  strcat(*entriesContentP, "\t");
  strcat(*entriesContentP, symbol.label);

  free(addressStr);
}

void handleOperationLine(char *line, binaryWordNode **opWordP, char **externalsContentP,
                         size_t *externalsContentCapacityP, const int lineNum, int *errorFlagP,
                         assemblerContext *context, const symbolTable *entriesTable) {
  WordType tempFirstOpWord = (*opWordP)->binaryWord;
  unsigned int opCode = tempFirstOpWord.opFirst.opcode_bits;
  unsigned int srcAddressMethod = tempFirstOpWord.opFirst.src_op_bits;
  unsigned int destAddressMethod = tempFirstOpWord.opFirst.dest_op_bits;

  switch (opCode) {
    case MOV:
    case CMP:
    case ADD:
    case SUB:
    case LEA:
      handleTwoOperandOpCode(line, opWordP, externalsContentP, externalsContentCapacityP,
                             srcAddressMethod, destAddressMethod, lineNum, errorFlagP, context,
                             entriesTable);
      if (fatalError) return;
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
      handleOneOperandOpCode(line, opWordP, externalsContentP, externalsContentCapacityP,
                             destAddressMethod, lineNum, errorFlagP, context, entriesTable);
      if (fatalError) return;
      break;

    case RTS:
    case STP:
      break;

    default:
      setFatalError(lineNum, ERR_INTERNAL);
      return;
  }
}

void handleTwoOperandOpCode(char *line, binaryWordNode **opWordP, char **externalsContentP,
                            size_t *externalsContentCapacityP, const unsigned srcAddressMethod,
                            const unsigned destAddressMethod, const int lineNum,
                            int *errorFlagP, assemblerContext *context,
                            const symbolTable *entriesTable) {
  char *label1;
  char *label2;

  strtok(line, " \t");
  label1 = strtok(NULL, ",");
  label2 = strtok(NULL, " \t");
  strtok(label1, "[,");
  strtok(label2, "[");

  /*advace to first payload word*/
  if ((*opWordP)->next != NULL) {
    *opWordP = (*opWordP)->next;
  }

  if (srcAddressMethod == DIRECT_ADDRESS_ENCODING ||
      srcAddressMethod == MATRIX_ADDRESS_ENCODING) {
    encodeOpPayloadWord(label1, *opWordP, externalsContentP, externalsContentCapacityP,
                        srcAddressMethod, lineNum, errorFlagP, context, entriesTable);
    if (fatalError) return;
  }

  if (srcAddressMethod == MATRIX_ADDRESS_ENCODING) {
    if ((*opWordP)->next != NULL && (*opWordP)->next->next != NULL) {
      *opWordP = (*opWordP)->next->next;
    } else {
      /*assembler memory management failure: missing required words */
      setFatalError(lineNum, ERR_MEM_ALLOC);
      return;
    }
  }

  else {
    if ((*opWordP)->next != NULL) {
      *opWordP = (*opWordP)->next;
    } else {
      /*assembler memory management failure: missing required words */
      setFatalError(lineNum, ERR_MEM_ALLOC);
      return;
    }
  }

  if (destAddressMethod == DIRECT_ADDRESS_ENCODING ||
      destAddressMethod == MATRIX_ADDRESS_ENCODING) {
    encodeOpPayloadWord(label2, *opWordP, externalsContentP, externalsContentCapacityP,
                        destAddressMethod, lineNum, errorFlagP, context, entriesTable);
    if (fatalError) return;
  }

  if (destAddressMethod == MATRIX_ADDRESS_ENCODING) {
    if ((*opWordP)->next != NULL && (*opWordP)->next->next != NULL) {
      *opWordP = (*opWordP)->next->next;
    } else {
      /*assembler memory management failure: missing required words */
      setFatalError(lineNum, ERR_MEM_ALLOC);
      return;
    }
  }

  else if (srcAddressMethod != REGISTER_ADDRESS_ENCODING ||
           destAddressMethod != REGISTER_ADDRESS_ENCODING) {
    if ((*opWordP)->next != NULL) {
      *opWordP = (*opWordP)->next;
    } else {
      /*assembler memory management failure: missing required words */
      setFatalError(lineNum, ERR_MEM_ALLOC);
      return;
    }
  }
}

void handleOneOperandOpCode(char *line, binaryWordNode **opWordP, char **externalsContentP,
                            size_t *externalsContentCapacityP,
                            const unsigned destAddressMethod, const int lineNum,
                            int *errorFlagP, assemblerContext *context,
                            const symbolTable *entriesTable) {
  char *label;

  strtok(line, " \t");
  label = strtok(NULL, " \t");
  strtok(label, " \t[");

  /*advace to the first payload word*/
  if ((*opWordP)->next != NULL) {
    *opWordP = (*opWordP)->next;
  }

  if (destAddressMethod == DIRECT_ADDRESS_ENCODING ||
      destAddressMethod == MATRIX_ADDRESS_ENCODING) {
    encodeOpPayloadWord(label, *opWordP, externalsContentP, externalsContentCapacityP,
                        destAddressMethod, lineNum, errorFlagP, context, entriesTable);
    if (fatalError) return;
  }

  if (destAddressMethod == MATRIX_ADDRESS_ENCODING) {
    if ((*opWordP)->next != NULL && (*opWordP)->next->next != NULL) {
      *opWordP = (*opWordP)->next->next;
    } else {
      /*assembler memory management failure: missing required words */
      setFatalError(lineNum, ERR_MEM_ALLOC);
      return;
    }
  }

  else {
    if ((*opWordP)->next != NULL) {
      *opWordP = (*opWordP)->next;
    } else {
      /*assembler memory management failure: missing required words */
      setFatalError(lineNum, ERR_MEM_ALLOC);
      return;
    }
  }
}
void encodeOpPayloadWord(const char *label, binaryWordNode *currentWordP,
                         char **externalsContentP, size_t *externalsContentCapacityP,
                         const unsigned addressMethod, const int lineNum, int *errorFlagP,
                         assemblerContext *context, const symbolTable *entriesTable) {
  symbol *tempSymbol;

  if ((tempSymbol = searchSymbol(*(context->symbolTable), label)) != NULL) {
    switch ((*tempSymbol).type) {
      case TYPE_EXTERNAL:
        handleExternal(label, currentWordP, externalsContentP, externalsContentCapacityP,
                       lineNum, errorFlagP, context, entriesTable);
        if (fatalError) return;
        break;

      case TYPE_DATA:
      case TYPE_CODE:
        handleRelocatable(*tempSymbol, currentWordP);
        if (fatalError) return;
        break;

      default:
        setFatalError(lineNum, ERR_INTERNAL);
        break;
    }
  }

  else {
    insertError((*context).errorList, ERR_UNDEFINED_SYMBOL, lineNum);
    *errorFlagP = TRUE;
    return;
  }
}

void handleExternal(const char *label, binaryWordNode *currentWordP, char **externalsContentP,
                    size_t *externalsContentCapacityP, const int lineNum, int *errorFlagP,
                    assemblerContext *context, const symbolTable *entriesTable) {
  symbol *tempEntry;
  if ((tempEntry = searchSymbol(*entriesTable, label)) != NULL) {
    currentWordP->binaryWord.payload.payload_bits = (*tempEntry).address;
    currentWordP->binaryWord.payload.aer_bits = AER_EXTERNAL_ENCODING;
    writeToExternals(label, currentWordP, externalsContentP, externalsContentCapacityP,
                     lineNum, errorFlagP, context);
    if (fatalError) return;
  } else {
    insertError((*context).errorList, ERR_ENTRY_SYMBOL_NOT_FOUND, lineNum);
    *errorFlagP = TRUE;
    return;
  }
}

void writeToExternals(const char *label, const binaryWordNode *currentWordP,
                      char **externalsContentP, size_t *externalsContentCapacityP,
                      const int lineNum, int *errorFlagP, assemblerContext *context) {
  int address = currentWordP->C + currentWordP->L;
  char *addressStr;
  int extraChars = (*externalsContentP == NULL) ? 3 : 2;
  size_t newCapacity;
  char *buffer;

  addressStr = intToBase4(address, ADDRESS_REP, lineNum);

  if (fatalError) return;

  newCapacity = *externalsContentCapacityP + strlen(addressStr) + BASE4_ADDRESS_PADDING +
                strlen(label) + extraChars;

  buffer = realloc(*externalsContentP, newCapacity);

  if (buffer == NULL) {
    setFatalError(lineNum, ERR_MEM_ALLOC);
    if (addressStr != NULL) free(addressStr);
    return;
  }

  if (*externalsContentP == NULL) {
    buffer[0] = '\0';
  }

  *externalsContentP = buffer;
  *externalsContentCapacityP = newCapacity;

  strcat(*externalsContentP, "\n");
  strcat(*externalsContentP, addressStr);
  strcat(*externalsContentP, "\t");
  strcat(*externalsContentP, label);

  free(addressStr);
}

void handleRelocatable(symbol symbol, binaryWordNode *currentWordP) {
  currentWordP->binaryWord.payload.payload_bits = symbol.address;
  currentWordP->binaryWord.payload.aer_bits = AER_RELOCATABLE_ENCODING;
}

void createObjectOutputFile(const char *fileName, const binaryWordList *codeImage,
                            const int ICF, const int DCF) {
  FILE *objectFile;
  char *objectFileName;
  char *icfStr = intToBase4(ICF - INITIAL_ADDRESS, COUNTER_REP, 0);
  char *dcfStr = intToBase4(DCF, COUNTER_REP, 0);
  char *addressStr = NULL;
  char *wordStr = NULL;
  char *buffer;
  unsigned int word;

  int i = INITIAL_ADDRESS;
  binaryWordNode *currentWordP = *codeImage;

  if (fatalError) {
    if (icfStr != NULL) free(icfStr);
    if (dcfStr != NULL) free(dcfStr);
    return;
  }

  objectFileName = calloc(strlen(fileName) + 4, sizeof(char));

  if (objectFileName == NULL) {
    setFatalError(0, ERR_MEM_ALLOC);
    if (icfStr != NULL) free(icfStr);
    if (dcfStr != NULL) free(dcfStr);
    return;
  }

  strcat(objectFileName, fileName);
  strcat(objectFileName, ".ob");

  objectFile = fopen(objectFileName, "w");

  if (objectFile == NULL) {
    setFatalError(0, ERR_FILE_OPEN);
    remove(objectFileName);
    free(objectFileName);
    if (icfStr != NULL) free(icfStr);
    if (dcfStr != NULL) free(dcfStr);
    return;
  }

  buffer = calloc(strlen(icfStr) + strlen(dcfStr) + 3, sizeof(char));

  if (buffer == NULL) {
    setFatalError(0, ERR_MEM_ALLOC);
    fclose(objectFile);
    remove(objectFileName);
    free(objectFileName);
    if (icfStr != NULL) free(icfStr);
    if (dcfStr != NULL) free(dcfStr);
    return;
  }

  sprintf(buffer, "%s\t%s\n", icfStr, dcfStr);
  fputs(buffer, objectFile);
  free(buffer);
  if (icfStr != NULL) free(icfStr);
  if (dcfStr != NULL) free(dcfStr);

  buffer = calloc(OBJECT_FILE_LINE_SIZE, sizeof(char));

  if (buffer == NULL) {
    setFatalError(0, ERR_MEM_ALLOC);
    fclose(objectFile);
    remove(objectFileName);
    free(objectFileName);
    return;
  }

  while (currentWordP != NULL && i < ICF + DCF) {
    if (addressStr != NULL) free(addressStr);
    if (wordStr != NULL) free(wordStr);
    addressStr = NULL;
    wordStr = NULL;

    addressStr = intToBase4(i, ADDRESS_REP, 0);
    word = BinaryWordToInt(*currentWordP);
    wordStr = intToBase4(word, CODE_REP, 0);

    if (fatalError) {
      fclose(objectFile);
      remove(objectFileName);
      free(objectFileName);
      if (wordStr != NULL) free(wordStr);
      if (addressStr != NULL) free(addressStr);
      free(buffer);
      return;
    }

    sprintf(buffer, "%s\t%s\n", addressStr, wordStr);
    fputs(buffer, objectFile);

    currentWordP = currentWordP->next;
    i++;
    printf("========================================");
  }

  printf("Object file generated successfuly\n\n");

  fclose(objectFile);
  free(objectFileName);
  free(buffer);
  if (wordStr != NULL) free(wordStr);
  if (addressStr != NULL) free(addressStr);
}

void createEntriesOutputFile(const char *fileName, const char *entriesContent) {
  FILE *entriesFile;
  char *entriesFileName = calloc(strlen(fileName) + 5, sizeof(char));

  if (entriesFileName == NULL) {
    setFatalError(0, ERR_MEM_ALLOC);
    return;
  }

  strcpy(entriesFileName, fileName);
  strcat(entriesFileName, ".ent");

  entriesFile = fopen(entriesFileName, "w");

  if (entriesFile == NULL) {
    setFatalError(0, ERR_MEM_ALLOC);
    remove(entriesFileName);
    return;
  }

  fputs(entriesContent, entriesFile);

  printf("Entries file generated successfuly\n\n");

  fclose(entriesFile);
  free(entriesFileName);
}

void createExternalsOutputFile(const char *fileName, const char *externalsContent) {
  FILE *externalsFile;
  char *externalsFileName = calloc(strlen(fileName) + 5, sizeof(char));

  if (externalsFileName == NULL) {
    setFatalError(0, ERR_MEM_ALLOC);
    return;
  }

  strcpy(externalsFileName, fileName);
  strcat(externalsFileName, ".ent");

  externalsFile = fopen(externalsFileName, "w");

  if (externalsFile == NULL) {
    setFatalError(0, ERR_MEM_ALLOC);
    remove(externalsFileName);
    return;
  }

  fputs(externalsContent, externalsFile);

  printf("Externals file generated successfuly\n\n");

  fclose(externalsFile);
  free(externalsFileName);
}

char *intToBase4(unsigned int integer, int rep, int lineNum) {
  char *result = NULL;
  char *temp;
  char *forPadding;
  char digit;
  size_t length = 0;
  int i;
  int j;

  if (integer == 0) {
    temp = malloc(2);
    if (temp == NULL) {
      setFatalError(lineNum, ERR_MEM_ALLOC);
      return NULL;
    }
    result = temp;
    result[0] = 'a';
    length = 1;
  }

  else {
    while (integer) {
      length++;
      temp = realloc(result, length + 1);

      if (temp == NULL) {
        setFatalError(lineNum, ERR_MEM_ALLOC);
        if (result != NULL) free(result);
        return NULL;
      }
      result = temp;
      digit = 'a' + integer % 4;

      result[length - 1] = digit;
      integer /= 4;
    }
  }
  /*by now result holds the reversed string that represents 'integer' in base 4*/

  /*add padding if needed*/
  switch (rep) {
    case ADDRESS_REP:
      i = length;
      length += (BASE4_ADDRESS_PADDING - length > 0) ? (BASE4_ADDRESS_PADDING - length) : 0;
      temp = realloc(result, length + 1);

      if (temp == NULL) {
        setFatalError(lineNum, ERR_MEM_ALLOC);
        if (result != NULL) free(result);
        return NULL;
      }
      result = temp;

      forPadding = result + i;

      while (i < length) {
        *forPadding = 'a';
        forPadding++;
        i++;
      }
      break;

    case CODE_REP:
      i = length;
      length += (BASE4_CODE_PADDING - length > 0) ? (BASE4_CODE_PADDING - length) : 0;
      temp = realloc(result, length + 1);

      if (temp == NULL) {
        setFatalError(lineNum, ERR_MEM_ALLOC);
        if (result != NULL) free(result);
        return NULL;
      }
      result = temp;

      forPadding = result + i;

      while (i < length) {
        *forPadding = 'a';
        forPadding++;
        i++;
      }
      break;

    case COUNTER_REP:
      /*no padding*/
      break;

    default:
      setFatalError(lineNum, ERR_INTERNAL);
      if (result != NULL) free(result);
      return NULL;
      break;
  }

  i = 0;
  j = length - 1;

  while (i < j) {
    digit = result[i];
    result[i] = result[j];
    result[j] = digit;
    i++;
    j--;
  }
  result[length] = 0;
  printf("\n");

  return result;
}

unsigned int BinaryWordToInt(binaryWordNode word) {
  unsigned int merge;

  switch (word.kind) {
    case DIR:
      merge = word.binaryWord.dir.data_bits;
      break;

    case OP_FIRST:
      merge = (word.binaryWord.opFirst.opcode_bits << 6) |
              (word.binaryWord.opFirst.src_op_bits << 4) |
              (word.binaryWord.opFirst.dest_op_bits << 2) |
              (word.binaryWord.opFirst.aer_bits << 0);
      break;

    case PAYLOAD:
      merge = (word.binaryWord.payload.payload_bits << 2) |
              (word.binaryWord.payload.aer_bits << 0);
      break;

    case REG_PAIR:
      merge =
          (word.binaryWord.regPair.more_padding << 9) |
          (word.binaryWord.regPair.reg1_bits << 6) | (word.binaryWord.regPair.padding << 5) |
          (word.binaryWord.regPair.reg2_bits << 2) | (word.binaryWord.regPair.aer_bits << 0);
      break;

    default:
      merge = 0;
      break;
  }

  return merge;
}
