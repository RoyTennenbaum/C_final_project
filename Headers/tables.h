#include "list-data-structure.h"

/* operations table - array */
typedef struct{
    char *name;
    int number;
} operation;

/* Symbol/label table - linked list */
typedef struct{
    char *label;
    int address;
} symbol;

typedef struct{
    
} symbolTable;
