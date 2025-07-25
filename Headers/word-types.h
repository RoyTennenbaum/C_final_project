#ifndef WORD_TYPES_H
#define WORD_TYPES_H

typedef struct
{
    unsigned short aer_bits : 2;
    unsigned short dest_op_bits : 2;
    unsigned short src_op_bits : 2;
    unsigned short opcode_bits : 4;
} cmdFirstWord;

typedef struct
{
    unsigned short aer_bits : 2;
    unsigned short value_bits : 8;
} immediateAddressingWord;

typedef struct
{
    unsigned short aer_bits : 2;
    unsigned short value_bits : 8;
} directAddressingWord;

typedef struct
{
    unsigned short aer_bits : 2;
    unsigned short label_address_bits : 8;
} matrixAddressingFirstWord;

typedef struct
{
    unsigned short aer_bits : 2;
    unsigned short col_reg_bits : 3;
    :1;
    unsigned short row_reg_bits : 3;
    :1;
} matrixAddressingSecondWord;

typedef struct
{
    unsigned short aer_bits : 2;
    unsigned short dest_reg_bits : 3;
    :1;
    unsigned short src_reg_bits : 3;
    :1;
} registerAddressingWord;

#endif