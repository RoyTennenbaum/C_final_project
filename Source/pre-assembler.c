#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../Headers/global.h"
#include "../Headers/pre-assembler.h"
#include "../Headers/dynamic-tables.h"
#include "../Headers/static-tables.h"

char *preAssembler(char *srcFileName, assemblerContext context)
{
    FILE *amFile;
    FILE *srcFile;
    char *amContent = calloc(LINE_SIZE, sizeof(char));
    char *prefix;
    char *amFileName;
    int lineNum = 0;
    char line[LINE_SIZE] = {'\0'};
    char *arg;
    int state = NORMAL_LINE;
    char *macroLabel;
    char *macroBody;
    int macroBodyLine;
    int macroFlag = FALSE;
    macro *tempMacro;
    int errorFlag = FALSE;

    srcFile = fopen(srcFileName, "r");

    while (fgets(line, LINE_SIZE, srcFile) != NULL)
    {
        lineNum++;
        int state = NORMAL_LINE;
        if (strlen(line) >= LINE_SIZE - 1)
        {
            errorFlag = TRUE;
            /*error*/
        }
        amContent = realloc(amContent, lineNum * LINE_SIZE * sizeof(char));
        arg = strtok(line, " ");
        if ((tempMacro = searchMac(context.macroTable, arg)) != NULL)
            state = MACRO_SPREAD;
        else if (strcmp(arg, "mcro") == 0)
            state = MACRO_DEFINE;
        switch (state)
        {
        case MACRO_SPREAD:
            strcat(amContent, (*tempMacro).body);
            arg = strtok(NULL, " ");
            while (arg != NULL)
            {
                strcat(amContent, " ");
                strcat(amContent, arg);
                arg = strtok(NULL, " ");
            }
            strcat(amContent, "\n");
            break;

        case MACRO_DEFINE:
            macroFlag = TRUE;
            arg = strtok(NULL, " ");

            if (arg == NULL)
            {
                errorFlag = TRUE;
                /*error*/
                macroFlag = FALSE;
            }
            else if (!isValidMacroLabel(arg, context))
            {
                errorFlag = TRUE;
                /*error*/
                macroFlag = FALSE;
            }
            else
            {
                macroLabel = malloc(strlen(arg) + 1);
                strcpy(macroLabel, arg);
                macroBodyLine = 0;
                macroBody = calloc(LINE_SIZE, sizeof(char));
                while (macroFlag && fgets(line, LINE_SIZE, srcFile) != NULL)
                {
                    lineNum++;
                    macroBodyLine++;
                    if (macroBodyLine > 1)
                        macroBody = realloc(macroBody, macroBodyLine * LINE_SIZE * sizeof(char));
                    arg = strtok(line, " ");
                    if (strcmp(arg, "mcroend") == 0)
                        macroFlag = FALSE;
                    else
                    {
                        while (arg != NULL)
                        {
                            strcat(macroBody, arg);
                            strcat(macroBody, " ");
                            arg = strtok(NULL, " ");
                        }
                        strcat(macroBody, "\n");
                    }
                }
                insertMacro(context.macroTable, macroLabel, macroBody);
                free(macroLabel);
                free(macroBody);
            }
            break;
        case NORMAL_LINE:
        default:
            while (arg != NULL)
            {
                strcat(amContent, arg);
                strcat(amContent, " ");
                arg = strtok(NULL, " ");
            }
            strcat(amContent, "\n");
            break;
        }
    }
    if (errorFlag == FALSE)
    {
        prefix = malloc(strlen(srcFileName) - 2);
        sscanf(srcFileName, "%[^.]", prefix);
        amFileName = malloc(strlen(srcFileName) + 1);
        sprintf(amFileName, "%s.am", prefix);
        amFile = fopen(amFileName, "w");
        fputs(amContent, amFile);
        fclose(amFile);
    }
    fclose(srcFile);
    free(amContent);
    return (errorFlag) ? NULL : amFileName;
}

int isValidMacroLabel(char *arg, assemblerContext context)
{
    return ((!isOperation(arg)) && (!isDirective(arg)) && (strlen(arg) <= MACRO_LABEL_BUFF)) ? TRUE : FALSE;
}
