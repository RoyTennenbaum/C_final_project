; Position calculation macro
mcro CALC_POSITION
        mov ROW_VAL, INDEX
        mul COLS_NUM, INDEX
        add COL_VAL, INDEX
endmcro

; Element loading macro
mcro LOAD_ELEMENT
        CALC_POSITION
        mov MATRIX[INDEX], VALUE
endmcro

; Matrix dimensions
ROWS_NUM = 2
COLS_NUM = 3

        ; Set matrix coordinates
        mov #1, ROW_VAL
        mov #2, COL_VAL
MATRIX: .data 1, 2, 3, 4, 5, 6
ROW_VAL: .data 0
COL_VAL: .data 0
INDEX: .data 0

        ; Load matrix element
        LOAD_ELEMENT
.entry MATRIX
VALUE: .data 0
        ; Display the loaded value
        prn VALUE
        stp
.entry VALUE