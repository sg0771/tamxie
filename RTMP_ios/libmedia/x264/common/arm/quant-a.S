



#ifdef __arm__





.syntax unified


        @.arch armv7-a






@.fpu neon



















.const_data
.align 4
pmovmskb_byte:
.byte 1,2,4,8,16,32,64,128
.byte 1,2,4,8,16,32,64,128

.text




        .align 2
        .globl _x264_quant_2x2_dc_neon


        @.func _x264_quant_2x2_dc_neon
_x264_quant_2x2_dc_neon:
    vld1.64 {d0}, [r0,:64]
    vabs.s16 d3, d0
    vdup.16 d2, r2
    vdup.16 d1, r1
    vadd.u16 d3, d3, d2
    vmull.u16 q3, d3, d1
    vshr.s16 d0, d0, #15
    vshrn.u32 d3, q3, #16
    veor d3, d3, d0
    vsub.s16 d3, d3, d0
    vst1.64 {d3}, [r0,:64]
    vmov r2, r3, d3
    orrs r0, r2, r3
    movne r0, #1
    bx lr

        @.endfunc


        .align 2
        .globl _x264_quant_4x4_dc_neon


        @.func _x264_quant_4x4_dc_neon
_x264_quant_4x4_dc_neon:
    vld1.64 {d28-d31}, [r0,:128]
    vabs.s16 q8, q14
    vabs.s16 q9, q15
    vdup.16 q0, r2
    vdup.16 q2, r1
    vadd.u16 q8, q8, q0
    vadd.u16 q9, q9, q0
    vmull.u16 q10, d16, d4
    vmull.u16 q11, d17, d5
    vmull.u16 q12, d18, d4
    vmull.u16 q13, d19, d5
    vshr.s16 q14, q14, #15
    vshr.s16 q15, q15, #15
    vshrn.u32 d16, q10, #16
    vshrn.u32 d17, q11, #16
    vshrn.u32 d18, q12, #16
    vshrn.u32 d19, q13, #16
    veor q8, q8, q14
    veor q9, q9, q15
    vsub.s16 q8, q8, q14
    vsub.s16 q9, q9, q15
    vorr q0, q8, q9
    vst1.64 {d16-d19}, [r0,:128]!
    vorr d0, d0, d1
    vmov r2, r3, d0
    orrs r0, r2, r3
    movne r0, #1
    bx lr

        @.endfunc


        .align 2
        .globl _x264_quant_4x4_neon


        @.func _x264_quant_4x4_neon
_x264_quant_4x4_neon:
    vld1.64 {d28-d31}, [r0,:128]
    vabs.s16 q8, q14
    vabs.s16 q9, q15
    vld1.64 {d0-d3}, [r2,:128]
    vld1.64 {d4-d7}, [r1,:128]
    vadd.u16 q8, q8, q0
    vadd.u16 q9, q9, q1
    vmull.u16 q10, d16, d4
    vmull.u16 q11, d17, d5
    vmull.u16 q12, d18, d6
    vmull.u16 q13, d19, d7
    vshr.s16 q14, q14, #15
    vshr.s16 q15, q15, #15
    vshrn.u32 d16, q10, #16
    vshrn.u32 d17, q11, #16
    vshrn.u32 d18, q12, #16
    vshrn.u32 d19, q13, #16
    veor q8, q8, q14
    veor q9, q9, q15
    vsub.s16 q8, q8, q14
    vsub.s16 q9, q9, q15
    vorr q0, q8, q9
    vst1.64 {d16-d19}, [r0,:128]!
    vorr d0, d0, d1
    vmov r2, r3, d0
    orrs r0, r2, r3
    movne r0, #1
    bx lr

        @.endfunc


        .align 2
        .globl _x264_quant_4x4x4_neon


        @.func _x264_quant_4x4x4_neon
_x264_quant_4x4x4_neon:
    vpush {d8-d15}
    vld1.64 {d28-d31}, [r0,:128]
    vabs.s16 q8, q14
    vabs.s16 q9, q15
    vld1.64 {d0-d3}, [r2,:128]
    vld1.64 {d4-d7}, [r1,:128]
    vadd.u16 q8, q8, q0
    vadd.u16 q9, q9, q1
    vmull.u16 q10, d16, d4
    vmull.u16 q11, d17, d5
    vmull.u16 q12, d18, d6
    vmull.u16 q13, d19, d7
    vshr.s16 q14, q14, #15
    vshr.s16 q15, q15, #15
    vshrn.u32 d16, q10, #16
    vshrn.u32 d17, q11, #16
    vshrn.u32 d18, q12, #16
    vshrn.u32 d19, q13, #16
    veor q8, q8, q14
    veor q9, q9, q15
    vsub.s16 q8, q8, q14
    vsub.s16 q9, q9, q15
    vorr q4, q8, q9
    vst1.64 {d16-d19}, [r0,:128]!
    vld1.64 {d28-d31}, [r0,:128]
    vabs.s16 q8, q14
    vabs.s16 q9, q15
    vadd.u16 q8, q8, q0
    vadd.u16 q9, q9, q1
    vmull.u16 q10, d16, d4
    vmull.u16 q11, d17, d5
    vmull.u16 q12, d18, d6
    vmull.u16 q13, d19, d7
    vshr.s16 q14, q14, #15
    vshr.s16 q15, q15, #15
    vshrn.u32 d16, q10, #16
    vshrn.u32 d17, q11, #16
    vshrn.u32 d18, q12, #16
    vshrn.u32 d19, q13, #16
    veor q8, q8, q14
    veor q9, q9, q15
    vsub.s16 q8, q8, q14
    vsub.s16 q9, q9, q15
    vorr q5, q8, q9
    vst1.64 {d16-d19}, [r0,:128]!
    vld1.64 {d28-d31}, [r0,:128]
    vabs.s16 q8, q14
    vabs.s16 q9, q15
    vadd.u16 q8, q8, q0
    vadd.u16 q9, q9, q1
    vmull.u16 q10, d16, d4
    vmull.u16 q11, d17, d5
    vmull.u16 q12, d18, d6
    vmull.u16 q13, d19, d7
    vshr.s16 q14, q14, #15
    vshr.s16 q15, q15, #15
    vshrn.u32 d16, q10, #16
    vshrn.u32 d17, q11, #16
    vshrn.u32 d18, q12, #16
    vshrn.u32 d19, q13, #16
    veor q8, q8, q14
    veor q9, q9, q15
    vsub.s16 q8, q8, q14
    vsub.s16 q9, q9, q15
    vorr q6, q8, q9
    vst1.64 {d16-d19}, [r0,:128]!
    vld1.64 {d28-d31}, [r0,:128]
    vabs.s16 q8, q14
    vabs.s16 q9, q15
    vadd.u16 q8, q8, q0
    vadd.u16 q9, q9, q1
    vmull.u16 q10, d16, d4
    vmull.u16 q11, d17, d5
    vmull.u16 q12, d18, d6
    vmull.u16 q13, d19, d7
    vshr.s16 q14, q14, #15
    vshr.s16 q15, q15, #15
    vshrn.u32 d16, q10, #16
    vshrn.u32 d17, q11, #16
    vshrn.u32 d18, q12, #16
    vshrn.u32 d19, q13, #16
    veor q8, q8, q14
    veor q9, q9, q15
    vsub.s16 q8, q8, q14
    vsub.s16 q9, q9, q15
    vorr q7, q8, q9
    vst1.64 {d16-d19}, [r0,:128]!
    vorr d8, d8, d9
    vorr d10, d10, d11
    vorr d12, d12, d13
    vorr d14, d14, d15
    vmov r0, r1, d8
    vmov r2, r3, d10
    orrs r0, r1
    movne r0, #1
    orrs r2, r3
    orrne r0, #2
    vmov r1, r2, d12
    vmov r3, ip, d14
    orrs r1, r2
    orrne r0, #4
    orrs r3, ip
    orrne r0, #8
    vpop {d8-d15}
    bx lr

        @.endfunc


        .align 2
        .globl _x264_quant_8x8_neon


        @.func _x264_quant_8x8_neon
_x264_quant_8x8_neon:
    vld1.64 {d28-d31}, [r0,:128]
    vabs.s16 q8, q14
    vabs.s16 q9, q15
    vld1.64 {d0-d3}, [r2,:128]!
    vld1.64 {d4-d7}, [r1,:128]!
    vadd.u16 q8, q8, q0
    vadd.u16 q9, q9, q1
    vmull.u16 q10, d16, d4
    vmull.u16 q11, d17, d5
    vmull.u16 q12, d18, d6
    vmull.u16 q13, d19, d7
    vshr.s16 q14, q14, #15
    vshr.s16 q15, q15, #15
    vshrn.u32 d16, q10, #16
    vshrn.u32 d17, q11, #16
    vshrn.u32 d18, q12, #16
    vshrn.u32 d19, q13, #16
    veor q8, q8, q14
    veor q9, q9, q15
    vsub.s16 q8, q8, q14
    vsub.s16 q9, q9, q15
    vorr q0, q8, q9
    vst1.64 {d16-d19}, [r0,:128]!

    vld1.64 {d28-d31}, [r0,:128]
    vabs.s16 q8, q14
    vabs.s16 q9, q15
    vld1.64 {d2-d5}, [r2,:128]!
    vadd.u16 q8, q8, q1
    vadd.u16 q9, q9, q2
    vld1.64 {d4-d7}, [r1,:128]!
    vmull.u16 q10, d16, d4
    vmull.u16 q11, d17, d5
    vmull.u16 q12, d18, d6
    vmull.u16 q13, d19, d7
    vshr.s16 q14, q14, #15
    vshr.s16 q15, q15, #15
    vshrn.u32 d16, q10, #16
    vshrn.u32 d17, q11, #16
    vshrn.u32 d18, q12, #16
    vshrn.u32 d19, q13, #16
    veor q8, q8, q14
    veor q9, q9, q15
    vsub.s16 q8, q8, q14
    vsub.s16 q9, q9, q15
    vorr q1, q8, q9
    vst1.64 {d16-d19}, [r0,:128]!
    vorr q0, q0, q1

    vld1.64 {d28-d31}, [r0,:128]
    vabs.s16 q8, q14
    vabs.s16 q9, q15
    vld1.64 {d2-d5}, [r2,:128]!
    vadd.u16 q8, q8, q1
    vadd.u16 q9, q9, q2
    vld1.64 {d4-d7}, [r1,:128]!
    vmull.u16 q10, d16, d4
    vmull.u16 q11, d17, d5
    vmull.u16 q12, d18, d6
    vmull.u16 q13, d19, d7
    vshr.s16 q14, q14, #15
    vshr.s16 q15, q15, #15
    vshrn.u32 d16, q10, #16
    vshrn.u32 d17, q11, #16
    vshrn.u32 d18, q12, #16
    vshrn.u32 d19, q13, #16
    veor q8, q8, q14
    veor q9, q9, q15
    vsub.s16 q8, q8, q14
    vsub.s16 q9, q9, q15
    vorr q1, q8, q9
    vst1.64 {d16-d19}, [r0,:128]!
    vorr q0, q0, q1

    vld1.64 {d28-d31}, [r0,:128]
    vabs.s16 q8, q14
    vabs.s16 q9, q15
    vld1.64 {d2-d5}, [r2,:128]!
    vadd.u16 q8, q8, q1
    vadd.u16 q9, q9, q2
    vld1.64 {d4-d7}, [r1,:128]!
    vmull.u16 q10, d16, d4
    vmull.u16 q11, d17, d5
    vmull.u16 q12, d18, d6
    vmull.u16 q13, d19, d7
    vshr.s16 q14, q14, #15
    vshr.s16 q15, q15, #15
    vshrn.u32 d16, q10, #16
    vshrn.u32 d17, q11, #16
    vshrn.u32 d18, q12, #16
    vshrn.u32 d19, q13, #16
    veor q8, q8, q14
    veor q9, q9, q15
    vsub.s16 q8, q8, q14
    vsub.s16 q9, q9, q15
    vorr q1, q8, q9
    vst1.64 {d16-d19}, [r0,:128]!
    vorr q0, q0, q1
    vorr d0, d0, d1
    vmov r2, r3, d0
    orrs r0, r2, r3
    movne r0, #1
    bx lr

        @.endfunc




        .align 2
        .globl _x264_dequant_4x4_neon


        @.func _x264_dequant_4x4_neon
_x264_dequant_4x4_neon:
    mov r3, #0x2b
    mul r3, r3, r2
    lsr r3, r3, #8
    add ip, r3, r3, lsl #1
    sub r2, r2, ip, lsl #1
    add r1, r1, r2, lsl #4+2
    subs r3, r3, #4
    blt dequant_4x4_rshift

    vdup.16 q15, r3
dequant_4x4_lshift_loop:
    vld1.32 {d16-d17}, [r1,:128]!
    vld1.32 {d18-d19}, [r1,:128]!
    vmovn.s32 d4, q8
    vld1.32 {d20-d21}, [r1,:128]!
    vmovn.s32 d5, q9
    vld1.32 {d22-d23}, [r1,:128]!
    vmovn.s32 d6, q10
    vld1.16 {d0-d3}, [r0,:128]
    vmovn.s32 d7, q11
    vmul.s16 q0, q0, q2
    vmul.s16 q1, q1, q3
    vshl.s16 q0, q0, q15
    vshl.s16 q1, q1, q15
    vst1.16 {d0-d3}, [r0,:128]!
    bx lr

dequant_4x4_rshift:
    vdup.32 q15, r3
    rsb r3, r3, #0
    mov ip, #1
    sub r3, r3, #1
    lsl ip, ip, r3

    vdup.32 q10, ip
    vld1.32 {d16-d17}, [r1,:128]!
    vdup.32 q11, ip
    vld1.32 {d18-d19}, [r1,:128]!
    vmovn.s32 d4, q8
    vld1.32 {d16-d17}, [r1,:128]!
    vmovn.s32 d5, q9
    vld1.32 {d18-d19}, [r1,:128]!
    vmovn.s32 d6, q8
    vld1.16 {d0-d3}, [r0,:128]
    vmovn.s32 d7, q9
    vdup.32 q12, ip
    vdup.32 q13, ip

    vmlal.s16 q10, d0, d4
    vmlal.s16 q11, d1, d5
    vmlal.s16 q12, d2, d6
    vmlal.s16 q13, d3, d7
    vshl.s32 q10, q10, q15
    vshl.s32 q11, q11, q15
    vshl.s32 q12, q12, q15
    vshl.s32 q13, q13, q15

    vmovn.s32 d0, q10
    vmovn.s32 d1, q11
    vmovn.s32 d2, q12
    vmovn.s32 d3, q13
    vst1.16 {d0-d3}, [r0,:128]!
    bx lr

        @.endfunc
        .align 2
        .globl _x264_dequant_8x8_neon


        @.func _x264_dequant_8x8_neon
_x264_dequant_8x8_neon:
    mov r3, #0x2b
    mul r3, r3, r2
    lsr r3, r3, #8
    add ip, r3, r3, lsl #1
    sub r2, r2, ip, lsl #1
    add r1, r1, r2, lsl #6+2
    subs r3, r3, #6
    mov r2, #4
    blt dequant_8x8_rshift

    vdup.16 q15, r3
dequant_8x8_lshift_loop:
    subs r2, r2, #1
    vld1.32 {d16-d17}, [r1,:128]!
    vld1.32 {d18-d19}, [r1,:128]!
    vmovn.s32 d4, q8
    vld1.32 {d20-d21}, [r1,:128]!
    vmovn.s32 d5, q9
    vld1.32 {d22-d23}, [r1,:128]!
    vmovn.s32 d6, q10
    vld1.16 {d0-d3}, [r0,:128]
    vmovn.s32 d7, q11
    vmul.s16 q0, q0, q2
    vmul.s16 q1, q1, q3
    vshl.s16 q0, q0, q15
    vshl.s16 q1, q1, q15
    vst1.16 {d0-d3}, [r0,:128]!
    bgt dequant_8x8_lshift_loop
    bx lr

dequant_8x8_rshift:
    vdup.32 q15, r3
    rsb r3, r3, #0
    mov ip, #1
    sub r3, r3, #1
    lsl ip, ip, r3

dequant_8x8_rshift_loop:
    subs r2, r2, #1
    vdup.32 q10, ip
    vld1.32 {d16-d17}, [r1,:128]!
    vdup.32 q11, ip
    vld1.32 {d18-d19}, [r1,:128]!
    vmovn.s32 d4, q8
    vld1.32 {d16-d17}, [r1,:128]!
    vmovn.s32 d5, q9
    vld1.32 {d18-d19}, [r1,:128]!
    vmovn.s32 d6, q8
    vld1.16 {d0-d3}, [r0,:128]
    vmovn.s32 d7, q9
    vdup.32 q12, ip
    vdup.32 q13, ip

    vmlal.s16 q10, d0, d4
    vmlal.s16 q11, d1, d5
    vmlal.s16 q12, d2, d6
    vmlal.s16 q13, d3, d7
    vshl.s32 q10, q10, q15
    vshl.s32 q11, q11, q15
    vshl.s32 q12, q12, q15
    vshl.s32 q13, q13, q15

    vmovn.s32 d0, q10
    vmovn.s32 d1, q11
    vmovn.s32 d2, q12
    vmovn.s32 d3, q13
    vst1.16 {d0-d3}, [r0,:128]!
    bgt dequant_8x8_rshift_loop
    bx lr

        @.endfunc


        .align 2
        .globl _x264_dequant_4x4_dc_neon


        @.func _x264_dequant_4x4_dc_neon
_x264_dequant_4x4_dc_neon:
    mov r3, #0x2b
    mul r3, r3, r2
    lsr r3, r3, #8
    add ip, r3, r3, lsl #1
    sub r2, r2, ip, lsl #1
    ldr r1, [r1, r2, lsl #6]
    subs r3, r3, #6
    blt dequant_4x4_dc_rshift

    lsl r1, r1, r3
    vdup.16 q2, r1
    vld1.16 {d0-d3}, [r0,:128]
    vdup.16 q15, r3

    vmul.s16 q0, q0, q2
    vmul.s16 q1, q1, q2
    vst1.16 {d0-d3}, [r0,:128]
    bx lr

dequant_4x4_dc_rshift:
    vdup.16 d4, r1
    vdup.32 q15, r3
    rsb r3, r3, #0
    mov ip, #1
    sub r3, r3, #1
    lsl ip, ip, r3

    vdup.32 q10, ip
    vdup.32 q11, ip
    vld1.16 {d0-d3}, [r0,:128]
    vdup.32 q12, ip
    vdup.32 q13, ip

    vmlal.s16 q10, d0, d4
    vmlal.s16 q11, d1, d4
    vmlal.s16 q12, d2, d4
    vmlal.s16 q13, d3, d4
    vshl.s32 q10, q10, q15
    vshl.s32 q11, q11, q15
    vshl.s32 q12, q12, q15
    vshl.s32 q13, q13, q15

    vmovn.s32 d0, q10
    vmovn.s32 d1, q11
    vmovn.s32 d2, q12
    vmovn.s32 d3, q13
    vst1.16 {d0-d3}, [r0,:128]
    bx lr

        @.endfunc



        .align 2
        .globl _x264_coeff_last4_arm


        @.func _x264_coeff_last4_arm
_x264_coeff_last4_arm:
    ldrd r2, r3, [r0]
    subs r0, r3, #0
    movne r0, #2
    movne r2, r3
    lsrs r2, r2, #16
    addne r0, r0, #1
    bx lr

        @.endfunc

        .align 2
        .globl _x264_coeff_last8_arm


        @.func _x264_coeff_last8_arm
_x264_coeff_last8_arm:
    ldrd r2, r3, [r0, #8]
    orrs ip, r2, r3
    movne r0, #4
    ldrdeq r2, r3, [r0]
    moveq r0, #0
    tst r3, r3
    addne r0, #2
    movne r2, r3
    lsrs r2, r2, #16
    addne r0, r0, #1
    bx lr

        @.endfunc


        .align 2
        .globl _x264_coeff_last15_neon


        @.func _x264_coeff_last15_neon
_x264_coeff_last15_neon:
    sub r0, r0, #2
    vld1.64 {d0-d3}, [r0]
    vtst.16 q0, q0
    vtst.16 q1, q1
    vshrn.u16 d0, q0, #8
    vshrn.u16 d1, q1, #8
    vshrn.u16 d0, q0, #4
    vclz.i32 d0, d0
    mov ip, #7
    mov r3, #15 - 9
    vmov r0, r1, d0

    subs r1, ip, r1, lsr #2
    addge r0, r1, #15 - 8
    subslt r0, r3, r0, lsr #2
    movlt r0, #0
    bx lr

        @.endfunc
        .align 2
        .globl _x264_coeff_last16_neon


        @.func _x264_coeff_last16_neon
_x264_coeff_last16_neon:
    vld1.64 {d0-d3}, [r0,:128]
    vtst.16 q0, q0
    vtst.16 q1, q1
    vshrn.u16 d0, q0, #8
    vshrn.u16 d1, q1, #8
    vshrn.u16 d0, q0, #4
    vclz.i32 d0, d0
    mov ip, #7
    mov r3, #16 - 9
    vmov r0, r1, d0

    subs r1, ip, r1, lsr #2
    addge r0, r1, #16 - 8
    subslt r0, r3, r0, lsr #2
    movlt r0, #0
    bx lr

        @.endfunc

        .align 2
        .globl _x264_coeff_last64_neon


        @.func _x264_coeff_last64_neon
_x264_coeff_last64_neon:
    vld1.64 {d16-d19}, [r0,:128]!
    vqmovn.u16 d16, q8
    vqmovn.u16 d17, q9
    vld1.64 {d20-d23}, [r0,:128]!
    vqmovn.u16 d18, q10
    vqmovn.u16 d19, q11
    vld1.64 {d24-d27}, [r0,:128]!
    vqmovn.u16 d20, q12
    vqmovn.u16 d21, q13
    vld1.64 {d28-d31}, [r0,:128]!
    vqmovn.u16 d22, q14
    vqmovn.u16 d23, q15





         ldr r1, Literal_0

    vld1.64 {d0-d1}, [r1,:128]

    vtst.8 q8, q8
    vtst.8 q9, q9
    vtst.8 q10, q10
    vtst.8 q11, q11

    vand q8, q8, q0
    vand q9, q9, q0
    vand q10, q10, q0
    vand q11, q11, q0

    vpadd.u8 d0, d16, d17
    vpadd.u8 d1, d18, d19
    vpadd.u8 d2, d20, d21
    vpadd.u8 d3, d22, d23
    vpadd.u8 d0, d0, d1
    vpadd.u8 d1, d2, d3
    vpadd.u8 d0, d0, d1
    vclz.i32 d0, d0
    mov ip, #31
    vmov r0, r1, d0

    subs r1, ip, r1
    addge r0, r1, #32
    subslt r0, ip, r0
    movlt r0, #0
    bx lr

        @.endfunc
.text
.align 2
Literal_0:
 .word pmovmskb_byte

 #endif
