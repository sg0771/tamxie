#ifdef __arm__
        @.arch armv7-a

        @.fpu neon        .syntax unified
        .thumb
RESULT .req a1
BUF .req a1
SIZE .req a2
PATTERN .req a3
PTR .req a4
DAT0 .req v1
DAT1 .req v2
DAT2 .req v3
DAT3 .req v4
TMP0 .req v5
TMP1 .req v6
TMP2 .req ip
TMP3 .req lr


        .set Lpic_idx, 0
        .set Lpic_gp, 0
        .text
        .align 2
        .globl _ff_startcode_find_candidate_armv6

        .thumb_func _ff_startcode_find_candidate_armv6
_ff_startcode_find_candidate_armv6:
        push {v1-v6,lr}
        mov PTR, BUF
        
        
        cmp SIZE, #(4 +3)*32 - 1
        blo 60f

        
        tst PTR, #3
        beq 2f
1: ldrb DAT0, [PTR], #1
        sub SIZE, SIZE, #1
        teq DAT0, #0
        beq 90f
        tst PTR, #3
        bne 1b
2: 
         ldr PATTERN, Literal_0
        setend be
        tst PTR, #12
        beq 4f
3:        ldr DAT0, [PTR], #4
        subs SIZE, SIZE, #4 
        sub TMP0, DAT0, PATTERN, lsr #14
        bic TMP0, TMP0, DAT0
        ands TMP0, TMP0, PATTERN
        bne 91f
        tst PTR, #12
        bne 3b
4: 
        tst PTR, #16
        beq 5f
        ldmia PTR!, {DAT0,DAT1,DAT2,DAT3}
        subs SIZE, SIZE, #16 
        sub TMP0, DAT0, PATTERN, lsr #14
        sub TMP1, DAT1, PATTERN, lsr #14
        bic TMP0, TMP0, DAT0
        bic TMP1, TMP1, DAT1
        sub TMP2, DAT2, PATTERN, lsr #14
        sub TMP3, DAT3, PATTERN, lsr #14
        ands TMP0, TMP0, PATTERN
        bic TMP2, TMP2, DAT2
        it eq
        andseq TMP1, TMP1, PATTERN
        bic TMP3, TMP3, DAT3
        itt eq
        andseq TMP2, TMP2, PATTERN
        andseq TMP3, TMP3, PATTERN
        bne 93f
5: 
        
        
        sub SIZE, SIZE, #(4 +2)*32
6:        ldmia PTR!, {DAT0,DAT1,DAT2,DAT3}
        pld [PTR, #4*32]
        sub TMP0, DAT0, PATTERN, lsr #14
        sub TMP1, DAT1, PATTERN, lsr #14
        bic TMP0, TMP0, DAT0
        bic TMP1, TMP1, DAT1
        sub TMP2, DAT2, PATTERN, lsr #14
        sub TMP3, DAT3, PATTERN, lsr #14
        ands TMP0, TMP0, PATTERN
        bic TMP2, TMP2, DAT2
        it eq
        andseq TMP1, TMP1, PATTERN
        bic TMP3, TMP3, DAT3
        itt eq
        andseq TMP2, TMP2, PATTERN
        andseq TMP3, TMP3, PATTERN
        bne 93f
        ldmia PTR!, {DAT0,DAT1,DAT2,DAT3}
        subs SIZE, SIZE, #32 
        sub TMP0, DAT0, PATTERN, lsr #14
        sub TMP1, DAT1, PATTERN, lsr #14
        bic TMP0, TMP0, DAT0
        bic TMP1, TMP1, DAT1
        sub TMP2, DAT2, PATTERN, lsr #14
        sub TMP3, DAT3, PATTERN, lsr #14
        ands TMP0, TMP0, PATTERN
        bic TMP2, TMP2, DAT2
        it eq
        andseq TMP1, TMP1, PATTERN
        bic TMP3, TMP3, DAT3
        itt eq
        andseq TMP2, TMP2, PATTERN
        andseq TMP3, TMP3, PATTERN
        bne 93f
        bcs 6b
        
        tst SIZE, #31
        beq 7f
        pld [PTR, #(4 +1)*32]
        
        
7: adds SIZE, SIZE, #(4 +2)*32 - 16
        bmi 9f
8:        ldmia PTR!, {DAT0,DAT1,DAT2,DAT3}
        subs SIZE, SIZE, #16 
        sub TMP0, DAT0, PATTERN, lsr #14
        sub TMP1, DAT1, PATTERN, lsr #14
        bic TMP0, TMP0, DAT0
        bic TMP1, TMP1, DAT1
        sub TMP2, DAT2, PATTERN, lsr #14
        sub TMP3, DAT3, PATTERN, lsr #14
        ands TMP0, TMP0, PATTERN
        bic TMP2, TMP2, DAT2
        it eq
        andseq TMP1, TMP1, PATTERN
        bic TMP3, TMP3, DAT3
        itt eq
        andseq TMP2, TMP2, PATTERN
        andseq TMP3, TMP3, PATTERN
        bne 93f
        bcs 8b
        
9: adds SIZE, SIZE, #16 - 4
        bmi 11f
10:        ldr DAT0, [PTR], #4
        subs SIZE, SIZE, #4 
        sub TMP0, DAT0, PATTERN, lsr #14
        bic TMP0, TMP0, DAT0
        ands TMP0, TMP0, PATTERN
        bne 91f
        bcs 10b
11: setend le
        
        ldrb DAT0, [PTR, #-1]
        teq DAT0, #0
        beq 90f
        
        tst SIZE, #3
        beq 13f
12: ldrb DAT0, [PTR], #1
        sub SIZE, SIZE, #1
        teq DAT0, #0
        beq 90f
        tst SIZE, #3
        bne 12b
        
13: sub RESULT, PTR, BUF
        b 99f

60: 
        subs SIZE, SIZE, #1
        bcc 99f
61: ldrb DAT0, [PTR], #1
        subs SIZE, SIZE, #1
        teq DAT0, #0
        beq 90f
        bcs 61b
        
        sub RESULT, PTR, BUF
        b 99f

90: 
        sub RESULT, PTR, BUF
        sub RESULT, RESULT, #1
        b 99f

91: 
        sub RESULT, PTR, BUF
        sub RESULT, RESULT, #4
        sub TMP0, DAT0, #0x20000
        bics TMP0, TMP0, DAT0
        itt pl
        ldrbpl DAT0, [PTR, #-3]
        addpl RESULT, RESULT, #2
        bpl 92f
        teq RESULT, #0
        beq 98f 
        ldrb DAT0, [PTR, #-5]
92: teq DAT0, #0
        it eq
        subeq RESULT, RESULT, #1
        b 98f

93: 
        sub RESULT, PTR, BUF
        sub RESULT, RESULT, #16
        teq TMP0, #0
        beq 95f 
        sub TMP0, DAT0, #0x20000
        bics TMP0, TMP0, DAT0
        itt pl
        ldrbpl DAT0, [PTR, #-15]
        addpl RESULT, RESULT, #2
        bpl 94f
        teq RESULT, #0
        beq 98f 
        ldrb DAT0, [PTR, #-17]
94: teq DAT0, #0
        it eq
        subeq RESULT, RESULT, #1
        b 98f
95: add RESULT, RESULT, #4
        teq TMP1, #0
        beq 96f 
        sub TMP1, DAT1, #0x20000
        bics TMP1, TMP1, DAT1
        itee mi
        ldrbmi DAT0, [PTR, #-13]
        ldrbpl DAT0, [PTR, #-11]
        addpl RESULT, RESULT, #2
        teq DAT0, #0
        it eq
        subeq RESULT, RESULT, #1
        b 98f
96: add RESULT, RESULT, #4
        teq TMP2, #0
        beq 97f 
        sub TMP2, DAT2, #0x20000
        bics TMP2, TMP2, DAT2
        itee mi
        ldrbmi DAT0, [PTR, #-9]
        ldrbpl DAT0, [PTR, #-7]
        addpl RESULT, RESULT, #2
        teq DAT0, #0
        it eq
        subeq RESULT, RESULT, #1
        b 98f
97: add RESULT, RESULT, #4
        sub TMP3, DAT3, #0x20000
        bics TMP3, TMP3, DAT3
        itee mi
        ldrbmi DAT0, [PTR, #-5]
        ldrbpl DAT0, [PTR, #-3]
        addpl RESULT, RESULT, #2
        teq DAT0, #0
        it eq
        subeq RESULT, RESULT, #1
        
98: setend le
99: pop {v1-v6,pc}

        @.endfunc

.unreq result
.unreq RESULT
.unreq buf
.unreq BUF
.unreq size
.unreq SIZE
.unreq pattern
.unreq PATTERN
.unreq ptr
.unreq PTR
.unreq dat0
.unreq DAT0
.unreq dat1
.unreq DAT1
.unreq dat2
.unreq DAT2
.unreq dat3
.unreq DAT3
.unreq tmp0
.unreq TMP0
.unreq tmp1
.unreq TMP1
.unreq tmp2
.unreq TMP2
.unreq tmp3
.unreq TMP3
.text
.align 2
Literal_0:
 .word 0x80008000
.thumb_func _ff_startcode_find_candidate_armv6

#endif
