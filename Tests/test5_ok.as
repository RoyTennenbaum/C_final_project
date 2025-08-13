; Factorial initialization macro
mcro INIT_FACTORIAL
        mov INPUT_NUM, N
        mov START_VAL, RESULT
endmcro

; Multiplication through addition macro
mcro MULTIPLY_ADD
        mov RESULT, TEMP
        mov N, TIMES
        add RESULT, TEMP
        dec N
endmcro

; Factorial configuration
INPUT_NUM = 4
START_VAL = 1

        ; Initialize factorial calculation
        INIT_FACTORIAL
N: .data 0
RESULT: .data 0
TEMP: .data 0
TIMES: .data 0

FACT:   cmp N, #1           ; Check if N equals 1
.entry FACT
        bne MULTIPLY        ; Continue multiplication if N > 1
        prn RESULT          ; Print final result
        stp
        
        ; Multiplication step
MULTIPLY: MULTIPLY_ADD      ; Perform multiplication step
        jmp FACT            ; Return to factorial check
.entry RESULT