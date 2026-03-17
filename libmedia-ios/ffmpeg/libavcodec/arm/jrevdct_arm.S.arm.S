#ifdef __arm__
        @.arch armv7-a
        @.fpu neon
        .syntax unified
        .thumb
        .set Lpic_idx, 0
        .set Lpic_gp, 0
        .text
        .align 2
        .globl _ff_j_rev_dct_arm

        .thumb_func _ff_j_rev_dct_arm
_ff_j_rev_dct_arm:
        push {r0, r4 - r11, lr}

        mov lr, r0 
        mov r12, #8 

        ldr r11, Lpicoff2
Lpic2:
        add r11, pc
        .set Lpic_idx, Lpic_idx + 1


row_loop:
        ldrsh r0, [lr, # 0] 
        ldrsh r2, [lr, # 2] 

        
        
        ldr r5, [lr, # 0]
        ldr r6, [lr, # 4]
        ldr r3, [lr, # 8]
        ldr r4, [lr, #12]
        orr r3, r3, r4
        orr r3, r3, r6
        orrs r5, r3, r5
        beq end_of_row_loop 
        orrs r3, r3, r2
        beq empty_row

        ldrsh r1, [lr, # 8] 
        ldrsh r4, [lr, # 4] 
        ldrsh r6, [lr, # 6] 

        ldr r3, [r11, #4]
        add r7, r2, r6
        ldr r5, [r11, #36]
        mul r7, r3, r7 
        ldr r3, [r11, #8]
        mla r6, r5, r6, r7 
        add r5, r0, r4 
        mla r2, r3, r2, r7 
        sub r3, r0, r4 

        add r0, r2, r5, lsl #13 
        rsb r2, r2, r5, lsl #13 
        add r4, r6, r3, lsl #13 
        rsb r3, r6, r3, lsl #13 

        push {r0, r2, r3, r4} 

        ldrsh r3, [lr, #10] 
        ldrsh r5, [lr, #12] 
        ldrsh r7, [lr, #14] 

        add r0, r3, r5 
        add r2, r1, r7 
        add r4, r3, r7 
        add r6, r1, r5 
        ldr r9, [r11, #12]
        add r8, r4, r6 
        ldr r10, [r11, #32]
        mul r8, r9, r8 
        ldr r9, [r11, #44]
        mul r2, r10, r2 
        ldr r10, [r11, #40]
        mul r0, r9, r0 
        ldr r9, [r11, #28]
        mla r4, r10, r4, r8 
        ldr r10, [r11, #0]
        mla r6, r9, r6, r8 
        ldr r9, [r11, #20]
        mla r7, r10, r7, r2 
        ldr r10, [r11, #24]
        mla r5, r9, r5, r0 
        ldr r9, [r11, #16]
        mla r3, r10, r3, r0 
        add r7, r7, r4 
        mla r1, r9, r1, r2 
        add r5, r5, r6 
        add r3, r3, r4 
        add r1, r1, r6 

        pop {r0, r2, r4, r6} 
                             

        
        add r8, r0, r1
        add.w r8, r8, #(1<<10)
        mov r8, r8, asr #11
        strh r8, [lr, # 0]

        
        sub r8, r0, r1
        add.w r8, r8, #(1<<10)
        mov r8, r8, asr #11
        strh r8, [lr, #14]

        
        add r8, r6, r3
        add.w r8, r8, #(1<<10)
        mov r8, r8, asr #11
        strh r8, [lr, # 2]

        
        sub r8, r6, r3
        add.w r8, r8, #(1<<10)
        mov r8, r8, asr #11
        strh r8, [lr, #12]

        
        add r8, r4, r5
        add.w r8, r8, #(1<<10)
        mov r8, r8, asr #11
        strh r8, [lr, # 4]

        
        sub r8, r4, r5
        add.w r8, r8, #(1<<10)
        mov r8, r8, asr #11
        strh r8, [lr, #10]

        
        add r8, r2, r7
        add.w r8, r8, #(1<<10)
        mov r8, r8, asr #11
        strh r8, [lr, # 6]

        
        sub r8, r2, r7
        add.w r8, r8, #(1<<10)
        mov r8, r8, asr #11
        strh r8, [lr, # 8]

        
        add lr, lr, #16
        subs r12, r12, #1
.thumb_func row_loop
        bne row_loop
        beq start_column_loop

empty_row:
        ldr r1, [r11, #48]
        mov r0, r0, lsl #2
        and r0, r0, r1
        add r0, r0, r0, lsl #16
        str r0, [lr, # 0]
        str r0, [lr, # 4]
        str r0, [lr, # 8]
        str r0, [lr, #12]

end_of_row_loop:
        
        add lr, lr, #16
        subs r12, r12, #1
.thumb_func row_loop
        bne row_loop

start_column_loop:
        
        pop {lr}
        mov r12, #8
column_loop:
        ldrsh r0, [lr, #( 0*8)] 
        ldrsh r2, [lr, #( 4*8)] 
        ldrsh r4, [lr, #( 8*8)] 
        ldrsh r6, [lr, #(12*8)] 

        ldr r3, [r11, #4]
        add r1, r2, r6
        ldr r5, [r11, #36]
        mul r1, r3, r1 
        ldr r3, [r11, #8]
        mla r6, r5, r6, r1 
        add r5, r0, r4 
        mla r2, r3, r2, r1 
        sub r3, r0, r4 

        add r0, r2, r5, lsl #13 
        rsb r2, r2, r5, lsl #13 
        add r4, r6, r3, lsl #13 
        rsb r6, r6, r3, lsl #13 

        ldrsh r1, [lr, #( 2*8)] 
        ldrsh r3, [lr, #( 6*8)] 
        ldrsh r5, [lr, #(10*8)] 
        ldrsh r7, [lr, #(14*8)] 

        
        orr r9, r1, r3
        orr r10, r5, r7
        orrs r10, r9, r10
        beq empty_odd_column

        push {r0, r2, r4, r6} 

        add r0, r3, r5 
        add r2, r1, r7 
        add r4, r3, r7 
        add r6, r1, r5 
        ldr r9, [r11, #12]
        add r8, r4, r6
        ldr r10, [r11, #32]
        mul r8, r9, r8 
        ldr r9, [r11, #44]
        mul r2, r10, r2 
        ldr r10, [r11, #40]
        mul r0, r9, r0 
        ldr r9, [r11, #28]
        mla r4, r10, r4, r8 
        ldr r10, [r11, #0]
        mla r6, r9, r6, r8 
        ldr r9, [r11, #20]
        mla r7, r10, r7, r2 
        ldr r10, [r11, #24]
        mla r5, r9, r5, r0 
        ldr r9, [r11, #16]
        mla r3, r10, r3, r0 
        add r7, r7, r4 
        mla r1, r9, r1, r2 
        add r5, r5, r6 
        add r3, r3, r4 
        add r1, r1, r6 

        pop {r0, r2, r4, r6} 
                             

        
        add r8, r0, r1
        add.w r8, r8, #(1<<17)
        mov r8, r8, asr #18
        strh r8, [lr, #( 0*8)]

        
        sub r8, r0, r1
        add.w r8, r8, #(1<<17)
        mov r8, r8, asr #18
        strh r8, [lr, #(14*8)]

        
        add r8, r4, r3
        add.w r8, r8, #(1<<17)
        mov r8, r8, asr #18
        strh r8, [lr, #( 2*8)]

        
        sub r8, r4, r3
        add.w r8, r8, #(1<<17)
        mov r8, r8, asr #18
        strh r8, [lr, #(12*8)]

        
        add r8, r6, r5
        add.w r8, r8, #(1<<17)
        mov r8, r8, asr #18
        strh r8, [lr, #( 4*8)]

        
        sub r8, r6, r5
        add.w r8, r8, #(1<<17)
        mov r8, r8, asr #18
        strh r8, [lr, #(10*8)]

        
        add r8, r2, r7
        add.w r8, r8, #(1<<17)
        mov r8, r8, asr #18
        strh r8, [lr, #( 6*8)]

        
        sub r8, r2, r7
        add.w r8, r8, #(1<<17)
        mov r8, r8, asr #18
        strh r8, [lr, #( 8*8)]

        
        add lr, lr, #2
        subs r12, r12, #1
.thumb_func column_loop
        bne column_loop
        beq the_end

empty_odd_column:
        
        
        add.w r0, r0, #(1<<17)
        mov r0, r0, asr #18
        strh r0, [lr, #( 0*8)]
        strh r0, [lr, #(14*8)]

        
        
        add.w r4, r4, #(1<<17)
        mov r4, r4, asr #18
        strh r4, [lr, #( 2*8)]
        strh r4, [lr, #(12*8)]

        
        
        add.w r6, r6, #(1<<17)
        mov r6, r6, asr #18
        strh r6, [lr, #( 4*8)]
        strh r6, [lr, #(10*8)]

        
        
        add.w r2, r2, #(1<<17)
        mov r2, r2, asr #18
        strh r2, [lr, #( 6*8)]
        strh r2, [lr, #( 8*8)]

        
        add lr, lr, #2
        subs r12, r12, #1
.thumb_func column_loop
        bne column_loop

the_end:
        
        pop {r4 - r11, pc}
        .align 2
Lpicoff2: .word const_array-(Lpic2+(8>>1))

        @.endfunc

.const_data
        .align 2
const_array:
        .word 2446
        .word 4433
        .word 6270
        .word 9633
        .word 12299
        .word 16819
        .word 25172
        .word -3196
        .word -7373
        .word -15137
        .word -16069
        .word -20995
        .word 0xFFFF

.text
.align 2
.thumb_func empty_odd_column
.thumb_func end_of_row_loop
.thumb_func _ff_j_rev_dct_arm
.thumb_func empty_row
.thumb_func the_end
.thumb_func start_column_loop
#endif
