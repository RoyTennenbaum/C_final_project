#ifndef WORD_TYPES_H
#define WORD_TYPES_H

typedef struct {
    unsigned int aer_bits : 2;
    unsigned int dest_op_bits : 2;
    unsigned int src_op_bits : 2;
    unsigned int opcode_bits : 4;
} cmdFirstWord;

typedef struct {
    unsigned int aer_bits : 2;
    unsigned int value_bits : 8;
} immediateAddressingWord;

typedef struct {
    unsigned int aer_bits : 2;
    unsigned int value_bits : 8;
} directAddressingWord;

typedef struct {
    unsigned int aer_bits : 2;
    unsigned int label_address_bits : 8;
} matrixAddressingFirstWord;

typedef struct {
    unsigned int aer_bits : 2;
    unsigned int col_reg_bits : 3;
    unsigned int unused_padding_1 : 1;
    unsigned int row_reg_bits : 3;
    unsigned int unused_padding_2 : 1;
} matrixAddressingSecondWord;

typedef struct {
    unsigned int aer_bits : 2;
    unsigned int dest_reg_bits : 3;
    unsigned int unused_padding_1 : 1;
    unsigned int src_reg_bits : 3;
    unsigned int unused_padding_2 : 1;
} registerAddressingWord;

#endif