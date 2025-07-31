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
 * Processes source file for macro definitions and expansions, generating
 * an intermediate .am file with all macros expanded.
 *
 * Parameters:
 *   srcFileName - Name of source assembly file to process
 *   context     - Assembler context containing symbol and macro tables
 *
 * Returns:
 *   1 on success, 0 on failure
 */
int preAssembler(char *srcFileName, assemblerContext *context) {
    /* File handling variables */
    FILE *srcFile;

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

    /* Open source file for reading */
    srcFile = fopen(srcFileName, "r");
    if (srcFile == NULL) {
        fprintf(stderr, "Error: Cannot open file '%s'\n", srcFileName);
        return 0;
    }

    /* Allocate initial content buffer */
    amContent = calloc(contentCapacity, sizeof(char));
    if (amContent == NULL) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        fclose(srcFile);
        return 0;
    }

    /* Main processing loop - read and process each line */
    while (fgets(line, LINE_SIZE, srcFile) != NULL) {
        lineNum++;
        state = NORMAL_LINE;

        /* Expand content buffer by one line size for each line read */
        contentCapacity += LINE_SIZE;
        newContent = realloc(amContent, contentCapacity);
        if (newContent == NULL) {
            fprintf(stderr, "Error at line %d: Memory allocation failed\n",
                    lineNum);
            errorFlag = TRUE;
            return FALSE; /* Critical error - cannot continue without memory */
        }
        amContent = newContent;

        /* Check for line length errors but continue processing */
        if (strlen(line) >= LINE_SIZE - 1) {
            fprintf(stderr, "Error at line %d: Line too long\n", lineNum);
            errorFlag = TRUE;
        }

        /* Create copy of line for tokenization (strtok destroys original) */
        strncpy(lineCopy, line, LINE_SIZE - 1);
        lineCopy[LINE_SIZE - 1] = '\0';
        arg = strtok(lineCopy, " \t");

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
            /* Add newline for empty lines */
            if (addToContent(&amContent, &contentCapacity, "\n") != TRUE) {
                fprintf(stderr, "Error at line %d: Failed to add content\n",
                        lineNum);
                errorFlag = TRUE;
            }
            break;

        case MACRO_SPREAD:
            /* Expand macro with its arguments */
            if (handleMacroSpread(tempMacro, arg, &amContent,
                                  &contentCapacity) != TRUE) {
                fprintf(stderr, "Error at line %d: Macro expansion failed\n",
                        lineNum);
                errorFlag = TRUE;
            }
            break;

        case MACRO_DEFINE:
            res = 0;
            /* Process macro definition */
            if ((res = handleMacroDefinition(arg, context, srcFile,
                                             &lineNum)) != TRUE) {
                fprintf(stderr, "Error at line %d: Invalid macro definition\n",
                        lineNum);
                errorFlag = TRUE;
                if (res == MEMORY_ALLOCATION_ERROR)
                    return FALSE;
            }
            break;

        case NORMAL_LINE:
            /* Copy normal assembly line as-is */
            if (addToContent(&amContent, &contentCapacity, line) != TRUE) {
                fprintf(stderr, "Error at line %d: Failed to add content\n",
                        lineNum);
                errorFlag = TRUE;
            }
            break;

        default:
            /* Unexpected line type */
            fprintf(stderr, "Error at line %d: Unknown line type\n", lineNum);
            errorFlag = TRUE;
            break;
        }
    }

    /* Create output file only if no errors occurred */
    if (errorFlag == FALSE) {
        res = 0;
        if (createOutputFile(srcFileName, amContent) != TRUE) {
            errorFlag = TRUE;
            fprintf(stderr, "Error: Failed to create output file.\n");
            if (res == MEMORY_ALLOCATION_ERROR)
                return FALSE;
        }
    }

    /* Cleanup resources */
    fclose(srcFile);
    free(amContent);

    return (errorFlag) ? FALSE : TRUE;
}

/*
 * Handle Macro Expansion
 *
 * Expands a macro call by inserting the macro body followed by any
 * arguments passed to the macro.
 *
 * Parameters:
 *   tempMacro       - Pointer to macro structure containing body
 *   firstArg        - First token (macro name) from tokenization
 *   amContent       - Pointer to content buffer
 *   contentCapacity - Pointer to content buffer capacity
 *
 * Returns:
 *   1 on success, 0 on failure
 */
int handleMacroSpread(macro *tempMacro, char *firstArg, char **amContent,
                      size_t *contentCapacity) {
    /* Insert macro body into output */
    if (addToContent(amContent, contentCapacity, (*tempMacro).body) != 1)
        return FALSE;

    /* Continue tokenization from where preAssembler left off */
    /* firstArg contains macro name, get subsequent arguments */
    firstArg = strtok(NULL, " \t");

    /* Add all remaining arguments separated by spaces */
    while (firstArg != NULL) {
        if (addToContent(amContent, contentCapacity, " ") != 1)
            return FALSE;

        if (addToContent(amContent, contentCapacity, firstArg) != 1)
            return FALSE;

        firstArg = strtok(NULL, " \t");
    }

    /* Add newline to complete the expansion */
    if (addToContent(amContent, contentCapacity, "\n") != 1)
        return FALSE;

    return TRUE;
}

/*
 * Handle Macro Definition
 *
 * Processes a macro definition starting with "mcro" and ending with "mcroend",
 * storing the macro name and body in the macro table.
 *
 * Parameters:
 *   firstArg - First token ("mcro") from tokenization
 *   context  - Assembler context containing macro table
 *   srcFile  - Source file handle for reading macro body
 *   lineNum  - Pointer to current line number (updated as lines are read)
 *
 * Returns:
 *   1 on success, 0 on failure
 */
int handleMacroDefinition(char *firstArg, assemblerContext *context,
                          FILE *srcFile, int *lineNum) {
    /* Macro storage variables */
    char *macroLabel;
    char *macroBody;
    char *newBody;

    /* Buffer management variables */
    size_t macroBodyCapacity;
    size_t macroBodySize;
    char macroLine[LINE_SIZE];

    /* Control variable */
    int macroFlag;

    /* Initialize buffer management */
    macroBodyCapacity = LINE_SIZE;
    macroBodySize = 0;
    macroFlag = TRUE; /* Continue reading until "mcroend" found */

    /* Get macro name (next token after "mcro") */
    firstArg = strtok(NULL, " \t");

    /* Validate macro name doesn't conflict with reserved words */
    if (!isValidMacroLabel(firstArg, context)) {
        fprintf(stderr, "Error at line %d: Invalid macro name '%s'\n", *lineNum,
                firstArg);
        return FALSE;
    }

    /* Allocate memory for macro name */
    macroLabel = malloc(strlen(firstArg) + 1);
    if (macroLabel == NULL) {
        fprintf(stderr,
                "Error at line %d: Memory allocation failed for macro label\n",
                *lineNum);
        return MEMORY_ALLOCATION_ERROR;
    }
    strcpy(macroLabel, firstArg);

    /* Allocate initial buffer for macro body */
    macroBody = calloc(macroBodyCapacity, sizeof(char));
    if (macroBody == NULL) {
        fprintf(stderr,
                "Error at line %d: Memory allocation failed for macro body\n",
                *lineNum);
        free(macroLabel);
        return MEMORY_ALLOCATION_ERROR;
    }

    /* Read macro body lines until "mcroend" is found */
    while (macroFlag && fgets(macroLine, LINE_SIZE, srcFile) != NULL) {
        (*lineNum)++;

        /* Expand buffer by one line size for each line read */
        macroBodyCapacity += LINE_SIZE;
        newBody = realloc(macroBody, macroBodyCapacity);
        if (newBody == NULL) {
            fprintf(stderr,
                    "Error at line %d: Memory allocation failed during macro "
                    "expansion\n",
                    *lineNum);
            free(macroLabel);
            free(macroBody);
            return MEMORY_ALLOCATION_ERROR;
        }
        macroBody = newBody;

        /* Check if this line contains "mcroend" */
        firstArg = strtok(macroLine, " \t");
        if (firstArg != NULL && strcmp(firstArg, "mcroend") == 0) {
            macroFlag = FALSE; /* End of macro definition */
        } else {
            /* Add this line to macro body */
            strcat(macroBody, macroLine);
            macroBodySize += strlen(macroLine);
        }
    }

    /* Store completed macro in macro table */
    insertMacro((*context).macroTable, macroLabel, macroBody);

    /* Cleanup allocated memory */
    free(macroLabel);
    free(macroBody);

    return 1;
}

/*
 * Add Content to Buffer
 *
 * Appends text to the content buffer. Buffer expansion is handled
 * externally by the calling function.
 *
 * Parameters:
 *   amContent       - Pointer to content buffer
 *   contentCapacity - Pointer to buffer capacity (unused but kept for consistency)
 *   text            - Text to append to buffer
 *
 * Returns:
 *   1 on success, 0 on failure
 */
int addToContent(char **amContent, size_t *contentCapacity, const char *text) {
    /* Append text to buffer (capacity managed by caller) */
    strcat(*amContent, text);
    return 1;
}

/*
 * Create Output File
 *
 * Creates the .am output file by extracting the prefix from the source
 * filename and writing the processed content.
 *
 * Parameters:
 *   srcFileName - Original source filename
 *   amContent   - Processed content to write
 *
 * Returns:
 *   1 on success, 0 on failure
 */
int createOutputFile(const char *srcFileName, const char *amContent) {
    FILE *amFile;
    char *prefix;
    char *amFileName;
    char *dotPosition;

    /* Find last dot in filename to separate name from extension */
    dotPosition = strrchr(srcFileName, '.');

    if (dotPosition != NULL) {
        /* Extract filename prefix (before the dot) */
        size_t prefixLen = dotPosition - srcFileName;
        prefix = malloc(prefixLen + 1);
        if (prefix == NULL) {
            fprintf(stderr,
                    "Error: Memory allocation failed for filename prefix\n");
            return 0;
        }
        strncpy(prefix, srcFileName, prefixLen);
        prefix[prefixLen] = '\0';
    } else {
        /* No extension - use entire filename as prefix */
        prefix = malloc(strlen(srcFileName) + 1);
        if (prefix == NULL) {
            fprintf(stderr,
                    "Error: Memory allocation failed for filename prefix\n");
            return 0;
        }
        strcpy(prefix, srcFileName);
    }

    /* Create output filename with .am extension */
    amFileName =
        malloc(strlen(prefix) + 4); /* +3 for ".am" +1 for null terminator */
    if (amFileName == NULL) {
        fprintf(stderr,
                "Error: Memory allocation failed for output filename\n");
        free(prefix);
        return 0;
    }
    sprintf(amFileName, "%s.am", prefix);

    /* Create and write to output file */
    amFile = fopen(amFileName, "w");
    if (amFile == NULL) {
        fprintf(stderr, "Error: Cannot create output file '%s'\n", amFileName);
        return 0;
    } else {
        if (fputs(amContent, amFile) == EOF) {
            fprintf(stderr, "Error: Failed to write to output file '%s'\n",
                    amFileName);
            return 0;
        }
        fclose(amFile);
    }

    /* Cleanup allocated memory */
    free(prefix);
    free(amFileName);

    return 1;
}

/*
 * Validate Macro Label
 *
 * Checks if a proposed macro name is valid by ensuring it doesn't
 * conflict with operations, directives, or registers, and meets
 * length requirements.
 *
 * Parameters:
 *   arg     - Proposed macro name to validate
 *   context - Assembler context containing operation, directive, and register tables
 *
 * Returns:
 *   TRUE if valid, FALSE if invalid
 */
int isValidMacroLabel(char *arg, assemblerContext *context) {
    return ((searchOperation(*(context->operationTable), arg) == NULL) &&
            (searchDirective(*(context->directiveTable), arg) == NULL) &&
            (searchRegister(*(context->registers), arg) == NULL) &&
            (strlen(arg) <= MACRO_LABEL_BUFF))
               ? TRUE
               : FALSE;
}