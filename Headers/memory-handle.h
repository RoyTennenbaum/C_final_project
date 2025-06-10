typedef struct{
    unsigned short aer_bits :2;
    unsigned short dest_op_bits :2;
    unsigned short src_op_bits :2;
    unsigned short opcode_bits :4;
} cmdFirstWord;

typedef struct{
    unsigned short aer_bits :2;
    unsigned short value_bits :8;
} immediateAddressWord;

typedef struct{
    unsigned short aer_bits :2;
    unsigned short value_bits :8;
} directAddressWord;

typedef struct{
    unsigned short aer_bits :2;
    unsigned short label_address_bits :8;
} matrixAddressFirstWord;

typedef struct{
    unsigned short aer_bits :2;
    unsigned short row_reg_bits :3;
    :1;
    unsigned short col_reg_bits :3;
    :1;
} matrixAddressSecondWord;

typedef struct{
    unsigned short aer_bits :2;
    unsigned short dest_reg_bits :3;
    :1;
    unsigned short src_reg_bits :3;
    :1;
} registerAddressCommonWord;