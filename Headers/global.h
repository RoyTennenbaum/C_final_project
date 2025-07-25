#ifndef GLOBAL_H
#define GLOBAL_H

enum { FALSE, TRUE };
enum { SUCCESS, ERROR };
#define LINE_SIZE 83

typedef struct {
    symbolTable *symbolTable;
    macroTable *macroTable;
    const operationTable *operationTable;
    const directiveTable *directiveTable;
    const registers *registers;
} assemblerContext;

/* Function prototypes */
int isKeyword(assemblerContext context);
int isNewSymbol(char *str, assemblerContext context);

#endif /* GLOBAL_H */