#ifndef STATIC_TABLES_H
#define STATIC_TABLES_H

#define OP_TABLE_SIZE 16
#define DIR_TABLE_SIZE 5
#define REGISTER_NUM 8

typedef enum { DATA, STRING, MAT, ENTRY, EXTERN } directiveType;

typedef struct {
    const char *name;
    int number;
} operation;

typedef struct {
    const char *name;
    directiveType type;
} directive;

typedef struct {
    const char *name;
    int number;
} registerInfo;

typedef operation operationTable[OP_TABLE_SIZE];
typedef directive directiveTable[DIR_TABLE_SIZE];
typedef registerInfo registers[REGISTER_NUM];

/* Function declarations */
const operation *searchOperation(const operationTable opTable, const char *str);
const directive *searchDirective(const directiveTable dirTable,
                                 const char *str);

#endif