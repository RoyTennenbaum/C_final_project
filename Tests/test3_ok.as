; String setup macro
mcro SETUP_STRING
        lea TEXT, r0
        mov STR_LEN, LENGTH
        mov #0, CHAR_COUNT
endmcro

; Character checking macro
mcro CHECK_CHAR
        mov TEXT[LENGTH], r1
        cmp r1, TARGET_CHAR
        bne SKIP_COUNT
        inc CHAR_COUNT
SKIP_COUNT:
endmcro

; String processing constants
STR_LEN = 5
TARGET_CHAR = 65

        ; Begin string analysis
        SETUP_STRING
TEXT: .string "HELLO"
LENGTH: .data 0
CHAR_COUNT: .data 0
        
CHECK:  CHECK_CHAR          ; Check current character
.entry CHECK
        dec LENGTH          ; Move to previous character
        cmp LENGTH, #-1     ; Check if finished
RESULT: .string "Count:"
        bne CHECK           ; Continue if not finished
        ; Output final count
        prn RESULT
        prn CHAR_COUNT
        stp
.entry CHAR_COUNT