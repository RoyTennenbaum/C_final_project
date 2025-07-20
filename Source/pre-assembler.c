#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../Headers/pre-assembler.h"
#include "../Headers/dynamic-tables.h"
#include "../Headers/static-tables.h"
#include "../Headers/global.h"

#define LINE_BUFF 83
#define LABEL_BUFF 31

void preAssembler(char *srcFileName, assemblerContext context)
{
    FILE *amFile;
    FILE *srcFile;
    char *amContent = calloc(LINE_BUFF, sizeof(char));
    char *prefix;
    char *amFileName;
    int lineNum = 0;
    char line[LINE_BUFF] = {'\0'};
    char *arg;
    int state = NORMAL_LINE;
    char *macroLabel;
    char *macroBody;
    int macroBodyLine;
    int macroFlag = NO;
    macro *tempMacro;
    int errorFlag = NO;

    srcFile = fopen(srcFileName, "r");

    while (fgets(line, LINE_BUFF, srcFile) != NULL)
    {
        lineNum++;
        int state = NORMAL_LINE;
        if (strlen(line) >= LINE_BUFF - 1)
        {
            errorFlag = YES;
            /*error*/
        }
        amContent = realloc(amContent, lineNum * LINE_BUFF * sizeof(char));
        arg = strtok(line, " ");
        if ((tempMacro = searchMac(context->macroTable, arg)) != NULL)
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
            macroFlag = YES;
            arg = strtok(NULL, " ");

            if (arg == NULL)
            {
                errorFlag = YES;
                /*error*/
                macroFlag = NO;
            }
            else if (!isValidLabel(arg, context))
            {
                errorFlag = YES;
                /*error*/
                macroFlag = NO;
            }
            else
            {
                macroLabel = malloc(strlen(arg) + 1);
                strcpy(macroLabel, arg);
                macroBodyLine = 0;
                macroBody = calloc(LINE_BUFF, sizeof(char));
                while (macroFlag && fgets(line, LINE_BUFF, srcFile) != NULL)
                {
                    lineNum++;
                    macroBodyLine++;
                    if (macroBodyLine > 1)
                        macroBody = realloc(macroBody, macroBodyLine * LINE_BUFF * sizeof(char));
                    arg = strtok(line, " ");
                    if (strcmp(arg, "mcroend") == 0)
                        macroFlag = NO;
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
                insertMac(context->macroTable, macroLabel, macroBody);
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
    if (errorFlag == NO)
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
    /*free macro table with dedecated function*/
}
