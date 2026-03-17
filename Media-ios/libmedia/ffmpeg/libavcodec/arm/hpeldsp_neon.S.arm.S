#ifdef __arm__
        @.arch armv7-a

        @.fpu neon        .syntax unified
        .thumb


        .set Lpic_idx, 0
        .set Lpic_gp, 0
        .text
        .align 2
        .globl _ff_put_h264_qpel16_mc00_neon

        .thumb_func _ff_put_h264_qpel16_mc00_neon
_ff_put_h264_qpel16_mc00_neon:
        mov r3, #16

        @.endfunc

        .set Lpic_idx, 0
        .set Lpic_gp, 0
        .text
        .align 2
        .globl _ff_put_pixels16_neon

        .thumb_func _ff_put_pixels16_neon
_ff_put_pixels16_neon:
1: vld1.8 {q0}, [r1], r2
        vld1.8 {q1}, [r1], r2
        vld1.8 {q2}, [r1], r2
        pld [r1, r2, lsl #2]
        vld1.8 {q3}, [r1], r2
        pld [r1]
        pld [r1, r2]
        pld [r1, r2, lsl #1]
        subs r3, r3, #4
        vst1.64 {q0}, [r0,:128], r2
        vst1.64 {q1}, [r0,:128], r2
        vst1.64 {q2}, [r0,:128], r2
        vst1.64 {q3}, [r0,:128], r2
        bne 1b
        bx lr

        @.endfunc
        .set Lpic_idx, 0
        .set Lpic_gp, 0
        .text
        .align 2
        .globl _ff_put_pixels16_x2_neon

        .thumb_func _ff_put_pixels16_x2_neon
_ff_put_pixels16_x2_neon:
1: vld1.8 {d0-d2}, [r1], r2
        vld1.8 {d4-d6}, [r1], r2
        pld [r1]
        pld [r1, r2]
        subs r3, r3, #2
        vext.8 q1, q0, q1, #1
        vrhadd.u8 q0, q0, q1
        vext.8 q3, q2, q3, #1
        vrhadd.u8 q2, q2, q3
        vst1.8 {q0}, [r0,:128], r2
        vst1.8 {q2}, [r0,:128], r2
        bne 1b
        bx lr

        @.endfunc
        .set Lpic_idx, 0
        .set Lpic_gp, 0
        .text
        .align 2
        .globl _ff_put_pixels16_x2_no_rnd_neon

        .thumb_func _ff_put_pixels16_x2_no_rnd_neon
_ff_put_pixels16_x2_no_rnd_neon:
1: vld1.8 {d0-d2}, [r1], r2
        vld1.8 {d4-d6}, [r1], r2
        pld [r1]
        pld [r1, r2]
        subs r3, r3, #2
        vext.8 q1, q0, q1, #1
        vhadd.u8 q0, q0, q1
        vext.8 q3, q2, q3, #1
        vhadd.u8 q2, q2, q3
        vst1.8 {q0}, [r0,:128], r2
        vst1.8 {q2}, [r0,:128], r2
        bne 1b
        bx lr

        @.endfunc
        .set Lpic_idx, 0
        .set Lpic_gp, 0
        .text
        .align 2
        .globl _ff_put_pixels16_y2_neon

        .thumb_func _ff_put_pixels16_y2_neon
_ff_put_pixels16_y2_neon:
        sub r3, r3, #2
        vld1.8 {q0}, [r1], r2
        vld1.8 {q1}, [r1], r2
1: subs r3, r3, #2
        vrhadd.u8 q2, q0, q1
        vld1.8 {q0}, [r1], r2
        vrhadd.u8 q3, q0, q1
        vld1.8 {q1}, [r1], r2
        pld [r1]
        pld [r1, r2]
        vst1.8 {q2}, [r0,:128], r2
        vst1.8 {q3}, [r0,:128], r2
        bne 1b

        vrhadd.u8 q2, q0, q1
        vld1.8 {q0}, [r1], r2
        vrhadd.u8 q3, q0, q1
        vst1.8 {q2}, [r0,:128], r2
        vst1.8 {q3}, [r0,:128], r2

        bx lr

        @.endfunc
        .set Lpic_idx, 0
        .set Lpic_gp, 0
        .text
        .align 2
        .globl _ff_put_pixels16_y2_no_rnd_neon

        .thumb_func _ff_put_pixels16_y2_no_rnd_neon
_ff_put_pixels16_y2_no_rnd_neon:
        sub r3, r3, #2
        vld1.8 {q0}, [r1], r2
        vld1.8 {q1}, [r1], r2
1: subs r3, r3, #2
        vhadd.u8 q2, q0, q1
        vld1.8 {q0}, [r1], r2
        vhadd.u8 q3, q0, q1
        vld1.8 {q1}, [r1], r2
        pld [r1]
        pld [r1, r2]
        vst1.8 {q2}, [r0,:128], r2
        vst1.8 {q3}, [r0,:128], r2
        bne 1b

        vhadd.u8 q2, q0, q1
        vld1.8 {q0}, [r1], r2
        vhadd.u8 q3, q0, q1
        vst1.8 {q2}, [r0,:128], r2
        vst1.8 {q3}, [r0,:128], r2

        bx lr

        @.endfunc
        .set Lpic_idx, 0
        .set Lpic_gp, 0
        .text
        .align 2
        .globl _ff_put_pixels16_xy2_neon

        .thumb_func _ff_put_pixels16_xy2_neon
_ff_put_pixels16_xy2_neon:
        sub r3, r3, #2
        vld1.8 {d0-d2}, [r1], r2
        vld1.8 {d4-d6}, [r1], r2
        pld [r1]
        pld [r1, r2]
        vext.8 q1, q0, q1, #1
        vext.8 q3, q2, q3, #1
        vaddl.u8 q8, d0, d2
        vaddl.u8 q10, d1, d3
        vaddl.u8 q9, d4, d6
        vaddl.u8 q11, d5, d7
1: subs r3, r3, #2
        vld1.8 {d0-d2}, [r1], r2
        vadd.u16 q12, q8, q9
        pld [r1]
        vext.8 q15, q0, q1, #1
        vadd.u16 q1 , q10, q11
        vrshrn.u16 d28, q12, #2
        vrshrn.u16 d29, q1, #2
        vaddl.u8 q8, d0, d30
        vld1.8 {d2-d4}, [r1], r2
        vaddl.u8 q10, d1, d31
        vst1.8 {q14}, [r0,:128], r2
        vadd.u16 q12, q8, q9
        pld [r1, r2]
        vext.8 q2, q1, q2, #1
        vadd.u16 q0, q10, q11
        vrshrn.u16 d30, q12, #2
        vrshrn.u16 d31, q0, #2
        vaddl.u8 q9, d2, d4
        vaddl.u8 q11, d3, d5
        vst1.8 {q15}, [r0,:128], r2
        bgt 1b

        vld1.8 {d0-d2}, [r1], r2
        vadd.u16 q12, q8, q9
        vext.8 q15, q0, q1, #1
        vadd.u16 q1 , q10, q11
        vrshrn.u16 d28, q12, #2
        vrshrn.u16 d29, q1, #2
        vaddl.u8 q8, d0, d30
        vaddl.u8 q10, d1, d31
        vst1.8 {q14}, [r0,:128], r2
        vadd.u16 q12, q8, q9
        vadd.u16 q0, q10, q11
        vrshrn.u16 d30, q12, #2
        vrshrn.u16 d31, q0, #2
        vst1.8 {q15}, [r0,:128], r2

        bx lr

        @.endfunc
        .set Lpic_idx, 0
        .set Lpic_gp, 0
        .text
        .align 2
        .globl _ff_put_pixels16_xy2_no_rnd_neon

        .thumb_func _ff_put_pixels16_xy2_no_rnd_neon
_ff_put_pixels16_xy2_no_rnd_neon:
        sub r3, r3, #2
        vld1.8 {d0-d2}, [r1], r2
        vld1.8 {d4-d6}, [r1], r2
        vmov.i16  q13, #1
        pld [r1]
        pld [r1, r2]
        vext.8 q1, q0, q1, #1
        vext.8 q3, q2, q3, #1
        vaddl.u8 q8, d0, d2
        vaddl.u8 q10, d1, d3
        vaddl.u8 q9, d4, d6
        vaddl.u8 q11, d5, d7
1: subs r3, r3, #2
        vld1.8 {d0-d2}, [r1], r2
        vadd.u16 q12, q8, q9
        pld [r1]
        vadd.u16  q12, q12, q13
        vext.8 q15, q0, q1, #1
        vadd.u16 q1 , q10, q11
        vshrn.u16 d28, q12, #2
        vadd.u16  q1, q1, q13
        vshrn.u16 d29, q1, #2
        vaddl.u8 q8, d0, d30
        vld1.8 {d2-d4}, [r1], r2
        vaddl.u8 q10, d1, d31
        vst1.8 {q14}, [r0,:128], r2
        vadd.u16 q12, q8, q9
        pld [r1, r2]
        vadd.u16  q12, q12, q13
        vext.8 q2, q1, q2, #1
        vadd.u16 q0, q10, q11
        vshrn.u16 d30, q12, #2
        vadd.u16  q0, q0, q13
        vshrn.u16 d31, q0, #2
        vaddl.u8 q9, d2, d4
        vaddl.u8 q11, d3, d5
        vst1.8 {q15}, [r0,:128], r2
        bgt 1b

        vld1.8 {d0-d2}, [r1], r2
        vadd.u16 q12, q8, q9
        vadd.u16  q12, q12, q13
        vext.8 q15, q0, q1, #1
        vadd.u16 q1 , q10, q11
        vshrn.u16 d28, q12, #2
        vadd.u16  q1, q1, q13
        vshrn.u16 d29, q1, #2
        vaddl.u8 q8, d0, d30
        vaddl.u8 q10, d1, d31
        vst1.8 {q14}, [r0,:128], r2
        vadd.u16 q12, q8, q9
        vadd.u16  q12, q12, q13
        vadd.u16 q0, q10, q11
        vshrn.u16 d30, q12, #2
        vadd.u16  q0, q0, q13
        vshrn.u16 d31, q0, #2
        vst1.8 {q15}, [r0,:128], r2

        bx lr

        @.endfunc

        .set Lpic_idx, 0
        .set Lpic_gp, 0
        .text
        .align 2
        .globl _ff_avg_h264_qpel16_mc00_neon

        .thumb_func _ff_avg_h264_qpel16_mc00_neon
_ff_avg_h264_qpel16_mc00_neon:
        mov r3, #16

        @.endfunc

        .set Lpic_idx, 0
        .set Lpic_gp, 0
        .text
        .align 2
        .globl _ff_avg_pixels16_neon

        .thumb_func _ff_avg_pixels16_neon
_ff_avg_pixels16_neon:
        mov r12, r0
1: vld1.8 {q0}, [r1], r2
        vld1.8 {q1}, [r1], r2
        vld1.8 {q2}, [r1], r2
        pld [r1, r2, lsl #2]
        vld1.8 {q3}, [r1], r2
        pld [r1]
        pld [r1, r2]
        pld [r1, r2, lsl #1]
        vld1.8 {q8}, [r12,:128], r2
        vrhadd.u8 q0, q0, q8
        vld1.8 {q9}, [r12,:128], r2
        vrhadd.u8 q1, q1, q9
        vld1.8 {q10}, [r12,:128], r2
        vrhadd.u8 q2, q2, q10
        vld1.8 {q11}, [r12,:128], r2
        vrhadd.u8 q3, q3, q11
        subs r3, r3, #4
        vst1.64 {q0}, [r0,:128], r2
        vst1.64 {q1}, [r0,:128], r2
        vst1.64 {q2}, [r0,:128], r2
        vst1.64 {q3}, [r0,:128], r2
        bne 1b
        bx lr

        @.endfunc
        .set Lpic_idx, 0
        .set Lpic_gp, 0
        .text
        .align 2
        .globl _ff_avg_pixels16_x2_neon

        .thumb_func _ff_avg_pixels16_x2_neon
_ff_avg_pixels16_x2_neon:
1: vld1.8 {d0-d2}, [r1], r2
        vld1.8 {d4-d6}, [r1], r2
        pld [r1]
        pld [r1, r2]
        subs r3, r3, #2
        vext.8 q1, q0, q1, #1
        vrhadd.u8 q0, q0, q1
        vext.8 q3, q2, q3, #1
        vrhadd.u8 q2, q2, q3
        vld1.8 {q1}, [r0,:128], r2
        vld1.8 {q3}, [r0,:128]
        vrhadd.u8 q0, q0, q1
        vrhadd.u8 q2, q2, q3
        sub r0, r0, r2
        vst1.8 {q0}, [r0,:128], r2
        vst1.8 {q2}, [r0,:128], r2
        bne 1b
        bx lr

        @.endfunc
        .set Lpic_idx, 0
        .set Lpic_gp, 0
        .text
        .align 2
        .globl _ff_avg_pixels16_x2_no_rnd_neon

        .thumb_func _ff_avg_pixels16_x2_no_rnd_neon
_ff_avg_pixels16_x2_no_rnd_neon:
1: vld1.8 {d0-d2}, [r1], r2
        vld1.8 {d4-d6}, [r1], r2
        pld [r1]
        pld [r1, r2]
        subs r3, r3, #2
        vext.8 q1, q0, q1, #1
        vhadd.u8 q0, q0, q1
        vext.8 q3, q2, q3, #1
        vhadd.u8 q2, q2, q3
        vld1.8 {q1}, [r0,:128], r2
        vld1.8 {q3}, [r0,:128]
        vrhadd.u8 q0, q0, q1
        vrhadd.u8 q2, q2, q3
        sub r0, r0, r2
        vst1.8 {q0}, [r0,:128], r2
        vst1.8 {q2}, [r0,:128], r2
        bne 1b
        bx lr

        @.endfunc
        .set Lpic_idx, 0
        .set Lpic_gp, 0
        .text
        .align 2
        .globl _ff_avg_pixels16_y2_neon

        .thumb_func _ff_avg_pixels16_y2_neon
_ff_avg_pixels16_y2_neon:
        sub r3, r3, #2
        vld1.8 {q0}, [r1], r2
        vld1.8 {q1}, [r1], r2
1: subs r3, r3, #2
        vrhadd.u8 q2, q0, q1
        vld1.8 {q0}, [r1], r2
        vrhadd.u8 q3, q0, q1
        vld1.8 {q1}, [r1], r2
        pld [r1]
        pld [r1, r2]
        vld1.8 {q8}, [r0,:128], r2
        vld1.8 {q9}, [r0,:128]
        vrhadd.u8 q2, q2, q8
        vrhadd.u8 q3, q3, q9
        sub r0, r0, r2
        vst1.8 {q2}, [r0,:128], r2
        vst1.8 {q3}, [r0,:128], r2
        bne 1b

        vrhadd.u8 q2, q0, q1
        vld1.8 {q0}, [r1], r2
        vrhadd.u8 q3, q0, q1
        vld1.8 {q8}, [r0,:128], r2
        vld1.8 {q9}, [r0,:128]
        vrhadd.u8 q2, q2, q8
        vrhadd.u8 q3, q3, q9
        sub r0, r0, r2
        vst1.8 {q2}, [r0,:128], r2
        vst1.8 {q3}, [r0,:128], r2

        bx lr

        @.endfunc
        .set Lpic_idx, 0
        .set Lpic_gp, 0
        .text
        .align 2
        .globl _ff_avg_pixels16_y2_no_rnd_neon

        .thumb_func _ff_avg_pixels16_y2_no_rnd_neon
_ff_avg_pixels16_y2_no_rnd_neon:
        sub r3, r3, #2
        vld1.8 {q0}, [r1], r2
        vld1.8 {q1}, [r1], r2
1: subs r3, r3, #2
        vhadd.u8 q2, q0, q1
        vld1.8 {q0}, [r1], r2
        vhadd.u8 q3, q0, q1
        vld1.8 {q1}, [r1], r2
        pld [r1]
        pld [r1, r2]
        vld1.8 {q8}, [r0,:128], r2
        vld1.8 {q9}, [r0,:128]
        vrhadd.u8 q2, q2, q8
        vrhadd.u8 q3, q3, q9
        sub r0, r0, r2
        vst1.8 {q2}, [r0,:128], r2
        vst1.8 {q3}, [r0,:128], r2
        bne 1b

        vhadd.u8 q2, q0, q1
        vld1.8 {q0}, [r1], r2
        vhadd.u8 q3, q0, q1
        vld1.8 {q8}, [r0,:128], r2
        vld1.8 {q9}, [r0,:128]
        vrhadd.u8 q2, q2, q8
        vrhadd.u8 q3, q3, q9
        sub r0, r0, r2
        vst1.8 {q2}, [r0,:128], r2
        vst1.8 {q3}, [r0,:128], r2

        bx lr

        @.endfunc
        .set Lpic_idx, 0
        .set Lpic_gp, 0
        .text
        .align 2
        .globl _ff_avg_pixels16_xy2_neon

        .thumb_func _ff_avg_pixels16_xy2_neon
_ff_avg_pixels16_xy2_neon:
        sub r3, r3, #2
        vld1.8 {d0-d2}, [r1], r2
        vld1.8 {d4-d6}, [r1], r2
        pld [r1]
        pld [r1, r2]
        vext.8 q1, q0, q1, #1
        vext.8 q3, q2, q3, #1
        vaddl.u8 q8, d0, d2
        vaddl.u8 q10, d1, d3
        vaddl.u8 q9, d4, d6
        vaddl.u8 q11, d5, d7
1: subs r3, r3, #2
        vld1.8 {d0-d2}, [r1], r2
        vadd.u16 q12, q8, q9
        pld [r1]
        vext.8 q15, q0, q1, #1
        vadd.u16 q1 , q10, q11
        vrshrn.u16 d28, q12, #2
        vrshrn.u16 d29, q1, #2
        vld1.8 {q8}, [r0,:128]
        vrhadd.u8 q14, q14, q8
        vaddl.u8 q8, d0, d30
        vld1.8 {d2-d4}, [r1], r2
        vaddl.u8 q10, d1, d31
        vst1.8 {q14}, [r0,:128], r2
        vadd.u16 q12, q8, q9
        pld [r1, r2]
        vext.8 q2, q1, q2, #1
        vadd.u16 q0, q10, q11
        vrshrn.u16 d30, q12, #2
        vrshrn.u16 d31, q0, #2
        vld1.8 {q9}, [r0,:128]
        vrhadd.u8 q15, q15, q9
        vaddl.u8 q9, d2, d4
        vaddl.u8 q11, d3, d5
        vst1.8 {q15}, [r0,:128], r2
        bgt 1b

        vld1.8 {d0-d2}, [r1], r2
        vadd.u16 q12, q8, q9
        vext.8 q15, q0, q1, #1
        vadd.u16 q1 , q10, q11
        vrshrn.u16 d28, q12, #2
        vrshrn.u16 d29, q1, #2
        vld1.8 {q8}, [r0,:128]
        vrhadd.u8 q14, q14, q8
        vaddl.u8 q8, d0, d30
        vaddl.u8 q10, d1, d31
        vst1.8 {q14}, [r0,:128], r2
        vadd.u16 q12, q8, q9
        vadd.u16 q0, q10, q11
        vrshrn.u16 d30, q12, #2
        vrshrn.u16 d31, q0, #2
        vld1.8 {q9}, [r0,:128]
        vrhadd.u8 q15, q15, q9
        vst1.8 {q15}, [r0,:128], r2

        bx lr

        @.endfunc
        .set Lpic_idx, 0
        .set Lpic_gp, 0
        .text
        .align 2
        .globl _ff_avg_pixels16_xy2_no_rnd_neon

        .thumb_func _ff_avg_pixels16_xy2_no_rnd_neon
_ff_avg_pixels16_xy2_no_rnd_neon:
        sub r3, r3, #2
        vld1.8 {d0-d2}, [r1], r2
        vld1.8 {d4-d6}, [r1], r2
        vmov.i16  q13, #1
        pld [r1]
        pld [r1, r2]
        vext.8 q1, q0, q1, #1
        vext.8 q3, q2, q3, #1
        vaddl.u8 q8, d0, d2
        vaddl.u8 q10, d1, d3
        vaddl.u8 q9, d4, d6
        vaddl.u8 q11, d5, d7
1: subs r3, r3, #2
        vld1.8 {d0-d2}, [r1], r2
        vadd.u16 q12, q8, q9
        pld [r1]
        vadd.u16  q12, q12, q13
        vext.8 q15, q0, q1, #1
        vadd.u16 q1 , q10, q11
        vshrn.u16 d28, q12, #2
        vadd.u16  q1, q1, q13
        vshrn.u16 d29, q1, #2
        vld1.8 {q8}, [r0,:128]
        vrhadd.u8 q14, q14, q8
        vaddl.u8 q8, d0, d30
        vld1.8 {d2-d4}, [r1], r2
        vaddl.u8 q10, d1, d31
        vst1.8 {q14}, [r0,:128], r2
        vadd.u16 q12, q8, q9
        pld [r1, r2]
        vadd.u16  q12, q12, q13
        vext.8 q2, q1, q2, #1
        vadd.u16 q0, q10, q11
        vshrn.u16 d30, q12, #2
        vadd.u16  q0, q0, q13
        vshrn.u16 d31, q0, #2
        vld1.8 {q9}, [r0,:128]
        vrhadd.u8 q15, q15, q9
        vaddl.u8 q9, d2, d4
        vaddl.u8 q11, d3, d5
        vst1.8 {q15}, [r0,:128], r2
        bgt 1b

        vld1.8 {d0-d2}, [r1], r2
        vadd.u16 q12, q8, q9
        vadd.u16  q12, q12, q13
        vext.8 q15, q0, q1, #1
        vadd.u16 q1 , q10, q11
        vshrn.u16 d28, q12, #2
        vadd.u16  q1, q1, q13
        vshrn.u16 d29, q1, #2
        vld1.8 {q8}, [r0,:128]
        vrhadd.u8 q14, q14, q8
        vaddl.u8 q8, d0, d30
        vaddl.u8 q10, d1, d31
        vst1.8 {q14}, [r0,:128], r2
        vadd.u16 q12, q8, q9
        vadd.u16  q12, q12, q13
        vadd.u16 q0, q10, q11
        vshrn.u16 d30, q12, #2
        vadd.u16  q0, q0, q13
        vshrn.u16 d31, q0, #2
        vld1.8 {q9}, [r0,:128]
        vrhadd.u8 q15, q15, q9
        vst1.8 {q15}, [r0,:128], r2

        bx lr

        @.endfunc

        .set Lpic_idx, 0
        .set Lpic_gp, 0
        .text
        .align 2
        .globl _ff_put_h264_qpel8_mc00_neon

        .thumb_func _ff_put_h264_qpel8_mc00_neon
_ff_put_h264_qpel8_mc00_neon:
        mov r3, #8

        @.endfunc

        .set Lpic_idx, 0
        .set Lpic_gp, 0
        .text
        .align 2
        .globl _ff_put_pixels8_neon

        .thumb_func _ff_put_pixels8_neon
_ff_put_pixels8_neon:
1: vld1.8 {d0}, [r1], r2
        vld1.8 {d1}, [r1], r2
        vld1.8 {d2}, [r1], r2
        pld [r1, r2, lsl #2]
        vld1.8 {d3}, [r1], r2
        pld [r1]
        pld [r1, r2]
        pld [r1, r2, lsl #1]
        subs r3, r3, #4
        vst1.8 {d0}, [r0,:64], r2
        vst1.8 {d1}, [r0,:64], r2
        vst1.8 {d2}, [r0,:64], r2
        vst1.8 {d3}, [r0,:64], r2
        bne 1b
        bx lr

        @.endfunc
        .set Lpic_idx, 0
        .set Lpic_gp, 0
        .text
        .align 2
        .globl _ff_put_pixels8_x2_neon

        .thumb_func _ff_put_pixels8_x2_neon
_ff_put_pixels8_x2_neon:
1: vld1.8 {q0}, [r1], r2
        vext.8 d1, d0, d1, #1
        vld1.8 {q1}, [r1], r2
        vext.8 d3, d2, d3, #1
        pld [r1]
        pld [r1, r2]
        subs r3, r3, #2
        vswp d1, d2
        vrhadd.u8 q0, q0, q1
        vst1.8 {d0}, [r0,:64], r2
        vst1.8 {d1}, [r0,:64], r2
        bne 1b
        bx lr

        @.endfunc
        .set Lpic_idx, 0
        .set Lpic_gp, 0
        .text
        .align 2
        .globl _ff_put_pixels8_x2_no_rnd_neon

        .thumb_func _ff_put_pixels8_x2_no_rnd_neon
_ff_put_pixels8_x2_no_rnd_neon:
1: vld1.8 {q0}, [r1], r2
        vext.8 d1, d0, d1, #1
        vld1.8 {q1}, [r1], r2
        vext.8 d3, d2, d3, #1
        pld [r1]
        pld [r1, r2]
        subs r3, r3, #2
        vswp d1, d2
        vhadd.u8 q0, q0, q1
        vst1.8 {d0}, [r0,:64], r2
        vst1.8 {d1}, [r0,:64], r2
        bne 1b
        bx lr

        @.endfunc
        .set Lpic_idx, 0
        .set Lpic_gp, 0
        .text
        .align 2
        .globl _ff_put_pixels8_y2_neon

        .thumb_func _ff_put_pixels8_y2_neon
_ff_put_pixels8_y2_neon:
        sub r3, r3, #2
        vld1.8 {d0}, [r1], r2
        vld1.8 {d1}, [r1], r2
1: subs r3, r3, #2
        vrhadd.u8 d4, d0, d1
        vld1.8 {d0}, [r1], r2
        vrhadd.u8 d5, d0, d1
        vld1.8 {d1}, [r1], r2
        pld [r1]
        pld [r1, r2]
        vst1.8 {d4}, [r0,:64], r2
        vst1.8 {d5}, [r0,:64], r2
        bne 1b

        vrhadd.u8 d4, d0, d1
        vld1.8 {d0}, [r1], r2
        vrhadd.u8 d5, d0, d1
        vst1.8 {d4}, [r0,:64], r2
        vst1.8 {d5}, [r0,:64], r2

        bx lr

        @.endfunc
        .set Lpic_idx, 0
        .set Lpic_gp, 0
        .text
        .align 2
        .globl _ff_put_pixels8_y2_no_rnd_neon

        .thumb_func _ff_put_pixels8_y2_no_rnd_neon
_ff_put_pixels8_y2_no_rnd_neon:
        sub r3, r3, #2
        vld1.8 {d0}, [r1], r2
        vld1.8 {d1}, [r1], r2
1: subs r3, r3, #2
        vhadd.u8 d4, d0, d1
        vld1.8 {d0}, [r1], r2
        vhadd.u8 d5, d0, d1
        vld1.8 {d1}, [r1], r2
        pld [r1]
        pld [r1, r2]
        vst1.8 {d4}, [r0,:64], r2
        vst1.8 {d5}, [r0,:64], r2
        bne 1b

        vhadd.u8 d4, d0, d1
        vld1.8 {d0}, [r1], r2
        vhadd.u8 d5, d0, d1
        vst1.8 {d4}, [r0,:64], r2
        vst1.8 {d5}, [r0,:64], r2

        bx lr

        @.endfunc
        .set Lpic_idx, 0
        .set Lpic_gp, 0
        .text
        .align 2
        .globl _ff_put_pixels8_xy2_neon

        .thumb_func _ff_put_pixels8_xy2_neon
_ff_put_pixels8_xy2_neon:
        sub r3, r3, #2
        vld1.8 {q0}, [r1], r2
        vld1.8 {q1}, [r1], r2
        pld [r1]
        pld [r1, r2]
        vext.8 d4, d0, d1, #1
        vext.8 d6, d2, d3, #1
        vaddl.u8 q8, d0, d4
        vaddl.u8 q9, d2, d6
1: subs r3, r3, #2
        vld1.8 {q0}, [r1], r2
        pld [r1]
        vadd.u16 q10, q8, q9
        vext.8 d4, d0, d1, #1
        vaddl.u8 q8, d0, d4
        vrshrn.u16 d5, q10, #2
        vld1.8 {q1}, [r1], r2
        vadd.u16 q10, q8, q9
        pld [r1, r2]
        vst1.8 {d5}, [r0,:64], r2
        vrshrn.u16 d7, q10, #2
        vext.8 d6, d2, d3, #1
        vaddl.u8 q9, d2, d6
        vst1.8 {d7}, [r0,:64], r2
        bgt 1b

        vld1.8 {q0}, [r1], r2
        vadd.u16 q10, q8, q9
        vext.8 d4, d0, d1, #1
        vaddl.u8 q8, d0, d4
        vrshrn.u16 d5, q10, #2
        vadd.u16 q10, q8, q9
        vst1.8 {d5}, [r0,:64], r2
        vrshrn.u16 d7, q10, #2
        vst1.8 {d7}, [r0,:64], r2

        bx lr

        @.endfunc
        .set Lpic_idx, 0
        .set Lpic_gp, 0
        .text
        .align 2
        .globl _ff_put_pixels8_xy2_no_rnd_neon

        .thumb_func _ff_put_pixels8_xy2_no_rnd_neon
_ff_put_pixels8_xy2_no_rnd_neon:
        sub r3, r3, #2
        vld1.8 {q0}, [r1], r2
        vld1.8 {q1}, [r1], r2
        vmov.i16  q11, #1
        pld [r1]
        pld [r1, r2]
        vext.8 d4, d0, d1, #1
        vext.8 d6, d2, d3, #1
        vaddl.u8 q8, d0, d4
        vaddl.u8 q9, d2, d6
1: subs r3, r3, #2
        vld1.8 {q0}, [r1], r2
        pld [r1]
        vadd.u16 q10, q8, q9
        vext.8 d4, d0, d1, #1
        vadd.u16  q10, q10, q11
        vaddl.u8 q8, d0, d4
        vshrn.u16 d5, q10, #2
        vld1.8 {q1}, [r1], r2
        vadd.u16 q10, q8, q9
        pld [r1, r2]
        vadd.u16  q10, q10, q11
        vst1.8 {d5}, [r0,:64], r2
        vshrn.u16 d7, q10, #2
        vext.8 d6, d2, d3, #1
        vaddl.u8 q9, d2, d6
        vst1.8 {d7}, [r0,:64], r2
        bgt 1b

        vld1.8 {q0}, [r1], r2
        vadd.u16 q10, q8, q9
        vext.8 d4, d0, d1, #1
        vadd.u16  q10, q10, q11
        vaddl.u8 q8, d0, d4
        vshrn.u16 d5, q10, #2
        vadd.u16 q10, q8, q9
        vadd.u16  q10, q10, q11
        vst1.8 {d5}, [r0,:64], r2
        vshrn.u16 d7, q10, #2
        vst1.8 {d7}, [r0,:64], r2

        bx lr

        @.endfunc

        .set Lpic_idx, 0
        .set Lpic_gp, 0
        .text
        .align 2
        .globl _ff_avg_h264_qpel8_mc00_neon

        .thumb_func _ff_avg_h264_qpel8_mc00_neon
_ff_avg_h264_qpel8_mc00_neon:
        mov r3, #8

        @.endfunc

        .set Lpic_idx, 0
        .set Lpic_gp, 0
        .text
        .align 2
        .globl _ff_avg_pixels8_neon

        .thumb_func _ff_avg_pixels8_neon
_ff_avg_pixels8_neon:
1: vld1.8 {d0}, [r1], r2
        vld1.8 {d1}, [r1], r2
        vld1.8 {d2}, [r1], r2
        pld [r1, r2, lsl #2]
        vld1.8 {d3}, [r1], r2
        pld [r1]
        pld [r1, r2]
        pld [r1, r2, lsl #1]
        vld1.8 {d4}, [r0,:64], r2
        vrhadd.u8 d0, d0, d4
        vld1.8 {d5}, [r0,:64], r2
        vrhadd.u8 d1, d1, d5
        vld1.8 {d6}, [r0,:64], r2
        vrhadd.u8 d2, d2, d6
        vld1.8 {d7}, [r0,:64], r2
        vrhadd.u8 d3, d3, d7
        sub r0, r0, r2, lsl #2
        subs r3, r3, #4
        vst1.8 {d0}, [r0,:64], r2
        vst1.8 {d1}, [r0,:64], r2
        vst1.8 {d2}, [r0,:64], r2
        vst1.8 {d3}, [r0,:64], r2
        bne 1b
        bx lr

        @.endfunc
        .set Lpic_idx, 0
        .set Lpic_gp, 0
        .text
        .align 2
        .globl _ff_avg_pixels8_x2_neon

        .thumb_func _ff_avg_pixels8_x2_neon
_ff_avg_pixels8_x2_neon:
1: vld1.8 {q0}, [r1], r2
        vext.8 d1, d0, d1, #1
        vld1.8 {q1}, [r1], r2
        vext.8 d3, d2, d3, #1
        pld [r1]
        pld [r1, r2]
        subs r3, r3, #2
        vswp d1, d2
        vrhadd.u8 q0, q0, q1
        vld1.8 {d4}, [r0,:64], r2
        vld1.8 {d5}, [r0,:64]
        vrhadd.u8 q0, q0, q2
        sub r0, r0, r2
        vst1.8 {d0}, [r0,:64], r2
        vst1.8 {d1}, [r0,:64], r2
        bne 1b
        bx lr

        @.endfunc
        .set Lpic_idx, 0
        .set Lpic_gp, 0
        .text
        .align 2
        .globl _ff_avg_pixels8_y2_neon

        .thumb_func _ff_avg_pixels8_y2_neon
_ff_avg_pixels8_y2_neon:
        sub r3, r3, #2
        vld1.8 {d0}, [r1], r2
        vld1.8 {d1}, [r1], r2
1: subs r3, r3, #2
        vrhadd.u8 d4, d0, d1
        vld1.8 {d0}, [r1], r2
        vrhadd.u8 d5, d0, d1
        vld1.8 {d1}, [r1], r2
        pld [r1]
        pld [r1, r2]
        vld1.8 {d2}, [r0,:64], r2
        vld1.8 {d3}, [r0,:64]
        vrhadd.u8 q2, q2, q1
        sub r0, r0, r2
        vst1.8 {d4}, [r0,:64], r2
        vst1.8 {d5}, [r0,:64], r2
        bne 1b

        vrhadd.u8 d4, d0, d1
        vld1.8 {d0}, [r1], r2
        vrhadd.u8 d5, d0, d1
        vld1.8 {d2}, [r0,:64], r2
        vld1.8 {d3}, [r0,:64]
        vrhadd.u8 q2, q2, q1
        sub r0, r0, r2
        vst1.8 {d4}, [r0,:64], r2
        vst1.8 {d5}, [r0,:64], r2

        bx lr

        @.endfunc
        .set Lpic_idx, 0
        .set Lpic_gp, 0
        .text
        .align 2
        .globl _ff_avg_pixels8_xy2_neon

        .thumb_func _ff_avg_pixels8_xy2_neon
_ff_avg_pixels8_xy2_neon:
        sub r3, r3, #2
        vld1.8 {q0}, [r1], r2
        vld1.8 {q1}, [r1], r2
        pld [r1]
        pld [r1, r2]
        vext.8 d4, d0, d1, #1
        vext.8 d6, d2, d3, #1
        vaddl.u8 q8, d0, d4
        vaddl.u8 q9, d2, d6
1: subs r3, r3, #2
        vld1.8 {q0}, [r1], r2
        pld [r1]
        vadd.u16 q10, q8, q9
        vext.8 d4, d0, d1, #1
        vaddl.u8 q8, d0, d4
        vrshrn.u16 d5, q10, #2
        vld1.8 {q1}, [r1], r2
        vadd.u16 q10, q8, q9
        pld [r1, r2]
        vld1.8 {d7}, [r0,:64]
        vrhadd.u8 d5, d5, d7
        vst1.8 {d5}, [r0,:64], r2
        vrshrn.u16 d7, q10, #2
        vld1.8 {d5}, [r0,:64]
        vrhadd.u8 d7, d7, d5
        vext.8 d6, d2, d3, #1
        vaddl.u8 q9, d2, d6
        vst1.8 {d7}, [r0,:64], r2
        bgt 1b

        vld1.8 {q0}, [r1], r2
        vadd.u16 q10, q8, q9
        vext.8 d4, d0, d1, #1
        vaddl.u8 q8, d0, d4
        vrshrn.u16 d5, q10, #2
        vadd.u16 q10, q8, q9
        vld1.8 {d7}, [r0,:64]
        vrhadd.u8 d5, d5, d7
        vst1.8 {d5}, [r0,:64], r2
        vrshrn.u16 d7, q10, #2
        vld1.8 {d5}, [r0,:64]
        vrhadd.u8 d7, d7, d5
        vst1.8 {d7}, [r0,:64], r2

        bx lr

        @.endfunc
.text
.align 2
.thumb_func _ff_put_h264_qpel8_mc00_neon
.thumb_func _ff_put_pixels8_x2_no_rnd_neon
.thumb_func _ff_avg_pixels8_xy2_neon
.thumb_func _ff_avg_h264_qpel16_mc00_neon
.thumb_func _ff_put_pixels16_y2_no_rnd_neon
.thumb_func _ff_put_pixels8_x2_neon
.thumb_func _ff_put_pixels16_x2_neon
.thumb_func _ff_avg_pixels16_x2_no_rnd_neon
.thumb_func _ff_put_pixels8_xy2_no_rnd_neon
.thumb_func _ff_avg_pixels16_xy2_no_rnd_neon
.thumb_func _ff_avg_pixels16_xy2_neon
.thumb_func _ff_avg_pixels16_y2_no_rnd_neon
.thumb_func _ff_put_pixels8_neon
.thumb_func _ff_put_pixels16_xy2_no_rnd_neon
.thumb_func _ff_put_pixels8_y2_no_rnd_neon
.thumb_func _ff_avg_pixels16_y2_neon
.thumb_func _ff_avg_pixels8_y2_neon
.thumb_func _ff_put_pixels16_y2_neon
.thumb_func _ff_avg_pixels16_x2_neon
.thumb_func _ff_avg_pixels16_neon
.thumb_func _ff_put_pixels16_xy2_neon
.thumb_func _ff_put_pixels16_x2_no_rnd_neon
.thumb_func _ff_put_pixels16_neon
.thumb_func _ff_put_pixels8_y2_neon
.thumb_func _ff_put_pixels8_xy2_neon
.thumb_func _ff_put_h264_qpel16_mc00_neon
.thumb_func _ff_avg_pixels8_neon
.thumb_func _ff_avg_pixels8_x2_neon
.thumb_func _ff_avg_h264_qpel8_mc00_neon

#endif
