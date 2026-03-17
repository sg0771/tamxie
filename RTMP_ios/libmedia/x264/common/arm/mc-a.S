





#ifdef __arm__



.syntax unified


        @.arch armv7-a






@.fpu neon



















.text





        .align 2
        .globl _x264_prefetch_ref_arm


        @.func _x264_prefetch_ref_arm
_x264_prefetch_ref_arm:
    sub r2, r2, #1
    add r0, r0, #64
    and r2, r2, r1
    add r0, r0, r2, lsl #3
    add r2, r1, r1, lsl #1
    pld [r0]
    pld [r0, r1]
    pld [r0, r1, lsl #1]
    add r3, r0, r1, lsl #2
    pld [r0, r2]
    pld [r3]
    pld [r3, r1]
    pld [r3, r1, lsl #1]
    pld [r3, r2]
    bx lr

        @.endfunc



        .align 2
        .globl _x264_prefetch_fenc_arm


        @.func _x264_prefetch_fenc_arm
_x264_prefetch_fenc_arm:
    ldr ip, [sp]
    push {lr}
    and lr, ip, #3
    smulbb lr, lr, r1
    and ip, ip, #6
    smulbb ip, ip, r3
    add r0, r0, #64
    add r2, r2, #64
    add r0, r0, lr, lsl #2
    pld [r0]
    add lr, r0, r1, lsl #1
    pld [r0, r1]
    pld [lr]
    add r2, r2, ip, lsl #2
    pld [lr, r1]
    pld [r2]
    add ip, r2, r3, lsl #1
    pld [r2, r3]
    pld [ip]
    pld [ip, r3]
    pop {pc}

        @.endfunc



        .align 2
        .globl _x264_memcpy_aligned_neon


        @.func _x264_memcpy_aligned_neon
_x264_memcpy_aligned_neon:
    orr r3, r0, r1, lsr #1




         ldr ip, Literal_0

    and r3, r3, #0xc
    ldr pc, [ip, r3]

        @.endfunc


        .align 2


        @.func memcpy_aligned_16_16_neon
memcpy_aligned_16_16_neon:
    mov r3, r0
    .set r1align, 16 * 8
    .set r3align, 16 * 8
    tst r2, #16
    beq 32f
    sub r2, #16
    vld1.64 {d0-d1}, [r1,:r1align]!
    vst1.64 {d0-d1}, [r3,:r3align]!
32:
    tst r2, #32
    beq 640f
    sub r2, #32
    vld1.64 {d0-d3}, [r1,:r1align]!
    vst1.64 {d0-d3}, [r3,:r3align]!
640:
    cmp r2, #0
    beq 1f
64:
    subs r2, #64
    vld1.64 {d0-d3}, [r1,:r1align]!
    vld1.64 {d4-d7}, [r1,:r1align]!
    vst1.64 {d0-d3}, [r3,:r3align]!
    vst1.64 {d4-d7}, [r3,:r3align]!
    bgt 64b
1:
    bx lr

        @.endfunc
        .align 2


        @.func memcpy_aligned_8_16_neon
memcpy_aligned_8_16_neon:
    mov r3, r0
    .set r1align, 16 * 8
    .set r3align, 8 * 8
    tst r2, #16
    beq 32f
    sub r2, #16
    vld1.64 {d0-d1}, [r1,:r1align]!
    vst1.64 {d0-d1}, [r3,:r3align]!
32:
    tst r2, #32
    beq 640f
    sub r2, #32
    vld1.64 {d0-d3}, [r1,:r1align]!
    vst1.64 {d0-d3}, [r3,:r3align]!
640:
    cmp r2, #0
    beq 1f
64:
    subs r2, #64
    vld1.64 {d0-d3}, [r1,:r1align]!
    vld1.64 {d4-d7}, [r1,:r1align]!
    vst1.64 {d0-d3}, [r3,:r3align]!
    vst1.64 {d4-d7}, [r3,:r3align]!
    bgt 64b
1:
    bx lr

        @.endfunc
        .align 2


        @.func memcpy_aligned_16_8_neon
memcpy_aligned_16_8_neon:
    mov r3, r0
    .set r1align, 8 * 8
    .set r3align, 16 * 8
    tst r2, #16
    beq 32f
    sub r2, #16
    vld1.64 {d0-d1}, [r1,:r1align]!
    vst1.64 {d0-d1}, [r3,:r3align]!
32:
    tst r2, #32
    beq 640f
    sub r2, #32
    vld1.64 {d0-d3}, [r1,:r1align]!
    vst1.64 {d0-d3}, [r3,:r3align]!
640:
    cmp r2, #0
    beq 1f
64:
    subs r2, #64
    vld1.64 {d0-d3}, [r1,:r1align]!
    vld1.64 {d4-d7}, [r1,:r1align]!
    vst1.64 {d0-d3}, [r3,:r3align]!
    vst1.64 {d4-d7}, [r3,:r3align]!
    bgt 64b
1:
    bx lr

        @.endfunc
        .align 2


        @.func memcpy_aligned_8_8_neon
memcpy_aligned_8_8_neon:
    mov r3, r0
    sub r2, #16
    vld1.64 {d0}, [r1,:64]!
    vst1.64 {d0}, [r3,:64]!
    .set r1align, 128
    .set r3align, 128
    tst r2, #16
    beq 32f
    sub r2, #16
    vld1.64 {d0-d1}, [r1,:r1align]!
    vst1.64 {d0-d1}, [r3,:r3align]!
32:
    tst r2, #32
    beq 640f
    sub r2, #32
    vld1.64 {d0-d3}, [r1,:r1align]!
    vst1.64 {d0-d3}, [r3,:r3align]!
640:
    cmp r2, #0
    beq 1f
64:
    subs r2, #64
    vld1.64 {d0-d3}, [r1,:r1align]!
    vld1.64 {d4-d7}, [r1,:r1align]!
    vst1.64 {d0-d3}, [r3,:r3align]!
    vst1.64 {d4-d7}, [r3,:r3align]!
    bgt 64b
1:
    vld1.64 {d0}, [r1,:64]!
    vst1.64 {d0}, [r3,:64]!
    bx lr

        @.endfunc

.const_data
memcpy_table:
.word memcpy_aligned_16_16_neon
.word memcpy_aligned_16_8_neon
.word memcpy_aligned_8_16_neon
.word memcpy_aligned_8_8_neon
.text

@.ltorg
.align 2
Literal_0:
 .word memcpy_table


        .align 2
        .globl _x264_memzero_aligned_neon


        @.func _x264_memzero_aligned_neon
_x264_memzero_aligned_neon:
    vmov.i8 q0, #0
    vmov.i8 q1, #0
memzero_loop:
    subs r1, #128

    vst1.64 {d0-d3}, [r0,:128]!

    vst1.64 {d0-d3}, [r0,:128]!

    vst1.64 {d0-d3}, [r0,:128]!

    vst1.64 {d0-d3}, [r0,:128]!
    bgt memzero_loop
    bx lr

        @.endfunc






        .align 2
        .globl _x264_pixel_avg_4x2_neon


        @.func _x264_pixel_avg_4x2_neon
_x264_pixel_avg_4x2_neon:
    ldr ip, [sp, #8]
    push {r4-r6,lr}
    cmp ip, #32
    ldrd r4, r5, [sp, #16]
    mov lr, #2
    beq x264_pixel_avg_w4_neon
    rsbs r6, ip, #64
    blt x264_pixel_avg_weight_w4_add_sub_neon
    cmp ip, #0
    bge x264_pixel_avg_weight_w4_add_add_neon
    b x264_pixel_avg_weight_w4_sub_add_neon

        @.endfunc
        .align 2
        .globl _x264_pixel_avg_4x4_neon


        @.func _x264_pixel_avg_4x4_neon
_x264_pixel_avg_4x4_neon:
    ldr ip, [sp, #8]
    push {r4-r6,lr}
    cmp ip, #32
    ldrd r4, r5, [sp, #16]
    mov lr, #4
    beq x264_pixel_avg_w4_neon
    rsbs r6, ip, #64
    blt x264_pixel_avg_weight_w4_add_sub_neon
    cmp ip, #0
    bge x264_pixel_avg_weight_w4_add_add_neon
    b x264_pixel_avg_weight_w4_sub_add_neon

        @.endfunc
        .align 2
        .globl _x264_pixel_avg_4x8_neon


        @.func _x264_pixel_avg_4x8_neon
_x264_pixel_avg_4x8_neon:
    ldr ip, [sp, #8]
    push {r4-r6,lr}
    cmp ip, #32
    ldrd r4, r5, [sp, #16]
    mov lr, #8
    beq x264_pixel_avg_w4_neon
    rsbs r6, ip, #64
    blt x264_pixel_avg_weight_w4_add_sub_neon
    cmp ip, #0
    bge x264_pixel_avg_weight_w4_add_add_neon
    b x264_pixel_avg_weight_w4_sub_add_neon

        @.endfunc
        .align 2
        .globl _x264_pixel_avg_4x16_neon


        @.func _x264_pixel_avg_4x16_neon
_x264_pixel_avg_4x16_neon:
    ldr ip, [sp, #8]
    push {r4-r6,lr}
    cmp ip, #32
    ldrd r4, r5, [sp, #16]
    mov lr, #16
    beq x264_pixel_avg_w4_neon
    rsbs r6, ip, #64
    blt x264_pixel_avg_weight_w4_add_sub_neon
    cmp ip, #0
    bge x264_pixel_avg_weight_w4_add_add_neon
    b x264_pixel_avg_weight_w4_sub_add_neon

        @.endfunc
        .align 2
        .globl _x264_pixel_avg_8x4_neon


        @.func _x264_pixel_avg_8x4_neon
_x264_pixel_avg_8x4_neon:
    ldr ip, [sp, #8]
    push {r4-r6,lr}
    cmp ip, #32
    ldrd r4, r5, [sp, #16]
    mov lr, #4
    beq x264_pixel_avg_w8_neon
    rsbs r6, ip, #64
    blt x264_pixel_avg_weight_w8_add_sub_neon
    cmp ip, #0
    bge x264_pixel_avg_weight_w8_add_add_neon
    b x264_pixel_avg_weight_w8_sub_add_neon

        @.endfunc
        .align 2
        .globl _x264_pixel_avg_8x8_neon


        @.func _x264_pixel_avg_8x8_neon
_x264_pixel_avg_8x8_neon:
    ldr ip, [sp, #8]
    push {r4-r6,lr}
    cmp ip, #32
    ldrd r4, r5, [sp, #16]
    mov lr, #8
    beq x264_pixel_avg_w8_neon
    rsbs r6, ip, #64
    blt x264_pixel_avg_weight_w8_add_sub_neon
    cmp ip, #0
    bge x264_pixel_avg_weight_w8_add_add_neon
    b x264_pixel_avg_weight_w8_sub_add_neon

        @.endfunc
        .align 2
        .globl _x264_pixel_avg_8x16_neon


        @.func _x264_pixel_avg_8x16_neon
_x264_pixel_avg_8x16_neon:
    ldr ip, [sp, #8]
    push {r4-r6,lr}
    cmp ip, #32
    ldrd r4, r5, [sp, #16]
    mov lr, #16
    beq x264_pixel_avg_w8_neon
    rsbs r6, ip, #64
    blt x264_pixel_avg_weight_w8_add_sub_neon
    cmp ip, #0
    bge x264_pixel_avg_weight_w8_add_add_neon
    b x264_pixel_avg_weight_w8_sub_add_neon

        @.endfunc
        .align 2
        .globl _x264_pixel_avg_16x8_neon


        @.func _x264_pixel_avg_16x8_neon
_x264_pixel_avg_16x8_neon:
    ldr ip, [sp, #8]
    push {r4-r6,lr}
    cmp ip, #32
    ldrd r4, r5, [sp, #16]
    mov lr, #8
    beq x264_pixel_avg_w16_neon
    rsbs r6, ip, #64
    blt x264_pixel_avg_weight_w16_add_sub_neon
    cmp ip, #0
    bge x264_pixel_avg_weight_w16_add_add_neon
    b x264_pixel_avg_weight_w16_sub_add_neon

        @.endfunc
        .align 2
        .globl _x264_pixel_avg_16x16_neon


        @.func _x264_pixel_avg_16x16_neon
_x264_pixel_avg_16x16_neon:
    ldr ip, [sp, #8]
    push {r4-r6,lr}
    cmp ip, #32
    ldrd r4, r5, [sp, #16]
    mov lr, #16
    beq x264_pixel_avg_w16_neon
    rsbs r6, ip, #64
    blt x264_pixel_avg_weight_w16_add_sub_neon
    cmp ip, #0
    bge x264_pixel_avg_weight_w16_add_add_neon
    b x264_pixel_avg_weight_w16_sub_add_neon

        @.endfunc














        .align 2


        @.func x264_pixel_avg_weight_w4_add_add_neon
x264_pixel_avg_weight_w4_add_add_neon:
    vdup.8 d30, ip
    vdup.8 d31, r6
1:
    subs lr, lr, #2
    vld1.32 {d0[]}, [r2], r3
    vld1.32 {d1[]}, [r4], r5
    vmull.u8 q8, d0, d30
    vmlal.u8 q8, d1, d31
    vld1.32 {d2[]}, [r2], r3
    vld1.32 {d3[]}, [r4], r5
    vqrshrun.s16 d0, q8, #6
    vmull.u8 q9, d2, d30
    vmlal.u8 q9, d3, d31
    vst1.32 {d0[0]}, [r0,:32], r1
    vqrshrun.s16 d1, q9, #6
    vst1.32 {d1[0]}, [r0,:32], r1
    bgt 1b
    pop {r4-r6,pc}

        @.endfunc

        .align 2


        @.func x264_pixel_avg_weight_w8_add_add_neon
x264_pixel_avg_weight_w8_add_add_neon:
    vdup.8 d30, ip
    vdup.8 d31, r6
1:
    subs lr, lr, #4
    vld1.32 {d0}, [r2], r3
    vld1.32 {d1}, [r4], r5
    vmull.u8 q8, d0, d30
    vmlal.u8 q8, d1, d31
    vld1.32 {d2}, [r2], r3
    vld1.32 {d3}, [r4], r5
    vmull.u8 q9, d2, d30
    vmlal.u8 q9, d3, d31
    vld1.32 {d4}, [r2], r3
    vld1.32 {d5}, [r4], r5
    vmull.u8 q10, d4, d30
    vmlal.u8 q10, d5, d31
    vld1.32 {d6}, [r2], r3
    vld1.32 {d7}, [r4], r5
    vmull.u8 q11, d6, d30
    vmlal.u8 q11, d7, d31
    vqrshrun.s16 d0, q8, #6
    vqrshrun.s16 d1, q9, #6
    vqrshrun.s16 d2, q10, #6
    vqrshrun.s16 d3, q11, #6
    vst1.64 {d0}, [r0,:64], r1
    vst1.64 {d1}, [r0,:64], r1
    vst1.64 {d2}, [r0,:64], r1
    vst1.64 {d3}, [r0,:64], r1
    bgt 1b
    pop {r4-r6,pc}

        @.endfunc

        .align 2


        @.func x264_pixel_avg_weight_w16_add_add_neon
x264_pixel_avg_weight_w16_add_add_neon:
    vdup.8 d30, ip
    vdup.8 d31, r6
1:
    subs lr, lr, #2
    vld1.32 {d0-d1}, [r2], r3
    vld1.32 {d2-d3}, [r4], r5
    vmull.u8 q8, d0, d30
    vmlal.u8 q8, d2, d31
    vmull.u8 q9, d1, d30
    vmlal.u8 q9, d3, d31
    vld1.32 {d4-d5}, [r2], r3
    vld1.32 {d6-d7}, [r4], r5
    vmull.u8 q10, d4, d30
    vmlal.u8 q10, d6, d31
    vmull.u8 q11, d5, d30
    vmlal.u8 q11, d7, d31
    vqrshrun.s16 d0, q8, #6
    vqrshrun.s16 d1, q9, #6
    vqrshrun.s16 d2, q10, #6
    vqrshrun.s16 d3, q11, #6
    vst1.64 {d0-d1}, [r0,:128], r1
    vst1.64 {d2-d3}, [r0,:128], r1
    bgt 1b
    pop {r4-r6,pc}

        @.endfunc
        .align 2


        @.func x264_pixel_avg_weight_w4_add_sub_neon
x264_pixel_avg_weight_w4_add_sub_neon:
    rsb r6, #0
    vdup.8 d30, ip
    vdup.8 d31, r6
1:
    subs lr, lr, #2
    vld1.32 {d0[]}, [r2], r3
    vld1.32 {d1[]}, [r4], r5
    vmull.u8 q8, d0, d30
    vmlsl.u8 q8, d1, d31
    vld1.32 {d2[]}, [r2], r3
    vld1.32 {d3[]}, [r4], r5
    vqrshrun.s16 d0, q8, #6
    vmull.u8 q9, d2, d30
    vmlsl.u8 q9, d3, d31
    vst1.32 {d0[0]}, [r0,:32], r1
    vqrshrun.s16 d1, q9, #6
    vst1.32 {d1[0]}, [r0,:32], r1
    bgt 1b
    pop {r4-r6,pc}

        @.endfunc

        .align 2


        @.func x264_pixel_avg_weight_w8_add_sub_neon
x264_pixel_avg_weight_w8_add_sub_neon:
    rsb r6, #0
    vdup.8 d30, ip
    vdup.8 d31, r6
1:
    subs lr, lr, #4
    vld1.32 {d0}, [r2], r3
    vld1.32 {d1}, [r4], r5
    vmull.u8 q8, d0, d30
    vmlsl.u8 q8, d1, d31
    vld1.32 {d2}, [r2], r3
    vld1.32 {d3}, [r4], r5
    vmull.u8 q9, d2, d30
    vmlsl.u8 q9, d3, d31
    vld1.32 {d4}, [r2], r3
    vld1.32 {d5}, [r4], r5
    vmull.u8 q10, d4, d30
    vmlsl.u8 q10, d5, d31
    vld1.32 {d6}, [r2], r3
    vld1.32 {d7}, [r4], r5
    vmull.u8 q11, d6, d30
    vmlsl.u8 q11, d7, d31
    vqrshrun.s16 d0, q8, #6
    vqrshrun.s16 d1, q9, #6
    vqrshrun.s16 d2, q10, #6
    vqrshrun.s16 d3, q11, #6
    vst1.64 {d0}, [r0,:64], r1
    vst1.64 {d1}, [r0,:64], r1
    vst1.64 {d2}, [r0,:64], r1
    vst1.64 {d3}, [r0,:64], r1
    bgt 1b
    pop {r4-r6,pc}

        @.endfunc

        .align 2


        @.func x264_pixel_avg_weight_w16_add_sub_neon
x264_pixel_avg_weight_w16_add_sub_neon:
    rsb r6, #0
    vdup.8 d30, ip
    vdup.8 d31, r6
1:
    subs lr, lr, #2
    vld1.32 {d0-d1}, [r2], r3
    vld1.32 {d2-d3}, [r4], r5
    vmull.u8 q8, d0, d30
    vmlsl.u8 q8, d2, d31
    vmull.u8 q9, d1, d30
    vmlsl.u8 q9, d3, d31
    vld1.32 {d4-d5}, [r2], r3
    vld1.32 {d6-d7}, [r4], r5
    vmull.u8 q10, d4, d30
    vmlsl.u8 q10, d6, d31
    vmull.u8 q11, d5, d30
    vmlsl.u8 q11, d7, d31
    vqrshrun.s16 d0, q8, #6
    vqrshrun.s16 d1, q9, #6
    vqrshrun.s16 d2, q10, #6
    vqrshrun.s16 d3, q11, #6
    vst1.64 {d0-d1}, [r0,:128], r1
    vst1.64 {d2-d3}, [r0,:128], r1
    bgt 1b
    pop {r4-r6,pc}

        @.endfunc
        .align 2


        @.func x264_pixel_avg_weight_w4_sub_add_neon
x264_pixel_avg_weight_w4_sub_add_neon:
    rsb ip, #0
    vdup.8 d31, r6
    vdup.8 d30, ip
1:
    subs lr, lr, #2
    vld1.32 {d1[]}, [r4], r5
    vld1.32 {d0[]}, [r2], r3
    vmull.u8 q8, d1, d31
    vmlsl.u8 q8, d0, d30
    vld1.32 {d3[]}, [r4], r5
    vld1.32 {d2[]}, [r2], r3
    vqrshrun.s16 d0, q8, #6
    vmull.u8 q9, d3, d31
    vmlsl.u8 q9, d2, d30
    vst1.32 {d0[0]}, [r0,:32], r1
    vqrshrun.s16 d1, q9, #6
    vst1.32 {d1[0]}, [r0,:32], r1
    bgt 1b
    pop {r4-r6,pc}

        @.endfunc

        .align 2


        @.func x264_pixel_avg_weight_w8_sub_add_neon
x264_pixel_avg_weight_w8_sub_add_neon:
    rsb ip, #0
    vdup.8 d31, r6
    vdup.8 d30, ip
1:
    subs lr, lr, #4
    vld1.32 {d1}, [r4], r5
    vld1.32 {d0}, [r2], r3
    vmull.u8 q8, d1, d31
    vmlsl.u8 q8, d0, d30
    vld1.32 {d3}, [r4], r5
    vld1.32 {d2}, [r2], r3
    vmull.u8 q9, d3, d31
    vmlsl.u8 q9, d2, d30
    vld1.32 {d5}, [r4], r5
    vld1.32 {d4}, [r2], r3
    vmull.u8 q10, d5, d31
    vmlsl.u8 q10, d4, d30
    vld1.32 {d7}, [r4], r5
    vld1.32 {d6}, [r2], r3
    vmull.u8 q11, d7, d31
    vmlsl.u8 q11, d6, d30
    vqrshrun.s16 d0, q8, #6
    vqrshrun.s16 d1, q9, #6
    vqrshrun.s16 d2, q10, #6
    vqrshrun.s16 d3, q11, #6
    vst1.64 {d0}, [r0,:64], r1
    vst1.64 {d1}, [r0,:64], r1
    vst1.64 {d2}, [r0,:64], r1
    vst1.64 {d3}, [r0,:64], r1
    bgt 1b
    pop {r4-r6,pc}

        @.endfunc

        .align 2


        @.func x264_pixel_avg_weight_w16_sub_add_neon
x264_pixel_avg_weight_w16_sub_add_neon:
    rsb ip, #0
    vdup.8 d31, r6
    vdup.8 d30, ip
1:
    subs lr, lr, #2
    vld1.32 {d2-d3}, [r4], r5
    vld1.32 {d0-d1}, [r2], r3
    vmull.u8 q8, d2, d31
    vmlsl.u8 q8, d0, d30
    vmull.u8 q9, d3, d31
    vmlsl.u8 q9, d1, d30
    vld1.32 {d6-d7}, [r4], r5
    vld1.32 {d4-d5}, [r2], r3
    vmull.u8 q10, d6, d31
    vmlsl.u8 q10, d4, d30
    vmull.u8 q11, d7, d31
    vmlsl.u8 q11, d5, d30
    vqrshrun.s16 d0, q8, #6
    vqrshrun.s16 d1, q9, #6
    vqrshrun.s16 d2, q10, #6
    vqrshrun.s16 d3, q11, #6
    vst1.64 {d0-d1}, [r0,:128], r1
    vst1.64 {d2-d3}, [r0,:128], r1
    bgt 1b
    pop {r4-r6,pc}

        @.endfunc

        .align 2


        @.func x264_pixel_avg_w4_neon
x264_pixel_avg_w4_neon:
    subs lr, lr, #2
    vld1.32 {d0[]}, [r2], r3
    vld1.32 {d2[]}, [r4], r5
    vrhadd.u8 d0, d0, d2
    vld1.32 {d1[]}, [r2], r3
    vld1.32 {d3[]}, [r4], r5
    vrhadd.u8 d1, d1, d3
    vst1.32 {d0[0]}, [r0,:32], r1
    vst1.32 {d1[0]}, [r0,:32], r1
    bgt x264_pixel_avg_w4_neon
    pop {r4-r6,pc}

        @.endfunc

        .align 2


        @.func x264_pixel_avg_w8_neon
x264_pixel_avg_w8_neon:
    subs lr, lr, #4
    vld1.64 {d0}, [r2], r3
    vld1.64 {d2}, [r4], r5
    vrhadd.u8 d0, d0, d2
    vld1.64 {d1}, [r2], r3
    vld1.64 {d3}, [r4], r5
    vrhadd.u8 d1, d1, d3
    vst1.64 {d0}, [r0,:64], r1
    vld1.64 {d2}, [r2], r3
    vld1.64 {d4}, [r4], r5
    vrhadd.u8 d2, d2, d4
    vst1.64 {d1}, [r0,:64], r1
    vld1.64 {d3}, [r2], r3
    vld1.64 {d5}, [r4], r5
    vrhadd.u8 d3, d3, d5
    vst1.64 {d2}, [r0,:64], r1
    vst1.64 {d3}, [r0,:64], r1
    bgt x264_pixel_avg_w8_neon
    pop {r4-r6,pc}

        @.endfunc

        .align 2


        @.func x264_pixel_avg_w16_neon
x264_pixel_avg_w16_neon:
    subs lr, lr, #4
    vld1.64 {d0-d1}, [r2], r3
    vld1.64 {d2-d3}, [r4], r5
    vrhadd.u8 q0, q0, q1
    vld1.64 {d2-d3}, [r2], r3
    vld1.64 {d4-d5}, [r4], r5
    vrhadd.u8 q1, q1, q2
    vst1.64 {d0-d1}, [r0,:128], r1
    vld1.64 {d4-d5}, [r2], r3
    vld1.64 {d6-d7}, [r4], r5
    vrhadd.u8 q2, q2, q3
    vst1.64 {d2-d3}, [r0,:128], r1
    vld1.64 {d6-d7}, [r2], r3
    vld1.64 {d0-d1}, [r4], r5
    vrhadd.u8 q3, q3, q0
    vst1.64 {d4-d5}, [r0,:128], r1
    vst1.64 {d6-d7}, [r0,:128], r1
    bgt x264_pixel_avg_w16_neon
    pop {r4-r6,pc}

        @.endfunc


        .align 2
        .globl _x264_pixel_avg2_w4_neon


        @.func _x264_pixel_avg2_w4_neon
_x264_pixel_avg2_w4_neon:
    ldr ip, [sp, #4]
    push {lr}
    ldr lr, [sp, #4]
avg2_w4_loop:
    subs ip, ip, #2
    vld1.32 {d0[]}, [r2], r3
    vld1.32 {d2[]}, [lr], r3
    vrhadd.u8 d0, d0, d2
    vld1.32 {d1[]}, [r2], r3
    vld1.32 {d3[]}, [lr], r3
    vrhadd.u8 d1, d1, d3
    vst1.32 {d0[0]}, [r0,:32], r1
    vst1.32 {d1[0]}, [r0,:32], r1
    bgt avg2_w4_loop
    pop {pc}

        @.endfunc

        .align 2
        .globl _x264_pixel_avg2_w8_neon


        @.func _x264_pixel_avg2_w8_neon
_x264_pixel_avg2_w8_neon:
    ldr ip, [sp, #4]
    push {lr}
    ldr lr, [sp, #4]
avg2_w8_loop:
    subs ip, ip, #2
    vld1.64 {d0}, [r2], r3
    vld1.64 {d2}, [lr], r3
    vrhadd.u8 d0, d0, d2
    vld1.64 {d1}, [r2], r3
    vld1.64 {d3}, [lr], r3
    vrhadd.u8 d1, d1, d3
    vst1.64 {d0}, [r0,:64], r1
    vst1.64 {d1}, [r0,:64], r1
    bgt avg2_w8_loop
    pop {pc}

        @.endfunc

        .align 2
        .globl _x264_pixel_avg2_w16_neon


        @.func _x264_pixel_avg2_w16_neon
_x264_pixel_avg2_w16_neon:
    ldr ip, [sp, #4]
    push {lr}
    ldr lr, [sp, #4]
avg2_w16_loop:
    subs ip, ip, #2
    vld1.64 {d0-d1}, [r2], r3
    vld1.64 {d2-d3}, [lr], r3
    vrhadd.u8 q0, q0, q1
    vld1.64 {d4-d5}, [r2], r3
    vld1.64 {d6-d7}, [lr], r3
    vrhadd.u8 q2, q2, q3
    vst1.64 {d0-d1}, [r0,:128], r1
    vst1.64 {d4-d5}, [r0,:128], r1
    bgt avg2_w16_loop
    pop {pc}

        @.endfunc

        .align 2
        .globl _x264_pixel_avg2_w20_neon


        @.func _x264_pixel_avg2_w20_neon
_x264_pixel_avg2_w20_neon:
    ldr ip, [sp, #4]
    push {lr}
    sub r1, r1, #16
    ldr lr, [sp, #4]
avg2_w20_loop:
    subs ip, ip, #2
    vld1.64 {d0-d2}, [r2], r3
    vld1.64 {d4-d6}, [lr], r3
    vrhadd.u8 q0, q0, q2
    vrhadd.u8 d2, d2, d6
    vld1.64 {d4-d6}, [r2], r3
    vld1.64 {d16-d18},[lr], r3
    vrhadd.u8 q2, q2, q8
    vst1.64 {d0-d1}, [r0,:128]!
    vrhadd.u8 d6, d6, d18
    vst1.32 {d2[0]}, [r0,:32], r1
    vst1.64 {d4-d5}, [r0,:128]!
    vst1.32 {d6[0]}, [r0,:32], r1
    bgt avg2_w20_loop
    pop {pc}

        @.endfunc





        .align 2
        .globl _x264_mc_weight_w20_neon


        @.func _x264_mc_weight_w20_neon
_x264_mc_weight_w20_neon:
    push {r4-r5,lr}
    ldr r4, [sp, #4*3]
    ldr ip, [sp, #4*3+4]
    ldr lr, [r4, #32]
    ldrd r4, r5, [r4, #32+4]
    vdup.8 d0, r4
    vdup.16 q1, r5
    rsb lr, lr, #0
    vdup.16 q2, lr
    sub r1, #16
weight20_loop:
    subs ip, #2
    vld1.8 {d17-d19}, [r2], r3
    vmull.u8 q10, d17, d0
    vmull.u8 q11, d18, d0
    vld1.8 {d16-d18}, [r2], r3
    vmull.u8 q12, d16, d0
    vmull.u8 q13, d17, d0
    vtrn.32 d19, d18
    vmull.u8 q14, d19, d0
    vrshl.s16 q10, q10, q2
    vrshl.s16 q11, q11, q2
    vrshl.s16 q12, q12, q2
    vrshl.s16 q13, q13, q2
    vrshl.s16 q14, q14, q2
    vadd.s16 q10, q10, q1
    vadd.s16 q11, q11, q1
    vadd.s16 q12, q12, q1
    vadd.s16 q13, q13, q1
    vadd.s16 q14, q14, q1
    vqmovun.s16 d16, q10
    vqmovun.s16 d17, q11
    vqmovun.s16 d18, q12
    vqmovun.s16 d19, q13
    vqmovun.s16 d20, q14
    vst1.8 {d16-d17}, [r0,:128]!
    vst1.32 {d20[0]}, [r0,:32], r1
    vst1.8 {d18-d19}, [r0,:128]!
    vst1.32 {d20[1]}, [r0,:32], r1
    bgt weight20_loop
    pop {r4-r5,pc}

        @.endfunc

        .align 2
        .globl _x264_mc_weight_w16_neon


        @.func _x264_mc_weight_w16_neon
_x264_mc_weight_w16_neon:
    push {r4-r5,lr}
    ldr r4, [sp, #4*3]
    ldr ip, [sp, #4*3+4]
    ldr lr, [r4, #32]
    ldrd r4, r5, [r4, #32+4]
    vdup.8 d0, r4
    vdup.16 q1, r5
    rsb lr, lr, #0
    vdup.16 q2, lr
weight16_loop:
    subs ip, #2
    vld1.8 {d16-d17}, [r2], r3
    vld1.8 {d18-d19}, [r2], r3
    vmull.u8 q10, d16, d0
    vmull.u8 q11, d17, d0
    vmull.u8 q12, d18, d0
    vmull.u8 q13, d19, d0
    vrshl.s16 q10, q10, q2
    vrshl.s16 q11, q11, q2
    vrshl.s16 q12, q12, q2
    vrshl.s16 q13, q13, q2
    vadd.s16 q10, q10, q1
    vadd.s16 q11, q11, q1
    vadd.s16 q12, q12, q1
    vadd.s16 q13, q13, q1
    vqmovun.s16 d16, q10
    vqmovun.s16 d17, q11
    vqmovun.s16 d18, q12
    vqmovun.s16 d19, q13
    vst1.8 {d16-d17}, [r0,:128], r1
    vst1.8 {d18-d19}, [r0,:128], r1
    bgt weight16_loop
    pop {r4-r5,pc}

        @.endfunc

        .align 2
        .globl _x264_mc_weight_w8_neon


        @.func _x264_mc_weight_w8_neon
_x264_mc_weight_w8_neon:
    push {r4-r5,lr}
    ldr r4, [sp, #4*3]
    ldr ip, [sp, #4*3+4]
    ldr lr, [r4, #32]
    ldrd r4, r5, [r4, #32+4]
    vdup.8 d0, r4
    vdup.16 q1, r5
    rsb lr, lr, #0
    vdup.16 q2, lr
weight8_loop:
    subs ip, #2
    vld1.8 {d16}, [r2], r3
    vld1.8 {d18}, [r2], r3
    vmull.u8 q8, d16, d0
    vmull.u8 q9, d18, d0
    vrshl.s16 q8, q8, q2
    vrshl.s16 q9, q9, q2
    vadd.s16 q8, q8, q1
    vadd.s16 q9, q9, q1
    vqmovun.s16 d16, q8
    vqmovun.s16 d18, q9
    vst1.8 {d16}, [r0,:64], r1
    vst1.8 {d18}, [r0,:64], r1
    bgt weight8_loop
    pop {r4-r5,pc}

        @.endfunc

        .align 2
        .globl _x264_mc_weight_w4_neon


        @.func _x264_mc_weight_w4_neon
_x264_mc_weight_w4_neon:
    push {r4-r5,lr}
    ldr r4, [sp, #4*3]
    ldr ip, [sp, #4*3+4]
    ldr lr, [r4, #32]
    ldrd r4, r5, [r4, #32+4]
    vdup.8 d0, r4
    vdup.16 q1, r5
    rsb lr, lr, #0
    vdup.16 q2, lr
weight4_loop:
    subs ip, #2
    vld1.32 {d16[0]}, [r2], r3
    vld1.32 {d16[1]}, [r2], r3
    vmull.u8 q8, d16, d0
    vrshl.s16 q8, q8, q2
    vadd.s16 q8, q8, q1
    vqmovun.s16 d16, q8
    vst1.32 {d16[0]}, [r0], r1
    vst1.32 {d16[1]}, [r0], r1
    bgt weight4_loop
    pop {r4-r5,pc}

        @.endfunc

        .align 2
        .globl _x264_mc_weight_w20_nodenom_neon


        @.func _x264_mc_weight_w20_nodenom_neon
_x264_mc_weight_w20_nodenom_neon:
    push {r4-r5,lr}
    ldr r4, [sp, #4*3]
    ldr ip, [sp, #4*3+4]
    ldrd r4, r5, [r4, #32+4]
    vdup.8 d0, r4
    vdup.16 q1, r5
    sub r1, #16
weight20_nodenom_loop:
    subs ip, #2
    vld1.8 {d26-d28}, [r2], r3
    vmov q8, q1
    vmov q9, q1
    vld1.8 {d29-d31}, [r2], r3
    vmov q10, q1
    vmov q11, q1
    vmov q12, q1
    vtrn.32 d28, d31
    vmlal.u8 q8, d26, d0
    vmlal.u8 q9, d27, d0
    vmlal.u8 q10, d29, d0
    vmlal.u8 q11, d30, d0
    vmlal.u8 q12, d28, d0
    vqmovun.s16 d16, q8
    vqmovun.s16 d17, q9
    vqmovun.s16 d18, q10
    vqmovun.s16 d19, q11
    vqmovun.s16 d20, q12
    vst1.8 {d16-d17}, [r0,:128]!
    vst1.32 {d20[0]}, [r0,:32], r1
    vst1.8 {d18-d19}, [r0,:128]!
    vst1.32 {d20[1]}, [r0,:32], r1
    bgt weight20_nodenom_loop
    pop {r4-r5,pc}

        @.endfunc

        .align 2
        .globl _x264_mc_weight_w16_nodenom_neon


        @.func _x264_mc_weight_w16_nodenom_neon
_x264_mc_weight_w16_nodenom_neon:
    push {r4-r5,lr}
    ldr r4, [sp, #4*3]
    ldr ip, [sp, #4*3+4]
    ldrd r4, r5, [r4, #32+4]
    vdup.8 d0, r4
    vdup.16 q1, r5
weight16_nodenom_loop:
    subs ip, #2
    vld1.8 {d16-d17}, [r2], r3
    vld1.8 {d18-d19}, [r2], r3
    vmov q12, q1
    vmov q13, q1
    vmov q14, q1
    vmov q15, q1
    vmlal.u8 q12, d16, d0
    vmlal.u8 q13, d17, d0
    vmlal.u8 q14, d18, d0
    vmlal.u8 q15, d19, d0
    vqmovun.s16 d16, q12
    vqmovun.s16 d17, q13
    vqmovun.s16 d18, q14
    vqmovun.s16 d19, q15
    vst1.8 {d16-d17}, [r0,:128], r1
    vst1.8 {d18-d19}, [r0,:128], r1
    bgt weight16_nodenom_loop
    pop {r4-r5,pc}

        @.endfunc

        .align 2
        .globl _x264_mc_weight_w8_nodenom_neon


        @.func _x264_mc_weight_w8_nodenom_neon
_x264_mc_weight_w8_nodenom_neon:
    push {r4-r5,lr}
    ldr r4, [sp, #4*3]
    ldr ip, [sp, #4*3+4]
    ldrd r4, r5, [r4, #32+4]
    vdup.8 d0, r4
    vdup.16 q1, r5
weight8_nodenom_loop:
    subs ip, #2
    vld1.8 {d16}, [r2], r3
    vld1.8 {d18}, [r2], r3
    vmov q10, q1
    vmov q11, q1
    vmlal.u8 q10, d16, d0
    vmlal.u8 q11, d18, d0
    vqmovun.s16 d16, q10
    vqmovun.s16 d17, q11
    vst1.8 {d16}, [r0,:64], r1
    vst1.8 {d17}, [r0,:64], r1
    bgt weight8_nodenom_loop
    pop {r4-r5,pc}

        @.endfunc

        .align 2
        .globl _x264_mc_weight_w4_nodenom_neon


        @.func _x264_mc_weight_w4_nodenom_neon
_x264_mc_weight_w4_nodenom_neon:
    push {r4-r5,lr}
    ldr r4, [sp, #4*3]
    ldr ip, [sp, #4*3+4]
    ldrd r4, r5, [r4, #32+4]
    vdup.8 d0, r4
    vdup.16 q1, r5
weight4_nodenom_loop:
    subs ip, #2
    vld1.32 {d16[0]}, [r2], r3
    vld1.32 {d16[1]}, [r2], r3
    vmov q10, q1
    vmlal.u8 q10, d16, d0
    vqmovun.s16 d16, q10
    vst1.32 {d16[0]}, [r0], r1
    vst1.32 {d16[1]}, [r0], r1
    bgt weight4_nodenom_loop
    pop {r4-r5,pc}

        @.endfunc



        .align 2
        .globl _x264_mc_weight_w20_offsetadd_neon


        @.func _x264_mc_weight_w20_offsetadd_neon
_x264_mc_weight_w20_offsetadd_neon:
    push {lr}
    ldr lr, [sp, #4]
    ldr ip, [sp, #8]
    ldr lr, [lr]
    vdup.8 q1, lr
weight20_offsetadd_loop:
    subs ip, #2
    vld1.8 {d16-d18}, [r2], r3
    vld1.8 {d19-d21}, [r2], r3
    vqadd.u8 q8, q8, q1
    vqadd.u8 q9, q9, q1
    vqadd.u8 q10, q10, q1
    vst1.8 {d16-d18}, [r0,:64], r1
    vst1.8 {d19-d21}, [r0,:64], r1
    bgt weight20_offsetadd_loop
    pop {pc}

        @.endfunc

        .align 2
        .globl _x264_mc_weight_w16_offsetadd_neon


        @.func _x264_mc_weight_w16_offsetadd_neon
_x264_mc_weight_w16_offsetadd_neon:
    push {lr}
    ldr lr, [sp, #4]
    ldr ip, [sp, #8]
    ldr lr, [lr]
    vdup.8 q1, lr
weight16_offsetadd_loop:
    subs ip, #2
    vld1.8 {d16-d17}, [r2], r3
    vld1.8 {d18-d19}, [r2], r3
    vqadd.u8 q8, q8, q1
    vqadd.u8 q9, q9, q1
    vst1.8 {d16-d17}, [r0,:128], r1
    vst1.8 {d18-d19}, [r0,:128], r1
    bgt weight16_offsetadd_loop
    pop {pc}

        @.endfunc

        .align 2
        .globl _x264_mc_weight_w8_offsetadd_neon


        @.func _x264_mc_weight_w8_offsetadd_neon
_x264_mc_weight_w8_offsetadd_neon:
    push {lr}
    ldr lr, [sp, #4]
    ldr ip, [sp, #8]
    ldr lr, [lr]
    vdup.8 q1, lr
weight8_offsetadd_loop:
    subs ip, #2
    vld1.8 {d16}, [r2], r3
    vld1.8 {d17}, [r2], r3
    vqadd.u8 q8, q8, q1
    vst1.8 {d16}, [r0,:64], r1
    vst1.8 {d17}, [r0,:64], r1
    bgt weight8_offsetadd_loop
    pop {pc}

        @.endfunc

        .align 2
        .globl _x264_mc_weight_w4_offsetadd_neon


        @.func _x264_mc_weight_w4_offsetadd_neon
_x264_mc_weight_w4_offsetadd_neon:
    push {lr}
    ldr lr, [sp, #4]
    ldr ip, [sp, #8]
    ldr lr, [lr]
    vdup.8 q1, lr
weight4_offsetadd_loop:
    subs ip, #2
    vld1.32 {d16[]}, [r2], r3
    vld1.32 {d17[]}, [r2], r3
    vqadd.u8 q8, q8, q1
    vst1.32 {d16[0]}, [r0], r1
    vst1.32 {d17[0]}, [r0], r1
    bgt weight4_offsetadd_loop
    pop {pc}

        @.endfunc
        .align 2
        .globl _x264_mc_weight_w20_offsetsub_neon


        @.func _x264_mc_weight_w20_offsetsub_neon
_x264_mc_weight_w20_offsetsub_neon:
    push {lr}
    ldr lr, [sp, #4]
    ldr ip, [sp, #8]
    ldr lr, [lr]
    vdup.8 q1, lr
weight20_offsetsub_loop:
    subs ip, #2
    vld1.8 {d16-d18}, [r2], r3
    vld1.8 {d19-d21}, [r2], r3
    vqsub.u8 q8, q8, q1
    vqsub.u8 q9, q9, q1
    vqsub.u8 q10, q10, q1
    vst1.8 {d16-d18}, [r0,:64], r1
    vst1.8 {d19-d21}, [r0,:64], r1
    bgt weight20_offsetsub_loop
    pop {pc}

        @.endfunc

        .align 2
        .globl _x264_mc_weight_w16_offsetsub_neon


        @.func _x264_mc_weight_w16_offsetsub_neon
_x264_mc_weight_w16_offsetsub_neon:
    push {lr}
    ldr lr, [sp, #4]
    ldr ip, [sp, #8]
    ldr lr, [lr]
    vdup.8 q1, lr
weight16_offsetsub_loop:
    subs ip, #2
    vld1.8 {d16-d17}, [r2], r3
    vld1.8 {d18-d19}, [r2], r3
    vqsub.u8 q8, q8, q1
    vqsub.u8 q9, q9, q1
    vst1.8 {d16-d17}, [r0,:128], r1
    vst1.8 {d18-d19}, [r0,:128], r1
    bgt weight16_offsetsub_loop
    pop {pc}

        @.endfunc

        .align 2
        .globl _x264_mc_weight_w8_offsetsub_neon


        @.func _x264_mc_weight_w8_offsetsub_neon
_x264_mc_weight_w8_offsetsub_neon:
    push {lr}
    ldr lr, [sp, #4]
    ldr ip, [sp, #8]
    ldr lr, [lr]
    vdup.8 q1, lr
weight8_offsetsub_loop:
    subs ip, #2
    vld1.8 {d16}, [r2], r3
    vld1.8 {d17}, [r2], r3
    vqsub.u8 q8, q8, q1
    vst1.8 {d16}, [r0,:64], r1
    vst1.8 {d17}, [r0,:64], r1
    bgt weight8_offsetsub_loop
    pop {pc}

        @.endfunc

        .align 2
        .globl _x264_mc_weight_w4_offsetsub_neon


        @.func _x264_mc_weight_w4_offsetsub_neon
_x264_mc_weight_w4_offsetsub_neon:
    push {lr}
    ldr lr, [sp, #4]
    ldr ip, [sp, #8]
    ldr lr, [lr]
    vdup.8 q1, lr
weight4_offsetsub_loop:
    subs ip, #2
    vld1.32 {d16[]}, [r2], r3
    vld1.32 {d17[]}, [r2], r3
    vqsub.u8 q8, q8, q1
    vst1.32 {d16[0]}, [r0], r1
    vst1.32 {d17[0]}, [r0], r1
    bgt weight4_offsetsub_loop
    pop {pc}

        @.endfunc



        .align 2
        .globl _x264_mc_copy_w4_neon


        @.func _x264_mc_copy_w4_neon
_x264_mc_copy_w4_neon:
    ldr ip, [sp]
copy_w4_loop:
    subs ip, ip, #4
    vld1.32 {d0[]}, [r2], r3
    vld1.32 {d1[]}, [r2], r3
    vld1.32 {d2[]}, [r2], r3
    vld1.32 {d3[]}, [r2], r3
    vst1.32 {d0[0]}, [r0,:32], r1
    vst1.32 {d1[0]}, [r0,:32], r1
    vst1.32 {d2[0]}, [r0,:32], r1
    vst1.32 {d3[0]}, [r0,:32], r1
    bgt copy_w4_loop
    bx lr

        @.endfunc

        .align 2
        .globl _x264_mc_copy_w8_neon


        @.func _x264_mc_copy_w8_neon
_x264_mc_copy_w8_neon:
    ldr ip, [sp]
copy_w8_loop:
    subs ip, ip, #4
    vld1.32 {d0}, [r2], r3
    vld1.32 {d1}, [r2], r3
    vld1.32 {d2}, [r2], r3
    vld1.32 {d3}, [r2], r3
    vst1.32 {d0}, [r0,:64], r1
    vst1.32 {d1}, [r0,:64], r1
    vst1.32 {d2}, [r0,:64], r1
    vst1.32 {d3}, [r0,:64], r1
    bgt copy_w8_loop
    bx lr

        @.endfunc

        .align 2
        .globl _x264_mc_copy_w16_neon


        @.func _x264_mc_copy_w16_neon
_x264_mc_copy_w16_neon:
    ldr ip, [sp]
copy_w16_loop:
    subs ip, ip, #4
    vld1.32 {d0-d1}, [r2], r3
    vld1.32 {d2-d3}, [r2], r3
    vld1.32 {d4-d5}, [r2], r3
    vld1.32 {d6-d7}, [r2], r3
    vst1.32 {d0-d1}, [r0,:128], r1
    vst1.32 {d2-d3}, [r0,:128], r1
    vst1.32 {d4-d5}, [r0,:128], r1
    vst1.32 {d6-d7}, [r0,:128], r1
    bgt copy_w16_loop
    bx lr

        @.endfunc

        .align 2
        .globl _x264_mc_copy_w16_aligned_neon


        @.func _x264_mc_copy_w16_aligned_neon
_x264_mc_copy_w16_aligned_neon:
    ldr ip, [sp]
copy_w16_aligned_loop:
    subs ip, ip, #4
    vld1.32 {d0-d1}, [r2,:128], r3
    vld1.32 {d2-d3}, [r2,:128], r3
    vld1.32 {d4-d5}, [r2,:128], r3
    vld1.32 {d6-d7}, [r2,:128], r3
    vst1.32 {d0-d1}, [r0,:128], r1
    vst1.32 {d2-d3}, [r0,:128], r1
    vst1.32 {d4-d5}, [r0,:128], r1
    vst1.32 {d6-d7}, [r0,:128], r1
    bgt copy_w16_aligned_loop
    bx lr

        @.endfunc






        .align 2
        .globl _x264_mc_chroma_neon


        @.func _x264_mc_chroma_neon
_x264_mc_chroma_neon:
    push {r4-r8, lr}
    vpush {d8-d11}
    ldrd r4, r5, [sp, #56]
    ldrd r6, r7, [sp, #64]

    asr lr, r6, #3
    mul lr, r4, lr
    add r3, r3, r5, asr #2
    cmp r7, #4

    and r5, r5, #7
    and r6, r6, #7

    add r3, r3, lr
    bic r3, r3, #0x1

    pld [r3]
    pld [r3, r4]

    bgt mc_chroma_w8
    beq mc_chroma_w4



mc_chroma_w2:
    muls lr, r5, r6
    rsb r7, lr, r6, lsl #3
    rsb ip, lr, r5, lsl #3
    sub r5, lr, r5, lsl #3
    sub r5, r5, r6, lsl #3
    add r5, r5, #64

    beq 2f
    vld2.8 {d4-d5}, [r3], r4

    vdup.8 d0, r5
    vdup.8 d1, ip

    vdup.8 d2, r7
    vld2.8 {d8-d9}, [r3], r4
    vdup.8 d3, lr
    ldr r5, [sp, #72]
    vext.8 d6, d4, d6, #1
    vext.8 d7, d5, d7, #1
    vext.8 d10, d8, d10, #1
    vext.8 d11, d9, d11, #1

    .set st2, 2

    vtrn.32 d4, d6
    vtrn.32 d5, d7
    vtrn.32 d8, d10
    vtrn.32 d9, d11

    vtrn.32 d0, d1
    vtrn.32 d2, d3

1:

    vmull.u8 q8, d4, d0
    vmlal.u8 q8, d8, d2
    vmull.u8 q9, d5, d0
    vmlal.u8 q9, d9, d2

    vld2.8 {d4-d5}, [r3], r4

    vext.8 d6, d4, d6, #1
    vext.8 d7, d5, d7, #1

    vadd.i16 d16, d16, d17
    vadd.i16 d17, d18, d19

    vtrn.32 d4, d6
    vtrn.32 d5, d7

    vmull.u8 q10, d8, d0
    vmlal.u8 q10, d4, d2
    vmull.u8 q11, d9, d0
    vmlal.u8 q11, d5, d2

    vld2.8 {d8-d9}, [r3], r4

    vrshrn.u16 d16, q8, #6

    vext.8 d10, d8, d10, #1
    vext.8 d11, d9, d11, #1

    vadd.i16 d18, d20, d21
    vadd.i16 d19, d22, d23

    vtrn.32 d8, d10
    vtrn.32 d9, d11

    vrshrn.u16 d18, q9, #6

    subs r5, r5, #2

    pld [r3]
    pld [r3, r4]

    vst1.16 {d16[0]}, [r0,:16], r2
    vst1.16 {d16[st2]}, [r1,:16], r2
    vst1.16 {d18[0]}, [r0,:16], r2
    vst1.16 {d18[st2]}, [r1,:16], r2
    bgt 1b

    vpop {d8-d11}
    pop {r4-r8, pc}

2:
    tst r7, r7
    add ip, ip, r7
    vdup.8 d0, r5
    ldr r5, [sp, #72]
    vdup.8 d1, ip

    beq 4f

    vld1.64 {d4}, [r3], r4
    vld1.64 {d6}, [r3], r4

3:

    vmull.u8 q8, d4, d0
    vmlal.u8 q8, d6, d1
    vmull.u8 q9, d6, d0
    vld1.64 {d4}, [r3], r4
    vmlal.u8 q9, d4, d1
    vld1.64 {d6}, [r3], r4

    vrshrn.u16 d16, q8, #6
    vrshrn.u16 d17, q9, #6
    subs r5, r5, #2
    vuzp.8 d16, d17

    pld [r3]
    pld [r3, r4]

    vst1.16 {d16[0]}, [r0,:16], r2
    vst1.16 {d16[st2]}, [r0,:16], r2
    vst1.16 {d17[0]}, [r1,:16], r2
    vst1.16 {d17[st2]}, [r1,:16], r2
    bgt 3b

    vpop {d8-d11}
    pop {r4-r8, pc}

4:

    vld1.64 {d4-d5}, [r3], r4
    vld1.64 {d6-d7}, [r3], r4

    vext.8 d5, d4, d5, #2
    vext.8 d7, d6, d7, #2

5:

    vmull.u8 q8, d4, d0
    vmlal.u8 q8, d5, d1
    vmull.u8 q9, d6, d0
    vmlal.u8 q9, d7, d1

    subs r5, r5, #2
    vld1.64 {d4-d5}, [r3], r4
    vld1.64 {d6-d7}, [r3], r4
    vext.8 d5, d4, d5, #2
    vrshrn.u16 d16, q8, #6
    vrshrn.u16 d17, q9, #6
    vext.8 d7, d6, d7, #2
    vuzp.8 d16, d17

    pld [r3]
    pld [r3, r4]

    vst1.16 {d16[0]}, [r0,:16], r2
    vst1.16 {d16[st2]}, [r0,:16], r2
    vst1.16 {d17[0]}, [r1,:16], r2
    vst1.16 {d17[st2]}, [r1,:16], r2
    bgt 5b

    vpop {d8-d11}
    pop {r4-r8, pc}
mc_chroma_w4:
    muls lr, r5, r6
    rsb r7, lr, r6, lsl #3
    rsb ip, lr, r5, lsl #3
    sub r5, lr, r5, lsl #3
    sub r5, r5, r6, lsl #3
    add r5, r5, #64

    beq 2f
    vld2.8 {d4-d5}, [r3], r4

    vdup.8 d0, r5
    vdup.8 d1, ip

    vdup.8 d2, r7
    vld2.8 {d8-d9}, [r3], r4
    vdup.8 d3, lr
    ldr r5, [sp, #72]
    vext.8 d6, d4, d6, #1
    vext.8 d7, d5, d7, #1
    vext.8 d10, d8, d10, #1
    vext.8 d11, d9, d11, #1

    .set st2, 1

    vtrn.32 d4, d6
    vtrn.32 d5, d7
    vtrn.32 d8, d10
    vtrn.32 d9, d11

    vtrn.32 d0, d1
    vtrn.32 d2, d3

1:

    vmull.u8 q8, d4, d0
    vmlal.u8 q8, d8, d2
    vmull.u8 q9, d5, d0
    vmlal.u8 q9, d9, d2

    vld2.8 {d4-d5}, [r3], r4

    vext.8 d6, d4, d6, #1
    vext.8 d7, d5, d7, #1

    vadd.i16 d16, d16, d17
    vadd.i16 d17, d18, d19

    vtrn.32 d4, d6
    vtrn.32 d5, d7

    vmull.u8 q10, d8, d0
    vmlal.u8 q10, d4, d2
    vmull.u8 q11, d9, d0
    vmlal.u8 q11, d5, d2

    vld2.8 {d8-d9}, [r3], r4

    vrshrn.u16 d16, q8, #6

    vext.8 d10, d8, d10, #1
    vext.8 d11, d9, d11, #1

    vadd.i16 d18, d20, d21
    vadd.i16 d19, d22, d23

    vtrn.32 d8, d10
    vtrn.32 d9, d11

    vrshrn.u16 d18, q9, #6

    subs r5, r5, #2

    pld [r3]
    pld [r3, r4]

    vst1.32 {d16[0]}, [r0,:32], r2
    vst1.32 {d16[st2]}, [r1,:32], r2
    vst1.32 {d18[0]}, [r0,:32], r2
    vst1.32 {d18[st2]}, [r1,:32], r2
    bgt 1b

    vpop {d8-d11}
    pop {r4-r8, pc}

2:
    tst r7, r7
    add ip, ip, r7
    vdup.8 d0, r5
    ldr r5, [sp, #72]
    vdup.8 d1, ip

    beq 4f

    vld1.64 {d4}, [r3], r4
    vld1.64 {d6}, [r3], r4

3:

    vmull.u8 q8, d4, d0
    vmlal.u8 q8, d6, d1
    vmull.u8 q9, d6, d0
    vld1.64 {d4}, [r3], r4
    vmlal.u8 q9, d4, d1
    vld1.64 {d6}, [r3], r4

    vrshrn.u16 d16, q8, #6
    vrshrn.u16 d17, q9, #6
    subs r5, r5, #2
    vuzp.8 d16, d17

    pld [r3]
    pld [r3, r4]

    vst1.32 {d16[0]}, [r0,:32], r2
    vst1.32 {d16[st2]}, [r0,:32], r2
    vst1.32 {d17[0]}, [r1,:32], r2
    vst1.32 {d17[st2]}, [r1,:32], r2
    bgt 3b

    vpop {d8-d11}
    pop {r4-r8, pc}

4:

    vld1.64 {d4-d5}, [r3], r4
    vld1.64 {d6-d7}, [r3], r4

    vext.8 d5, d4, d5, #2
    vext.8 d7, d6, d7, #2

5:

    vmull.u8 q8, d4, d0
    vmlal.u8 q8, d5, d1
    vmull.u8 q9, d6, d0
    vmlal.u8 q9, d7, d1

    subs r5, r5, #2
    vld1.64 {d4-d5}, [r3], r4
    vld1.64 {d6-d7}, [r3], r4
    vext.8 d5, d4, d5, #2
    vrshrn.u16 d16, q8, #6
    vrshrn.u16 d17, q9, #6
    vext.8 d7, d6, d7, #2
    vuzp.8 d16, d17

    pld [r3]
    pld [r3, r4]

    vst1.32 {d16[0]}, [r0,:32], r2
    vst1.32 {d16[st2]}, [r0,:32], r2
    vst1.32 {d17[0]}, [r1,:32], r2
    vst1.32 {d17[st2]}, [r1,:32], r2
    bgt 5b

    vpop {d8-d11}
    pop {r4-r8, pc}

mc_chroma_w8:
    muls lr, r5, r6
    rsb r7, lr, r6, lsl #3
    rsb ip, lr, r5, lsl #3
    sub r5, lr, r5, lsl #3
    sub r5, r5, r6, lsl #3
    add r5, r5, #64

    beq 2f
    vld2.8 {d4-d7}, [r3], r4

    vdup.8 d0, r5
    vdup.8 d1, ip

    vdup.8 d2, r7
    vld2.8 {d8-d11}, [r3], r4
    vdup.8 d3, lr
    ldr r5, [sp, #72]
    vext.8 d5, d4, d5, #1
    vext.8 d9, d8, d9, #1
    vext.8 d7, d6, d7, #1
    vext.8 d11, d10, d11, #1

1:
    vmull.u8 q8, d4, d0
    vmlal.u8 q8, d5, d1
    vmlal.u8 q8, d8, d2
    vmlal.u8 q8, d9, d3

    vmull.u8 q9, d6, d0
    vmlal.u8 q9, d7, d1
    vmlal.u8 q9, d10, d2
    vmlal.u8 q9, d11, d3

    vld2.8 {d4-d7}, [r3], r4

    vext.8 d5, d4, d5, #1
    vext.8 d7, d6, d7, #1

    vmull.u8 q10, d8, d0
    vmlal.u8 q10, d9, d1
    vmlal.u8 q10, d4, d2
    vmlal.u8 q10, d5, d3

    vmull.u8 q11, d10, d0
    vmlal.u8 q11, d11, d1
    vmlal.u8 q11, d6, d2
    vmlal.u8 q11, d7, d3

    subs r5, r5, #2
    vld2.8 {d8-d11}, [r3], r4

    vrshrn.u16 d16, q8, #6
    vrshrn.u16 d17, q9, #6
    vrshrn.u16 d18, q10, #6
    vext.8 d9, d8, d9, #1
    vrshrn.u16 d19, q11, #6
    vext.8 d11, d10, d11, #1

    pld [r3]
    pld [r3, r4]

    vst1.64 {d16}, [r0,:64], r2
    vst1.64 {d17}, [r1,:64], r2
    vst1.64 {d18}, [r0,:64], r2
    vst1.64 {d19}, [r1,:64], r2

    bgt 1b

    vpop {d8-d11}
    pop {r4-r8, pc}

2:
    tst r7, r7
    add ip, ip, r7
    vdup.8 d0, r5
    ldr r5, [sp, #72]
    vdup.8 d1, ip

    beq 4f

    vld2.8 {d4-d5}, [r3], r4
    vld2.8 {d6-d7}, [r3], r4

3:
    vmull.u8 q8, d4, d0
    vmlal.u8 q8, d6, d1
    vmull.u8 q9, d5, d0
    vmlal.u8 q9, d7, d1

    vld2.8 {d4-d5}, [r3], r4

    vmull.u8 q10, d6, d0
    vmlal.u8 q10, d4, d1
    vmull.u8 q11, d7, d0
    vmlal.u8 q11, d5, d1

    vld2.8 {d6-d7}, [r3], r4

    vrshrn.u16 d16, q8, #6
    vrshrn.u16 d17, q9, #6
    vrshrn.u16 d18, q10, #6
    vrshrn.u16 d19, q11, #6
    subs r5, r5, #2

    pld [r3]
    pld [r3, r4]

    vst1.64 {d16}, [r0,:64], r2
    vst1.64 {d17}, [r1,:64], r2
    vst1.64 {d18}, [r0,:64], r2
    vst1.64 {d19}, [r1,:64], r2

    bgt 3b

    vpop {d8-d11}
    pop {r4-r8, pc}

4:

    vld2.8 {d4-d7}, [r3], r4
    vld2.8 {d8-d11}, [r3], r4
    vext.8 d5, d4, d5, #1
    vext.8 d7, d6, d7, #1
    vext.8 d9, d8, d9, #1
    vext.8 d11, d10, d11, #1

5:
    subs r5, r5, #2
    vmull.u8 q8, d4, d0
    vmlal.u8 q8, d5, d1
    vmull.u8 q9, d6, d0
    vmlal.u8 q9, d7, d1

    vld2.8 {d4-d7}, [r3], r4

    vmull.u8 q10, d8, d0
    vmlal.u8 q10, d9, d1
    vmull.u8 q11, d10, d0
    vmlal.u8 q11, d11, d1

    vld2.8 {d8-d11}, [r3], r4

    vext.8 d5, d4, d5, #1
    vrshrn.u16 d16, q8, #6
    vext.8 d7, d6, d7, #1
    vrshrn.u16 d17, q9, #6
    vext.8 d9, d8, d9, #1
    vrshrn.u16 d18, q10, #6
    vext.8 d11, d10, d11, #1
    vrshrn.u16 d19, q11, #6

    pld [r3]
    pld [r3, r4]

    vst1.64 {d16}, [r0,:64], r2
    vst1.64 {d17}, [r1,:64], r2
    vst1.64 {d18}, [r0,:64], r2
    vst1.64 {d19}, [r1,:64], r2
    bgt 5b

    vpop {d8-d11}
    pop {r4-r8, pc}


        @.endfunc



        .align 2
        .globl _x264_hpel_filter_v_neon


        @.func _x264_hpel_filter_v_neon
_x264_hpel_filter_v_neon:
    ldr ip, [sp]
    sub r1, r1, r3, lsl #1
    push {lr}
    add lr, r1, ip
    vmov.u8 d30, #5
    vmov.u8 d31, #20

filter_v_loop:
    subs ip, ip, #16
    vld1.64 {d0-d1}, [r1,:128], r3
    vld1.64 {d2-d3}, [r1,:128], r3
    vld1.64 {d4-d5}, [r1,:128], r3
    vld1.64 {d6-d7}, [r1,:128], r3
    vld1.64 {d16-d17}, [r1,:128], r3
    vld1.64 {d18-d19}, [r1,:128], r3
    sub r1, lr, ip

    vaddl.u8 q10, d0, d18
    vmlsl.u8 q10, d2, d30
    vmlal.u8 q10, d4, d31
    vmlal.u8 q10, d6, d31
    vmlsl.u8 q10, d16, d30

    vaddl.u8 q11, d1, d19
    vmlsl.u8 q11, d3, d30
    vmlal.u8 q11, d5, d31
    vmlal.u8 q11, d7, d31
    vmlsl.u8 q11, d17, d30

    vqrshrun.s16 d0, q10, #5
    vst1.64 {d20-d21}, [r2,:128]!
    vqrshrun.s16 d1, q11, #5
    vst1.64 {d22-d23}, [r2,:128]!
    vst1.64 {d0-d1}, [r0,:128]!
    bgt filter_v_loop
    pop {pc}

        @.endfunc


        .align 2
        .globl _x264_hpel_filter_c_neon


        @.func _x264_hpel_filter_c_neon
_x264_hpel_filter_c_neon:
    sub r1, #16
    vld1.64 {d0-d3}, [r1,:128]!


filter_c_loop:
    subs r2, r2, #16
    vld1.64 {d4-d7}, [r1,:128]!
    vext.16 q8, q0, q1, #6
    vext.16 q12, q1, q2, #3
    vadd.s16 q8, q8, q12
    vext.16 q9, q0, q1, #7
    vext.16 q11, q1, q2, #2
    vadd.s16 q9, q9, q11
    vext.16 q10, q1, q2, #1
    vext.16 q11, q1, q2, #6
    vadd.s16 q10, q1, q10
    vsub.s16 q8, q8, q9
    vext.16 q15, q2, q3, #3
    vsub.s16 q9, q9, q10

    vext.16 q12, q1, q2, #7
    vshr.s16 q8, q8, #2
    vadd.s16 q11, q11, q15
    vext.16 q14, q2, q3, #2
    vsub.s16 q8, q8, q9
    vadd.s16 q12, q12, q14
    vext.16 q13, q2, q3, #1

    vshr.s16 q8, q8, #2
    vadd.s16 q13, q2, q13
    vadd.s16 q8, q8, q10
    vsub.s16 q11, q11, q12
    vsub.s16 q12, q12, q13
    vshr.s16 q11, q11, #2
    vqrshrun.s16 d30, q8, #6
    vsub.s16 q11, q11, q12
    vshr.s16 q11, q11, #2
    vld1.64 {d0-d3}, [r1,:128]!
    vadd.s16 q11, q11, q13

    vext.16 q8, q2, q3, #6
    vqrshrun.s16 d31, q11, #6
    vext.16 q12, q3, q0, #3
    vadd.s16 q8, q8, q12
    vext.16 q9, q2, q3, #7
    vst1.64 {d30-d31}, [r0,:128]!
    bxle lr
    subs r2, r2, #16

    vext.16 q11, q3, q0, #2
    vadd.s16 q9, q9, q11
    vext.16 q10, q3, q0, #1
    vext.16 q11, q3, q0, #6
    vadd.s16 q10, q3, q10
    vsub.s16 q8, q8, q9
    vext.16 q15, q0, q1, #3
    vsub.s16 q9, q9, q10

    vext.16 q12, q3, q0, #7
    vshr.s16 q8, q8, #2
    vadd.s16 q11, q11, q15
    vext.16 q14, q0, q1, #2
    vsub.s16 q8, q8, q9
    vadd.s16 q12, q12, q14
    vext.16 q13, q0, q1, #1

    vshr.s16 q8, q8, #2
    vadd.s16 q13, q0, q13
    vadd.s16 q8, q8, q10
    vsub.s16 q11, q11, q12
    vsub.s16 q12, q12, q13
    vshr.s16 q11, q11, #2
    vqrshrun.s16 d30, q8, #6
    vsub.s16 q11, q11, q12
    vshr.s16 q11, q11, #2
    vadd.s16 q11, q11, q13

    vqrshrun.s16 d31, q11, #6
    vst1.64 {d30-d31}, [r0,:128]!
    bgt filter_c_loop
    bx lr

        @.endfunc


        .align 2
        .globl _x264_hpel_filter_h_neon


        @.func _x264_hpel_filter_h_neon
_x264_hpel_filter_h_neon:
    sub r1, #16
    vmov.u8 d30, #5
    vld1.64 {d0-d3}, [r1,:128]!
    vmov.u8 d31, #20



filter_h_loop:
    subs r2, r2, #16
    vld1.64 {d4-d5}, [r1,:128]!
    vext.8 q8, q0, q1, #14
    vext.8 q12, q1, q2, #3
    vaddl.u8 q13, d16, d24
    vext.8 q9, q0, q1, #15
    vaddl.u8 q14, d17, d25

    vext.8 q10, q1, q2, #1
    vmlal.u8 q13, d2, d31
    vmlsl.u8 q13, d18, d30
    vext.8 q11, q1, q2, #2
    vmlal.u8 q13, d20, d31
    vmlsl.u8 q13, d22, d30

    vmlsl.u8 q14, d19, d30
    vmlal.u8 q14, d3, d31
    vmlal.u8 q14, d21, d31
    vmlsl.u8 q14, d23, d30
    vqrshrun.s16 d6, q13, #5

    vld1.64 {d0-d1}, [r1,:128]!
    vext.8 q8, q1, q2, #14
    vext.8 q12, q2, q0, #3
    vaddl.u8 q13, d16, d24
    vqrshrun.s16 d7, q14, #5
    vext.8 q9, q1, q2, #15
    vaddl.u8 q14, d17, d25

    vst1.64 {d6-d7}, [r0,:128]!
    bxle lr
    subs r2, r2, #16

    vext.8 q10, q2, q0, #1
    vmlal.u8 q13, d4, d31
    vmlsl.u8 q13, d18, d30
    vext.8 q11, q2, q0, #2
    vmlal.u8 q13, d20, d31
    vmlsl.u8 q13, d22, d30

    vmlsl.u8 q14, d19, d30
    vmlal.u8 q14, d5, d31
    vmlal.u8 q14, d21, d31
    vmlsl.u8 q14, d23, d30
    vqrshrun.s16 d6, q13, #5

    vld1.64 {d2-d3}, [r1,:128]!
    vext.8 q8, q2, q0, #14
    vext.8 q12, q0, q1, #3
    vaddl.u8 q13, d16, d24
    vqrshrun.s16 d7, q14, #5
    vext.8 q9, q2, q0, #15
    vaddl.u8 q14, d17, d25

    vst1.64 {d6-d7}, [r0,:128]!
    bxle lr
    subs r2, r2, #16

    vext.8 q10, q0, q1, #1
    vmlal.u8 q13, d0, d31
    vmlsl.u8 q13, d18, d30
    vext.8 q11, q0, q1, #2
    vmlal.u8 q13, d20, d31
    vmlsl.u8 q13, d22, d30

    vmlsl.u8 q14, d19, d30
    vmlal.u8 q14, d1, d31
    vmlal.u8 q14, d21, d31
    vmlsl.u8 q14, d23, d30

    vqrshrun.s16 d6, q13, #5
    vqrshrun.s16 d7, q14, #5
    vst1.64 {d6-d7}, [r0,:128]!
    bgt filter_h_loop
    bx lr

        @.endfunc





        .align 2
        .globl _x264_frame_init_lowres_core_neon


        @.func _x264_frame_init_lowres_core_neon
_x264_frame_init_lowres_core_neon:
    push {r4-r10,lr}
    vpush {d8-d15}
    ldrd r4, r5, [sp, #96]
    ldrd r6, r7, [sp, #104]
    ldr lr, [sp, #112]
    sub r10, r6, r7
    and r10, r10, #~15

lowres_yloop:
    mov ip, r7
    mov r6, r0
    add r8, r0, r5
    add r9, r0, r5, lsl #1

    vld2.8 {d8, d10}, [r6,:128]!
    vld2.8 {d12,d14}, [r8,:128]!
    vld2.8 {d16,d18}, [r9,:128]!

lowres_xloop:
    subs ip, ip, #16

    vld2.8 {d9, d11}, [r6,:128]!
    vld2.8 {d13,d15}, [r8,:128]!
    vrhadd.u8 q0, q4, q6
    vld2.8 {d17,d19}, [r9,:128]!
    vrhadd.u8 q5, q5, q7
    vld2.8 {d20,d22}, [r6,:128]!
    vrhadd.u8 q1, q6, q8
    vld2.8 {d24,d26}, [r8,:128]!
    vrhadd.u8 q7, q7, q9
    vext.8 q4, q4, q10, #1
    vrhadd.u8 q0, q0, q5
    vext.8 q6, q6, q12, #1
    vrhadd.u8 q1, q1, q7
    vld2.8 {d28,d30}, [r9,:128]!
    vrhadd.u8 q4, q4, q6
    vext.8 q8, q8, q14, #1
    vrhadd.u8 q6, q6, q8
    vst1.64 {d0-d1}, [r1,:128]!
    vrhadd.u8 q2, q4, q5
    vst1.64 {d2-d3}, [r3,:128]!
    vrhadd.u8 q3, q6, q7
    vst1.64 {d4-d5}, [r2,:128]!
    vst1.64 {d6-d7}, [r4,:128]!

    ble lowres_xloop_end
    subs ip, ip, #16

    vld2.8 {d21,d23}, [r6,:128]!
    vld2.8 {d25,d27}, [r8,:128]!
    vrhadd.u8 q0, q10, q12
    vld2.8 {d29,d31}, [r9,:128]!
    vrhadd.u8 q11, q11, q13
    vld2.8 {d8, d10}, [r6,:128]!
    vrhadd.u8 q1, q12, q14
    vld2.8 {d12,d14}, [r8,:128]!
    vrhadd.u8 q13, q13, q15
    vext.8 q10, q10, q4, #1
    vrhadd.u8 q0, q0, q11
    vext.8 q12, q12, q6, #1
    vrhadd.u8 q1, q1, q13
    vld2.8 {d16,d18}, [r9,:128]!
    vrhadd.u8 q10, q10, q12
    vext.8 q14, q14, q8, #1
    vrhadd.u8 q12, q12, q14
    vst1.64 {d0-d1}, [r1,:128]!
    vrhadd.u8 q2, q10, q11
    vst1.64 {d2-d3}, [r3,:128]!
    vrhadd.u8 q3, q12, q13
    vst1.64 {d4-d5}, [r2,:128]!
    vst1.64 {d6-d7}, [r4,:128]!

    bgt lowres_xloop

lowres_xloop_end:
    subs lr, lr, #1
    add r0, r0, r5, lsl #1
    add r1, r1, r10
    add r2, r2, r10
    add r3, r3, r10
    add r4, r4, r10
    bgt lowres_yloop

    vpop {d8-d15}
    pop {r4-r10,pc}

        @.endfunc

        .align 2
        .globl _x264_load_deinterleave_chroma_fdec_neon


        @.func _x264_load_deinterleave_chroma_fdec_neon
_x264_load_deinterleave_chroma_fdec_neon:
    mov ip, #32/2
1:
    vld2.8 {d0-d1}, [r1,:128], r2
    subs r3, r3, #1
    pld [r1]
    vst1.8 {d0}, [r0,:64], ip
    vst1.8 {d1}, [r0,:64], ip
    bgt 1b

    bx lr

        @.endfunc

        .align 2
        .globl _x264_load_deinterleave_chroma_fenc_neon


        @.func _x264_load_deinterleave_chroma_fenc_neon
_x264_load_deinterleave_chroma_fenc_neon:
    mov ip, #16/2
1:
    vld2.8 {d0-d1}, [r1,:128], r2
    subs r3, r3, #1
    pld [r1]
    vst1.8 {d0}, [r0,:64], ip
    vst1.8 {d1}, [r0,:64], ip
    bgt 1b

    bx lr

        @.endfunc

        .align 2
        .globl _x264_plane_copy_deinterleave_neon


        @.func _x264_plane_copy_deinterleave_neon
_x264_plane_copy_deinterleave_neon:
    push {r4-r7, lr}
    ldrd r6, r7, [sp, #28]
    ldrd r4, r5, [sp, #20]
    add lr, r6, #15
    bic lr, lr, #15
    sub r1, r1, lr
    sub r3, r3, lr
    sub r5, r5, lr, lsl #1
block:
    vld2.8 {d0-d3}, [r4,:128]!
    subs lr, lr, #16
    vst1.8 {q0}, [r0]!
    vst1.8 {q1}, [r2]!
    bgt block

    add r4, r4, r5
    subs r7, r7, #1
    add r0, r0, r1
    add r2, r2, r3
    mov lr, r6
    bgt block

    pop {r4-r7, pc}

        @.endfunc

        .align 2
        .globl _x264_plane_copy_deinterleave_rgb_neon


        @.func _x264_plane_copy_deinterleave_rgb_neon
_x264_plane_copy_deinterleave_rgb_neon:
    push {r4-r8, r10, r11, lr}
    ldrd r4, r5, [sp, #32]
    ldrd r6, r7, [sp, #40]
    ldr r8, [sp, #48]
    ldrd r10, r11, [sp, #52]
    add lr, r10, #7
    subs r8, r8, #3
    bic lr, lr, #7
    sub r7, r7, lr, lsl #1
    sub r1, r1, lr
    sub r3, r3, lr
    sub r5, r5, lr
    subne r7, r7, lr, lsl #1
    subeq r7, r7, lr
    bne block4
block3:
    vld3.8 {d0,d1,d2}, [r6]!
    subs lr, lr, #8
    vst1.8 {d0}, [r0]!
    vst1.8 {d1}, [r2]!
    vst1.8 {d2}, [r4]!
    bgt block3

    subs r11, r11, #1
    add r0, r0, r1
    add r2, r2, r3
    add r4, r4, r5
    add r6, r6, r7
    mov lr, r10
    bgt block3

    pop {r4-r8, r10, r11, pc}
block4:
    vld4.8 {d0,d1,d2,d3}, [r6]!
    subs lr, lr, #8
    vst1.8 {d0}, [r0]!
    vst1.8 {d1}, [r2]!
    vst1.8 {d2}, [r4]!
    bgt block4

    subs r11, r11, #1
    add r0, r0, r1
    add r2, r2, r3
    add r4, r4, r5
    add r6, r6, r7
    mov lr, r10
    bgt block4

    pop {r4-r8, r10, r11, pc}

        @.endfunc

        .align 2
        .globl _x264_plane_copy_interleave_neon


        @.func _x264_plane_copy_interleave_neon
_x264_plane_copy_interleave_neon:
    push {r4-r7, lr}
    ldrd r6, r7, [sp, #28]
    ldrd r4, r5, [sp, #20]
    add lr, r6, #15
    bic lr, lr, #15
    sub r1, r1, lr, lsl #1
    sub r3, r3, lr
    sub r5, r5, lr
blocki:
    vld1.8 {q0}, [r2]!
    vld1.8 {q1}, [r4]!
    subs lr, lr, #16
    vst2.8 {d0,d2}, [r0]!
    vst2.8 {d1,d3}, [r0]!
    bgt blocki

    subs r7, r7, #1
    add r0, r0, r1
    add r2, r2, r3
    add r4, r4, r5
    mov lr, r6
    bgt blocki

    pop {r4-r7, pc}

        @.endfunc

        .align 2
        .globl _x264_store_interleave_chroma_neon


        @.func _x264_store_interleave_chroma_neon
_x264_store_interleave_chroma_neon:
    push {lr}
    ldr lr, [sp, #4]
    mov ip, #32
1:
    vld1.8 {d0}, [r2], ip
    vld1.8 {d1}, [r3], ip
    subs lr, lr, #1
    vst2.8 {d0,d1}, [r0,:128], r1
    bgt 1b

    pop {pc}

        @.endfunc
.text
.align 2

#endif
