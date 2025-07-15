#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../Headers/pre-assembler.h"
#include "../Headers/dynamic-tables.h"
#include "../Headers/static-tables.h"
isValidLabel(char *arg, macroTable macroTable, operationTable operationTable,
             directiveTable directiveTable);

enum flag
{
    NO,
    YES
};
enum states
{
    MACRO_SPREAD,
    MACRO_DEFINE,
};

#define LINE_BUFF 83
#define LABEL_BUFF 31

void preAssembler(char *srcFileName, operationTable operationTable,
                  directiveTable directiveTable)
{
    FILE *amFile;
    FILE *srcFile;
    char *amContent = calloc(LINE_BUFF, sizeof(char));
    char *prefix;
    char *amFileName;
    int lineNum = 0;
    char line[LINE_BUFF];
    char *arg;
    int state;
    char *macroLabel;
    char *macroBody;
    int macroBodyLine;
    int macroFlag = NO;
    macro *tempMacro;
    macroTable macroTable = malloc(sizeof(macroTable));
    int errorFlag = NO;

    srcFile = fopen(srcFileName, "r");

    while (fgets(line, LINE_BUFF, srcFile) != NULL)
    {
        lineNum++;
        if (strlen(line) >= LINE_BUFF)
        {
            errorFlag = YES;
            /*error*/
        }
        amContent = realloc(amContent, lineNum * LINE_BUFF * sizeof(char));
        arg = strtok(line, " ");
        if ((tempMacro = searchMac(macroTable, arg)) != NULL)
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
            else if (!isValidLabel(arg, macroTable, operationTable, directiveTable))
            {
                errorFlag = YES;
                /*error*/
                macroFlag = NO;
            }
            else
            {
                macroLabel = malloc(sizeof(strlen(arg) + 1));
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
                insertMac(macroTable, macroLabel, macroBody);
                free(macroLabel);
                free(macroBody);
            }
            break;
        default:
            break;
        }
    }
    if (errorFlag == NO)
    {
        sscanf(prefix, "%s.");
        sprintf(amFileName, "%s.am", prefix);
        amFile = fopen(amFileName, "w");
        fputs(amContent, amFile);
        fclose(amFile);
    }
    fclose(srcFile);
    free(amContent);
    /*free macro table with dedecated function*/
}
