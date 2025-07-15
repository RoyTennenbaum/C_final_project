#define OP_TABLE_SIZE 16
#define DIR_TABLE_SIZE 5

typedef struct {
    char *name;
    int number;
} operation, operationTable[OP_TABLE_SIZE];

typedef struct {
    char *name;
    int number;
} directive, directiveTable[DIR_TABLE_SIZE];

int isOperation(const char *str);
int isDirective(const char *str);