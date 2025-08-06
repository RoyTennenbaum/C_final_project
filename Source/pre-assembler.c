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
 *
 * Parameters:
 *   fileName - source file name without extension
 *   context  - assembler context with tables
 *
 * Returns:
 *   TRUE on success, FALSE on failure
 */
int preAssembler(char *fileName, assemblerContext *context)
{
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

    srcFileName = malloc(strlen(fileName) + 4);
    if (srcFileName == NULL)
    {
        fprintf(stderr, "Error: failed to allocate memory\n");
        return FALSE;
    }

    strcpy(srcFileName, fileName);
    strcat(srcFileName, ".as"); /* Make filename.as */

    /* Open source file for reading */
    srcFile = fopen(srcFileName, "r");
    if (srcFile == NULL)
    {
        fprintf(stderr, "Error: cannot open file '%s'\n", srcFileName);
        free(srcFileName);
        return FALSE;
    }

    /* Allocate initial content buffer */
    amContent = calloc(contentCapacity, sizeof(char));
    if (amContent == NULL)
    {
        fprintf(stderr, "Error: failed to allocate memory\n");
        free(srcFileName);
        fclose(srcFile);
        return FALSE;
    }

    /* Main processing loop - read and process each line */
    while (fgets(line, LINE_SIZE, srcFile) != NULL)
    {
        lineNum++;
        state = NORMAL_LINE;

        /* Ensure sufficient buffer capacity for new content */
        contentCapacity += LINE_SIZE;
        newContent = realloc(amContent, contentCapacity);
        if (newContent == NULL)
        {
            fprintf(stderr, "Error at line %d: failed to allocate memory\n", lineNum);
            free(srcFileName);
            free(amContent);
            fclose(srcFile);
            return FALSE;
        }
        amContent = newContent;

        /* Check for line length errors but continue processing */
        if (strlen(line) >= LINE_SIZE - 1)
        {
            fprintf(stderr, "Error at line %d: line is too long\n", lineNum);
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
        switch (state)
        {
        case EMPTY_LINE:
            /* Handle empty or whitespace-only lines */
            res = handleEmptyLine(&amContent, &contentCapacity, lineNum);
            if (res == MEMORY_ALLOCATION_ERROR)
            {
                free(srcFileName);
                free(amContent);
                fclose(srcFile);
                return FALSE;
            }
            else if (res != TRUE)
                errorFlag = TRUE;

            break;

        case MACRO_SPREAD:
            /* Expand macro call with its arguments */
            res = handleMacroSpread(tempMacro, &arg, &amContent, &contentCapacity, lineNum);
            if (res == MEMORY_ALLOCATION_ERROR)
            {
                free(srcFileName);
                free(amContent);
                fclose(srcFile);
                return FALSE;
            }
            else if (res != TRUE)
                errorFlag = TRUE;

            break;

        case MACRO_DEFINE:
            /* Process macro definition from "mcro" to "mcroend" */
            res = handleMacroDefinition(&arg, context, srcFile, &lineNum);
            if (res == MEMORY_ALLOCATION_ERROR)
            {
                free(srcFileName);
                free(amContent);
                fclose(srcFile);
                return FALSE;
            }
            else if (res != TRUE)
                errorFlag = TRUE;

            break;

        case NORMAL_LINE:
            /* Copy regular assembly instruction as-is */
            res = addToContent(&amContent, &contentCapacity, line, lineNum);
            if (res == MEMORY_ALLOCATION_ERROR)
            {
                free(srcFileName);
                free(amContent);
                fclose(srcFile);
                return FALSE;
            }
            else if (res != TRUE)
                errorFlag = TRUE;

            break;

        default:
            /* Unexpected line type */
            fprintf(stderr, "Error at line %d: unknown line type\n", lineNum);
            errorFlag = TRUE;
            break;
        }
    }

    /* Create output .am file only if no syntax errors occurred */
    if (errorFlag == FALSE)
    {
        res = createOutputFile(fileName, amContent);
        if (res == MEMORY_ALLOCATION_ERROR)
        {
            free(srcFileName);
            free(amContent);
            fclose(srcFile);
            return FALSE;
        }
        else if (res != TRUE)
        {
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
int handleEmptyLine(char **amContent, size_t *contentCapacity, int lineNum)
{
    int res;

    res = addToContent(amContent, contentCapacity, "\n", lineNum);
    if (res == MEMORY_ALLOCATION_ERROR)
    {
        fprintf(stderr, "Error at line %d: failed to allocate memory\n", lineNum);
        return MEMORY_ALLOCATION_ERROR;
    }
    else if (res != TRUE)
    {
        fprintf(stderr, "Error at line %d: failed to insert empty line\n", lineNum);
        return FALSE;
    }
    return TRUE;
}

/*
 * Handle Macro Expansion
 *
 * Replaces macro call with macro body + arguments
 */
int handleMacroSpread(macro *tempMacro, char **argP, char **amContent, size_t *contentCapacity, int lineNum)
{
    int res;

    /* Insert macro body into output */
    res = addToContent(amContent, contentCapacity, (*tempMacro).body, lineNum);
    if (res == MEMORY_ALLOCATION_ERROR)
    {
        fprintf(stderr, "Error at line %d: failed to allocate memory\n", lineNum);
        return MEMORY_ALLOCATION_ERROR;
    }
    else if (res != TRUE)
    {
        fprintf(stderr, "Error at line %d: macro expansion failed\n", lineNum);
        return FALSE;
    }

    /* Get macro arguments and add them */
    *argP = strtok(NULL, " \t");

    /* Add each argument with space before it */
    while (*argP != NULL)
    {
        res = addToContent(amContent, contentCapacity, " ", lineNum);
        if (res == MEMORY_ALLOCATION_ERROR)
        {
            fprintf(stderr, "Error at line %d: failed to allocate memory\n", lineNum);
            return MEMORY_ALLOCATION_ERROR;
        }
        else if (res != TRUE)
        {
            fprintf(stderr, "Error at line %d: macro expansion failed\n", lineNum);
            return FALSE;
        }

        res = addToContent(amContent, contentCapacity, *argP, lineNum);
        if (res == MEMORY_ALLOCATION_ERROR)
        {
            fprintf(stderr, "Error at line %d: failed to allocate memory\n", lineNum);
            return MEMORY_ALLOCATION_ERROR;
        }
        else if (res != TRUE)
        {
            fprintf(stderr, "Error at line %d: macro expansion failed\n", lineNum);
            return FALSE;
        }

        *argP = strtok(NULL, " \t");
    }

    /* Add newline to complete the expansion */
    res = addToContent(amContent, contentCapacity, "\n", lineNum);
    if (res == MEMORY_ALLOCATION_ERROR)
    {
        fprintf(stderr, "Error at line %d: failed to allocate memory\n", lineNum);
        return MEMORY_ALLOCATION_ERROR;
    }
    else if (res != TRUE)
    {
        fprintf(stderr, "Error at line %d: macro expansion failed\n", lineNum);
        return FALSE;
    }

    return TRUE;
}

/*
 * Handle Macro Definition
 *
 * Reads macro from "mcro" to "mcroend" and stores it
 */
int handleMacroDefinition(char **argP, assemblerContext *context, FILE *srcFile, int *lineNumP)
{
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
    *argP = strtok(NULL, " \t");

    /* Validate macro name doesn't conflict with reserved words */
    if (!isValidMacroLabel(*argP, context))
    {
        fprintf(stderr, "Error at line %d: invalid macro name '%s'\n", *lineNumP, *argP);
        return FALSE;
    }

    /* Allocate memory for macro name */
    macroLabel = malloc(strlen(*argP) + 1);
    if (macroLabel == NULL)
    {
        fprintf(stderr, "Error at line %d: failed to allocate memory\n", *lineNumP);
        return MEMORY_ALLOCATION_ERROR;
    }
    strcpy(macroLabel, *argP);

    /* Allocate initial buffer for macro body */
    macroBody = calloc(macroBodyCapacity, sizeof(char));
    if (macroBody == NULL)
    {
        fprintf(stderr, "Error at line %d: failed to allocate memory\n", *lineNumP);
        free(macroLabel);
        return MEMORY_ALLOCATION_ERROR;
    }

    /* Read lines until we find "mcroend" */
    while (macroFlag && fgets(macroLine, LINE_SIZE, srcFile) != NULL)
    {
        (*lineNumP)++;

        /* Make room for this line */
        macroBodyCapacity += LINE_SIZE;
        newBody = realloc(macroBody, macroBodyCapacity);
        if (newBody == NULL)
        {
            fprintf(stderr, "Error at line %d: failed to allocate memory\n", *lineNumP);
            free(macroLabel);
            free(macroBody);
            return MEMORY_ALLOCATION_ERROR;
        }
        macroBody = newBody;

        /* Check if line has "mcroend" */
        *argP = strtok(macroLine, " \t");
        if (*argP != NULL && strcmp(*argP, "mcroend") == 0)
        {
            macroFlag = FALSE; /* Done reading macro */
        }
        else
        {
            /* Add line to macro body */
            strcat(macroBody, macroLine);
            macroBodySize += strlen(macroLine);
        }
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
 * Adds text to buffer
 */
int addToContent(char **amContentP, size_t *contentCapacityP, const char *text, int lineNum)
{
    /* Make buffer bigger */
    size_t newCapacity = *contentCapacityP + LINE_SIZE;
    char *newContent = (char *)malloc(newCapacity);

    if (newContent == NULL)
    {
        fprintf(stderr, "Error at line %d: failed to allocate memory\n", lineNum);
        return MEMORY_ALLOCATION_ERROR;
    }

    *amContentP = newContent;
    strcat(newContent, text);
    *contentCapacityP = newCapacity;

    return TRUE;
}

/*
 * Create Output File
 *
 * Creates .am file with processed content
 */
int createOutputFile(const char *fileName, const char *amContent)
{
    FILE *amFile;
    char *amFileName;

    amFileName = malloc(strlen(fileName) + 4); /* +4 for ".am\0" */
    if (amFileName == NULL)
    {
        fprintf(stderr, "Error: failed to allocate memory\n");
        return MEMORY_ALLOCATION_ERROR;
    }

    strcpy(amFileName, fileName);
    strcat(amFileName, ".am"); /* Make filename.am */

    amFile = fopen(amFileName, "w");
    if (amFile == NULL)
    {
        fprintf(stderr, "Error: cannot open file '%s'\n", amFileName);
        free(amFileName);
        return FALSE;
    }

    /* Write content to file */
    fputs(amContent, amFile);
    fclose(amFile);

    /* Free filename */
    free(amFileName);

    return TRUE;
}

/*
 * Validate Macro Label
 *
 * Checks if macro name is valid (not reserved word, not too long)
 */
int isValidMacroLabel(char *arg, assemblerContext *context)
{
    return ((searchOperation(*(context->operationTable), arg) == NULL) &&
            (searchDirective(*(context->directiveTable), arg) == NULL) &&
            (searchRegister(*(context->registers), arg) == NULL) &&
            (strlen(arg) <= MACRO_LABEL_BUFF))
               ? TRUE
               : FALSE;
}