; Array search macro definitions
mcro LOAD_ARRAY
        lea ARRAY, r0
        mov SIZE, r1
endmcro

mcro PRINT_RESULT
        prn RESULT_MSG
        prn FOUND
endmcro

; Constants for array processing
SIZE = 5
TARGET = 20

        ; Initialize array processing
        LOAD_ARRAY
ARRAY: .data 10, 20, 30, 15, 25
        mov #TARGET, r2
.entry SEARCH
SEARCH: mov ARRAY[#1], r3  ; Load element at index 1
        cmp r3, r2           ; Compare with target
FOUND: .data 0
        bne NOT_MATCH        ; Branch if not equal
        mov #1, FOUND        ; Set found flag
.entry FOUND
NOT_MATCH: PRINT_RESULT     ; Display results
        stp
RESULT_MSG: .string "Found:"