; Position calculation macro
mcro CALC_POSITION
        mov ROW_VAL, INDEX
        mul #3, INDEX
        add COL_VAL, INDEX
mcroend

; Element loading macro
mcro LOAD_ELEMENT
        mov MATRIX[r1][r2], VALUE
mcroend

; Set matrix coordinates
        mov #1, ROW_VAL
        mov #2, COL_VAL
MATRIX: .mat [2][3] 1, 2, 3, 4, 5, 6
ROW_VAL: .data 3
COL_VAL: .data 3
INDEX: .data 0

; Load matrix element
        LOAD_ELEMENT
.entry MATRIX
VALUE: .data 0
; Display the loaded value
        prn VALUE
        stop
.entry VALUE