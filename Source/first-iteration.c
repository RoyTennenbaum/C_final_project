#include <stdio.h>
#include "first-iteration.h"

int firstIteration(char *fileName) {
    int IC = 0, DC = 0;

    char *lineStr, *firstStr;
    int lineNum = 0; /* Mark line number for future error messages */
    File *fp = fopen(fileName, "r");
    int scan; /* Scan status */

    if (fp != NULL) {
        printf("Error: file could not be opened.");
        return 1;
    }

    while (fgets(lineStr, LINE_SIZE, fp) != NULL) {
        lineNum++;

        scan = sscanf(lineStr, "%s ", firstStr);
        if (scan != SCAN_OK) {
            printf("Error: scan type mismatch.");
            return 1;
        }

        /* if(isSymbol(symbolTable, firstStr)) */
        /* isOperation(operationTable, firstStr, opNum) */
        /* isDirective(directiveTable, firstStr) */
    }

    fclose(fp);
    return 0;
}