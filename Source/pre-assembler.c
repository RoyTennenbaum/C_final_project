/*
 * Pre-Assembler Implementation
 *
 * This module handles the pre-assembly phase of the assembler, processing
 * macro definitions and expansions to produce an intermediate .am file.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../Headers/pre-assembler.h"

/*
 * Pre-Assembler Main Function
 *
 * Reads the source file and processes macros to create a .am file
 */
int preAssembler(char *fileName, assemblerContext *context) {
    /* File handling variables */
    FILE *srcFile;
    char *srcFileName;

    /* Content buffer variables */
    char *amContent;
    char *newContent;
    size_t contentCapacity = LINE_SIZE;

    /* Line processing variables */
    int lineNum = 0;
    char line[LINE_SIZE] = {'\0'};
    char lineCopy[LINE_SIZE];
    char *arg;

    /* State and control variables */
    int state;
    macro *tempMacro;
    int errorFlag = FALSE;
    int res;

    /* Allocate memory for source filename with .as extension */
    srcFileName = malloc(strlen(fileName) + 4);
    if (srcFileName == NULL) {
        insertError((*context).errorList, ERR_MEM_ALLOC, lineNum);
        return FALSE;
    }

    strcpy(srcFileName, fileName);
    strcat(srcFileName, ".as");

    /* Open source file for reading */
    srcFile = fopen(srcFileName, "r");
    if (srcFile == NULL) {
        insertError((*context).errorList, ERR_FILE_OPEN, lineNum);
        free(srcFileName);
        return FALSE;
    }

    /* Allocate initial content buffer */
    amContent = calloc(contentCapacity, sizeof(char));
    if (amContent == NULL) {
        insertError((*context).errorList, ERR_MEM_ALLOC, lineNum);
        free(srcFileName);
        fclose(srcFile);
        return FALSE;
    }

    /* Main processing loop - read and process each line */
    while (fgets(line, LINE_SIZE, srcFile) != NULL) {
        lineNum++;
        state = NORMAL_LINE;

        /* Expand buffer capacity for new content */
        contentCapacity += LINE_SIZE;
        newContent = realloc(amContent, contentCapacity);
        if (newContent == NULL) {
            insertError((*context).errorList, ERR_MEM_ALLOC, lineNum);
            free(srcFileName);
            free(amContent);
            fclose(srcFile);
            return FALSE;
        }
        amContent = newContent;

        /* Check for line length errors but continue processing */
        if (strlen(line) >= LINE_SIZE - 1) {
            insertError((*context).errorList, ERR_LINE_TOO_LONG, lineNum);
            errorFlag = TRUE;
        }

        /* Create copy of line for tokenization (strtok destroys original) */
        strncpy(lineCopy, line, LINE_SIZE - 1);
        lineCopy[LINE_SIZE - 1] = '\0';
        arg = strtok(lineCopy, " \t\n");

        /* Determine line type based on first token */
        if (arg == NULL)
            state = EMPTY_LINE;
        else if ((tempMacro = searchMacro(*(context->macroTable), arg)) != NULL)
            state = MACRO_SPREAD;
        else if (strcmp(arg, "mcro") == 0)
            state = MACRO_DEFINE;

        /* Process line according to its type */
        switch (state) {
        case EMPTY_LINE:
            /* Handle empty or whitespace-only lines */
            res = handleEmptyLine(&amContent, &contentCapacity, lineNum, context);
            if (res == MEMORY_ALLOCATION_ERROR) {
                free(srcFileName);
                free(amContent);
                fclose(srcFile);
                return FALSE;
            } else if (res != TRUE)
                errorFlag = TRUE;
            break;

        case MACRO_SPREAD:
            /* Expand macro call with its arguments */
            res = handleMacroSpread(tempMacro, &arg, &amContent, &contentCapacity, lineNum, context);
            if (res == MEMORY_ALLOCATION_ERROR) {
                free(srcFileName);
                free(amContent);
                fclose(srcFile);
                return FALSE;
            } else if (res != TRUE)
                errorFlag = TRUE;
            break;

        case MACRO_DEFINE:
            /* Process macro definition from "mcro" to "mcroend" */
            res = handleMacroDefinition(&arg, context, srcFile, &lineNum);
            if (res == MEMORY_ALLOCATION_ERROR) {
                free(srcFileName);
                free(amContent);
                fclose(srcFile);
                return FALSE;
            } else if (res != TRUE)
                errorFlag = TRUE;
            break;

        case NORMAL_LINE:
            /* Copy regular assembly instruction as-is */
            res = addToContent(line, &amContent, &contentCapacity, lineNum, context);
            if (res == MEMORY_ALLOCATION_ERROR) {
                free(srcFileName);
                free(amContent);
                fclose(srcFile);
                return FALSE;
            } else if (res != TRUE)
                errorFlag = TRUE;
            break;

        default:
            /* Unexpected line type */
            insertError((*context).errorList, ERR_UNKNOWN_LINE_TYPE, lineNum);
            errorFlag = TRUE;
            break;
        }
    }

    /* Create output .am file only if no syntax errors occurred */
    if (errorFlag == FALSE) {
        res = createOutputFile(fileName, amContent, lineNum, context);
        if (res == MEMORY_ALLOCATION_ERROR) {
            free(srcFileName);
            free(amContent);
            fclose(srcFile);
            return FALSE;
        } else if (res != TRUE) {
            errorFlag = TRUE;
        }
    }

    /* Cleanup resources */
    free(srcFileName);
    free(amContent);
    fclose(srcFile);

    return (errorFlag) ? FALSE : TRUE;
}

/*
 * Handle Empty Line
 *
 * Adds a newline to keep empty lines in output
 */
int handleEmptyLine(char **amContent, size_t *contentCapacity, int lineNum, assemblerContext *context) {
    int res;

    res = addToContent("\n", amContent, contentCapacity, lineNum, context);
    if (res == MEMORY_ALLOCATION_ERROR) {
        insertError((*context).errorList, ERR_MEM_ALLOC, lineNum);
        return MEMORY_ALLOCATION_ERROR;
    } else if (res != TRUE) {
        insertError((*context).errorList, ERR_INSERT_EMPTY_LINE_FAIL, lineNum);

        return FALSE;
    }
    return TRUE;
}

/*
 * Handle Macro Expansion
 *
 * Replaces macro call with macro body plus arguments
 */
int handleMacroSpread(macro *tempMacro, char **argP, char **amContent, size_t *contentCapacity, int lineNum,
                      assemblerContext *context) {
    int res;

    /* Insert macro body into output */
    res = addToContent((*tempMacro).body, amContent, contentCapacity, lineNum, context);
    if (res == MEMORY_ALLOCATION_ERROR) {
        insertError((*context).errorList, ERR_MEM_ALLOC, lineNum);
        return MEMORY_ALLOCATION_ERROR;
    } else if (res != TRUE) {
        insertError((*context).errorList, ERR_MACRO_EXPANSION_FAIL, lineNum);
        return FALSE;
    }

    /* Get macro arguments and add them */
    *argP = strtok(NULL, " \t\n");

    /* Add each argument with space before it */
    while (*argP != NULL) {
        res = addToContent(" ", amContent, contentCapacity, lineNum, context);
        if (res == MEMORY_ALLOCATION_ERROR) {
            insertError((*context).errorList, ERR_MEM_ALLOC, lineNum);
            return MEMORY_ALLOCATION_ERROR;
        } else if (res != TRUE) {
            insertError((*context).errorList, ERR_MACRO_EXPANSION_FAIL, lineNum);
            return FALSE;
        }

        res = addToContent(*argP, amContent, contentCapacity, lineNum, context);
        if (res == MEMORY_ALLOCATION_ERROR) {
            insertError((*context).errorList, ERR_MEM_ALLOC, lineNum);
            return MEMORY_ALLOCATION_ERROR;
        } else if (res != TRUE) {
            insertError((*context).errorList, ERR_MACRO_EXPANSION_FAIL, lineNum);
            return FALSE;
        }

        *argP = strtok(NULL, " \t\n");
    }

    /* Add newline to complete the expansion */
    res = addToContent("\n", amContent, contentCapacity, lineNum, context);
    if (res == MEMORY_ALLOCATION_ERROR) {
        insertError((*context).errorList, ERR_MEM_ALLOC, lineNum);
        return MEMORY_ALLOCATION_ERROR;
    } else if (res != TRUE) {
        insertError((*context).errorList, ERR_MACRO_EXPANSION_FAIL, lineNum);
        return FALSE;
    }

    return TRUE;
}

/*
 * Handle Macro Definition
 *
 * Reads macro from "mcro" to "mcroend" and stores it
 */
int handleMacroDefinition(char **argP, assemblerContext *context, FILE *srcFile, int *lineNumP) {
    /* Macro storage variables */
    char *macroLabel;
    char *macroBody;
    char *newBody;

    /* Buffer management variables */
    size_t macroBodyCapacity = LINE_SIZE;
    char macroLine[LINE_SIZE];
    char lineCopy[LINE_SIZE];

    /* Get macro name (next token after "mcro") */
    *argP = strtok(NULL, " \t\n");

    /* Validate macro name doesn't conflict with reserved words */
    if (!isValidMacroLabel(*argP, context)) {
        insertError((*context).errorList, ERR_ILLEGAL_MACRO_NAME, *lineNumP);
        return FALSE;
    }

    /* Allocate memory for macro name */
    macroLabel = malloc(strlen(*argP) + 1);
    if (macroLabel == NULL) {
        insertError((*context).errorList, ERR_MEM_ALLOC, *lineNumP);
        return MEMORY_ALLOCATION_ERROR;
    }
    strcpy(macroLabel, *argP);

    /* Allocate initial buffer for macro body */
    macroBody = calloc(macroBodyCapacity, sizeof(char));
    if (macroBody == NULL) {
        insertError((*context).errorList, ERR_MEM_ALLOC, *lineNumP);
        free(macroLabel);
        return MEMORY_ALLOCATION_ERROR;
    }

    /* Read lines until we find "mcroend" */
    while (fgets(macroLine, LINE_SIZE, srcFile) != NULL) {
        (*lineNumP)++;

        /* Check for "mcroend" before processing the line */
        strncpy(lineCopy, macroLine, LINE_SIZE - 1);
        lineCopy[LINE_SIZE - 1] = '\0';

        /* Tokenize copy to preserve original line */
        *argP = strtok(lineCopy, " \t\n");
        if (*argP != NULL && strcmp(*argP, "mcroend") == 0) {
            break; /* Exit loop immediately when "mcroend" is found */
        }

        /* Expand buffer capacity for this line */
        macroBodyCapacity += LINE_SIZE;
        newBody = realloc(macroBody, macroBodyCapacity);
        if (newBody == NULL) {
            insertError((*context).errorList, ERR_MEM_ALLOC, *lineNumP);
            free(macroLabel);
            free(macroBody);
            return MEMORY_ALLOCATION_ERROR;
        }
        macroBody = newBody;

        /* Add line to macro body */
        strcat(macroBody, macroLine);
    }

    /* Save macro in table */
    insertMacro((*context).macroTable, macroLabel, macroBody);

    /* Free memory */
    free(macroLabel);
    free(macroBody);

    return TRUE;
}

/*
 * Add Content to Buffer
 *
 * Adds text to existing buffer, preserving previous content
 */
int addToContent(const char *text, char **amContentP, size_t *contentCapacityP, int lineNum,
                 assemblerContext *context) {
    /* Expand buffer capacity */
    size_t newCapacity = *contentCapacityP + LINE_SIZE;
    char *newContent = (char *)calloc(newCapacity, sizeof(char));

    if (newContent == NULL) {
        insertError((*context).errorList, ERR_MEM_ALLOC, lineNum);
        return MEMORY_ALLOCATION_ERROR;
    }

    /* Preserve existing content if buffer is not empty */
    if (*amContentP != NULL) {
        strcpy(newContent, *amContentP);
        free(*amContentP);
    }

    /* Append new text to buffer */
    strcat(newContent, text);

    *amContentP = newContent;
    *contentCapacityP = newCapacity;

    return TRUE;
}

/*
 * Create Output File
 *
 * Creates .am file with processed content
 */
int createOutputFile(const char *fileName, const char *amContent, int lineNum, assemblerContext *context) {
    FILE *amFile;
    char *amFileName;

    /* Allocate memory for output filename with .am extension */
    amFileName = malloc(strlen(fileName) + 4);
    if (amFileName == NULL) {
        insertError((*context).errorList, ERR_MEM_ALLOC, lineNum);
        return MEMORY_ALLOCATION_ERROR;
    }

    strcpy(amFileName, fileName);
    strcat(amFileName, ".am");

    /* Open output file for writing */
    amFile = fopen(amFileName, "w");
    if (amFile == NULL) {
        insertError((*context).errorList, ERR_FILE_OPEN, lineNum);
        free(amFileName);
        return FALSE;
    }

    /* Write content to file */
    fputs(amContent, amFile);
    fclose(amFile);

    /* Free filename buffer */
    free(amFileName);

    return TRUE;
}

/*
 * Validate Macro Label
 *
 * Checks if macro name is valid (not reserved word, not too long)
 */
int isValidMacroLabel(char *arg, assemblerContext *context) {
    return ((searchOperation(*(context->operationTable), arg) == NULL) &&
            (searchDirective(*(context->directiveTable), arg) == NULL) &&
            (searchRegister(*(context->registers), arg) == NULL) && (strlen(arg) <= MACRO_LABEL_BUFF))
               ? TRUE
               : FALSE;
}