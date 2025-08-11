#include <stdlib.h>
#include <string.h>
#include "../Headers/dir-handlers.h"

void handleDirective(const directive *dir, int *DC, char *symbolName,
                     binaryWordList *dirList, assemblerContext *context) {
    /*directiveWord dirWord;*/

    switch ((*dir).type) {
    case DATA:
        if (symbolName != NULL) {
            insertSymbol((*context).symbolTable, symbolName, *DC, TYPE_DATA);
        }
        /*
        while ((arg = strtok(NULL, " \t")) != NULL) {
            if (isNum(arg)) {
                dirWord.data_bits = arg;
                //improve insertLineData to handle DC AND IC...
                //insertLineData(codeImage, )
            } else {
                printf("ERROR: arg is not a number");
            }
        }
        */
        /* Code into "word-type" memory */
        /* encodeDataDir()*/
        /* Update DC accordingly */
        break;

    case STRING:
        if (symbolName != NULL) {
            insertSymbol((*context).symbolTable, symbolName, *DC, TYPE_DATA);
        }
        /* Code into "word-type" memory */
        /* Update DC accordingly */
        break;

    case MAT:
        if (symbolName != NULL) {
            insertSymbol((*context).symbolTable, symbolName, *DC, TYPE_DATA);
        }
        /* Code into "word-type" memory */
        /* Update DC accordingly */
        break;

    case ENTRY:
        /* handled in second iteration */
        break;

    case EXTERN:
        insertSymbol((*context).symbolTable, symbolName, 0, TYPE_EXTERNAL);
        break;

    default:
        /* ERROR */
        break;
    }
}