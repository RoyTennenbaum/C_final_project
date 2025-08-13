; String setup macro
mcro SETUP_STRING
        lea TEXT, r0
        mov #5, LENGTH
        mov #0, CHAR_COUNT
mcroend

; Character checking macro
mcro CHECK_CHAR
        mov TEXT[LENGTH], r1
        cmp r1, #65
        bne SKIP_COUNT
        inc CHAR_COUNT
SKIP_COUNT:
mcroend

; Begin string analysis
        SETUP_STRING
TEXT: .string "HELLO"
LENGTH: .data 0
CHAR_COUNT: .data 0
        
; Check current character
CHECK:  CHECK_CHAR
.entry CHECK
; Move to previous character
        dec LENGTH
; Check if finished
        cmp LENGTH, #-1
RESULT: .string "Count:"
; Continue if not finished
        bne CHECK
; Output final count
        prn RESULT
        prn CHAR_COUNT
        stp
.entry CHAR_COUNT