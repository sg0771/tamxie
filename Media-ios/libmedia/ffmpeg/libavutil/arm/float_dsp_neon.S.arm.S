#ifdef __arm__
        @.arch armv7-a
        @.fpu neon
        .syntax unified
        .thumb
        .set Lpic_idx, 0
        .set Lpic_gp, 0
        .text
        .align 2
        .globl _ff_vector_fmul_neon

        .thumb_func _ff_vector_fmul_neon
_ff_vector_fmul_neon:
        subs r3, r3, #8
        vld1.32 {d0-d3}, [r1,:128]!
        vld1.32 {d4-d7}, [r2,:128]!
        vmul.f32 q8, q0, q2
        vmul.f32 q9, q1, q3
        beq 3f
        bics ip, r3, #15
        beq 2f
1: subs ip, ip, #16
        vld1.32 {d0-d1}, [r1,:128]!
        vld1.32 {d4-d5}, [r2,:128]!
        vmul.f32 q10, q0, q2
        vld1.32 {d2-d3}, [r1,:128]!
        vld1.32 {d6-d7}, [r2,:128]!
        vmul.f32 q11, q1, q3
        vst1.32 {d16-d19},[r0,:128]!
        vld1.32 {d0-d1}, [r1,:128]!
        vld1.32 {d4-d5}, [r2,:128]!
        vmul.f32 q8, q0, q2
        vld1.32 {d2-d3}, [r1,:128]!
        vld1.32 {d6-d7}, [r2,:128]!
        vmul.f32 q9, q1, q3
        vst1.32 {d20-d23},[r0,:128]!
        bne 1b
        ands r3, r3, #15
        beq 3f
2: vld1.32 {d0-d1}, [r1,:128]!
        vld1.32 {d4-d5}, [r2,:128]!
        vst1.32 {d16-d17},[r0,:128]!
        vmul.f32 q8, q0, q2
        vld1.32 {d2-d3}, [r1,:128]!
        vld1.32 {d6-d7}, [r2,:128]!
        vst1.32 {d18-d19},[r0,:128]!
        vmul.f32 q9, q1, q3
3: vst1.32 {d16-d19},[r0,:128]!
        bx lr

        @.endfunc

        .set Lpic_idx, 0
        .set Lpic_gp, 0
        .text
        .align 2
        .globl _ff_vector_fmac_scalar_neon

        .thumb_func _ff_vector_fmac_scalar_neon
_ff_vector_fmac_scalar_neon:


        len .req r3
        acc .req r2

        vdup.32 q15, r2
        bics r12, len, #15
        mov acc, r0
        beq 3f
        vld1.32 {q0}, [r1,:128]!
        vld1.32 {q8}, [acc,:128]!
        vld1.32 {q1}, [r1,:128]!
        vld1.32 {q9}, [acc,:128]!
1: vmla.f32 q8, q0, q15
        vld1.32 {q2}, [r1,:128]!
        vld1.32 {q10}, [acc,:128]!
        vmla.f32 q9, q1, q15
        vld1.32 {q3}, [r1,:128]!
        vld1.32 {q11}, [acc,:128]!
        vmla.f32 q10, q2, q15
        vst1.32 {q8}, [r0,:128]!
        vmla.f32 q11, q3, q15
        vst1.32 {q9}, [r0,:128]!
        subs r12, r12, #16
        beq 2f
        vld1.32 {q0}, [r1,:128]!
        vld1.32 {q8}, [acc,:128]!
        vst1.32 {q10}, [r0,:128]!
        vld1.32 {q1}, [r1,:128]!
        vld1.32 {q9}, [acc,:128]!
        vst1.32 {q11}, [r0,:128]!
        b 1b
2: vst1.32 {q10}, [r0,:128]!
        vst1.32 {q11}, [r0,:128]!
        ands len, len, #15
        it eq
        bxeq lr
3: vld1.32 {q0}, [r1,:128]!
        vld1.32 {q8}, [acc,:128]!
        vmla.f32 q8, q0, q15
        vst1.32 {q8}, [r0,:128]!
        subs len, len, #4
        bgt 3b
        bx lr
.unreq len
.unreq LEN

        @.endfunc

        .set Lpic_idx, 0
        .set Lpic_gp, 0
        .text
        .align 2
        .globl _ff_vector_fmul_scalar_neon

        .thumb_func _ff_vector_fmul_scalar_neon
_ff_vector_fmul_scalar_neon:

        len .req r3

        vdup.32 q8, r2
        bics r12, len, #15
        beq 3f
        vld1.32 {q0},[r1,:128]!
        vld1.32 {q1},[r1,:128]!
1: vmul.f32 q0, q0, q8
        vld1.32 {q2},[r1,:128]!
        vmul.f32 q1, q1, q8
        vld1.32 {q3},[r1,:128]!
        vmul.f32 q2, q2, q8
        vst1.32 {q0},[r0,:128]!
        vmul.f32 q3, q3, q8
        vst1.32 {q1},[r0,:128]!
        subs r12, r12, #16
        beq 2f
        vld1.32 {q0},[r1,:128]!
        vst1.32 {q2},[r0,:128]!
        vld1.32 {q1},[r1,:128]!
        vst1.32 {q3},[r0,:128]!
        b 1b
2: vst1.32 {q2},[r0,:128]!
        vst1.32 {q3},[r0,:128]!
        ands len, len, #15
        it eq
        bxeq lr
3: vld1.32 {q0},[r1,:128]!
        vmul.f32 q0, q0, q8
        vst1.32 {q0},[r0,:128]!
        subs len, len, #4
        bgt 3b
        bx lr
.unreq len
.unreq LEN

        @.endfunc

        .set Lpic_idx, 0
        .set Lpic_gp, 0
        .text
        .align 2
        .globl _ff_vector_fmul_window_neon

        .thumb_func _ff_vector_fmul_window_neon
_ff_vector_fmul_window_neon:
        push {r4,r5,lr}
        ldr lr, [sp, #12]
        sub r2, r2, #8
        sub r5, lr, #2
        add r2, r2, r5, lsl #2
        add r4, r3, r5, lsl #3
        add ip, r0, r5, lsl #3
        mov r5, #-16
        vld1.32 {d0,d1}, [r1,:128]!
        vld1.32 {d2,d3}, [r2,:128], r5
        vld1.32 {d4,d5}, [r3,:128]!
        vld1.32 {d6,d7}, [r4,:128], r5
1: subs lr, lr, #4
        vmul.f32 d22, d0, d4
        vrev64.32 q3, q3
        vmul.f32 d23, d1, d5
        vrev64.32 q1, q1
        vmul.f32 d20, d0, d7
        vmul.f32 d21, d1, d6
        beq 2f
        vmla.f32 d22, d3, d7
        vld1.32 {d0,d1}, [r1,:128]!
        vmla.f32 d23, d2, d6
        vld1.32 {d18,d19},[r2,:128], r5
        vmls.f32 d20, d3, d4
        vld1.32 {d24,d25},[r3,:128]!
        vmls.f32 d21, d2, d5
        vld1.32 {d6,d7}, [r4,:128], r5
        vmov q1, q9
        vrev64.32 q11, q11
        vmov q2, q12
        vswp d22, d23
        vst1.32 {d20,d21},[r0,:128]!
        vst1.32 {d22,d23},[ip,:128], r5
        b 1b
2: vmla.f32 d22, d3, d7
        vmla.f32 d23, d2, d6
        vmls.f32 d20, d3, d4
        vmls.f32 d21, d2, d5
        vrev64.32 q11, q11
        vswp d22, d23
        vst1.32 {d20,d21},[r0,:128]!
        vst1.32 {d22,d23},[ip,:128], r5
        pop {r4,r5,pc}

        @.endfunc

        .set Lpic_idx, 0
        .set Lpic_gp, 0
        .text
        .align 2
        .globl _ff_vector_fmul_add_neon

        .thumb_func _ff_vector_fmul_add_neon
_ff_vector_fmul_add_neon:
        ldr r12, [sp]
        vld1.32 {q0-q1}, [r1,:128]!
        vld1.32 {q8-q9}, [r2,:128]!
        vld1.32 {q2-q3}, [r3,:128]!
        vmul.f32 q10, q0, q8
        vmul.f32 q11, q1, q9
1: vadd.f32 q12, q2, q10
        vadd.f32 q13, q3, q11
        pld [r1, #16]
        pld [r2, #16]
        pld [r3, #16]
        subs r12, r12, #8
        beq 2f
        vld1.32 {q0}, [r1,:128]!
        vld1.32 {q8}, [r2,:128]!
        vmul.f32 q10, q0, q8
        vld1.32 {q1}, [r1,:128]!
        vld1.32 {q9}, [r2,:128]!
        vmul.f32 q11, q1, q9
        vld1.32 {q2-q3}, [r3,:128]!
        vst1.32 {q12-q13},[r0,:128]!
        b 1b
2: vst1.32 {q12-q13},[r0,:128]!
        bx lr

        @.endfunc

        .set Lpic_idx, 0
        .set Lpic_gp, 0
        .text
        .align 2
        .globl _ff_vector_fmul_reverse_neon

        .thumb_func _ff_vector_fmul_reverse_neon
_ff_vector_fmul_reverse_neon:
        add r2, r2, r3, lsl #2
        sub r2, r2, #32
        mov r12, #-32
        vld1.32 {q0-q1}, [r1,:128]!
        vld1.32 {q2-q3}, [r2,:128], r12
1: pld [r1, #32]
        vrev64.32 q3, q3
        vmul.f32 d16, d0, d7
        vmul.f32 d17, d1, d6
        pld [r2, #-32]
        vrev64.32 q2, q2
        vmul.f32 d18, d2, d5
        vmul.f32 d19, d3, d4
        subs r3, r3, #8
        beq 2f
        vld1.32 {q0-q1}, [r1,:128]!
        vld1.32 {q2-q3}, [r2,:128], r12
        vst1.32 {q8-q9}, [r0,:128]!
        b 1b
2: vst1.32 {q8-q9}, [r0,:128]!
        bx lr

        @.endfunc

        .set Lpic_idx, 0
        .set Lpic_gp, 0
        .text
        .align 2
        .globl _ff_butterflies_float_neon

        .thumb_func _ff_butterflies_float_neon
_ff_butterflies_float_neon:
1: vld1.32 {q0},[r0,:128]
        vld1.32 {q1},[r1,:128]
        vsub.f32 q2, q0, q1
        vadd.f32 q1, q0, q1
        vst1.32 {q2},[r1,:128]!
        vst1.32 {q1},[r0,:128]!
        subs r2, r2, #4
        bgt 1b
        bx lr

        @.endfunc

        .set Lpic_idx, 0
        .set Lpic_gp, 0
        .text
        .align 2
        .globl _ff_scalarproduct_float_neon

        .thumb_func _ff_scalarproduct_float_neon
_ff_scalarproduct_float_neon:
        vmov.f32 q2, #0.0
1: vld1.32 {q0},[r0,:128]!
        vld1.32 {q1},[r1,:128]!
        vmla.f32 q2, q0, q1
        subs r2, r2, #4
        bgt 1b
        vadd.f32 d0, d4, d5
        vpadd.f32 d0, d0, d0
        vmov.32 r0, d0[0]
        bx lr

        @.endfunc
.text
.align 2
.thumb_func _ff_vector_fmul_window_neon
.thumb_func _ff_vector_fmac_scalar_neon
.thumb_func _ff_butterflies_float_neon
.thumb_func _ff_scalarproduct_float_neon
.thumb_func _ff_vector_fmul_scalar_neon
.thumb_func _ff_vector_fmul_add_neon
.thumb_func _ff_vector_fmul_reverse_neon
.thumb_func _ff_vector_fmul_neon
#endif