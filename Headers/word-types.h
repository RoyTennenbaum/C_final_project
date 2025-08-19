#ifndef WORD_TYPES_H
#define WORD_TYPES_H

/* Properties to differentiate between different word categories */
typedef enum { DIR, OP_FIRST, PAYLOAD, REG_PAIR } wordKind;

/* Format of every directive word */
typedef struct {
    unsigned int data_bits : 10;
} directiveWord;

/* Format of the first word of an operation sentence */
typedef struct {
    unsigned int aer_bits : 2;
    unsigned int dest_op_bits : 2;
    unsigned int src_op_bits : 2;
    unsigned int opcode_bits : 4;
} opFirstWord;

/* Word format used for:
 * - Immediate addressing: payload stores a signed 8-bit constant.
 * - Direct addressing: payload stores the address of a label.
 * - Matrix first word: payload stores the address of a matrix label.
 */
typedef struct {
    unsigned int aer_bits : 2;
    unsigned int payload_bits : 8;
} PayloadWord;

/* Word format used for:
 * - Matrix second word: reg1 represents matrix rows, reg2 for columns.
 * - Register addressing: reg1 is a source register and res2 is a destination register.
 */
typedef struct {
    unsigned int aer_bits : 2;
    unsigned int reg2_bits : 3;
    unsigned int padding : 1;
    unsigned int reg1_bits : 3;
    unsigned int more_padding : 1;
} registerPairWord;

typedef union {
    directiveWord dir;
    opFirstWord opFirst;
    PayloadWord payload;
    registerPairWord regPair;
} WordType;

#endif