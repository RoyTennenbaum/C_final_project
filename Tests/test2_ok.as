; Counter initialization macro
mcro INIT_COUNTER
        mov MAX_VAL, COUNT
        mov #0, STEP_COUNT
endmcro

; Decrement operation macro
mcro DECREMENT_STEP
        sub STEP_SIZE, COUNT
        inc STEP_COUNT
endmcro

; Configuration constants
MAX_VAL = 6
STEP_SIZE = 2

        ; Start counter program
        INIT_COUNTER
COUNT: .data 0
STEP_COUNT: .data 0
LOOP:   DECREMENT_STEP      ; Perform decrement step
.entry LOOP  
        cmp COUNT, #0        ; Check if counter reached zero
        bne LOOP            ; Continue if not zero
END_MSG: .string "Steps taken:"
        ; Display final results
        prn END_MSG
        prn STEP_COUNT
        stp
.entry STEP_COUNT