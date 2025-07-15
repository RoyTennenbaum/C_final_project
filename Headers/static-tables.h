#define OP_TABLE_SIZE 16
#define DIR_TABLE_SIZE 5

typedef struct {
    char *name;
    int number;
} instruction,

    // define operation table array
    typedef instruction operation, operationTable[OP_TABLE_SIZE];

// define directive table array
typedef instruction directive, directiveTable[DIR_TABLE_SIZE];

int isInTable(const instruction *table, int tableSize, const char *str);