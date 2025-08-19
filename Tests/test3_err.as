; String setup macro
mcro SETUP_STRING
        lea TEXT, r0
        mov #5, LENGTH
        mov #0, CHAR_COUNT
mcroend

; Character checking macro
mcro CHECK_CHAR
        mov TEXT[LENGTH], r1
        cmp r1, aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
        bne SKIP_COUNT
        inc CHAR_COUNT
SKIP_COUNT:
mcroend

; Begin string analysis
        SETUP_STRING
TEXT: .string "HELLO"
LENGTH: .data 0
LENGTH: .data 3
CHAR_COUNT: .data 0,a,a,a,41
        
; Check current character
CHECK:  cmp #90, r6
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
        stopppppprightnow
.entry CHAR_COUNT