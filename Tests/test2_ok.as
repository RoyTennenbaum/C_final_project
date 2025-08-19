; Factorial initialization macro
mcro INIT_FACTORIAL
        mov #4, N
        mov #1, RESULT
mcroend

; addition macro
mcro ADD
        mov RESULT, TEMP
        add RESULT, TEMP
        dec N
mcroend

; wrong factorial calculation:
        INIT_FACTORIAL
N: .data 0
RESULT: .data 0
TEMP: .data 0

        

; Return to factorial check
        jmp RESULT
.entry RESULT