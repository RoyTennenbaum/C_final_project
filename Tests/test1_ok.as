; Array search macro definitions
mcro LOAD_ARRAY
        lea ARRAY, r0
        mov #5, r1
endmcro

mcro PRINT_RESULT
        prn RESULT_MSG
        prn FOUND
endmcro

; Initialize array processing
        LOAD_ARRAY
ARRAY: .data 10, 20, 30, 15, 25
        mov #20, r2
.entry SEARCH
; Load element at index 1
SEARCH: mov ARRAY[#1], r3
; Compare with target
        cmp r3, r2
FOUND: .data 0
; Branch if not equal
        bne NOT_MATCH
; Set found flag
        mov #1, FOUND
.entry FOUND