#ifdef __arm__
        @.arch armv7-a
        @.fpu neon
        .syntax unified
        .thumb
        .set Lpic_idx, 0
        .set Lpic_gp, 0
        .text
        .align 2
        .globl _ff_dct_unquantize_h263_inter_neon

        .thumb_func _ff_dct_unquantize_h263_inter_neon
_ff_dct_unquantize_h263_inter_neon:
        add r12, r0, #0x10
        ldr r12, [r12, r2, lsl #2]
        add r0, r0, #0x88
        ldrb r12, [r0, r12]
        sub r2, r3, #1
        lsl r0, r3, #1
        orr r2, r2, #1
        add r3, r12, #1

        @.endfunc

        .set Lpic_idx, 0
        .set Lpic_gp, 0
        .text
        .align 2
        .globl _ff_dct_unquantize_h263_neon

        .thumb_func _ff_dct_unquantize_h263_neon
_ff_dct_unquantize_h263_neon:
        vdup.16 q15, r0 
        vdup.16 q14, r2 
        vneg.s16 q13, q14
        cmp r3, #4
        mov r0, r1
        ble 2f
1:
        vld1.16 {q0}, [r0,:128]!
        vclt.s16 q3, q0, #0
        vld1.16 {q8}, [r0,:128]!
        vceq.s16 q1, q0, #0
        vmul.s16 q2, q0, q15
        vclt.s16 q11, q8, #0
        vmul.s16 q10, q8, q15
        vbsl q3, q13, q14
        vbsl q11, q13, q14
        vadd.s16 q2, q2, q3
        vceq.s16 q9, q8, #0
        vadd.s16 q10, q10, q11
        vbif q0, q2, q1
        vbif q8, q10, q9
        subs r3, r3, #16
        vst1.16 {q0}, [r1,:128]!
        vst1.16 {q8}, [r1,:128]!
        it le
        bxle lr
        cmp r3, #8
        bgt 1b
2:
        vld1.16 {d0}, [r0,:64]
        vclt.s16 d3, d0, #0
        vceq.s16 d1, d0, #0
        vmul.s16 d2, d0, d30
        vbsl d3, d26, d28
        vadd.s16 d2, d2, d3
        vbif d0, d2, d1
        vst1.16 {d0}, [r1,:64]
        bx lr

        @.endfunc

        .set Lpic_idx, 0
        .set Lpic_gp, 0
        .text
        .align 2
        .globl _ff_dct_unquantize_h263_intra_neon

        .thumb_func _ff_dct_unquantize_h263_intra_neon
_ff_dct_unquantize_h263_intra_neon:
        push {r4-r6,lr}
        add r12, r0, #0x10
        ldr r6, [r0, #0x0c]
        add lr, r0, #0x88
        cmp r6, #0
        it ne
        movne r12, #63
        bne 1f
        ldr r12, [r12, r2, lsl #2]
        ldrb r12, [lr, r12]
1: ldr r5, [r0, #0x40]
        ldrsh r4, [r1]
        cmp r5, #0
        mov r5, r1
        it ne
        movne r2, #0
        bne 2f
        cmp r2, #4
        it ge
        addge r0, r0, #4
        sub r2, r3, #1
        ldr r6, [r0, #0x04]
        orr r2, r2, #1
        smulbb r4, r4, r6
2: lsl r0, r3, #1
        add r3, r12, #1
.thumb_func _ff_dct_unquantize_h263_neon
        bl _ff_dct_unquantize_h263_neon
        vmov.16 d0[0], r4
        vst1.16 {d0[0]}, [r5]
        pop {r4-r6,pc}

        @.endfunc
.text
.align 2
.thumb_func _ff_dct_unquantize_h263_inter_neon
.thumb_func _ff_dct_unquantize_h263_intra_neon
.thumb_func _ff_dct_unquantize_h263_neon
#endif
