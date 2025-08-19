; Array search macro definitions
mcro LOAD_ARRAY
        lea ARRAY, r0
        mov #5, r9
mcroend

mcro PRINT_RESULT
        prn RESULT_MSG
        prn                                                                                   FOUND
mcroend

; Initialize array processing
        LOAD_ARRAY
ARRAY: .data 10, 20, 30, 15, 25,
        mov #2000, r2
.entry SEARCH_ALOT
; Load element at index 1
SEARCH: mov [#1]]]]], r3
; Compare with target
        cmp r3, r2
FOUND: .data 0
; Branch if not equal
 PRINT_RESULT
; Set found flag
        mov #1, #2
.entry FOUND