; Factorial initialization macro
mcro INIT_FACTORIAL
        mov #4, N
        mov #1, RESULT
endmcro

; Multiplication through addition macro
mcro MULTIPLY_ADD
        mov RESULT, TEMP
        add RESULT, TEMP
        dec N
endmcro

; Initialize factorial calculation
        INIT_FACTORIAL
N: .data 0
RESULT: .data 0
TEMP: .data 0

; Check if N equals 1
FACT:   cmp N, #1
.entry FACT
; Continue multiplication if N > 1
        bne MULTIPLY
; Print final result
        prn RESULT
        stp
        
; Multiplication step
MULTIPLY: MULTIPLY_ADD
; Return to factorial check
        jmp FACT
.entry RESULT