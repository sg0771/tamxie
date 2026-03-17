#ifdef __arm__
        @.arch armv7-a
        @.fpu neon
        .syntax unified
        .thumb
        .set Lpic_idx, 0
        .set Lpic_gp, 0
        .text
        .align 2
        .globl _ff_simple_idct_arm

        .thumb_func _ff_simple_idct_arm
_ff_simple_idct_arm:

        stmfd sp!, {r4-r11, r14}
        add r14, r0, #112
        sub sp, sp, #8 
        str r0, [sp, #0] 

__row_loop:
        
        ldr r1, [r14, #0] 
        ldr r2, [r14, #4] 
        ldr r3, [r14, #8] 
        ldr r4, [r14, #12] 
        
        
        
        
        
        orr r5, r4, r3 
        orr r5, r5, r2 
        orrs r6, r5, r1 
        beq __end_row_loop
        mov r7, r1, asr #16 
        ldrsh r6, [r14, #0] 
        orrs r5, r5, r7 
        beq __almost_empty_row


        
        
        
        

        
        
        
        
        
        
        
        
         ldr r8, Literal_0
        mov r2, r2, asr #16 
        mul r0, r8, r7 
         ldr r9, Literal_1
         ldr r10, Literal_2
        mul r1, r9, r7 
         ldr r11, Literal_3
        mul r5, r10, r7 
        mul r7, r11, r7 
        teq r2, #0 
        itttt ne
        mlane r0, r9, r2, r0 
        rsbne r2, r2, #0 
        mlane r1, r11, r2, r1 
        mlane r5, r8, r2, r5 
        it ne
        mlane r7, r10, r2, r7 

        
        
        
        
        
        orrs r2, r3, r4 
        beq __end_b_evaluation

        
        
        
        
        
        
        
        
        
        
        
        mov r3, r3, asr #16 
        teq r3, #0 
        it ne
        mlane r0, r10, r3, r0 
        mov r4, r4, asr #16 
        itttt ne
        mlane r5, r11, r3, r5 
        mlane r7, r9, r3, r7 
        rsbne r3, r3, #0 
        mlane r1, r8, r3, r1 
        
        teq r4, #0 
        itttt ne
        mlane r0, r11, r4, r0 
        mlane r5, r9, r4, r5 
        rsbne r4, r4, #0 
        mlane r7, r8, r4, r7 
        it ne
        mlane r1, r10, r4, r1 
        
__end_b_evaluation:
        
        
        


        
        
        
        
        
         ldr r9, Literal_4
        mul r6, r9, r6 
         ldr r10, Literal_5
        ldrsh r4, [r14, #4] 
        add.w r6, r6, #1024 

        mul r11, r10, r4 
         ldr r8, Literal_6
        sub r3, r6, r11 
        
        
        teq r2, #0
        beq __end_bef_a_evaluation

        add r2, r6, r11 
        mul r11, r8, r4 
        sub r4, r6, r11 
        add r6, r6, r11 


        
        
        


        
        
        
        
        ldrsh r11, [r14, #8] 
        teq r11, #0 
        it ne
        mulne r11, r9, r11 
        
        ldrsh r9, [r14, #12] 
        itttt ne
        addne r6, r6, r11 
        subne r2, r2, r11 
        subne r3, r3, r11 
        addne r4, r4, r11 
        
        teq r9, #0 
        itttt ne
        mulne r11, r10, r9 
        addne r6, r6, r11 
        mulne r10, r8, r9 
        
        
        
        
        subne r4, r4, r11 
        itt ne
        subne r2, r2, r10 
        addne r3, r3, r10 

__end_a_evaluation:
        
        
        
        
        
        
        
        
        
        
        
        add r8, r6, r0 
        add r9, r2, r1 
        
        
         ldr r10, Literal_7
        and r9, r10, r9, lsl #(16-11) 
        mvn r11, r10 
        and r8, r11, r8, asr #11 
        orr r8, r8, r9
        str r8, [r14, #0]

        add r8, r3, r5 
        add r9, r4, r7 
        and r9, r10, r9, lsl #(16-11) 
        and r8, r11, r8, asr #11 
        orr r8, r8, r9
        str r8, [r14, #4]

        sub r8, r4, r7 
        sub r9, r3, r5 
        and r9, r10, r9, lsl #(16-11) 
        and r8, r11, r8, asr #11 
        orr r8, r8, r9
        str r8, [r14, #8]

        sub r8, r2, r1 
        sub r9, r6, r0 
        and r9, r10, r9, lsl #(16-11) 
        and r8, r11, r8, asr #11 
        orr r8, r8, r9
        str r8, [r14, #12]

        bal __end_row_loop

__almost_empty_row:
        
        
        
        
        mov r8, #0x10000 
        sub r8, r8, #1 
        and r5, r8, r6, lsl #3 
        orr r5, r5, r5, lsl #16 
        str r5, [r14, #0] 
        str r5, [r14, #4] 
        str r5, [r14, #8] 
        str r5, [r14, #12] 

__end_row_loop:
        
        
        ldr r0, [sp, #0] 
        teq r0, r14 
        sub r14, r14, #16
.thumb_func __row_loop
        bne __row_loop        
        
        add r14, r0, #14 
__col_loop:


        
        
        
        
        
        
        
        
        
        
        
         ldr r8, Literal_0
        ldrsh r7, [r14, #16]
        mul r0, r8, r7 
         ldr r9, Literal_1
         ldr r10, Literal_2
        mul r1, r9, r7 
         ldr r11, Literal_3
        mul r5, r10, r7 
        ldrsh r2, [r14, #48]
        mul r7, r11, r7 
        teq r2, #0 
        itttt ne
        mlane r0, r9, r2, r0 
        rsbne r2, r2, #0 
        mlane r1, r11, r2, r1 
        mlane r5, r8, r2, r5 
        it ne
        mlane r7, r10, r2, r7 

        
        
        
        
        
        
        
        
        
        
        
        ldrsh r3, [r14, #80] 
        teq r3, #0 
        itttt ne
        mlane r0, r10, r3, r0 
        mlane r5, r11, r3, r5 
        mlane r7, r9, r3, r7 
        rsbne r3, r3, #0 
        ldrsh r4, [r14, #112] 
        it ne
        mlane r1, r8, r3, r1 
        
        teq r4, #0 
        itttt ne
        mlane r0, r11, r4, r0 
        mlane r5, r9, r4, r5 
        rsbne r4, r4, #0 
        mlane r7, r8, r4, r7 
        it ne
        mlane r1, r10, r4, r1 
        

        
        
        


        
        
        
        
        
        ldrsh r6, [r14, #0]
         ldr r9, Literal_4
        mul r6, r9, r6 
         ldr r10, Literal_5
        ldrsh r4, [r14, #32] 
        add.w r6, r6, #524288 
        mul r11, r10, r4 
         ldr r8, Literal_6
        add r2, r6, r11 
        sub r3, r6, r11 
        mul r11, r8, r4 
        sub r4, r6, r11 
        add r6, r6, r11 

        
        
        
        
        
        
        
        ldrsh r11, [r14, #64] 
        teq r11, #0 
        itttt ne
        mulne r11, r9, r11 
        
        addne r6, r6, r11 
        subne r2, r2, r11 
        subne r3, r3, r11 
        ldrsh r9, [r14, #96] 
        it ne
        addne r4, r4, r11 
        
        teq r9, #0 
        itttt ne
        mulne r11, r10, r9 
        addne r6, r6, r11 
        mulne r10, r8, r9 
        
        
        
        
        subne r4, r4, r11 
        itt ne
        subne r2, r2, r10 
        addne r3, r3, r10 

        
        
        
        
        
        
        
        
        
        
        
        
        add r8, r6, r0 
        add r9, r2, r1 
        mov r8, r8, asr #20
        mov r9, r9, asr #20
        strh r8, [r14, #0]
        strh r9, [r14, #16]
        add r8, r3, r5 
        add r9, r4, r7 
        mov r8, r8, asr #20
        mov r9, r9, asr #20
        strh r8, [r14, #32]
        strh r9, [r14, #48]
        sub r8, r4, r7 
        sub r9, r3, r5 
        mov r8, r8, asr #20
        mov r9, r9, asr #20
        strh r8, [r14, #64]
        strh r9, [r14, #80]
        sub r8, r2, r1 
        sub r9, r6, r0 
        mov r8, r8, asr #20
        mov r9, r9, asr #20
        strh r8, [r14, #96]
        strh r9, [r14, #112]


        
        
        ldr r0, [sp, #0] 
        teq r0, r14 
        sub r14, r14, #2
.thumb_func __col_loop
        bne __col_loop

        
        add sp, sp, #8 
        ldmfd sp!, {r4-r11, r15} 
__end_bef_a_evaluation:
        add r2, r6, r11 
        mul r11, r8, r4 
        sub r4, r6, r11 
        add r6, r6, r11 
.thumb_func __end_a_evaluation
        bal __end_a_evaluation

        @.endfunc
.text
.align 2
Literal_7:
 .word 0xFFFF0000 
Literal_5:
 .word 8867 
Literal_2:
 .word 12873 
Literal_0:
 .word 22725 
Literal_4:
 .word 16383 
Literal_6:
 .word 21407 
Literal_3:
 .word 4520 
Literal_1:
 .word 19266 
.thumb_func _ff_simple_idct_arm
.thumb_func __almost_empty_row
.thumb_func __end_row_loop
.thumb_func __end_b_evaluation
.thumb_func __end_bef_a_evaluation
#endif
