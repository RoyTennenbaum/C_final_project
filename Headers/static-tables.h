#define OP_TABLE_SIZE 16
#define DIR_TABLE_SIZE 5
#define REGISTER_NUM 8

typedef struct {
    char *name;
    int number;
} operation, operationTable[OP_TABLE_SIZE];

typedef char *directive;
typedef directive directiveTable[DIR_TABLE_SIZE];

typedef struct {
    char *name;
    int number;
} registerInfo, registers[REGISTER_NUM];

operation searchOperation(const char *str);
directive searchDirective(const char *str);
int isOperation(const char *str);
int isDirective(const char *str);