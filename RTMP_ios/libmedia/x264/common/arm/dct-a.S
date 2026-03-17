



#ifdef __arm__





.syntax unified


        @.arch armv7-a






@.fpu neon



















.const_data
.align 4

scan4x4_frame:
.byte 0,1, 8,9, 2,3, 4,5
.byte 2,3, 8,9, 16,17, 10,11
.byte 12,13, 6,7, 14,15, 20,21
.byte 10,11, 12,13, 6,7, 14,15

.text








        .align 2
        .globl _x264_dct4x4dc_neon


        @.func _x264_dct4x4dc_neon
_x264_dct4x4dc_neon:
    vld1.64 {d0-d3}, [r0,:128]
    vadd.s16 d4, d0, d1
    vsub.s16 d5, d0, d1
    vadd.s16 d6, d2, d3
    vsub.s16 d7, d2, d3
    vadd.s16 d0, d4, d6
    vsub.s16 d2, d4, d6
    vadd.s16 d3, d5, d7
    vsub.s16 d1, d5, d7

    vmov.s16 d31, #1
    vtrn.16 q0, q1
    vadd.s16 q2, q0, q1
    vsub.s16 q3, q0, q1
    vtrn.32 d4, d5
    vadd.s16 d16, d4, d31
    vtrn.32 d6, d7
    vadd.s16 d17, d6, d31
    vrhadd.s16 d0, d4, d5
    vhsub.s16 d1, d16, d5
    vhsub.s16 d2, d17, d7
    vrhadd.s16 d3, d6, d7
    vst1.64 {d0-d3}, [r0,:128]
    bx lr

        @.endfunc

        .align 2
        .globl _x264_idct4x4dc_neon


        @.func _x264_idct4x4dc_neon
_x264_idct4x4dc_neon:
    vld1.64 {d0-d3}, [r0,:128]
    vadd.s16 d4, d0, d1
    vsub.s16 d5, d0, d1
    vadd.s16 d6, d2, d3
    vsub.s16 d7, d2, d3
    vadd.s16 d0, d4, d6
    vsub.s16 d2, d4, d6
    vadd.s16 d3, d5, d7
    vsub.s16 d1, d5, d7

    vtrn.16 q0, q1
    vadd.s16 q2, q0, q1
    vsub.s16 q3, q0, q1
    vtrn.32 d4, d5
    vadd.s16 d0, d4, d5
    vsub.s16 d1, d4, d5
    vtrn.32 d6, d7
    vadd.s16 d3, d6, d7
    vsub.s16 d2, d6, d7
    vst1.64 {d0-d3}, [r0,:128]
    bx lr

        @.endfunc



        .align 2
        .globl _x264_sub4x4_dct_neon


        @.func _x264_sub4x4_dct_neon
_x264_sub4x4_dct_neon:
    mov r3, #16
    mov ip, #32
    vld1.32 {d0[]}, [r1,:32], r3
    vld1.32 {d1[]}, [r2,:32], ip
    vld1.32 {d2[]}, [r1,:32], r3
    vsubl.u8 q8, d0, d1
    vld1.32 {d3[]}, [r2,:32], ip
    vld1.32 {d4[]}, [r1,:32], r3
    vsubl.u8 q9, d2, d3
    vld1.32 {d5[]}, [r2,:32], ip
    vld1.32 {d6[]}, [r1,:32], r3
    vsubl.u8 q10, d4, d5
    vld1.32 {d7[]}, [r2,:32], ip
    vsubl.u8 q11, d6, d7

    vadd.s16 d1, d18, d20
    vsub.s16 d20, d18, d20
    vadd.s16 d3, d16, d22
    vsub.s16 d22, d16, d22
    vadd.s16 d0, d3, d1
    vadd.s16 d16, d22, d22
    vadd.s16 d18, d20, d20
    vsub.s16 d2, d3, d1
    vadd.s16 d1, d16, d20
    vsub.s16 d3, d22, d18
    vtrn.32 d0, d2
    vtrn.32 d1, d3
    vtrn.16 d0, d1
    vtrn.16 d2, d3
    vadd.s16 d5, d1, d2
    vsub.s16 d2, d1, d2
    vadd.s16 d7, d0, d3
    vsub.s16 d3, d0, d3
    vadd.s16 d4, d7, d5
    vadd.s16 d0, d3, d3
    vadd.s16 d1, d2, d2
    vsub.s16 d6, d7, d5
    vadd.s16 d5, d0, d2
    vsub.s16 d7, d3, d1
    vst1.64 {d4-d7}, [r0,:128]
    bx lr

        @.endfunc

        .align 2


        @.func x264_sub8x4_dct_neon
x264_sub8x4_dct_neon:
    vld1.64 {d0}, [r1,:64], r3
    vld1.64 {d1}, [r2,:64], ip
    vsubl.u8 q8, d0, d1
    vld1.64 {d2}, [r1,:64], r3
    vld1.64 {d3}, [r2,:64], ip
    vsubl.u8 q9, d2, d3
    vld1.64 {d4}, [r1,:64], r3
    vld1.64 {d5}, [r2,:64], ip
    vsubl.u8 q10, d4, d5
    vld1.64 {d6}, [r1,:64], r3
    vld1.64 {d7}, [r2,:64], ip
    vsubl.u8 q11, d6, d7

    vadd.s16 q1, q9, q10
    vsub.s16 q10, q9, q10
    vadd.s16 q3, q8, q11
    vsub.s16 q11, q8, q11
    vadd.s16 q0, q3, q1
    vadd.s16 q8, q11, q11
    vadd.s16 q9, q10, q10
    vsub.s16 q2, q3, q1
    vadd.s16 q1, q8, q10
    vsub.s16 q3, q11, q9
    vtrn.32 q0, q2
    vtrn.32 q1, q3
    vtrn.16 q0, q1
    vtrn.16 q2, q3

    vadd.s16 q8, q0, q3
    vsub.s16 q12, q0, q3
    vadd.s16 q9, q1, q2
    vsub.s16 q10, q1, q2
    vadd.i16 q13, q12, q12
    vadd.i16 q11, q10, q10
    vadd.i16 d0, d16, d18
    vadd.i16 d1, d26, d20
    vsub.i16 d2, d16, d18
    vsub.i16 d3, d24, d22
    vst1.64 {d0-d1}, [r0,:128]!
    vadd.i16 d4, d17, d19
    vadd.i16 d5, d27, d21
    vst1.64 {d2-d3}, [r0,:128]!
    vsub.i16 d6, d17, d19
    vsub.i16 d7, d25, d23
    vst1.64 {d4-d5}, [r0,:128]!
    vst1.64 {d6-d7}, [r0,:128]!
    bx lr

        @.endfunc

        .align 2
        .globl _x264_sub8x8_dct_neon


        @.func _x264_sub8x8_dct_neon
_x264_sub8x8_dct_neon:
    push {lr}
    mov r3, #16
    mov ip, #32
    bl x264_sub8x4_dct_neon
    pop {lr}
    b x264_sub8x4_dct_neon

        @.endfunc

        .align 2
        .globl _x264_sub16x16_dct_neon


        @.func _x264_sub16x16_dct_neon
_x264_sub16x16_dct_neon:
    push {lr}
    mov r3, #16
    mov ip, #32
    bl x264_sub8x4_dct_neon
    bl x264_sub8x4_dct_neon
    sub r1, r1, #8*16 -8
    sub r2, r2, #8*32 -8
    bl x264_sub8x4_dct_neon
    bl x264_sub8x4_dct_neon
    sub r1, r1, #8
    sub r2, r2, #8
    bl x264_sub8x4_dct_neon
    bl x264_sub8x4_dct_neon
    sub r1, r1, #8*16 -8
    sub r2, r2, #8*32 -8
    bl x264_sub8x4_dct_neon
    pop {lr}
    b x264_sub8x4_dct_neon

        @.endfunc



        .align 2
        .globl _x264_sub8x8_dct8_neon


        @.func _x264_sub8x8_dct8_neon
_x264_sub8x8_dct8_neon:
    mov r3, #16
    mov ip, #32
    vld1.64 {d16}, [r1,:64], r3
    vld1.64 {d17}, [r2,:64], ip
    vsubl.u8 q8, d16, d17
    vld1.64 {d18}, [r1,:64], r3
    vld1.64 {d19}, [r2,:64], ip
    vsubl.u8 q9, d18, d19
    vld1.64 {d20}, [r1,:64], r3
    vld1.64 {d21}, [r2,:64], ip
    vsubl.u8 q10, d20, d21
    vld1.64 {d22}, [r1,:64], r3
    vld1.64 {d23}, [r2,:64], ip
    vsubl.u8 q11, d22, d23
    vld1.64 {d24}, [r1,:64], r3
    vld1.64 {d25}, [r2,:64], ip
    vsubl.u8 q12, d24, d25
    vld1.64 {d26}, [r1,:64], r3
    vld1.64 {d27}, [r2,:64], ip
    vsubl.u8 q13, d26, d27
    vld1.64 {d28}, [r1,:64], r3
    vld1.64 {d29}, [r2,:64], ip
    vsubl.u8 q14, d28, d29
    vld1.64 {d30}, [r1,:64], r3
    vld1.64 {d31}, [r2,:64], ip
    vsubl.u8 q15, d30, d31

    vadd.s16 q2, q11, q12
    vsub.s16 q1, q11, q12
    vadd.s16 q3, q10, q13
    vsub.s16 q11, q10, q13
    vadd.s16 q13, q9, q14
    vsub.s16 q10, q9, q14
    vadd.s16 q14, q8, q15
    vsub.s16 q8, q8, q15

    vadd.s16 q9, q14, q2
    vsub.s16 q2, q14, q2
    vadd.s16 q12, q13, q3
    vsub.s16 q14, q13, q3

    vadd.s16 q3, q8, q1
    vsub.s16 q13, q8, q1
    vshr.s16 q0, q10, #1
    vshr.s16 q15, q11, #1
    vadd.s16 q0, q0, q10
    vadd.s16 q15, q15, q11
    vsub.s16 q3, q3, q0
    vsub.s16 q13, q13, q15

    vadd.s16 q0, q10, q11
    vsub.s16 q15, q10, q11
    vshr.s16 q10, q8, #1
    vshr.s16 q11, q1, #1
    vadd.s16 q10, q10, q8
    vadd.s16 q11, q11, q1
    vadd.s16 q10, q0, q10
    vadd.s16 q15, q15, q11

    vadd.s16 q8, q9, q12
    vsub.s16 q12, q9, q12
    vshr.s16 q0, q15, #2
    vshr.s16 q1, q10, #2
    vadd.s16 q9, q10, q0
    vsub.s16 q15, q1, q15
    vshr.s16 q0, q14, #1
    vshr.s16 q1, q2, #1
    vadd.s16 q10, q2, q0
    vsub.s16 q14, q1, q14
    vshr.s16 q0, q3, #2
    vshr.s16 q1, q13, #2
    vadd.s16 q11, q0, q13
    vsub.s16 q13, q3, q1
    vswp d17, d24
    vswp d21, d28
    vtrn.32 q8, q10
    vtrn.32 q12, q14

    vswp d19, d26
    vswp d23, d30
    vtrn.32 q9, q11
    vtrn.32 q13, q15

    vtrn.16 q10, q11
    vtrn.16 q12, q13
    vtrn.16 q8, q9
    vtrn.16 q14, q15
    vadd.s16 q2, q11, q12
    vsub.s16 q1, q11, q12
    vadd.s16 q3, q10, q13
    vsub.s16 q11, q10, q13
    vadd.s16 q13, q9, q14
    vsub.s16 q10, q9, q14
    vadd.s16 q14, q8, q15
    vsub.s16 q8, q8, q15

    vadd.s16 q9, q14, q2
    vsub.s16 q2, q14, q2
    vadd.s16 q12, q13, q3
    vsub.s16 q14, q13, q3

    vadd.s16 q3, q8, q1
    vsub.s16 q13, q8, q1
    vshr.s16 q0, q10, #1
    vshr.s16 q15, q11, #1
    vadd.s16 q0, q0, q10
    vadd.s16 q15, q15, q11
    vsub.s16 q3, q3, q0
    vsub.s16 q13, q13, q15

    vadd.s16 q0, q10, q11
    vsub.s16 q15, q10, q11
    vshr.s16 q10, q8, #1
    vshr.s16 q11, q1, #1
    vadd.s16 q10, q10, q8
    vadd.s16 q11, q11, q1
    vadd.s16 q10, q0, q10
    vadd.s16 q15, q15, q11

    vadd.s16 q8, q9, q12
    vsub.s16 q12, q9, q12
    vshr.s16 q0, q15, #2
    vshr.s16 q1, q10, #2
    vadd.s16 q9, q10, q0
    vsub.s16 q15, q1, q15
    vshr.s16 q0, q14, #1
    vshr.s16 q1, q2, #1
    vadd.s16 q10, q2, q0
    vsub.s16 q14, q1, q14
    vshr.s16 q0, q3, #2
    vshr.s16 q1, q13, #2
    vadd.s16 q11, q0, q13
    vsub.s16 q13, q3, q1

    vst1.64 {d16-d19}, [r0,:128]!
    vst1.64 {d20-d23}, [r0,:128]!
    vst1.64 {d24-d27}, [r0,:128]!
    vst1.64 {d28-d31}, [r0,:128]!
    bx lr

        @.endfunc

        .align 2
        .globl _x264_sub16x16_dct8_neon


        @.func _x264_sub16x16_dct8_neon
_x264_sub16x16_dct8_neon:
    push {lr}
    bl _x264_sub8x8_dct8_neon
    sub r1, r1, #16*8 - 8
    sub r2, r2, #32*8 - 8
    bl _x264_sub8x8_dct8_neon
    sub r1, r1, #8
    sub r2, r2, #8
    bl _x264_sub8x8_dct8_neon
    pop {lr}
    sub r1, r1, #16*8 - 8
    sub r2, r2, #32*8 - 8
    b _x264_sub8x8_dct8_neon

        @.endfunc




        .align 2
        .globl _x264_add4x4_idct_neon


        @.func _x264_add4x4_idct_neon
_x264_add4x4_idct_neon:
    mov r2, #32
    vld1.64 {d0-d3}, [r1,:128]

    vadd.s16 d4, d0, d2
    vsub.s16 d5, d0, d2
    vshr.s16 d7, d1, #1
    vshr.s16 d6, d3, #1
    vsub.s16 d7, d7, d3
    vadd.s16 d6, d6, d1
    vld1.32 {d30[0]}, [r0,:32], r2
    vadd.s16 q0, q2, q3
    vsub.s16 q1, q2, q3

    vtrn.32 d0, d3
    vtrn.32 d1, d2
    vtrn.16 d0, d1
    vtrn.16 d3, d2

    vadd.s16 d4, d0, d3
    vsub.s16 d5, d0, d3
    vshr.s16 d7, d1, #1
    vshr.s16 d6, d2, #1
    vsub.s16 d7, d7, d2
    vadd.s16 d6, d6, d1
    vld1.32 {d30[1]}, [r0,:32], r2
    vadd.s16 q0, q2, q3
    vsub.s16 q1, q2, q3

    vrshr.s16 q0, q0, #6
    vld1.32 {d31[1]}, [r0,:32], r2
    vrshr.s16 q1, q1, #6
    vld1.32 {d31[0]}, [r0,:32], r2

    sub r0, r0, r2, lsl #2
    vaddw.u8 q0, q0, d30
    vaddw.u8 q1, q1, d31
    vqmovun.s16 d0, q0
    vqmovun.s16 d2, q1

    vst1.32 {d0[0]}, [r0,:32], r2
    vst1.32 {d0[1]}, [r0,:32], r2
    vst1.32 {d2[1]}, [r0,:32], r2
    vst1.32 {d2[0]}, [r0,:32], r2
    bx lr

        @.endfunc

        .align 2


        @.func x264_add8x4_idct_neon
x264_add8x4_idct_neon:
    vld1.64 {d0-d3}, [r1,:128]!
    vadd.s16 d16, d0, d2
    vsub.s16 d18, d0, d2
    vshr.s16 d22, d1, #1
    vshr.s16 d20, d3, #1
    vsub.s16 d22, d22, d3
    vadd.s16 d20, d20, d1
    vld1.64 {d4-d7}, [r1,:128]!
    vadd.s16 d17, d4, d6
    vsub.s16 d19, d4, d6
    vshr.s16 d23, d5, #1
    vshr.s16 d21, d7, #1
    vsub.s16 d23, d23, d7
    vadd.s16 d21, d21, d5
    vadd.s16 q0, q8, q10
    vsub.s16 q3, q8, q10
    vadd.s16 q1, q9, q11
    vsub.s16 q2, q9, q11

    vtrn.32 q0, q2
    vtrn.32 q1, q3
    vtrn.16 q0, q1
    vtrn.16 q2, q3

    vadd.s16 q8, q0, q2
    vsub.s16 q9, q0, q2
    vshr.s16 q11, q1, #1
    vshr.s16 q10, q3, #1
    vsub.s16 q11, q11, q3
    vadd.s16 q10, q10, q1
    vadd.s16 q0, q8, q10
    vsub.s16 q3, q8, q10
    vadd.s16 q1, q9, q11
    vsub.s16 q2, q9, q11

    vrshr.s16 q0, q0, #6
    vld1.32 {d28}, [r0,:64], r2
    vrshr.s16 q1, q1, #6
    vld1.32 {d29}, [r0,:64], r2
    vrshr.s16 q2, q2, #6
    vld1.32 {d30}, [r0,:64], r2
    vrshr.s16 q3, q3, #6
    vld1.32 {d31}, [r0,:64], r2

    sub r0, r0, r2, lsl #2
    vaddw.u8 q0, q0, d28
    vaddw.u8 q1, q1, d29
    vaddw.u8 q2, q2, d30
    vaddw.u8 q3, q3, d31

    vqmovun.s16 d0, q0
    vqmovun.s16 d1, q1
    vst1.32 {d0}, [r0,:64], r2
    vqmovun.s16 d2, q2
    vst1.32 {d1}, [r0,:64], r2
    vqmovun.s16 d3, q3
    vst1.32 {d2}, [r0,:64], r2
    vst1.32 {d3}, [r0,:64], r2
    bx lr

        @.endfunc

        .align 2
        .globl _x264_add8x8_idct_neon


        @.func _x264_add8x8_idct_neon
_x264_add8x8_idct_neon:
    mov r2, #32
    mov ip, lr
    bl x264_add8x4_idct_neon
    mov lr, ip
    b x264_add8x4_idct_neon

        @.endfunc

        .align 2
        .globl _x264_add16x16_idct_neon


        @.func _x264_add16x16_idct_neon
_x264_add16x16_idct_neon:
    mov r2, #32
    mov ip, lr
    bl x264_add8x4_idct_neon
    bl x264_add8x4_idct_neon
    sub r0, r0, #8*32 -8
    bl x264_add8x4_idct_neon
    bl x264_add8x4_idct_neon
    sub r0, r0, #8
    bl x264_add8x4_idct_neon
    bl x264_add8x4_idct_neon
    sub r0, r0, #8*32 -8
    bl x264_add8x4_idct_neon
    mov lr, ip
    b x264_add8x4_idct_neon

        @.endfunc



        .align 2
        .globl _x264_add8x8_idct8_neon


        @.func _x264_add8x8_idct8_neon
_x264_add8x8_idct8_neon:
    mov r2, #32
    vld1.64 {d16-d19}, [r1,:128]!
    vld1.64 {d20-d23}, [r1,:128]!
    vld1.64 {d24-d27}, [r1,:128]!

    vadd.s16 q0, q8, q12
    vsub.s16 q1, q8, q12
    vld1.64 {d28-d31}, [r1,:128]!
    vshr.s16 q8, q14, #1
    vshr.s16 q12, q10, #1
    vadd.s16 q2, q10, q8
    vsub.s16 q3, q12, q14
    vadd.s16 q8, q13, q11
    vsub.s16 q10, q13, q11
    vshr.s16 q12, q9, #1
    vshr.s16 q14, q15, #1
    vadd.s16 q12, q12, q9
    vadd.s16 q14, q14, q15
    vadd.s16 q8, q8, q12
    vsub.s16 q10, q10, q14
    vadd.s16 q14, q15, q9
    vsub.s16 q15, q15, q9
    vshr.s16 q12, q13, #1
    vshr.s16 q9, q11, #1
    vadd.s16 q12, q12, q13
    vadd.s16 q9, q9, q11
    vadd.s16 q15, q15, q12
    vsub.s16 q14, q14, q9

    vshr.s16 q11, q15, #2
    vshr.s16 q9, q14, #2
    vadd.s16 q13, q14, q11
    vsub.s16 q14, q9, q15
    vshr.s16 q11, q8, #2
    vshr.s16 q9, q10, #2
    vadd.s16 q12, q11, q10
    vsub.s16 q15, q8, q9

    vadd.s16 q10, q0, q2
    vsub.s16 q2, q0, q2
    vadd.s16 q11, q1, q3
    vsub.s16 q3, q1, q3

    vadd.s16 q8, q10, q15
    vsub.s16 q15, q10, q15
    vadd.s16 q9, q11, q14
    vsub.s16 q14, q11, q14
    vadd.s16 q10, q3, q13
    vsub.s16 q13, q3, q13
    vtrn.16 q8, q9
    vadd.s16 q11, q2, q12
    vsub.s16 q12, q2, q12
    vtrn.16 q10, q11
    vtrn.16 q12, q13
    vtrn.16 q14, q15
    vtrn.32 q8, q10
    vtrn.32 q9, q11
    vtrn.32 q12, q14
    vtrn.32 q13, q15
    vswp d17, d24
    vswp d21, d28
    vadd.s16 q0, q8, q12
    vsub.s16 q1, q8, q12
    vswp d19, d26
    vshr.s16 q8, q14, #1
    vshr.s16 q12, q10, #1
    vadd.s16 q2, q10, q8
    vsub.s16 q3, q12, q14
    vswp d23, d30
    vadd.s16 q8, q13, q11
    vsub.s16 q10, q13, q11
    vshr.s16 q12, q9, #1
    vshr.s16 q14, q15, #1
    vadd.s16 q12, q12, q9
    vadd.s16 q14, q14, q15
    vadd.s16 q8, q8, q12
    vsub.s16 q10, q10, q14
    vadd.s16 q14, q15, q9
    vsub.s16 q15, q15, q9
    vshr.s16 q12, q13, #1
    vshr.s16 q9, q11, #1
    vadd.s16 q12, q12, q13
    vadd.s16 q9, q9, q11
    vadd.s16 q15, q15, q12
    vsub.s16 q14, q14, q9

    vshr.s16 q11, q15, #2
    vshr.s16 q9, q14, #2
    vadd.s16 q13, q14, q11
    vsub.s16 q14, q9, q15
    vshr.s16 q11, q8, #2
    vshr.s16 q9, q10, #2
    vadd.s16 q12, q11, q10
    vsub.s16 q15, q8, q9

    vadd.s16 q10, q0, q2
    vsub.s16 q2, q0, q2
    vadd.s16 q11, q1, q3
    vsub.s16 q3, q1, q3

    vadd.s16 q8, q10, q15
    vsub.s16 q15, q10, q15
    vadd.s16 q9, q11, q14
    vsub.s16 q14, q11, q14
    vadd.s16 q10, q3, q13
    vsub.s16 q13, q3, q13
    vadd.s16 q11, q2, q12
    vsub.s16 q12, q2, q12

    vld1.64 {d0}, [r0,:64], r2
    vrshr.s16 q8, q8, #6
    vld1.64 {d1}, [r0,:64], r2
    vrshr.s16 q9, q9, #6
    vld1.64 {d2}, [r0,:64], r2
    vrshr.s16 q10, q10, #6
    vld1.64 {d3}, [r0,:64], r2
    vrshr.s16 q11, q11, #6
    vld1.64 {d4}, [r0,:64], r2
    vrshr.s16 q12, q12, #6
    vld1.64 {d5}, [r0,:64], r2
    vrshr.s16 q13, q13, #6
    vld1.64 {d6}, [r0,:64], r2
    vrshr.s16 q14, q14, #6
    vld1.64 {d7}, [r0,:64], r2
    vrshr.s16 q15, q15, #6
    sub r0, r0, r2, lsl #3

    vaddw.u8 q8, q8, d0
    vaddw.u8 q9, q9, d1
    vaddw.u8 q10, q10, d2
    vqmovun.s16 d0, q8
    vqmovun.s16 d1, q9
    vqmovun.s16 d2, q10
    vaddw.u8 q11, q11, d3
    vst1.64 {d0}, [r0,:64], r2
    vaddw.u8 q12, q12, d4
    vst1.64 {d1}, [r0,:64], r2
    vaddw.u8 q13, q13, d5
    vst1.64 {d2}, [r0,:64], r2
    vqmovun.s16 d3, q11
    vqmovun.s16 d4, q12
    vaddw.u8 q14, q14, d6
    vaddw.u8 q15, q15, d7
    vst1.64 {d3}, [r0,:64], r2
    vqmovun.s16 d5, q13
    vst1.64 {d4}, [r0,:64], r2
    vqmovun.s16 d6, q14
    vqmovun.s16 d7, q15
    vst1.64 {d5}, [r0,:64], r2
    vst1.64 {d6}, [r0,:64], r2
    vst1.64 {d7}, [r0,:64], r2
    bx lr

        @.endfunc

        .align 2
        .globl _x264_add16x16_idct8_neon


        @.func _x264_add16x16_idct8_neon
_x264_add16x16_idct8_neon:
    mov ip, lr
    bl _x264_add8x8_idct8_neon
    sub r0, r0, #8*32 -8
    bl _x264_add8x8_idct8_neon
    sub r0, r0, #8
    bl _x264_add8x8_idct8_neon
    sub r0, r0, #8*32 -8
    mov lr, ip
    b _x264_add8x8_idct8_neon

        @.endfunc


        .align 2
        .globl _x264_add8x8_idct_dc_neon


        @.func _x264_add8x8_idct_dc_neon
_x264_add8x8_idct_dc_neon:
    mov r2, #32
    vld1.64 {d16}, [r1,:64]
    vrshr.s16 d16, d16, #6
    vld1.64 {d0}, [r0,:64], r2
    vmov.i16 q15, #0
    vld1.64 {d1}, [r0,:64], r2
    vld1.64 {d2}, [r0,:64], r2
    vdup.16 d20, d16[0]
    vld1.64 {d3}, [r0,:64], r2
    vdup.16 d21, d16[1]
    vld1.64 {d4}, [r0,:64], r2
    vdup.16 d22, d16[2]
    vld1.64 {d5}, [r0,:64], r2
    vdup.16 d23, d16[3]
    vld1.64 {d6}, [r0,:64], r2
    vsub.s16 q12, q15, q10
    vld1.64 {d7}, [r0,:64], r2
    vsub.s16 q13, q15, q11

    sub r0, r0, #8*32

    vqmovun.s16 d20, q10
    vqmovun.s16 d22, q11
    vqmovun.s16 d24, q12
    vqmovun.s16 d26, q13

    vmov d21, d20
    vqadd.u8 q0, q0, q10
    vmov d23, d22
    vqadd.u8 q1, q1, q10
    vmov d25, d24
    vqadd.u8 q2, q2, q11
    vmov d27, d26
    vqadd.u8 q3, q3, q11
    vqsub.u8 q0, q0, q12
    vqsub.u8 q1, q1, q12
    vqsub.u8 q2, q2, q13

    vst1.64 {d0}, [r0,:64], r2
    vqsub.u8 q3, q3, q13
    vst1.64 {d1}, [r0,:64], r2
    vst1.64 {d2}, [r0,:64], r2
    vst1.64 {d3}, [r0,:64], r2
    vst1.64 {d4}, [r0,:64], r2
    vst1.64 {d5}, [r0,:64], r2
    vst1.64 {d6}, [r0,:64], r2
    vst1.64 {d7}, [r0,:64], r2
    bx lr

        @.endfunc


        .align 2
        .globl _x264_add16x16_idct_dc_neon


        @.func _x264_add16x16_idct_dc_neon
_x264_add16x16_idct_dc_neon:
    mov r2, r0
    mov r3, #32
    vmov.i16 q15, #0

    vld1.64 {d0-d3}, [r1,:64]
    vrshr.s16 q0, #6
    vrshr.s16 q1, #6

    vld1.64 {d16-d17}, [r0,:128], r3
    vld1.64 {d18-d19}, [r0,:128], r3
    vdup.16 d4, d0[0]
    vdup.16 d5, d0[1]
    vld1.64 {d20-d21}, [r0,:128], r3
    vdup.16 d6, d0[2]
    vdup.16 d7, d0[3]
    vld1.64 {d22-d23}, [r0,:128], r3
    vsub.s16 q12, q15, q2
    vsub.s16 q13, q15, q3

    vqmovun.s16 d4, q2
    vqmovun.s16 d5, q3
    vqmovun.s16 d6, q12
    vqmovun.s16 d7, q13

    vqadd.u8 q8, q8, q2
    vqadd.u8 q9, q9, q2
    vqadd.u8 q10, q10, q2
    vqadd.u8 q11, q11, q2

    vqsub.u8 q8, q8, q3
    vqsub.u8 q9, q9, q3
    vqsub.u8 q10, q10, q3
    vst1.64 {d16-d17}, [r2,:128], r3
    vqsub.u8 q11, q11, q3
    vst1.64 {d18-d19}, [r2,:128], r3
    vst1.64 {d20-d21}, [r2,:128], r3
    vst1.64 {d22-d23}, [r2,:128], r3
    vld1.64 {d16-d17}, [r0,:128], r3
    vld1.64 {d18-d19}, [r0,:128], r3
    vdup.16 d4, d1[0]
    vdup.16 d5, d1[1]
    vld1.64 {d20-d21}, [r0,:128], r3
    vdup.16 d6, d1[2]
    vdup.16 d7, d1[3]
    vld1.64 {d22-d23}, [r0,:128], r3
    vsub.s16 q12, q15, q2
    vsub.s16 q13, q15, q3

    vqmovun.s16 d4, q2
    vqmovun.s16 d5, q3
    vqmovun.s16 d6, q12
    vqmovun.s16 d7, q13

    vqadd.u8 q8, q8, q2
    vqadd.u8 q9, q9, q2
    vqadd.u8 q10, q10, q2
    vqadd.u8 q11, q11, q2

    vqsub.u8 q8, q8, q3
    vqsub.u8 q9, q9, q3
    vqsub.u8 q10, q10, q3
    vst1.64 {d16-d17}, [r2,:128], r3
    vqsub.u8 q11, q11, q3
    vst1.64 {d18-d19}, [r2,:128], r3
    vst1.64 {d20-d21}, [r2,:128], r3
    vst1.64 {d22-d23}, [r2,:128], r3
    vld1.64 {d16-d17}, [r0,:128], r3
    vld1.64 {d18-d19}, [r0,:128], r3
    vdup.16 d4, d2[0]
    vdup.16 d5, d2[1]
    vld1.64 {d20-d21}, [r0,:128], r3
    vdup.16 d6, d2[2]
    vdup.16 d7, d2[3]
    vld1.64 {d22-d23}, [r0,:128], r3
    vsub.s16 q12, q15, q2
    vsub.s16 q13, q15, q3

    vqmovun.s16 d4, q2
    vqmovun.s16 d5, q3
    vqmovun.s16 d6, q12
    vqmovun.s16 d7, q13

    vqadd.u8 q8, q8, q2
    vqadd.u8 q9, q9, q2
    vqadd.u8 q10, q10, q2
    vqadd.u8 q11, q11, q2

    vqsub.u8 q8, q8, q3
    vqsub.u8 q9, q9, q3
    vqsub.u8 q10, q10, q3
    vst1.64 {d16-d17}, [r2,:128], r3
    vqsub.u8 q11, q11, q3
    vst1.64 {d18-d19}, [r2,:128], r3
    vst1.64 {d20-d21}, [r2,:128], r3
    vst1.64 {d22-d23}, [r2,:128], r3
    vld1.64 {d16-d17}, [r0,:128], r3
    vld1.64 {d18-d19}, [r0,:128], r3
    vdup.16 d4, d3[0]
    vdup.16 d5, d3[1]
    vld1.64 {d20-d21}, [r0,:128], r3
    vdup.16 d6, d3[2]
    vdup.16 d7, d3[3]
    vld1.64 {d22-d23}, [r0,:128], r3
    vsub.s16 q12, q15, q2
    vsub.s16 q13, q15, q3

    vqmovun.s16 d4, q2
    vqmovun.s16 d5, q3
    vqmovun.s16 d6, q12
    vqmovun.s16 d7, q13

    vqadd.u8 q8, q8, q2
    vqadd.u8 q9, q9, q2
    vqadd.u8 q10, q10, q2
    vqadd.u8 q11, q11, q2

    vqsub.u8 q8, q8, q3
    vqsub.u8 q9, q9, q3
    vqsub.u8 q10, q10, q3
    vst1.64 {d16-d17}, [r2,:128], r3
    vqsub.u8 q11, q11, q3
    vst1.64 {d18-d19}, [r2,:128], r3
    vst1.64 {d20-d21}, [r2,:128], r3
    vst1.64 {d22-d23}, [r2,:128], r3
    bx lr

        @.endfunc

        .align 2
        .globl _x264_sub8x8_dct_dc_neon


        @.func _x264_sub8x8_dct_dc_neon
_x264_sub8x8_dct_dc_neon:
    mov r3, #16
    mov ip, #32
    vld1.64 {d16}, [r1,:64], r3
    vld1.64 {d17}, [r2,:64], ip
    vsubl.u8 q8, d16, d17
    vld1.64 {d18}, [r1,:64], r3
    vld1.64 {d19}, [r2,:64], ip
    vsubl.u8 q9, d18, d19
    vld1.64 {d20}, [r1,:64], r3
    vld1.64 {d21}, [r2,:64], ip
    vsubl.u8 q10, d20, d21
    vld1.64 {d22}, [r1,:64], r3
    vadd.s16 q0, q8, q9
    vld1.64 {d23}, [r2,:64], ip
    vsubl.u8 q11, d22, d23
    vld1.64 {d24}, [r1,:64], r3
    vadd.s16 q0, q0, q10
    vld1.64 {d25}, [r2,:64], ip
    vsubl.u8 q12, d24, d25
    vld1.64 {d26}, [r1,:64], r3
    vadd.s16 q0, q0, q11
    vld1.64 {d27}, [r2,:64], ip
    vsubl.u8 q13, d26, d27
    vld1.64 {d28}, [r1,:64], r3
    vld1.64 {d29}, [r2,:64], ip
    vsubl.u8 q14, d28, d29
    vld1.64 {d30}, [r1,:64], r3
    vadd.s16 q1, q12, q13
    vld1.64 {d31}, [r2,:64], ip
    vsubl.u8 q15, d30, d31
    vadd.s16 q1, q1, q14

    vadd.s16 d4, d0, d1
    vadd.s16 q1, q1, q15
    vsub.s16 d5, d0, d1
    vadd.s16 d6, d2, d3
    vsub.s16 d7, d2, d3
    vadd.s16 q0, q2, q3
    vsub.s16 q1, q2, q3

    vpadd.s16 d0, d0, d2
    vpadd.s16 d1, d1, d3
    vpadd.s16 d0, d0, d1
    vst1.64 {d0}, [r0,:64]
    bx lr

        @.endfunc


        .align 2
        .globl _x264_zigzag_scan_4x4_frame_neon


        @.func _x264_zigzag_scan_4x4_frame_neon
_x264_zigzag_scan_4x4_frame_neon:




         ldr r2, Literal_0

    vld1.64 {d0-d3}, [r1,:128]
    vld1.64 {d16-d19}, [r2,:128]
    vtbl.8 d4, {d0-d1}, d16
    vtbl.8 d5, {d1-d3}, d17
    vtbl.8 d6, {d0-d2}, d18
    vtbl.8 d7, {d2-d3}, d19
    vst1.64 {d4-d7}, [r0,:128]
    bx lr

        @.endfunc
.text
.align 2
Literal_0:
 .word scan4x4_frame


#endif