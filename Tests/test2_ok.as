; Counter initialization macro
mcro INIT_COUNTER
        mov #6, COUNT
        mov #0, STEP_COUNT
mcroend

; Decrement operation macro
mcro DECREMENT_STEP
        sub #2, COUNT
        inc STEP_COUNT
mcroend

; Start counter program
        INIT_COUNTER
COUNT: .data 0
STEP_COUNT: .data 0
; Perform decrement step
LOOP:   DECREMENT_STEP
.entry LOOP  
; Check if counter reached zero
        cmp COUNT, #0
; Continue if not zero
        bne LOOP
END_MSG: .string "Steps taken:"
; Display final results
        prn END_MSG
        prn STEP_COUNT
        stp
.entry STEP_COUNT