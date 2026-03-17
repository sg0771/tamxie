#ifdef __arm64__

.const_data
        .const_data
        .align 2
mask:

.byte 0xff

.byte 0xff

.byte 0xff

.byte 0xff

.byte 0xff

.byte 0xff

.byte 0xff

.byte 0xff

.byte 0xff

.byte 0xff

.byte 0xff

.byte 0xff

.byte 0xff

.byte 0xff

.byte 0xff

.byte 0xff

.byte 0x00

.byte 0x00

.byte 0x00

.byte 0x00

.byte 0x00

.byte 0x00

.byte 0x00

.byte 0x00

.byte 0x00

.byte 0x00

.byte 0x00

.byte 0x00

.byte 0x00

.byte 0x00

.byte 0x00

.byte 0x00
 # //.size mask, . - mask

.const_data
        .const_data
        .align 2
mask_ac_4_8:
.short 0, -1, -1, -1, 0, -1, -1, -1
.short 0, -1, -1, -1, -1, -1, -1, -1
 # //.size mask_ac_4_8, . - mask_ac_4_8








        .text
        .align 2
        .globl _x264_pixel_sad_4x4_neon
 # //.type _x264_pixel_sad_4x4_neon, %function
        //.func _x264_pixel_sad_4x4_neon
_x264_pixel_sad_4x4_neon:
    ld1 {v1.s}[0], [x2], x3
    ld1 {v0.s}[0], [x0], x1
    ld1 {v1.s}[1], [x2], x3
    ld1 {v0.s}[1], [x0], x1
    uabdl v16.8h, v0.8b, v1.8b


    ld1 {v1.s}[0], [x2], x3
    ld1 {v0.s}[0], [x0], x1
    ld1 {v1.s}[1], [x2], x3
    ld1 {v0.s}[1], [x0], x1
    uabal v16.8h, v0.8b, v1.8b
    uaddlv s0, v16.8h
    fmov w0, s0
    ret
 # //.size x264_pixel_sad_4x4_neon, . - x264_pixel_sad_4x4_neon
        //.endfunc
        .text
        .align 2
        .globl _x264_pixel_sad_4x8_neon
 # //.type _x264_pixel_sad_4x8_neon, %function
        //.func _x264_pixel_sad_4x8_neon
_x264_pixel_sad_4x8_neon:
    ld1 {v1.s}[0], [x2], x3
    ld1 {v0.s}[0], [x0], x1
    ld1 {v1.s}[1], [x2], x3
    ld1 {v0.s}[1], [x0], x1
    uabdl v16.8h, v0.8b, v1.8b


    ld1 {v1.s}[0], [x2], x3
    ld1 {v0.s}[0], [x0], x1
    ld1 {v1.s}[1], [x2], x3
    ld1 {v0.s}[1], [x0], x1
    uabal v16.8h, v0.8b, v1.8b

    ld1 {v1.s}[0], [x2], x3
    ld1 {v0.s}[0], [x0], x1
    ld1 {v1.s}[1], [x2], x3
    ld1 {v0.s}[1], [x0], x1
    uabal v16.8h, v0.8b, v1.8b

    ld1 {v1.s}[0], [x2], x3
    ld1 {v0.s}[0], [x0], x1
    ld1 {v1.s}[1], [x2], x3
    ld1 {v0.s}[1], [x0], x1
    uabal v16.8h, v0.8b, v1.8b
    uaddlv s0, v16.8h
    fmov w0, s0
    ret
 # //.size x264_pixel_sad_4x8_neon, . - x264_pixel_sad_4x8_neon
        //.endfunc
        .text
        .align 2
        .globl _x264_pixel_sad_8x4_neon
 # //.type _x264_pixel_sad_8x4_neon, %function
        //.func _x264_pixel_sad_8x4_neon
_x264_pixel_sad_8x4_neon:
    ld1 {v1.8b}, [x2], x3
    ld1 {v0.8b}, [x0], x1
    ld1 {v3.8b}, [x2], x3
    ld1 {v2.8b}, [x0], x1
    uabdl v16.8h, v0.8b, v1.8b
    uabdl v17.8h, v2.8b, v3.8b


    ld1 {v1.8b}, [x2], x3
    ld1 {v0.8b}, [x0], x1
    ld1 {v3.8b}, [x2], x3
    ld1 {v2.8b}, [x0], x1
    uabal v16.8h, v0.8b, v1.8b
    uabal v17.8h, v2.8b, v3.8b
    add v16.8h, v16.8h, v17.8h
    uaddlv s0, v16.8h
    fmov w0, s0
    ret
 # //.size x264_pixel_sad_8x4_neon, . - x264_pixel_sad_8x4_neon
        //.endfunc
        .text
        .align 2
        .globl _x264_pixel_sad_8x8_neon
 # //.type _x264_pixel_sad_8x8_neon, %function
        //.func _x264_pixel_sad_8x8_neon
_x264_pixel_sad_8x8_neon:
    ld1 {v1.8b}, [x2], x3
    ld1 {v0.8b}, [x0], x1
    ld1 {v3.8b}, [x2], x3
    ld1 {v2.8b}, [x0], x1
    uabdl v16.8h, v0.8b, v1.8b
    uabdl v17.8h, v2.8b, v3.8b


    ld1 {v1.8b}, [x2], x3
    ld1 {v0.8b}, [x0], x1
    ld1 {v3.8b}, [x2], x3
    ld1 {v2.8b}, [x0], x1
    uabal v16.8h, v0.8b, v1.8b
    uabal v17.8h, v2.8b, v3.8b

    ld1 {v1.8b}, [x2], x3
    ld1 {v0.8b}, [x0], x1
    ld1 {v3.8b}, [x2], x3
    ld1 {v2.8b}, [x0], x1
    uabal v16.8h, v0.8b, v1.8b
    uabal v17.8h, v2.8b, v3.8b

    ld1 {v1.8b}, [x2], x3
    ld1 {v0.8b}, [x0], x1
    ld1 {v3.8b}, [x2], x3
    ld1 {v2.8b}, [x0], x1
    uabal v16.8h, v0.8b, v1.8b
    uabal v17.8h, v2.8b, v3.8b
    add v16.8h, v16.8h, v17.8h
    uaddlv s0, v16.8h
    fmov w0, s0
    ret
 # //.size x264_pixel_sad_8x8_neon, . - x264_pixel_sad_8x8_neon
        //.endfunc
        .text
        .align 2
        .globl _x264_pixel_sad_8x16_neon
 # //.type _x264_pixel_sad_8x16_neon, %function
        //.func _x264_pixel_sad_8x16_neon
_x264_pixel_sad_8x16_neon:
    ld1 {v1.8b}, [x2], x3
    ld1 {v0.8b}, [x0], x1
    ld1 {v3.8b}, [x2], x3
    ld1 {v2.8b}, [x0], x1
    uabdl v16.8h, v0.8b, v1.8b
    uabdl v17.8h, v2.8b, v3.8b


    ld1 {v1.8b}, [x2], x3
    ld1 {v0.8b}, [x0], x1
    ld1 {v3.8b}, [x2], x3
    ld1 {v2.8b}, [x0], x1
    uabal v16.8h, v0.8b, v1.8b
    uabal v17.8h, v2.8b, v3.8b

    ld1 {v1.8b}, [x2], x3
    ld1 {v0.8b}, [x0], x1
    ld1 {v3.8b}, [x2], x3
    ld1 {v2.8b}, [x0], x1
    uabal v16.8h, v0.8b, v1.8b
    uabal v17.8h, v2.8b, v3.8b

    ld1 {v1.8b}, [x2], x3
    ld1 {v0.8b}, [x0], x1
    ld1 {v3.8b}, [x2], x3
    ld1 {v2.8b}, [x0], x1
    uabal v16.8h, v0.8b, v1.8b
    uabal v17.8h, v2.8b, v3.8b

    ld1 {v1.8b}, [x2], x3
    ld1 {v0.8b}, [x0], x1
    ld1 {v3.8b}, [x2], x3
    ld1 {v2.8b}, [x0], x1
    uabal v16.8h, v0.8b, v1.8b
    uabal v17.8h, v2.8b, v3.8b

    ld1 {v1.8b}, [x2], x3
    ld1 {v0.8b}, [x0], x1
    ld1 {v3.8b}, [x2], x3
    ld1 {v2.8b}, [x0], x1
    uabal v16.8h, v0.8b, v1.8b
    uabal v17.8h, v2.8b, v3.8b

    ld1 {v1.8b}, [x2], x3
    ld1 {v0.8b}, [x0], x1
    ld1 {v3.8b}, [x2], x3
    ld1 {v2.8b}, [x0], x1
    uabal v16.8h, v0.8b, v1.8b
    uabal v17.8h, v2.8b, v3.8b

    ld1 {v1.8b}, [x2], x3
    ld1 {v0.8b}, [x0], x1
    ld1 {v3.8b}, [x2], x3
    ld1 {v2.8b}, [x0], x1
    uabal v16.8h, v0.8b, v1.8b
    uabal v17.8h, v2.8b, v3.8b
    add v16.8h, v16.8h, v17.8h
    uaddlv s0, v16.8h
    fmov w0, s0
    ret
 # //.size x264_pixel_sad_8x16_neon, . - x264_pixel_sad_8x16_neon
        //.endfunc
        .text
        .align 2
        .globl _x264_pixel_sad_16x8_neon
 # //.type _x264_pixel_sad_16x8_neon, %function
        //.func _x264_pixel_sad_16x8_neon
_x264_pixel_sad_16x8_neon:
    ld1 {v1.16b}, [x2], x3
    ld1 {v0.16b}, [x0], x1
    ld1 {v3.16b}, [x2], x3
    ld1 {v2.16b}, [x0], x1
    uabdl v16.8h, v0.8b, v1.8b
    uabdl2 v17.8h, v0.16b, v1.16b
    uabal v16.8h, v2.8b, v3.8b
    uabal2 v17.8h, v2.16b, v3.16b


    ld1 {v1.16b}, [x2], x3
    ld1 {v0.16b}, [x0], x1
    ld1 {v3.16b}, [x2], x3
    ld1 {v2.16b}, [x0], x1
    uabal v16.8h, v0.8b, v1.8b
    uabal2 v17.8h, v0.16b, v1.16b
    uabal v16.8h, v2.8b, v3.8b
    uabal2 v17.8h, v2.16b, v3.16b

    ld1 {v1.16b}, [x2], x3
    ld1 {v0.16b}, [x0], x1
    ld1 {v3.16b}, [x2], x3
    ld1 {v2.16b}, [x0], x1
    uabal v16.8h, v0.8b, v1.8b
    uabal2 v17.8h, v0.16b, v1.16b
    uabal v16.8h, v2.8b, v3.8b
    uabal2 v17.8h, v2.16b, v3.16b

    ld1 {v1.16b}, [x2], x3
    ld1 {v0.16b}, [x0], x1
    ld1 {v3.16b}, [x2], x3
    ld1 {v2.16b}, [x0], x1
    uabal v16.8h, v0.8b, v1.8b
    uabal2 v17.8h, v0.16b, v1.16b
    uabal v16.8h, v2.8b, v3.8b
    uabal2 v17.8h, v2.16b, v3.16b
    add v16.8h, v16.8h, v17.8h
    uaddlv s0, v16.8h
    fmov w0, s0
    ret
 # //.size x264_pixel_sad_16x8_neon, . - x264_pixel_sad_16x8_neon
        //.endfunc
        .text
        .align 2
        .globl _x264_pixel_sad_16x16_neon
 # //.type _x264_pixel_sad_16x16_neon, %function
        //.func _x264_pixel_sad_16x16_neon
_x264_pixel_sad_16x16_neon:
    ld1 {v1.16b}, [x2], x3
    ld1 {v0.16b}, [x0], x1
    ld1 {v3.16b}, [x2], x3
    ld1 {v2.16b}, [x0], x1
    uabdl v16.8h, v0.8b, v1.8b
    uabdl2 v17.8h, v0.16b, v1.16b
    uabal v16.8h, v2.8b, v3.8b
    uabal2 v17.8h, v2.16b, v3.16b


    ld1 {v1.16b}, [x2], x3
    ld1 {v0.16b}, [x0], x1
    ld1 {v3.16b}, [x2], x3
    ld1 {v2.16b}, [x0], x1
    uabal v16.8h, v0.8b, v1.8b
    uabal2 v17.8h, v0.16b, v1.16b
    uabal v16.8h, v2.8b, v3.8b
    uabal2 v17.8h, v2.16b, v3.16b

    ld1 {v1.16b}, [x2], x3
    ld1 {v0.16b}, [x0], x1
    ld1 {v3.16b}, [x2], x3
    ld1 {v2.16b}, [x0], x1
    uabal v16.8h, v0.8b, v1.8b
    uabal2 v17.8h, v0.16b, v1.16b
    uabal v16.8h, v2.8b, v3.8b
    uabal2 v17.8h, v2.16b, v3.16b

    ld1 {v1.16b}, [x2], x3
    ld1 {v0.16b}, [x0], x1
    ld1 {v3.16b}, [x2], x3
    ld1 {v2.16b}, [x0], x1
    uabal v16.8h, v0.8b, v1.8b
    uabal2 v17.8h, v0.16b, v1.16b
    uabal v16.8h, v2.8b, v3.8b
    uabal2 v17.8h, v2.16b, v3.16b

    ld1 {v1.16b}, [x2], x3
    ld1 {v0.16b}, [x0], x1
    ld1 {v3.16b}, [x2], x3
    ld1 {v2.16b}, [x0], x1
    uabal v16.8h, v0.8b, v1.8b
    uabal2 v17.8h, v0.16b, v1.16b
    uabal v16.8h, v2.8b, v3.8b
    uabal2 v17.8h, v2.16b, v3.16b

    ld1 {v1.16b}, [x2], x3
    ld1 {v0.16b}, [x0], x1
    ld1 {v3.16b}, [x2], x3
    ld1 {v2.16b}, [x0], x1
    uabal v16.8h, v0.8b, v1.8b
    uabal2 v17.8h, v0.16b, v1.16b
    uabal v16.8h, v2.8b, v3.8b
    uabal2 v17.8h, v2.16b, v3.16b

    ld1 {v1.16b}, [x2], x3
    ld1 {v0.16b}, [x0], x1
    ld1 {v3.16b}, [x2], x3
    ld1 {v2.16b}, [x0], x1
    uabal v16.8h, v0.8b, v1.8b
    uabal2 v17.8h, v0.16b, v1.16b
    uabal v16.8h, v2.8b, v3.8b
    uabal2 v17.8h, v2.16b, v3.16b

    ld1 {v1.16b}, [x2], x3
    ld1 {v0.16b}, [x0], x1
    ld1 {v3.16b}, [x2], x3
    ld1 {v2.16b}, [x0], x1
    uabal v16.8h, v0.8b, v1.8b
    uabal2 v17.8h, v0.16b, v1.16b
    uabal v16.8h, v2.8b, v3.8b
    uabal2 v17.8h, v2.16b, v3.16b
    add v16.8h, v16.8h, v17.8h
    uaddlv s0, v16.8h
    fmov w0, s0
    ret
 # //.size x264_pixel_sad_16x16_neon, . - x264_pixel_sad_16x16_neon
        //.endfunc





        .text
        .align 2
        .globl _x264_pixel_sad_x3_4x4_neon
 # //.type _x264_pixel_sad_x3_4x4_neon, %function
        //.func _x264_pixel_sad_x3_4x4_neon
_x264_pixel_sad_x3_4x4_neon:
    mov x6, x5
    mov x5, x4
    mov x7, #16

    ld1 {v0.s}[0], [x0], x7
    ld1 {v1.s}[0], [x1], x5
    ld1 {v0.s}[1], [x0], x7
    ld1 {v1.s}[1], [x1], x5
    uabdl v16.8h, v1.8b, v0.8b
    ld1 {v2.s}[0], [x2], x5
    ld1 {v2.s}[1], [x2], x5
    uabdl v17.8h, v2.8b, v0.8b
    ld1 {v3.s}[0], [x3], x5
    ld1 {v3.s}[1], [x3], x5
    uabdl v18.8h, v3.8b, v0.8b


    ld1 {v0.s}[0], [x0], x7
    ld1 {v1.s}[0], [x1], x5
    ld1 {v0.s}[1], [x0], x7
    ld1 {v1.s}[1], [x1], x5
    uabal v16.8h, v1.8b, v0.8b
    ld1 {v2.s}[0], [x2], x5
    ld1 {v2.s}[1], [x2], x5
    uabal v17.8h, v2.8b, v0.8b
    ld1 {v3.s}[0], [x3], x5
    ld1 {v3.s}[1], [x3], x5
    uabal v18.8h, v3.8b, v0.8b


    uaddlv s0, v16.8h
    uaddlv s1, v17.8h
    uaddlv s2, v18.8h

    stp s0, s1, [x6], #8
    str s2, [x6]
    ret
 # //.size x264_pixel_sad_x3_4x4_neon, . - x264_pixel_sad_x3_4x4_neon
        //.endfunc
        .text
        .align 2
        .globl _x264_pixel_sad_x3_4x8_neon
 # //.type _x264_pixel_sad_x3_4x8_neon, %function
        //.func _x264_pixel_sad_x3_4x8_neon
_x264_pixel_sad_x3_4x8_neon:
    mov x6, x5
    mov x5, x4
    mov x7, #16

    ld1 {v0.s}[0], [x0], x7
    ld1 {v1.s}[0], [x1], x5
    ld1 {v0.s}[1], [x0], x7
    ld1 {v1.s}[1], [x1], x5
    uabdl v16.8h, v1.8b, v0.8b
    ld1 {v2.s}[0], [x2], x5
    ld1 {v2.s}[1], [x2], x5
    uabdl v17.8h, v2.8b, v0.8b
    ld1 {v3.s}[0], [x3], x5
    ld1 {v3.s}[1], [x3], x5
    uabdl v18.8h, v3.8b, v0.8b


    ld1 {v0.s}[0], [x0], x7
    ld1 {v1.s}[0], [x1], x5
    ld1 {v0.s}[1], [x0], x7
    ld1 {v1.s}[1], [x1], x5
    uabal v16.8h, v1.8b, v0.8b
    ld1 {v2.s}[0], [x2], x5
    ld1 {v2.s}[1], [x2], x5
    uabal v17.8h, v2.8b, v0.8b
    ld1 {v3.s}[0], [x3], x5
    ld1 {v3.s}[1], [x3], x5
    uabal v18.8h, v3.8b, v0.8b

    ld1 {v0.s}[0], [x0], x7
    ld1 {v1.s}[0], [x1], x5
    ld1 {v0.s}[1], [x0], x7
    ld1 {v1.s}[1], [x1], x5
    uabal v16.8h, v1.8b, v0.8b
    ld1 {v2.s}[0], [x2], x5
    ld1 {v2.s}[1], [x2], x5
    uabal v17.8h, v2.8b, v0.8b
    ld1 {v3.s}[0], [x3], x5
    ld1 {v3.s}[1], [x3], x5
    uabal v18.8h, v3.8b, v0.8b

    ld1 {v0.s}[0], [x0], x7
    ld1 {v1.s}[0], [x1], x5
    ld1 {v0.s}[1], [x0], x7
    ld1 {v1.s}[1], [x1], x5
    uabal v16.8h, v1.8b, v0.8b
    ld1 {v2.s}[0], [x2], x5
    ld1 {v2.s}[1], [x2], x5
    uabal v17.8h, v2.8b, v0.8b
    ld1 {v3.s}[0], [x3], x5
    ld1 {v3.s}[1], [x3], x5
    uabal v18.8h, v3.8b, v0.8b


    uaddlv s0, v16.8h
    uaddlv s1, v17.8h
    uaddlv s2, v18.8h

    stp s0, s1, [x6], #8
    str s2, [x6]
    ret
 # //.size x264_pixel_sad_x3_4x8_neon, . - x264_pixel_sad_x3_4x8_neon
        //.endfunc
        .text
        .align 2
        .globl _x264_pixel_sad_x3_8x4_neon
 # //.type _x264_pixel_sad_x3_8x4_neon, %function
        //.func _x264_pixel_sad_x3_8x4_neon
_x264_pixel_sad_x3_8x4_neon:
    mov x6, x5
    mov x5, x4
    mov x7, #16

    ld1 {v0.8b}, [x0], x7
    ld1 {v1.8b}, [x1], x5
    uabdl v16.8h, v1.8b, v0.8b
    ld1 {v2.8b}, [x2], x5
    ld1 {v5.8b}, [x0], x7
    uabdl v17.8h, v2.8b, v0.8b
    ld1 {v3.8b}, [x3], x5
    ld1 {v1.8b}, [x1], x5
   uabdl v18.8h, v3.8b, v0.8b
    uabal v16.8h, v1.8b, v5.8b
    ld1 {v2.8b}, [x2], x5
    ld1 {v3.8b}, [x3], x5
    uabal v17.8h, v2.8b, v5.8b
    uabal v18.8h, v3.8b, v5.8b


    ld1 {v0.8b}, [x0], x7
    ld1 {v1.8b}, [x1], x5
    uabal v16.8h, v1.8b, v0.8b
    ld1 {v2.8b}, [x2], x5
    ld1 {v5.8b}, [x0], x7
    uabal v17.8h, v2.8b, v0.8b
    ld1 {v3.8b}, [x3], x5
    ld1 {v1.8b}, [x1], x5
   uabal v18.8h, v3.8b, v0.8b
    uabal v16.8h, v1.8b, v5.8b
    ld1 {v2.8b}, [x2], x5
    ld1 {v3.8b}, [x3], x5
    uabal v17.8h, v2.8b, v5.8b
    uabal v18.8h, v3.8b, v5.8b


    uaddlv s0, v16.8h
    uaddlv s1, v17.8h
    uaddlv s2, v18.8h

    stp s0, s1, [x6], #8
    str s2, [x6]
    ret
 # //.size x264_pixel_sad_x3_8x4_neon, . - x264_pixel_sad_x3_8x4_neon
        //.endfunc
        .text
        .align 2
        .globl _x264_pixel_sad_x3_8x8_neon
 # //.type _x264_pixel_sad_x3_8x8_neon, %function
        //.func _x264_pixel_sad_x3_8x8_neon
_x264_pixel_sad_x3_8x8_neon:
    mov x6, x5
    mov x5, x4
    mov x7, #16

    ld1 {v0.8b}, [x0], x7
    ld1 {v1.8b}, [x1], x5
    uabdl v16.8h, v1.8b, v0.8b
    ld1 {v2.8b}, [x2], x5
    ld1 {v5.8b}, [x0], x7
    uabdl v17.8h, v2.8b, v0.8b
    ld1 {v3.8b}, [x3], x5
    ld1 {v1.8b}, [x1], x5
   uabdl v18.8h, v3.8b, v0.8b
    uabal v16.8h, v1.8b, v5.8b
    ld1 {v2.8b}, [x2], x5
    ld1 {v3.8b}, [x3], x5
    uabal v17.8h, v2.8b, v5.8b
    uabal v18.8h, v3.8b, v5.8b


    ld1 {v0.8b}, [x0], x7
    ld1 {v1.8b}, [x1], x5
    uabal v16.8h, v1.8b, v0.8b
    ld1 {v2.8b}, [x2], x5
    ld1 {v5.8b}, [x0], x7
    uabal v17.8h, v2.8b, v0.8b
    ld1 {v3.8b}, [x3], x5
    ld1 {v1.8b}, [x1], x5
   uabal v18.8h, v3.8b, v0.8b
    uabal v16.8h, v1.8b, v5.8b
    ld1 {v2.8b}, [x2], x5
    ld1 {v3.8b}, [x3], x5
    uabal v17.8h, v2.8b, v5.8b
    uabal v18.8h, v3.8b, v5.8b

    ld1 {v0.8b}, [x0], x7
    ld1 {v1.8b}, [x1], x5
    uabal v16.8h, v1.8b, v0.8b
    ld1 {v2.8b}, [x2], x5
    ld1 {v5.8b}, [x0], x7
    uabal v17.8h, v2.8b, v0.8b
    ld1 {v3.8b}, [x3], x5
    ld1 {v1.8b}, [x1], x5
   uabal v18.8h, v3.8b, v0.8b
    uabal v16.8h, v1.8b, v5.8b
    ld1 {v2.8b}, [x2], x5
    ld1 {v3.8b}, [x3], x5
    uabal v17.8h, v2.8b, v5.8b
    uabal v18.8h, v3.8b, v5.8b

    ld1 {v0.8b}, [x0], x7
    ld1 {v1.8b}, [x1], x5
    uabal v16.8h, v1.8b, v0.8b
    ld1 {v2.8b}, [x2], x5
    ld1 {v5.8b}, [x0], x7
    uabal v17.8h, v2.8b, v0.8b
    ld1 {v3.8b}, [x3], x5
    ld1 {v1.8b}, [x1], x5
   uabal v18.8h, v3.8b, v0.8b
    uabal v16.8h, v1.8b, v5.8b
    ld1 {v2.8b}, [x2], x5
    ld1 {v3.8b}, [x3], x5
    uabal v17.8h, v2.8b, v5.8b
    uabal v18.8h, v3.8b, v5.8b


    uaddlv s0, v16.8h
    uaddlv s1, v17.8h
    uaddlv s2, v18.8h

    stp s0, s1, [x6], #8
    str s2, [x6]
    ret
 # //.size x264_pixel_sad_x3_8x8_neon, . - x264_pixel_sad_x3_8x8_neon
        //.endfunc
        .text
        .align 2
        .globl _x264_pixel_sad_x3_8x16_neon
 # //.type _x264_pixel_sad_x3_8x16_neon, %function
        //.func _x264_pixel_sad_x3_8x16_neon
_x264_pixel_sad_x3_8x16_neon:
    mov x6, x5
    mov x5, x4
    mov x7, #16

    ld1 {v0.8b}, [x0], x7
    ld1 {v1.8b}, [x1], x5
    uabdl v16.8h, v1.8b, v0.8b
    ld1 {v2.8b}, [x2], x5
    ld1 {v5.8b}, [x0], x7
    uabdl v17.8h, v2.8b, v0.8b
    ld1 {v3.8b}, [x3], x5
    ld1 {v1.8b}, [x1], x5
   uabdl v18.8h, v3.8b, v0.8b
    uabal v16.8h, v1.8b, v5.8b
    ld1 {v2.8b}, [x2], x5
    ld1 {v3.8b}, [x3], x5
    uabal v17.8h, v2.8b, v5.8b
    uabal v18.8h, v3.8b, v5.8b


    ld1 {v0.8b}, [x0], x7
    ld1 {v1.8b}, [x1], x5
    uabal v16.8h, v1.8b, v0.8b
    ld1 {v2.8b}, [x2], x5
    ld1 {v5.8b}, [x0], x7
    uabal v17.8h, v2.8b, v0.8b
    ld1 {v3.8b}, [x3], x5
    ld1 {v1.8b}, [x1], x5
   uabal v18.8h, v3.8b, v0.8b
    uabal v16.8h, v1.8b, v5.8b
    ld1 {v2.8b}, [x2], x5
    ld1 {v3.8b}, [x3], x5
    uabal v17.8h, v2.8b, v5.8b
    uabal v18.8h, v3.8b, v5.8b

    ld1 {v0.8b}, [x0], x7
    ld1 {v1.8b}, [x1], x5
    uabal v16.8h, v1.8b, v0.8b
    ld1 {v2.8b}, [x2], x5
    ld1 {v5.8b}, [x0], x7
    uabal v17.8h, v2.8b, v0.8b
    ld1 {v3.8b}, [x3], x5
    ld1 {v1.8b}, [x1], x5
   uabal v18.8h, v3.8b, v0.8b
    uabal v16.8h, v1.8b, v5.8b
    ld1 {v2.8b}, [x2], x5
    ld1 {v3.8b}, [x3], x5
    uabal v17.8h, v2.8b, v5.8b
    uabal v18.8h, v3.8b, v5.8b

    ld1 {v0.8b}, [x0], x7
    ld1 {v1.8b}, [x1], x5
    uabal v16.8h, v1.8b, v0.8b
    ld1 {v2.8b}, [x2], x5
    ld1 {v5.8b}, [x0], x7
    uabal v17.8h, v2.8b, v0.8b
    ld1 {v3.8b}, [x3], x5
    ld1 {v1.8b}, [x1], x5
   uabal v18.8h, v3.8b, v0.8b
    uabal v16.8h, v1.8b, v5.8b
    ld1 {v2.8b}, [x2], x5
    ld1 {v3.8b}, [x3], x5
    uabal v17.8h, v2.8b, v5.8b
    uabal v18.8h, v3.8b, v5.8b

    ld1 {v0.8b}, [x0], x7
    ld1 {v1.8b}, [x1], x5
    uabal v16.8h, v1.8b, v0.8b
    ld1 {v2.8b}, [x2], x5
    ld1 {v5.8b}, [x0], x7
    uabal v17.8h, v2.8b, v0.8b
    ld1 {v3.8b}, [x3], x5
    ld1 {v1.8b}, [x1], x5
   uabal v18.8h, v3.8b, v0.8b
    uabal v16.8h, v1.8b, v5.8b
    ld1 {v2.8b}, [x2], x5
    ld1 {v3.8b}, [x3], x5
    uabal v17.8h, v2.8b, v5.8b
    uabal v18.8h, v3.8b, v5.8b

    ld1 {v0.8b}, [x0], x7
    ld1 {v1.8b}, [x1], x5
    uabal v16.8h, v1.8b, v0.8b
    ld1 {v2.8b}, [x2], x5
    ld1 {v5.8b}, [x0], x7
    uabal v17.8h, v2.8b, v0.8b
    ld1 {v3.8b}, [x3], x5
    ld1 {v1.8b}, [x1], x5
   uabal v18.8h, v3.8b, v0.8b
    uabal v16.8h, v1.8b, v5.8b
    ld1 {v2.8b}, [x2], x5
    ld1 {v3.8b}, [x3], x5
    uabal v17.8h, v2.8b, v5.8b
    uabal v18.8h, v3.8b, v5.8b

    ld1 {v0.8b}, [x0], x7
    ld1 {v1.8b}, [x1], x5
    uabal v16.8h, v1.8b, v0.8b
    ld1 {v2.8b}, [x2], x5
    ld1 {v5.8b}, [x0], x7
    uabal v17.8h, v2.8b, v0.8b
    ld1 {v3.8b}, [x3], x5
    ld1 {v1.8b}, [x1], x5
   uabal v18.8h, v3.8b, v0.8b
    uabal v16.8h, v1.8b, v5.8b
    ld1 {v2.8b}, [x2], x5
    ld1 {v3.8b}, [x3], x5
    uabal v17.8h, v2.8b, v5.8b
    uabal v18.8h, v3.8b, v5.8b

    ld1 {v0.8b}, [x0], x7
    ld1 {v1.8b}, [x1], x5
    uabal v16.8h, v1.8b, v0.8b
    ld1 {v2.8b}, [x2], x5
    ld1 {v5.8b}, [x0], x7
    uabal v17.8h, v2.8b, v0.8b
    ld1 {v3.8b}, [x3], x5
    ld1 {v1.8b}, [x1], x5
   uabal v18.8h, v3.8b, v0.8b
    uabal v16.8h, v1.8b, v5.8b
    ld1 {v2.8b}, [x2], x5
    ld1 {v3.8b}, [x3], x5
    uabal v17.8h, v2.8b, v5.8b
    uabal v18.8h, v3.8b, v5.8b


    uaddlv s0, v16.8h
    uaddlv s1, v17.8h
    uaddlv s2, v18.8h

    stp s0, s1, [x6], #8
    str s2, [x6]
    ret
 # //.size x264_pixel_sad_x3_8x16_neon, . - x264_pixel_sad_x3_8x16_neon
        //.endfunc
        .text
        .align 2
        .globl _x264_pixel_sad_x3_16x8_neon
 # //.type _x264_pixel_sad_x3_16x8_neon, %function
        //.func _x264_pixel_sad_x3_16x8_neon
_x264_pixel_sad_x3_16x8_neon:
    mov x6, x5
    mov x5, x4
    mov x7, #16

    ld1 {v0.16b}, [x0], x7
    ld1 {v1.16b}, [x1], x5
    uabdl v16.8h, v1.8b, v0.8b
    uabdl2 v20.8h, v1.16b, v0.16b
    ld1 {v2.16b}, [x2], x5
    ld1 {v5.16b}, [x0], x7
    uabdl v17.8h, v2.8b, v0.8b
    uabdl2 v21.8h, v2.16b, v0.16b
    ld1 {v3.16b}, [x3], x5
    ld1 {v1.16b}, [x1], x5
    uabdl v18.8h, v3.8b, v0.8b
    uabdl2 v22.8h, v3.16b, v0.16b
    uabal v16.8h, v1.8b, v5.8b
    uabal2 v20.8h, v1.16b, v5.16b
    ld1 {v2.16b}, [x2], x5
    ld1 {v3.16b}, [x3], x5
    uabal v17.8h, v2.8b, v5.8b
    uabal2 v21.8h, v2.16b, v5.16b
    uabal v18.8h, v3.8b, v5.8b
    uabal2 v22.8h, v3.16b, v5.16b


    ld1 {v0.16b}, [x0], x7
    ld1 {v1.16b}, [x1], x5
    uabal v16.8h, v1.8b, v0.8b
    uabal2 v20.8h, v1.16b, v0.16b
    ld1 {v2.16b}, [x2], x5
    ld1 {v5.16b}, [x0], x7
    uabal v17.8h, v2.8b, v0.8b
    uabal2 v21.8h, v2.16b, v0.16b
    ld1 {v3.16b}, [x3], x5
    ld1 {v1.16b}, [x1], x5
    uabal v18.8h, v3.8b, v0.8b
    uabal2 v22.8h, v3.16b, v0.16b
    uabal v16.8h, v1.8b, v5.8b
    uabal2 v20.8h, v1.16b, v5.16b
    ld1 {v2.16b}, [x2], x5
    ld1 {v3.16b}, [x3], x5
    uabal v17.8h, v2.8b, v5.8b
    uabal2 v21.8h, v2.16b, v5.16b
    uabal v18.8h, v3.8b, v5.8b
    uabal2 v22.8h, v3.16b, v5.16b

    ld1 {v0.16b}, [x0], x7
    ld1 {v1.16b}, [x1], x5
    uabal v16.8h, v1.8b, v0.8b
    uabal2 v20.8h, v1.16b, v0.16b
    ld1 {v2.16b}, [x2], x5
    ld1 {v5.16b}, [x0], x7
    uabal v17.8h, v2.8b, v0.8b
    uabal2 v21.8h, v2.16b, v0.16b
    ld1 {v3.16b}, [x3], x5
    ld1 {v1.16b}, [x1], x5
    uabal v18.8h, v3.8b, v0.8b
    uabal2 v22.8h, v3.16b, v0.16b
    uabal v16.8h, v1.8b, v5.8b
    uabal2 v20.8h, v1.16b, v5.16b
    ld1 {v2.16b}, [x2], x5
    ld1 {v3.16b}, [x3], x5
    uabal v17.8h, v2.8b, v5.8b
    uabal2 v21.8h, v2.16b, v5.16b
    uabal v18.8h, v3.8b, v5.8b
    uabal2 v22.8h, v3.16b, v5.16b

    ld1 {v0.16b}, [x0], x7
    ld1 {v1.16b}, [x1], x5
    uabal v16.8h, v1.8b, v0.8b
    uabal2 v20.8h, v1.16b, v0.16b
    ld1 {v2.16b}, [x2], x5
    ld1 {v5.16b}, [x0], x7
    uabal v17.8h, v2.8b, v0.8b
    uabal2 v21.8h, v2.16b, v0.16b
    ld1 {v3.16b}, [x3], x5
    ld1 {v1.16b}, [x1], x5
    uabal v18.8h, v3.8b, v0.8b
    uabal2 v22.8h, v3.16b, v0.16b
    uabal v16.8h, v1.8b, v5.8b
    uabal2 v20.8h, v1.16b, v5.16b
    ld1 {v2.16b}, [x2], x5
    ld1 {v3.16b}, [x3], x5
    uabal v17.8h, v2.8b, v5.8b
    uabal2 v21.8h, v2.16b, v5.16b
    uabal v18.8h, v3.8b, v5.8b
    uabal2 v22.8h, v3.16b, v5.16b

    add v16.8h, v16.8h, v20.8h
    add v17.8h, v17.8h, v21.8h
    add v18.8h, v18.8h, v22.8h

    uaddlv s0, v16.8h
    uaddlv s1, v17.8h
    uaddlv s2, v18.8h

    stp s0, s1, [x6], #8
    str s2, [x6]
    ret
 # //.size x264_pixel_sad_x3_16x8_neon, . - x264_pixel_sad_x3_16x8_neon
        //.endfunc
        .text
        .align 2
        .globl _x264_pixel_sad_x3_16x16_neon
 # //.type _x264_pixel_sad_x3_16x16_neon, %function
        //.func _x264_pixel_sad_x3_16x16_neon
_x264_pixel_sad_x3_16x16_neon:
    mov x6, x5
    mov x5, x4
    mov x7, #16

    ld1 {v0.16b}, [x0], x7
    ld1 {v1.16b}, [x1], x5
    uabdl v16.8h, v1.8b, v0.8b
    uabdl2 v20.8h, v1.16b, v0.16b
    ld1 {v2.16b}, [x2], x5
    ld1 {v5.16b}, [x0], x7
    uabdl v17.8h, v2.8b, v0.8b
    uabdl2 v21.8h, v2.16b, v0.16b
    ld1 {v3.16b}, [x3], x5
    ld1 {v1.16b}, [x1], x5
    uabdl v18.8h, v3.8b, v0.8b
    uabdl2 v22.8h, v3.16b, v0.16b
    uabal v16.8h, v1.8b, v5.8b
    uabal2 v20.8h, v1.16b, v5.16b
    ld1 {v2.16b}, [x2], x5
    ld1 {v3.16b}, [x3], x5
    uabal v17.8h, v2.8b, v5.8b
    uabal2 v21.8h, v2.16b, v5.16b
    uabal v18.8h, v3.8b, v5.8b
    uabal2 v22.8h, v3.16b, v5.16b


    ld1 {v0.16b}, [x0], x7
    ld1 {v1.16b}, [x1], x5
    uabal v16.8h, v1.8b, v0.8b
    uabal2 v20.8h, v1.16b, v0.16b
    ld1 {v2.16b}, [x2], x5
    ld1 {v5.16b}, [x0], x7
    uabal v17.8h, v2.8b, v0.8b
    uabal2 v21.8h, v2.16b, v0.16b
    ld1 {v3.16b}, [x3], x5
    ld1 {v1.16b}, [x1], x5
    uabal v18.8h, v3.8b, v0.8b
    uabal2 v22.8h, v3.16b, v0.16b
    uabal v16.8h, v1.8b, v5.8b
    uabal2 v20.8h, v1.16b, v5.16b
    ld1 {v2.16b}, [x2], x5
    ld1 {v3.16b}, [x3], x5
    uabal v17.8h, v2.8b, v5.8b
    uabal2 v21.8h, v2.16b, v5.16b
    uabal v18.8h, v3.8b, v5.8b
    uabal2 v22.8h, v3.16b, v5.16b

    ld1 {v0.16b}, [x0], x7
    ld1 {v1.16b}, [x1], x5
    uabal v16.8h, v1.8b, v0.8b
    uabal2 v20.8h, v1.16b, v0.16b
    ld1 {v2.16b}, [x2], x5
    ld1 {v5.16b}, [x0], x7
    uabal v17.8h, v2.8b, v0.8b
    uabal2 v21.8h, v2.16b, v0.16b
    ld1 {v3.16b}, [x3], x5
    ld1 {v1.16b}, [x1], x5
    uabal v18.8h, v3.8b, v0.8b
    uabal2 v22.8h, v3.16b, v0.16b
    uabal v16.8h, v1.8b, v5.8b
    uabal2 v20.8h, v1.16b, v5.16b
    ld1 {v2.16b}, [x2], x5
    ld1 {v3.16b}, [x3], x5
    uabal v17.8h, v2.8b, v5.8b
    uabal2 v21.8h, v2.16b, v5.16b
    uabal v18.8h, v3.8b, v5.8b
    uabal2 v22.8h, v3.16b, v5.16b

    ld1 {v0.16b}, [x0], x7
    ld1 {v1.16b}, [x1], x5
    uabal v16.8h, v1.8b, v0.8b
    uabal2 v20.8h, v1.16b, v0.16b
    ld1 {v2.16b}, [x2], x5
    ld1 {v5.16b}, [x0], x7
    uabal v17.8h, v2.8b, v0.8b
    uabal2 v21.8h, v2.16b, v0.16b
    ld1 {v3.16b}, [x3], x5
    ld1 {v1.16b}, [x1], x5
    uabal v18.8h, v3.8b, v0.8b
    uabal2 v22.8h, v3.16b, v0.16b
    uabal v16.8h, v1.8b, v5.8b
    uabal2 v20.8h, v1.16b, v5.16b
    ld1 {v2.16b}, [x2], x5
    ld1 {v3.16b}, [x3], x5
    uabal v17.8h, v2.8b, v5.8b
    uabal2 v21.8h, v2.16b, v5.16b
    uabal v18.8h, v3.8b, v5.8b
    uabal2 v22.8h, v3.16b, v5.16b

    ld1 {v0.16b}, [x0], x7
    ld1 {v1.16b}, [x1], x5
    uabal v16.8h, v1.8b, v0.8b
    uabal2 v20.8h, v1.16b, v0.16b
    ld1 {v2.16b}, [x2], x5
    ld1 {v5.16b}, [x0], x7
    uabal v17.8h, v2.8b, v0.8b
    uabal2 v21.8h, v2.16b, v0.16b
    ld1 {v3.16b}, [x3], x5
    ld1 {v1.16b}, [x1], x5
    uabal v18.8h, v3.8b, v0.8b
    uabal2 v22.8h, v3.16b, v0.16b
    uabal v16.8h, v1.8b, v5.8b
    uabal2 v20.8h, v1.16b, v5.16b
    ld1 {v2.16b}, [x2], x5
    ld1 {v3.16b}, [x3], x5
    uabal v17.8h, v2.8b, v5.8b
    uabal2 v21.8h, v2.16b, v5.16b
    uabal v18.8h, v3.8b, v5.8b
    uabal2 v22.8h, v3.16b, v5.16b

    ld1 {v0.16b}, [x0], x7
    ld1 {v1.16b}, [x1], x5
    uabal v16.8h, v1.8b, v0.8b
    uabal2 v20.8h, v1.16b, v0.16b
    ld1 {v2.16b}, [x2], x5
    ld1 {v5.16b}, [x0], x7
    uabal v17.8h, v2.8b, v0.8b
    uabal2 v21.8h, v2.16b, v0.16b
    ld1 {v3.16b}, [x3], x5
    ld1 {v1.16b}, [x1], x5
    uabal v18.8h, v3.8b, v0.8b
    uabal2 v22.8h, v3.16b, v0.16b
    uabal v16.8h, v1.8b, v5.8b
    uabal2 v20.8h, v1.16b, v5.16b
    ld1 {v2.16b}, [x2], x5
    ld1 {v3.16b}, [x3], x5
    uabal v17.8h, v2.8b, v5.8b
    uabal2 v21.8h, v2.16b, v5.16b
    uabal v18.8h, v3.8b, v5.8b
    uabal2 v22.8h, v3.16b, v5.16b

    ld1 {v0.16b}, [x0], x7
    ld1 {v1.16b}, [x1], x5
    uabal v16.8h, v1.8b, v0.8b
    uabal2 v20.8h, v1.16b, v0.16b
    ld1 {v2.16b}, [x2], x5
    ld1 {v5.16b}, [x0], x7
    uabal v17.8h, v2.8b, v0.8b
    uabal2 v21.8h, v2.16b, v0.16b
    ld1 {v3.16b}, [x3], x5
    ld1 {v1.16b}, [x1], x5
    uabal v18.8h, v3.8b, v0.8b
    uabal2 v22.8h, v3.16b, v0.16b
    uabal v16.8h, v1.8b, v5.8b
    uabal2 v20.8h, v1.16b, v5.16b
    ld1 {v2.16b}, [x2], x5
    ld1 {v3.16b}, [x3], x5
    uabal v17.8h, v2.8b, v5.8b
    uabal2 v21.8h, v2.16b, v5.16b
    uabal v18.8h, v3.8b, v5.8b
    uabal2 v22.8h, v3.16b, v5.16b

    ld1 {v0.16b}, [x0], x7
    ld1 {v1.16b}, [x1], x5
    uabal v16.8h, v1.8b, v0.8b
    uabal2 v20.8h, v1.16b, v0.16b
    ld1 {v2.16b}, [x2], x5
    ld1 {v5.16b}, [x0], x7
    uabal v17.8h, v2.8b, v0.8b
    uabal2 v21.8h, v2.16b, v0.16b
    ld1 {v3.16b}, [x3], x5
    ld1 {v1.16b}, [x1], x5
    uabal v18.8h, v3.8b, v0.8b
    uabal2 v22.8h, v3.16b, v0.16b
    uabal v16.8h, v1.8b, v5.8b
    uabal2 v20.8h, v1.16b, v5.16b
    ld1 {v2.16b}, [x2], x5
    ld1 {v3.16b}, [x3], x5
    uabal v17.8h, v2.8b, v5.8b
    uabal2 v21.8h, v2.16b, v5.16b
    uabal v18.8h, v3.8b, v5.8b
    uabal2 v22.8h, v3.16b, v5.16b

    add v16.8h, v16.8h, v20.8h
    add v17.8h, v17.8h, v21.8h
    add v18.8h, v18.8h, v22.8h

    uaddlv s0, v16.8h
    uaddlv s1, v17.8h
    uaddlv s2, v18.8h

    stp s0, s1, [x6], #8
    str s2, [x6]
    ret
 # //.size x264_pixel_sad_x3_16x16_neon, . - x264_pixel_sad_x3_16x16_neon
        //.endfunc

        .text
        .align 2
        .globl _x264_pixel_sad_x4_4x4_neon
 # //.type _x264_pixel_sad_x4_4x4_neon, %function
        //.func _x264_pixel_sad_x4_4x4_neon
_x264_pixel_sad_x4_4x4_neon:
    mov x7, #16

    ld1 {v0.s}[0], [x0], x7
    ld1 {v1.s}[0], [x1], x5
    ld1 {v0.s}[1], [x0], x7
    ld1 {v1.s}[1], [x1], x5
    uabdl v16.8h, v1.8b, v0.8b
    ld1 {v2.s}[0], [x2], x5
    ld1 {v2.s}[1], [x2], x5
    uabdl v17.8h, v2.8b, v0.8b
    ld1 {v3.s}[0], [x3], x5
    ld1 {v3.s}[1], [x3], x5
    uabdl v18.8h, v3.8b, v0.8b
    ld1 {v4.s}[0], [x4], x5
    ld1 {v4.s}[1], [x4], x5
    uabdl v19.8h, v4.8b, v0.8b


    ld1 {v0.s}[0], [x0], x7
    ld1 {v1.s}[0], [x1], x5
    ld1 {v0.s}[1], [x0], x7
    ld1 {v1.s}[1], [x1], x5
    uabal v16.8h, v1.8b, v0.8b
    ld1 {v2.s}[0], [x2], x5
    ld1 {v2.s}[1], [x2], x5
    uabal v17.8h, v2.8b, v0.8b
    ld1 {v3.s}[0], [x3], x5
    ld1 {v3.s}[1], [x3], x5
    uabal v18.8h, v3.8b, v0.8b
    ld1 {v4.s}[0], [x4], x5
    ld1 {v4.s}[1], [x4], x5
    uabal v19.8h, v4.8b, v0.8b


    uaddlv s0, v16.8h
    uaddlv s1, v17.8h
    uaddlv s2, v18.8h

    stp s0, s1, [x6], #8
    uaddlv s3, v19.8h
    stp s2, s3, [x6]
    ret
 # //.size x264_pixel_sad_x4_4x4_neon, . - x264_pixel_sad_x4_4x4_neon
        //.endfunc
        .text
        .align 2
        .globl _x264_pixel_sad_x4_4x8_neon
 # //.type _x264_pixel_sad_x4_4x8_neon, %function
        //.func _x264_pixel_sad_x4_4x8_neon
_x264_pixel_sad_x4_4x8_neon:
    mov x7, #16

    ld1 {v0.s}[0], [x0], x7
    ld1 {v1.s}[0], [x1], x5
    ld1 {v0.s}[1], [x0], x7
    ld1 {v1.s}[1], [x1], x5
    uabdl v16.8h, v1.8b, v0.8b
    ld1 {v2.s}[0], [x2], x5
    ld1 {v2.s}[1], [x2], x5
    uabdl v17.8h, v2.8b, v0.8b
    ld1 {v3.s}[0], [x3], x5
    ld1 {v3.s}[1], [x3], x5
    uabdl v18.8h, v3.8b, v0.8b
    ld1 {v4.s}[0], [x4], x5
    ld1 {v4.s}[1], [x4], x5
    uabdl v19.8h, v4.8b, v0.8b


    ld1 {v0.s}[0], [x0], x7
    ld1 {v1.s}[0], [x1], x5
    ld1 {v0.s}[1], [x0], x7
    ld1 {v1.s}[1], [x1], x5
    uabal v16.8h, v1.8b, v0.8b
    ld1 {v2.s}[0], [x2], x5
    ld1 {v2.s}[1], [x2], x5
    uabal v17.8h, v2.8b, v0.8b
    ld1 {v3.s}[0], [x3], x5
    ld1 {v3.s}[1], [x3], x5
    uabal v18.8h, v3.8b, v0.8b
    ld1 {v4.s}[0], [x4], x5
    ld1 {v4.s}[1], [x4], x5
    uabal v19.8h, v4.8b, v0.8b

    ld1 {v0.s}[0], [x0], x7
    ld1 {v1.s}[0], [x1], x5
    ld1 {v0.s}[1], [x0], x7
    ld1 {v1.s}[1], [x1], x5
    uabal v16.8h, v1.8b, v0.8b
    ld1 {v2.s}[0], [x2], x5
    ld1 {v2.s}[1], [x2], x5
    uabal v17.8h, v2.8b, v0.8b
    ld1 {v3.s}[0], [x3], x5
    ld1 {v3.s}[1], [x3], x5
    uabal v18.8h, v3.8b, v0.8b
    ld1 {v4.s}[0], [x4], x5
    ld1 {v4.s}[1], [x4], x5
    uabal v19.8h, v4.8b, v0.8b

    ld1 {v0.s}[0], [x0], x7
    ld1 {v1.s}[0], [x1], x5
    ld1 {v0.s}[1], [x0], x7
    ld1 {v1.s}[1], [x1], x5
    uabal v16.8h, v1.8b, v0.8b
    ld1 {v2.s}[0], [x2], x5
    ld1 {v2.s}[1], [x2], x5
    uabal v17.8h, v2.8b, v0.8b
    ld1 {v3.s}[0], [x3], x5
    ld1 {v3.s}[1], [x3], x5
    uabal v18.8h, v3.8b, v0.8b
    ld1 {v4.s}[0], [x4], x5
    ld1 {v4.s}[1], [x4], x5
    uabal v19.8h, v4.8b, v0.8b


    uaddlv s0, v16.8h
    uaddlv s1, v17.8h
    uaddlv s2, v18.8h

    stp s0, s1, [x6], #8
    uaddlv s3, v19.8h
    stp s2, s3, [x6]
    ret
 # //.size x264_pixel_sad_x4_4x8_neon, . - x264_pixel_sad_x4_4x8_neon
        //.endfunc
        .text
        .align 2
        .globl _x264_pixel_sad_x4_8x4_neon
 # //.type _x264_pixel_sad_x4_8x4_neon, %function
        //.func _x264_pixel_sad_x4_8x4_neon
_x264_pixel_sad_x4_8x4_neon:
    mov x7, #16

    ld1 {v0.8b}, [x0], x7
    ld1 {v1.8b}, [x1], x5
    uabdl v16.8h, v1.8b, v0.8b
    ld1 {v2.8b}, [x2], x5
    ld1 {v5.8b}, [x0], x7
    uabdl v17.8h, v2.8b, v0.8b
    ld1 {v3.8b}, [x3], x5
    ld1 {v1.8b}, [x1], x5
   uabdl v18.8h, v3.8b, v0.8b
    uabal v16.8h, v1.8b, v5.8b
    ld1 {v2.8b}, [x2], x5
    ld1 {v3.8b}, [x3], x5
    uabal v17.8h, v2.8b, v5.8b
    uabal v18.8h, v3.8b, v5.8b
    ld1 {v4.8b}, [x4], x5
    uabdl v19.8h, v4.8b, v0.8b
    ld1 {v4.8b}, [x4], x5
    uabal v19.8h, v4.8b, v5.8b


    ld1 {v0.8b}, [x0], x7
    ld1 {v1.8b}, [x1], x5
    uabal v16.8h, v1.8b, v0.8b
    ld1 {v2.8b}, [x2], x5
    ld1 {v5.8b}, [x0], x7
    uabal v17.8h, v2.8b, v0.8b
    ld1 {v3.8b}, [x3], x5
    ld1 {v1.8b}, [x1], x5
   uabal v18.8h, v3.8b, v0.8b
    uabal v16.8h, v1.8b, v5.8b
    ld1 {v2.8b}, [x2], x5
    ld1 {v3.8b}, [x3], x5
    uabal v17.8h, v2.8b, v5.8b
    uabal v18.8h, v3.8b, v5.8b
    ld1 {v4.8b}, [x4], x5
    uabal v19.8h, v4.8b, v0.8b
    ld1 {v4.8b}, [x4], x5
    uabal v19.8h, v4.8b, v5.8b


    uaddlv s0, v16.8h
    uaddlv s1, v17.8h
    uaddlv s2, v18.8h

    stp s0, s1, [x6], #8
    uaddlv s3, v19.8h
    stp s2, s3, [x6]
    ret
 # //.size x264_pixel_sad_x4_8x4_neon, . - x264_pixel_sad_x4_8x4_neon
        //.endfunc
        .text
        .align 2
        .globl _x264_pixel_sad_x4_8x8_neon
 # //.type _x264_pixel_sad_x4_8x8_neon, %function
        //.func _x264_pixel_sad_x4_8x8_neon
_x264_pixel_sad_x4_8x8_neon:
    mov x7, #16

    ld1 {v0.8b}, [x0], x7
    ld1 {v1.8b}, [x1], x5
    uabdl v16.8h, v1.8b, v0.8b
    ld1 {v2.8b}, [x2], x5
    ld1 {v5.8b}, [x0], x7
    uabdl v17.8h, v2.8b, v0.8b
    ld1 {v3.8b}, [x3], x5
    ld1 {v1.8b}, [x1], x5
   uabdl v18.8h, v3.8b, v0.8b
    uabal v16.8h, v1.8b, v5.8b
    ld1 {v2.8b}, [x2], x5
    ld1 {v3.8b}, [x3], x5
    uabal v17.8h, v2.8b, v5.8b
    uabal v18.8h, v3.8b, v5.8b
    ld1 {v4.8b}, [x4], x5
    uabdl v19.8h, v4.8b, v0.8b
    ld1 {v4.8b}, [x4], x5
    uabal v19.8h, v4.8b, v5.8b


    ld1 {v0.8b}, [x0], x7
    ld1 {v1.8b}, [x1], x5
    uabal v16.8h, v1.8b, v0.8b
    ld1 {v2.8b}, [x2], x5
    ld1 {v5.8b}, [x0], x7
    uabal v17.8h, v2.8b, v0.8b
    ld1 {v3.8b}, [x3], x5
    ld1 {v1.8b}, [x1], x5
   uabal v18.8h, v3.8b, v0.8b
    uabal v16.8h, v1.8b, v5.8b
    ld1 {v2.8b}, [x2], x5
    ld1 {v3.8b}, [x3], x5
    uabal v17.8h, v2.8b, v5.8b
    uabal v18.8h, v3.8b, v5.8b
    ld1 {v4.8b}, [x4], x5
    uabal v19.8h, v4.8b, v0.8b
    ld1 {v4.8b}, [x4], x5
    uabal v19.8h, v4.8b, v5.8b

    ld1 {v0.8b}, [x0], x7
    ld1 {v1.8b}, [x1], x5
    uabal v16.8h, v1.8b, v0.8b
    ld1 {v2.8b}, [x2], x5
    ld1 {v5.8b}, [x0], x7
    uabal v17.8h, v2.8b, v0.8b
    ld1 {v3.8b}, [x3], x5
    ld1 {v1.8b}, [x1], x5
   uabal v18.8h, v3.8b, v0.8b
    uabal v16.8h, v1.8b, v5.8b
    ld1 {v2.8b}, [x2], x5
    ld1 {v3.8b}, [x3], x5
    uabal v17.8h, v2.8b, v5.8b
    uabal v18.8h, v3.8b, v5.8b
    ld1 {v4.8b}, [x4], x5
    uabal v19.8h, v4.8b, v0.8b
    ld1 {v4.8b}, [x4], x5
    uabal v19.8h, v4.8b, v5.8b

    ld1 {v0.8b}, [x0], x7
    ld1 {v1.8b}, [x1], x5
    uabal v16.8h, v1.8b, v0.8b
    ld1 {v2.8b}, [x2], x5
    ld1 {v5.8b}, [x0], x7
    uabal v17.8h, v2.8b, v0.8b
    ld1 {v3.8b}, [x3], x5
    ld1 {v1.8b}, [x1], x5
   uabal v18.8h, v3.8b, v0.8b
    uabal v16.8h, v1.8b, v5.8b
    ld1 {v2.8b}, [x2], x5
    ld1 {v3.8b}, [x3], x5
    uabal v17.8h, v2.8b, v5.8b
    uabal v18.8h, v3.8b, v5.8b
    ld1 {v4.8b}, [x4], x5
    uabal v19.8h, v4.8b, v0.8b
    ld1 {v4.8b}, [x4], x5
    uabal v19.8h, v4.8b, v5.8b


    uaddlv s0, v16.8h
    uaddlv s1, v17.8h
    uaddlv s2, v18.8h

    stp s0, s1, [x6], #8
    uaddlv s3, v19.8h
    stp s2, s3, [x6]
    ret
 # //.size x264_pixel_sad_x4_8x8_neon, . - x264_pixel_sad_x4_8x8_neon
        //.endfunc
        .text
        .align 2
        .globl _x264_pixel_sad_x4_8x16_neon
 # //.type _x264_pixel_sad_x4_8x16_neon, %function
        //.func _x264_pixel_sad_x4_8x16_neon
_x264_pixel_sad_x4_8x16_neon:
    mov x7, #16

    ld1 {v0.8b}, [x0], x7
    ld1 {v1.8b}, [x1], x5
    uabdl v16.8h, v1.8b, v0.8b
    ld1 {v2.8b}, [x2], x5
    ld1 {v5.8b}, [x0], x7
    uabdl v17.8h, v2.8b, v0.8b
    ld1 {v3.8b}, [x3], x5
    ld1 {v1.8b}, [x1], x5
   uabdl v18.8h, v3.8b, v0.8b
    uabal v16.8h, v1.8b, v5.8b
    ld1 {v2.8b}, [x2], x5
    ld1 {v3.8b}, [x3], x5
    uabal v17.8h, v2.8b, v5.8b
    uabal v18.8h, v3.8b, v5.8b
    ld1 {v4.8b}, [x4], x5
    uabdl v19.8h, v4.8b, v0.8b
    ld1 {v4.8b}, [x4], x5
    uabal v19.8h, v4.8b, v5.8b


    ld1 {v0.8b}, [x0], x7
    ld1 {v1.8b}, [x1], x5
    uabal v16.8h, v1.8b, v0.8b
    ld1 {v2.8b}, [x2], x5
    ld1 {v5.8b}, [x0], x7
    uabal v17.8h, v2.8b, v0.8b
    ld1 {v3.8b}, [x3], x5
    ld1 {v1.8b}, [x1], x5
   uabal v18.8h, v3.8b, v0.8b
    uabal v16.8h, v1.8b, v5.8b
    ld1 {v2.8b}, [x2], x5
    ld1 {v3.8b}, [x3], x5
    uabal v17.8h, v2.8b, v5.8b
    uabal v18.8h, v3.8b, v5.8b
    ld1 {v4.8b}, [x4], x5
    uabal v19.8h, v4.8b, v0.8b
    ld1 {v4.8b}, [x4], x5
    uabal v19.8h, v4.8b, v5.8b

    ld1 {v0.8b}, [x0], x7
    ld1 {v1.8b}, [x1], x5
    uabal v16.8h, v1.8b, v0.8b
    ld1 {v2.8b}, [x2], x5
    ld1 {v5.8b}, [x0], x7
    uabal v17.8h, v2.8b, v0.8b
    ld1 {v3.8b}, [x3], x5
    ld1 {v1.8b}, [x1], x5
   uabal v18.8h, v3.8b, v0.8b
    uabal v16.8h, v1.8b, v5.8b
    ld1 {v2.8b}, [x2], x5
    ld1 {v3.8b}, [x3], x5
    uabal v17.8h, v2.8b, v5.8b
    uabal v18.8h, v3.8b, v5.8b
    ld1 {v4.8b}, [x4], x5
    uabal v19.8h, v4.8b, v0.8b
    ld1 {v4.8b}, [x4], x5
    uabal v19.8h, v4.8b, v5.8b

    ld1 {v0.8b}, [x0], x7
    ld1 {v1.8b}, [x1], x5
    uabal v16.8h, v1.8b, v0.8b
    ld1 {v2.8b}, [x2], x5
    ld1 {v5.8b}, [x0], x7
    uabal v17.8h, v2.8b, v0.8b
    ld1 {v3.8b}, [x3], x5
    ld1 {v1.8b}, [x1], x5
   uabal v18.8h, v3.8b, v0.8b
    uabal v16.8h, v1.8b, v5.8b
    ld1 {v2.8b}, [x2], x5
    ld1 {v3.8b}, [x3], x5
    uabal v17.8h, v2.8b, v5.8b
    uabal v18.8h, v3.8b, v5.8b
    ld1 {v4.8b}, [x4], x5
    uabal v19.8h, v4.8b, v0.8b
    ld1 {v4.8b}, [x4], x5
    uabal v19.8h, v4.8b, v5.8b

    ld1 {v0.8b}, [x0], x7
    ld1 {v1.8b}, [x1], x5
    uabal v16.8h, v1.8b, v0.8b
    ld1 {v2.8b}, [x2], x5
    ld1 {v5.8b}, [x0], x7
    uabal v17.8h, v2.8b, v0.8b
    ld1 {v3.8b}, [x3], x5
    ld1 {v1.8b}, [x1], x5
   uabal v18.8h, v3.8b, v0.8b
    uabal v16.8h, v1.8b, v5.8b
    ld1 {v2.8b}, [x2], x5
    ld1 {v3.8b}, [x3], x5
    uabal v17.8h, v2.8b, v5.8b
    uabal v18.8h, v3.8b, v5.8b
    ld1 {v4.8b}, [x4], x5
    uabal v19.8h, v4.8b, v0.8b
    ld1 {v4.8b}, [x4], x5
    uabal v19.8h, v4.8b, v5.8b

    ld1 {v0.8b}, [x0], x7
    ld1 {v1.8b}, [x1], x5
    uabal v16.8h, v1.8b, v0.8b
    ld1 {v2.8b}, [x2], x5
    ld1 {v5.8b}, [x0], x7
    uabal v17.8h, v2.8b, v0.8b
    ld1 {v3.8b}, [x3], x5
    ld1 {v1.8b}, [x1], x5
   uabal v18.8h, v3.8b, v0.8b
    uabal v16.8h, v1.8b, v5.8b
    ld1 {v2.8b}, [x2], x5
    ld1 {v3.8b}, [x3], x5
    uabal v17.8h, v2.8b, v5.8b
    uabal v18.8h, v3.8b, v5.8b
    ld1 {v4.8b}, [x4], x5
    uabal v19.8h, v4.8b, v0.8b
    ld1 {v4.8b}, [x4], x5
    uabal v19.8h, v4.8b, v5.8b

    ld1 {v0.8b}, [x0], x7
    ld1 {v1.8b}, [x1], x5
    uabal v16.8h, v1.8b, v0.8b
    ld1 {v2.8b}, [x2], x5
    ld1 {v5.8b}, [x0], x7
    uabal v17.8h, v2.8b, v0.8b
    ld1 {v3.8b}, [x3], x5
    ld1 {v1.8b}, [x1], x5
   uabal v18.8h, v3.8b, v0.8b
    uabal v16.8h, v1.8b, v5.8b
    ld1 {v2.8b}, [x2], x5
    ld1 {v3.8b}, [x3], x5
    uabal v17.8h, v2.8b, v5.8b
    uabal v18.8h, v3.8b, v5.8b
    ld1 {v4.8b}, [x4], x5
    uabal v19.8h, v4.8b, v0.8b
    ld1 {v4.8b}, [x4], x5
    uabal v19.8h, v4.8b, v5.8b

    ld1 {v0.8b}, [x0], x7
    ld1 {v1.8b}, [x1], x5
    uabal v16.8h, v1.8b, v0.8b
    ld1 {v2.8b}, [x2], x5
    ld1 {v5.8b}, [x0], x7
    uabal v17.8h, v2.8b, v0.8b
    ld1 {v3.8b}, [x3], x5
    ld1 {v1.8b}, [x1], x5
   uabal v18.8h, v3.8b, v0.8b
    uabal v16.8h, v1.8b, v5.8b
    ld1 {v2.8b}, [x2], x5
    ld1 {v3.8b}, [x3], x5
    uabal v17.8h, v2.8b, v5.8b
    uabal v18.8h, v3.8b, v5.8b
    ld1 {v4.8b}, [x4], x5
    uabal v19.8h, v4.8b, v0.8b
    ld1 {v4.8b}, [x4], x5
    uabal v19.8h, v4.8b, v5.8b


    uaddlv s0, v16.8h
    uaddlv s1, v17.8h
    uaddlv s2, v18.8h

    stp s0, s1, [x6], #8
    uaddlv s3, v19.8h
    stp s2, s3, [x6]
    ret
 # //.size x264_pixel_sad_x4_8x16_neon, . - x264_pixel_sad_x4_8x16_neon
        //.endfunc
        .text
        .align 2
        .globl _x264_pixel_sad_x4_16x8_neon
 # //.type _x264_pixel_sad_x4_16x8_neon, %function
        //.func _x264_pixel_sad_x4_16x8_neon
_x264_pixel_sad_x4_16x8_neon:
    mov x7, #16

    ld1 {v0.16b}, [x0], x7
    ld1 {v1.16b}, [x1], x5
    uabdl v16.8h, v1.8b, v0.8b
    uabdl2 v20.8h, v1.16b, v0.16b
    ld1 {v2.16b}, [x2], x5
    ld1 {v5.16b}, [x0], x7
    uabdl v17.8h, v2.8b, v0.8b
    uabdl2 v21.8h, v2.16b, v0.16b
    ld1 {v3.16b}, [x3], x5
    ld1 {v1.16b}, [x1], x5
    uabdl v18.8h, v3.8b, v0.8b
    uabdl2 v22.8h, v3.16b, v0.16b
    uabal v16.8h, v1.8b, v5.8b
    uabal2 v20.8h, v1.16b, v5.16b
    ld1 {v2.16b}, [x2], x5
    ld1 {v3.16b}, [x3], x5
    uabal v17.8h, v2.8b, v5.8b
    uabal2 v21.8h, v2.16b, v5.16b
    uabal v18.8h, v3.8b, v5.8b
    uabal2 v22.8h, v3.16b, v5.16b
    ld1 {v4.16b}, [x4], x5
    uabdl v19.8h, v4.8b, v0.8b
    uabdl2 v23.8h, v4.16b, v0.16b
    ld1 {v4.16b}, [x4], x5
    uabal v19.8h, v4.8b, v5.8b
    uabal2 v23.8h, v4.16b, v5.16b


    ld1 {v0.16b}, [x0], x7
    ld1 {v1.16b}, [x1], x5
    uabal v16.8h, v1.8b, v0.8b
    uabal2 v20.8h, v1.16b, v0.16b
    ld1 {v2.16b}, [x2], x5
    ld1 {v5.16b}, [x0], x7
    uabal v17.8h, v2.8b, v0.8b
    uabal2 v21.8h, v2.16b, v0.16b
    ld1 {v3.16b}, [x3], x5
    ld1 {v1.16b}, [x1], x5
    uabal v18.8h, v3.8b, v0.8b
    uabal2 v22.8h, v3.16b, v0.16b
    uabal v16.8h, v1.8b, v5.8b
    uabal2 v20.8h, v1.16b, v5.16b
    ld1 {v2.16b}, [x2], x5
    ld1 {v3.16b}, [x3], x5
    uabal v17.8h, v2.8b, v5.8b
    uabal2 v21.8h, v2.16b, v5.16b
    uabal v18.8h, v3.8b, v5.8b
    uabal2 v22.8h, v3.16b, v5.16b
    ld1 {v4.16b}, [x4], x5
    uabal v19.8h, v4.8b, v0.8b
    uabal2 v23.8h, v4.16b, v0.16b
    ld1 {v4.16b}, [x4], x5
    uabal v19.8h, v4.8b, v5.8b
    uabal2 v23.8h, v4.16b, v5.16b

    ld1 {v0.16b}, [x0], x7
    ld1 {v1.16b}, [x1], x5
    uabal v16.8h, v1.8b, v0.8b
    uabal2 v20.8h, v1.16b, v0.16b
    ld1 {v2.16b}, [x2], x5
    ld1 {v5.16b}, [x0], x7
    uabal v17.8h, v2.8b, v0.8b
    uabal2 v21.8h, v2.16b, v0.16b
    ld1 {v3.16b}, [x3], x5
    ld1 {v1.16b}, [x1], x5
    uabal v18.8h, v3.8b, v0.8b
    uabal2 v22.8h, v3.16b, v0.16b
    uabal v16.8h, v1.8b, v5.8b
    uabal2 v20.8h, v1.16b, v5.16b
    ld1 {v2.16b}, [x2], x5
    ld1 {v3.16b}, [x3], x5
    uabal v17.8h, v2.8b, v5.8b
    uabal2 v21.8h, v2.16b, v5.16b
    uabal v18.8h, v3.8b, v5.8b
    uabal2 v22.8h, v3.16b, v5.16b
    ld1 {v4.16b}, [x4], x5
    uabal v19.8h, v4.8b, v0.8b
    uabal2 v23.8h, v4.16b, v0.16b
    ld1 {v4.16b}, [x4], x5
    uabal v19.8h, v4.8b, v5.8b
    uabal2 v23.8h, v4.16b, v5.16b

    ld1 {v0.16b}, [x0], x7
    ld1 {v1.16b}, [x1], x5
    uabal v16.8h, v1.8b, v0.8b
    uabal2 v20.8h, v1.16b, v0.16b
    ld1 {v2.16b}, [x2], x5
    ld1 {v5.16b}, [x0], x7
    uabal v17.8h, v2.8b, v0.8b
    uabal2 v21.8h, v2.16b, v0.16b
    ld1 {v3.16b}, [x3], x5
    ld1 {v1.16b}, [x1], x5
    uabal v18.8h, v3.8b, v0.8b
    uabal2 v22.8h, v3.16b, v0.16b
    uabal v16.8h, v1.8b, v5.8b
    uabal2 v20.8h, v1.16b, v5.16b
    ld1 {v2.16b}, [x2], x5
    ld1 {v3.16b}, [x3], x5
    uabal v17.8h, v2.8b, v5.8b
    uabal2 v21.8h, v2.16b, v5.16b
    uabal v18.8h, v3.8b, v5.8b
    uabal2 v22.8h, v3.16b, v5.16b
    ld1 {v4.16b}, [x4], x5
    uabal v19.8h, v4.8b, v0.8b
    uabal2 v23.8h, v4.16b, v0.16b
    ld1 {v4.16b}, [x4], x5
    uabal v19.8h, v4.8b, v5.8b
    uabal2 v23.8h, v4.16b, v5.16b

    add v16.8h, v16.8h, v20.8h
    add v17.8h, v17.8h, v21.8h
    add v18.8h, v18.8h, v22.8h
    add v19.8h, v19.8h, v23.8h

    uaddlv s0, v16.8h
    uaddlv s1, v17.8h
    uaddlv s2, v18.8h

    stp s0, s1, [x6], #8
    uaddlv s3, v19.8h
    stp s2, s3, [x6]
    ret
 # //.size x264_pixel_sad_x4_16x8_neon, . - x264_pixel_sad_x4_16x8_neon
        //.endfunc
        .text
        .align 2
        .globl _x264_pixel_sad_x4_16x16_neon
 # //.type _x264_pixel_sad_x4_16x16_neon, %function
        //.func _x264_pixel_sad_x4_16x16_neon
_x264_pixel_sad_x4_16x16_neon:
    mov x7, #16

    ld1 {v0.16b}, [x0], x7
    ld1 {v1.16b}, [x1], x5
    uabdl v16.8h, v1.8b, v0.8b
    uabdl2 v20.8h, v1.16b, v0.16b
    ld1 {v2.16b}, [x2], x5
    ld1 {v5.16b}, [x0], x7
    uabdl v17.8h, v2.8b, v0.8b
    uabdl2 v21.8h, v2.16b, v0.16b
    ld1 {v3.16b}, [x3], x5
    ld1 {v1.16b}, [x1], x5
    uabdl v18.8h, v3.8b, v0.8b
    uabdl2 v22.8h, v3.16b, v0.16b
    uabal v16.8h, v1.8b, v5.8b
    uabal2 v20.8h, v1.16b, v5.16b
    ld1 {v2.16b}, [x2], x5
    ld1 {v3.16b}, [x3], x5
    uabal v17.8h, v2.8b, v5.8b
    uabal2 v21.8h, v2.16b, v5.16b
    uabal v18.8h, v3.8b, v5.8b
    uabal2 v22.8h, v3.16b, v5.16b
    ld1 {v4.16b}, [x4], x5
    uabdl v19.8h, v4.8b, v0.8b
    uabdl2 v23.8h, v4.16b, v0.16b
    ld1 {v4.16b}, [x4], x5
    uabal v19.8h, v4.8b, v5.8b
    uabal2 v23.8h, v4.16b, v5.16b


    ld1 {v0.16b}, [x0], x7
    ld1 {v1.16b}, [x1], x5
    uabal v16.8h, v1.8b, v0.8b
    uabal2 v20.8h, v1.16b, v0.16b
    ld1 {v2.16b}, [x2], x5
    ld1 {v5.16b}, [x0], x7
    uabal v17.8h, v2.8b, v0.8b
    uabal2 v21.8h, v2.16b, v0.16b
    ld1 {v3.16b}, [x3], x5
    ld1 {v1.16b}, [x1], x5
    uabal v18.8h, v3.8b, v0.8b
    uabal2 v22.8h, v3.16b, v0.16b
    uabal v16.8h, v1.8b, v5.8b
    uabal2 v20.8h, v1.16b, v5.16b
    ld1 {v2.16b}, [x2], x5
    ld1 {v3.16b}, [x3], x5
    uabal v17.8h, v2.8b, v5.8b
    uabal2 v21.8h, v2.16b, v5.16b
    uabal v18.8h, v3.8b, v5.8b
    uabal2 v22.8h, v3.16b, v5.16b
    ld1 {v4.16b}, [x4], x5
    uabal v19.8h, v4.8b, v0.8b
    uabal2 v23.8h, v4.16b, v0.16b
    ld1 {v4.16b}, [x4], x5
    uabal v19.8h, v4.8b, v5.8b
    uabal2 v23.8h, v4.16b, v5.16b

    ld1 {v0.16b}, [x0], x7
    ld1 {v1.16b}, [x1], x5
    uabal v16.8h, v1.8b, v0.8b
    uabal2 v20.8h, v1.16b, v0.16b
    ld1 {v2.16b}, [x2], x5
    ld1 {v5.16b}, [x0], x7
    uabal v17.8h, v2.8b, v0.8b
    uabal2 v21.8h, v2.16b, v0.16b
    ld1 {v3.16b}, [x3], x5
    ld1 {v1.16b}, [x1], x5
    uabal v18.8h, v3.8b, v0.8b
    uabal2 v22.8h, v3.16b, v0.16b
    uabal v16.8h, v1.8b, v5.8b
    uabal2 v20.8h, v1.16b, v5.16b
    ld1 {v2.16b}, [x2], x5
    ld1 {v3.16b}, [x3], x5
    uabal v17.8h, v2.8b, v5.8b
    uabal2 v21.8h, v2.16b, v5.16b
    uabal v18.8h, v3.8b, v5.8b
    uabal2 v22.8h, v3.16b, v5.16b
    ld1 {v4.16b}, [x4], x5
    uabal v19.8h, v4.8b, v0.8b
    uabal2 v23.8h, v4.16b, v0.16b
    ld1 {v4.16b}, [x4], x5
    uabal v19.8h, v4.8b, v5.8b
    uabal2 v23.8h, v4.16b, v5.16b

    ld1 {v0.16b}, [x0], x7
    ld1 {v1.16b}, [x1], x5
    uabal v16.8h, v1.8b, v0.8b
    uabal2 v20.8h, v1.16b, v0.16b
    ld1 {v2.16b}, [x2], x5
    ld1 {v5.16b}, [x0], x7
    uabal v17.8h, v2.8b, v0.8b
    uabal2 v21.8h, v2.16b, v0.16b
    ld1 {v3.16b}, [x3], x5
    ld1 {v1.16b}, [x1], x5
    uabal v18.8h, v3.8b, v0.8b
    uabal2 v22.8h, v3.16b, v0.16b
    uabal v16.8h, v1.8b, v5.8b
    uabal2 v20.8h, v1.16b, v5.16b
    ld1 {v2.16b}, [x2], x5
    ld1 {v3.16b}, [x3], x5
    uabal v17.8h, v2.8b, v5.8b
    uabal2 v21.8h, v2.16b, v5.16b
    uabal v18.8h, v3.8b, v5.8b
    uabal2 v22.8h, v3.16b, v5.16b
    ld1 {v4.16b}, [x4], x5
    uabal v19.8h, v4.8b, v0.8b
    uabal2 v23.8h, v4.16b, v0.16b
    ld1 {v4.16b}, [x4], x5
    uabal v19.8h, v4.8b, v5.8b
    uabal2 v23.8h, v4.16b, v5.16b

    ld1 {v0.16b}, [x0], x7
    ld1 {v1.16b}, [x1], x5
    uabal v16.8h, v1.8b, v0.8b
    uabal2 v20.8h, v1.16b, v0.16b
    ld1 {v2.16b}, [x2], x5
    ld1 {v5.16b}, [x0], x7
    uabal v17.8h, v2.8b, v0.8b
    uabal2 v21.8h, v2.16b, v0.16b
    ld1 {v3.16b}, [x3], x5
    ld1 {v1.16b}, [x1], x5
    uabal v18.8h, v3.8b, v0.8b
    uabal2 v22.8h, v3.16b, v0.16b
    uabal v16.8h, v1.8b, v5.8b
    uabal2 v20.8h, v1.16b, v5.16b
    ld1 {v2.16b}, [x2], x5
    ld1 {v3.16b}, [x3], x5
    uabal v17.8h, v2.8b, v5.8b
    uabal2 v21.8h, v2.16b, v5.16b
    uabal v18.8h, v3.8b, v5.8b
    uabal2 v22.8h, v3.16b, v5.16b
    ld1 {v4.16b}, [x4], x5
    uabal v19.8h, v4.8b, v0.8b
    uabal2 v23.8h, v4.16b, v0.16b
    ld1 {v4.16b}, [x4], x5
    uabal v19.8h, v4.8b, v5.8b
    uabal2 v23.8h, v4.16b, v5.16b

    ld1 {v0.16b}, [x0], x7
    ld1 {v1.16b}, [x1], x5
    uabal v16.8h, v1.8b, v0.8b
    uabal2 v20.8h, v1.16b, v0.16b
    ld1 {v2.16b}, [x2], x5
    ld1 {v5.16b}, [x0], x7
    uabal v17.8h, v2.8b, v0.8b
    uabal2 v21.8h, v2.16b, v0.16b
    ld1 {v3.16b}, [x3], x5
    ld1 {v1.16b}, [x1], x5
    uabal v18.8h, v3.8b, v0.8b
    uabal2 v22.8h, v3.16b, v0.16b
    uabal v16.8h, v1.8b, v5.8b
    uabal2 v20.8h, v1.16b, v5.16b
    ld1 {v2.16b}, [x2], x5
    ld1 {v3.16b}, [x3], x5
    uabal v17.8h, v2.8b, v5.8b
    uabal2 v21.8h, v2.16b, v5.16b
    uabal v18.8h, v3.8b, v5.8b
    uabal2 v22.8h, v3.16b, v5.16b
    ld1 {v4.16b}, [x4], x5
    uabal v19.8h, v4.8b, v0.8b
    uabal2 v23.8h, v4.16b, v0.16b
    ld1 {v4.16b}, [x4], x5
    uabal v19.8h, v4.8b, v5.8b
    uabal2 v23.8h, v4.16b, v5.16b

    ld1 {v0.16b}, [x0], x7
    ld1 {v1.16b}, [x1], x5
    uabal v16.8h, v1.8b, v0.8b
    uabal2 v20.8h, v1.16b, v0.16b
    ld1 {v2.16b}, [x2], x5
    ld1 {v5.16b}, [x0], x7
    uabal v17.8h, v2.8b, v0.8b
    uabal2 v21.8h, v2.16b, v0.16b
    ld1 {v3.16b}, [x3], x5
    ld1 {v1.16b}, [x1], x5
    uabal v18.8h, v3.8b, v0.8b
    uabal2 v22.8h, v3.16b, v0.16b
    uabal v16.8h, v1.8b, v5.8b
    uabal2 v20.8h, v1.16b, v5.16b
    ld1 {v2.16b}, [x2], x5
    ld1 {v3.16b}, [x3], x5
    uabal v17.8h, v2.8b, v5.8b
    uabal2 v21.8h, v2.16b, v5.16b
    uabal v18.8h, v3.8b, v5.8b
    uabal2 v22.8h, v3.16b, v5.16b
    ld1 {v4.16b}, [x4], x5
    uabal v19.8h, v4.8b, v0.8b
    uabal2 v23.8h, v4.16b, v0.16b
    ld1 {v4.16b}, [x4], x5
    uabal v19.8h, v4.8b, v5.8b
    uabal2 v23.8h, v4.16b, v5.16b

    ld1 {v0.16b}, [x0], x7
    ld1 {v1.16b}, [x1], x5
    uabal v16.8h, v1.8b, v0.8b
    uabal2 v20.8h, v1.16b, v0.16b
    ld1 {v2.16b}, [x2], x5
    ld1 {v5.16b}, [x0], x7
    uabal v17.8h, v2.8b, v0.8b
    uabal2 v21.8h, v2.16b, v0.16b
    ld1 {v3.16b}, [x3], x5
    ld1 {v1.16b}, [x1], x5
    uabal v18.8h, v3.8b, v0.8b
    uabal2 v22.8h, v3.16b, v0.16b
    uabal v16.8h, v1.8b, v5.8b
    uabal2 v20.8h, v1.16b, v5.16b
    ld1 {v2.16b}, [x2], x5
    ld1 {v3.16b}, [x3], x5
    uabal v17.8h, v2.8b, v5.8b
    uabal2 v21.8h, v2.16b, v5.16b
    uabal v18.8h, v3.8b, v5.8b
    uabal2 v22.8h, v3.16b, v5.16b
    ld1 {v4.16b}, [x4], x5
    uabal v19.8h, v4.8b, v0.8b
    uabal2 v23.8h, v4.16b, v0.16b
    ld1 {v4.16b}, [x4], x5
    uabal v19.8h, v4.8b, v5.8b
    uabal2 v23.8h, v4.16b, v5.16b

    add v16.8h, v16.8h, v20.8h
    add v17.8h, v17.8h, v21.8h
    add v18.8h, v18.8h, v22.8h
    add v19.8h, v19.8h, v23.8h

    uaddlv s0, v16.8h
    uaddlv s1, v17.8h
    uaddlv s2, v18.8h

    stp s0, s1, [x6], #8
    uaddlv s3, v19.8h
    stp s2, s3, [x6]
    ret
 # //.size x264_pixel_sad_x4_16x16_neon, . - x264_pixel_sad_x4_16x16_neon
        //.endfunc












        .text
        .align 2
        .globl _x264_pixel_ssd_4x4_neon
 # //.type _x264_pixel_ssd_4x4_neon, %function
        //.func _x264_pixel_ssd_4x4_neon
_x264_pixel_ssd_4x4_neon:
    ld1 {v16.s}[0], [x0], x1
    ld1 {v17.s}[0], [x2], x3
    usubl v2.8h, v16.8b, v17.8b
    ld1 {v16.s}[0], [x0], x1
    ld1 {v17.s}[0], [x2], x3
    smull v0.4s, v2.4h, v2.4h

    usubl v2.8h, v16.8b, v17.8b
    ld1 {v16.s}[0], [x0], x1
    ld1 {v17.s}[0], [x2], x3
    smlal v0.4s, v2.4h, v2.4h

    usubl v2.8h, v16.8b, v17.8b
    ld1 {v16.s}[0], [x0], x1
    ld1 {v17.s}[0], [x2], x3
    smlal v0.4s, v2.4h, v2.4h
    usubl v2.8h, v16.8b, v17.8b
    smlal v0.4s, v2.4h, v2.4h

    addv s0, v0.4s
    mov w0, v0.s[0]
    ret
 # //.size x264_pixel_ssd_4x4_neon, . - x264_pixel_ssd_4x4_neon
        //.endfunc
        .text
        .align 2
        .globl _x264_pixel_ssd_4x8_neon
 # //.type _x264_pixel_ssd_4x8_neon, %function
        //.func _x264_pixel_ssd_4x8_neon
_x264_pixel_ssd_4x8_neon:
    ld1 {v16.s}[0], [x0], x1
    ld1 {v17.s}[0], [x2], x3
    usubl v2.8h, v16.8b, v17.8b
    ld1 {v16.s}[0], [x0], x1
    ld1 {v17.s}[0], [x2], x3
    smull v0.4s, v2.4h, v2.4h

    usubl v2.8h, v16.8b, v17.8b
    ld1 {v16.s}[0], [x0], x1
    ld1 {v17.s}[0], [x2], x3
    smlal v0.4s, v2.4h, v2.4h

    usubl v2.8h, v16.8b, v17.8b
    ld1 {v16.s}[0], [x0], x1
    ld1 {v17.s}[0], [x2], x3
    smlal v0.4s, v2.4h, v2.4h

    usubl v2.8h, v16.8b, v17.8b
    ld1 {v16.s}[0], [x0], x1
    ld1 {v17.s}[0], [x2], x3
    smlal v0.4s, v2.4h, v2.4h

    usubl v2.8h, v16.8b, v17.8b
    ld1 {v16.s}[0], [x0], x1
    ld1 {v17.s}[0], [x2], x3
    smlal v0.4s, v2.4h, v2.4h

    usubl v2.8h, v16.8b, v17.8b
    ld1 {v16.s}[0], [x0], x1
    ld1 {v17.s}[0], [x2], x3
    smlal v0.4s, v2.4h, v2.4h

    usubl v2.8h, v16.8b, v17.8b
    ld1 {v16.s}[0], [x0], x1
    ld1 {v17.s}[0], [x2], x3
    smlal v0.4s, v2.4h, v2.4h
    usubl v2.8h, v16.8b, v17.8b
    smlal v0.4s, v2.4h, v2.4h

    addv s0, v0.4s
    mov w0, v0.s[0]
    ret
 # //.size x264_pixel_ssd_4x8_neon, . - x264_pixel_ssd_4x8_neon
        //.endfunc
        .text
        .align 2
        .globl _x264_pixel_ssd_8x4_neon
 # //.type _x264_pixel_ssd_8x4_neon, %function
        //.func _x264_pixel_ssd_8x4_neon
_x264_pixel_ssd_8x4_neon:
    ld1 {v16.8b}, [x0], x1
    ld1 {v17.8b}, [x2], x3
    usubl v2.8h, v16.8b, v17.8b
    ld1 {v16.8b}, [x0], x1
    smull v0.4s, v2.4h, v2.4h
    ld1 {v17.8b}, [x2], x3
    smlal2 v0.4s, v2.8h, v2.8h

    usubl v2.8h, v16.8b, v17.8b
    ld1 {v16.8b}, [x0], x1
    smlal v0.4s, v2.4h, v2.4h
    ld1 {v17.8b}, [x2], x3
    smlal2 v0.4s, v2.8h, v2.8h

    usubl v2.8h, v16.8b, v17.8b
    ld1 {v16.8b}, [x0], x1
    smlal v0.4s, v2.4h, v2.4h
    ld1 {v17.8b}, [x2], x3
    smlal2 v0.4s, v2.8h, v2.8h
    usubl v2.8h, v16.8b, v17.8b
    smlal v0.4s, v2.4h, v2.4h
    smlal2 v0.4s, v2.8h, v2.8h

    addv s0, v0.4s
    mov w0, v0.s[0]
    ret
 # //.size x264_pixel_ssd_8x4_neon, . - x264_pixel_ssd_8x4_neon
        //.endfunc
        .text
        .align 2
        .globl _x264_pixel_ssd_8x8_neon
 # //.type _x264_pixel_ssd_8x8_neon, %function
        //.func _x264_pixel_ssd_8x8_neon
_x264_pixel_ssd_8x8_neon:
    ld1 {v16.8b}, [x0], x1
    ld1 {v17.8b}, [x2], x3
    usubl v2.8h, v16.8b, v17.8b
    ld1 {v16.8b}, [x0], x1
    smull v0.4s, v2.4h, v2.4h
    ld1 {v17.8b}, [x2], x3
    smlal2 v0.4s, v2.8h, v2.8h

    usubl v2.8h, v16.8b, v17.8b
    ld1 {v16.8b}, [x0], x1
    smlal v0.4s, v2.4h, v2.4h
    ld1 {v17.8b}, [x2], x3
    smlal2 v0.4s, v2.8h, v2.8h

    usubl v2.8h, v16.8b, v17.8b
    ld1 {v16.8b}, [x0], x1
    smlal v0.4s, v2.4h, v2.4h
    ld1 {v17.8b}, [x2], x3
    smlal2 v0.4s, v2.8h, v2.8h

    usubl v2.8h, v16.8b, v17.8b
    ld1 {v16.8b}, [x0], x1
    smlal v0.4s, v2.4h, v2.4h
    ld1 {v17.8b}, [x2], x3
    smlal2 v0.4s, v2.8h, v2.8h

    usubl v2.8h, v16.8b, v17.8b
    ld1 {v16.8b}, [x0], x1
    smlal v0.4s, v2.4h, v2.4h
    ld1 {v17.8b}, [x2], x3
    smlal2 v0.4s, v2.8h, v2.8h

    usubl v2.8h, v16.8b, v17.8b
    ld1 {v16.8b}, [x0], x1
    smlal v0.4s, v2.4h, v2.4h
    ld1 {v17.8b}, [x2], x3
    smlal2 v0.4s, v2.8h, v2.8h

    usubl v2.8h, v16.8b, v17.8b
    ld1 {v16.8b}, [x0], x1
    smlal v0.4s, v2.4h, v2.4h
    ld1 {v17.8b}, [x2], x3
    smlal2 v0.4s, v2.8h, v2.8h
    usubl v2.8h, v16.8b, v17.8b
    smlal v0.4s, v2.4h, v2.4h
    smlal2 v0.4s, v2.8h, v2.8h

    addv s0, v0.4s
    mov w0, v0.s[0]
    ret
 # //.size x264_pixel_ssd_8x8_neon, . - x264_pixel_ssd_8x8_neon
        //.endfunc
        .text
        .align 2
        .globl _x264_pixel_ssd_8x16_neon
 # //.type _x264_pixel_ssd_8x16_neon, %function
        //.func _x264_pixel_ssd_8x16_neon
_x264_pixel_ssd_8x16_neon:
    ld1 {v16.8b}, [x0], x1
    ld1 {v17.8b}, [x2], x3
    usubl v2.8h, v16.8b, v17.8b
    ld1 {v16.8b}, [x0], x1
    smull v0.4s, v2.4h, v2.4h
    ld1 {v17.8b}, [x2], x3
    smlal2 v0.4s, v2.8h, v2.8h

    usubl v2.8h, v16.8b, v17.8b
    ld1 {v16.8b}, [x0], x1
    smlal v0.4s, v2.4h, v2.4h
    ld1 {v17.8b}, [x2], x3
    smlal2 v0.4s, v2.8h, v2.8h

    usubl v2.8h, v16.8b, v17.8b
    ld1 {v16.8b}, [x0], x1
    smlal v0.4s, v2.4h, v2.4h
    ld1 {v17.8b}, [x2], x3
    smlal2 v0.4s, v2.8h, v2.8h

    usubl v2.8h, v16.8b, v17.8b
    ld1 {v16.8b}, [x0], x1
    smlal v0.4s, v2.4h, v2.4h
    ld1 {v17.8b}, [x2], x3
    smlal2 v0.4s, v2.8h, v2.8h

    usubl v2.8h, v16.8b, v17.8b
    ld1 {v16.8b}, [x0], x1
    smlal v0.4s, v2.4h, v2.4h
    ld1 {v17.8b}, [x2], x3
    smlal2 v0.4s, v2.8h, v2.8h

    usubl v2.8h, v16.8b, v17.8b
    ld1 {v16.8b}, [x0], x1
    smlal v0.4s, v2.4h, v2.4h
    ld1 {v17.8b}, [x2], x3
    smlal2 v0.4s, v2.8h, v2.8h

    usubl v2.8h, v16.8b, v17.8b
    ld1 {v16.8b}, [x0], x1
    smlal v0.4s, v2.4h, v2.4h
    ld1 {v17.8b}, [x2], x3
    smlal2 v0.4s, v2.8h, v2.8h

    usubl v2.8h, v16.8b, v17.8b
    ld1 {v16.8b}, [x0], x1
    smlal v0.4s, v2.4h, v2.4h
    ld1 {v17.8b}, [x2], x3
    smlal2 v0.4s, v2.8h, v2.8h

    usubl v2.8h, v16.8b, v17.8b
    ld1 {v16.8b}, [x0], x1
    smlal v0.4s, v2.4h, v2.4h
    ld1 {v17.8b}, [x2], x3
    smlal2 v0.4s, v2.8h, v2.8h

    usubl v2.8h, v16.8b, v17.8b
    ld1 {v16.8b}, [x0], x1
    smlal v0.4s, v2.4h, v2.4h
    ld1 {v17.8b}, [x2], x3
    smlal2 v0.4s, v2.8h, v2.8h

    usubl v2.8h, v16.8b, v17.8b
    ld1 {v16.8b}, [x0], x1
    smlal v0.4s, v2.4h, v2.4h
    ld1 {v17.8b}, [x2], x3
    smlal2 v0.4s, v2.8h, v2.8h

    usubl v2.8h, v16.8b, v17.8b
    ld1 {v16.8b}, [x0], x1
    smlal v0.4s, v2.4h, v2.4h
    ld1 {v17.8b}, [x2], x3
    smlal2 v0.4s, v2.8h, v2.8h

    usubl v2.8h, v16.8b, v17.8b
    ld1 {v16.8b}, [x0], x1
    smlal v0.4s, v2.4h, v2.4h
    ld1 {v17.8b}, [x2], x3
    smlal2 v0.4s, v2.8h, v2.8h

    usubl v2.8h, v16.8b, v17.8b
    ld1 {v16.8b}, [x0], x1
    smlal v0.4s, v2.4h, v2.4h
    ld1 {v17.8b}, [x2], x3
    smlal2 v0.4s, v2.8h, v2.8h

    usubl v2.8h, v16.8b, v17.8b
    ld1 {v16.8b}, [x0], x1
    smlal v0.4s, v2.4h, v2.4h
    ld1 {v17.8b}, [x2], x3
    smlal2 v0.4s, v2.8h, v2.8h
    usubl v2.8h, v16.8b, v17.8b
    smlal v0.4s, v2.4h, v2.4h
    smlal2 v0.4s, v2.8h, v2.8h

    addv s0, v0.4s
    mov w0, v0.s[0]
    ret
 # //.size x264_pixel_ssd_8x16_neon, . - x264_pixel_ssd_8x16_neon
        //.endfunc
        .text
        .align 2
        .globl _x264_pixel_ssd_16x8_neon
 # //.type _x264_pixel_ssd_16x8_neon, %function
        //.func _x264_pixel_ssd_16x8_neon
_x264_pixel_ssd_16x8_neon:
    ld1 {v16.16b}, [x0], x1
    ld1 {v17.16b}, [x2], x3
    usubl v2.8h, v16.8b, v17.8b
    usubl2 v3.8h, v16.16b, v17.16b
    ld1 {v16.16b}, [x0], x1
    smull v0.4s, v2.4h, v2.4h
    smull2 v1.4s, v2.8h, v2.8h
    ld1 {v17.16b}, [x2], x3
    smlal v0.4s, v3.4h, v3.4h
    smlal2 v1.4s, v3.8h, v3.8h

    usubl v2.8h, v16.8b, v17.8b
    usubl2 v3.8h, v16.16b, v17.16b
    ld1 {v16.16b}, [x0], x1
    smlal v0.4s, v2.4h, v2.4h
    smlal2 v1.4s, v2.8h, v2.8h
    ld1 {v17.16b}, [x2], x3
    smlal v0.4s, v3.4h, v3.4h
    smlal2 v1.4s, v3.8h, v3.8h

    usubl v2.8h, v16.8b, v17.8b
    usubl2 v3.8h, v16.16b, v17.16b
    ld1 {v16.16b}, [x0], x1
    smlal v0.4s, v2.4h, v2.4h
    smlal2 v1.4s, v2.8h, v2.8h
    ld1 {v17.16b}, [x2], x3
    smlal v0.4s, v3.4h, v3.4h
    smlal2 v1.4s, v3.8h, v3.8h

    usubl v2.8h, v16.8b, v17.8b
    usubl2 v3.8h, v16.16b, v17.16b
    ld1 {v16.16b}, [x0], x1
    smlal v0.4s, v2.4h, v2.4h
    smlal2 v1.4s, v2.8h, v2.8h
    ld1 {v17.16b}, [x2], x3
    smlal v0.4s, v3.4h, v3.4h
    smlal2 v1.4s, v3.8h, v3.8h

    usubl v2.8h, v16.8b, v17.8b
    usubl2 v3.8h, v16.16b, v17.16b
    ld1 {v16.16b}, [x0], x1
    smlal v0.4s, v2.4h, v2.4h
    smlal2 v1.4s, v2.8h, v2.8h
    ld1 {v17.16b}, [x2], x3
    smlal v0.4s, v3.4h, v3.4h
    smlal2 v1.4s, v3.8h, v3.8h

    usubl v2.8h, v16.8b, v17.8b
    usubl2 v3.8h, v16.16b, v17.16b
    ld1 {v16.16b}, [x0], x1
    smlal v0.4s, v2.4h, v2.4h
    smlal2 v1.4s, v2.8h, v2.8h
    ld1 {v17.16b}, [x2], x3
    smlal v0.4s, v3.4h, v3.4h
    smlal2 v1.4s, v3.8h, v3.8h

    usubl v2.8h, v16.8b, v17.8b
    usubl2 v3.8h, v16.16b, v17.16b
    ld1 {v16.16b}, [x0], x1
    smlal v0.4s, v2.4h, v2.4h
    smlal2 v1.4s, v2.8h, v2.8h
    ld1 {v17.16b}, [x2], x3
    smlal v0.4s, v3.4h, v3.4h
    smlal2 v1.4s, v3.8h, v3.8h
    usubl v2.8h, v16.8b, v17.8b
    usubl2 v3.8h, v16.16b, v17.16b
    smlal v0.4s, v2.4h, v2.4h
    smlal2 v1.4s, v2.8h, v2.8h
    smlal v0.4s, v3.4h, v3.4h
    smlal2 v1.4s, v3.8h, v3.8h
    add v0.4s, v0.4s, v1.4s

    addv s0, v0.4s
    mov w0, v0.s[0]
    ret
 # //.size x264_pixel_ssd_16x8_neon, . - x264_pixel_ssd_16x8_neon
        //.endfunc
        .text
        .align 2
        .globl _x264_pixel_ssd_16x16_neon
 # //.type _x264_pixel_ssd_16x16_neon, %function
        //.func _x264_pixel_ssd_16x16_neon
_x264_pixel_ssd_16x16_neon:
    ld1 {v16.16b}, [x0], x1
    ld1 {v17.16b}, [x2], x3
    usubl v2.8h, v16.8b, v17.8b
    usubl2 v3.8h, v16.16b, v17.16b
    ld1 {v16.16b}, [x0], x1
    smull v0.4s, v2.4h, v2.4h
    smull2 v1.4s, v2.8h, v2.8h
    ld1 {v17.16b}, [x2], x3
    smlal v0.4s, v3.4h, v3.4h
    smlal2 v1.4s, v3.8h, v3.8h

    usubl v2.8h, v16.8b, v17.8b
    usubl2 v3.8h, v16.16b, v17.16b
    ld1 {v16.16b}, [x0], x1
    smlal v0.4s, v2.4h, v2.4h
    smlal2 v1.4s, v2.8h, v2.8h
    ld1 {v17.16b}, [x2], x3
    smlal v0.4s, v3.4h, v3.4h
    smlal2 v1.4s, v3.8h, v3.8h

    usubl v2.8h, v16.8b, v17.8b
    usubl2 v3.8h, v16.16b, v17.16b
    ld1 {v16.16b}, [x0], x1
    smlal v0.4s, v2.4h, v2.4h
    smlal2 v1.4s, v2.8h, v2.8h
    ld1 {v17.16b}, [x2], x3
    smlal v0.4s, v3.4h, v3.4h
    smlal2 v1.4s, v3.8h, v3.8h

    usubl v2.8h, v16.8b, v17.8b
    usubl2 v3.8h, v16.16b, v17.16b
    ld1 {v16.16b}, [x0], x1
    smlal v0.4s, v2.4h, v2.4h
    smlal2 v1.4s, v2.8h, v2.8h
    ld1 {v17.16b}, [x2], x3
    smlal v0.4s, v3.4h, v3.4h
    smlal2 v1.4s, v3.8h, v3.8h

    usubl v2.8h, v16.8b, v17.8b
    usubl2 v3.8h, v16.16b, v17.16b
    ld1 {v16.16b}, [x0], x1
    smlal v0.4s, v2.4h, v2.4h
    smlal2 v1.4s, v2.8h, v2.8h
    ld1 {v17.16b}, [x2], x3
    smlal v0.4s, v3.4h, v3.4h
    smlal2 v1.4s, v3.8h, v3.8h

    usubl v2.8h, v16.8b, v17.8b
    usubl2 v3.8h, v16.16b, v17.16b
    ld1 {v16.16b}, [x0], x1
    smlal v0.4s, v2.4h, v2.4h
    smlal2 v1.4s, v2.8h, v2.8h
    ld1 {v17.16b}, [x2], x3
    smlal v0.4s, v3.4h, v3.4h
    smlal2 v1.4s, v3.8h, v3.8h

    usubl v2.8h, v16.8b, v17.8b
    usubl2 v3.8h, v16.16b, v17.16b
    ld1 {v16.16b}, [x0], x1
    smlal v0.4s, v2.4h, v2.4h
    smlal2 v1.4s, v2.8h, v2.8h
    ld1 {v17.16b}, [x2], x3
    smlal v0.4s, v3.4h, v3.4h
    smlal2 v1.4s, v3.8h, v3.8h

    usubl v2.8h, v16.8b, v17.8b
    usubl2 v3.8h, v16.16b, v17.16b
    ld1 {v16.16b}, [x0], x1
    smlal v0.4s, v2.4h, v2.4h
    smlal2 v1.4s, v2.8h, v2.8h
    ld1 {v17.16b}, [x2], x3
    smlal v0.4s, v3.4h, v3.4h
    smlal2 v1.4s, v3.8h, v3.8h

    usubl v2.8h, v16.8b, v17.8b
    usubl2 v3.8h, v16.16b, v17.16b
    ld1 {v16.16b}, [x0], x1
    smlal v0.4s, v2.4h, v2.4h
    smlal2 v1.4s, v2.8h, v2.8h
    ld1 {v17.16b}, [x2], x3
    smlal v0.4s, v3.4h, v3.4h
    smlal2 v1.4s, v3.8h, v3.8h

    usubl v2.8h, v16.8b, v17.8b
    usubl2 v3.8h, v16.16b, v17.16b
    ld1 {v16.16b}, [x0], x1
    smlal v0.4s, v2.4h, v2.4h
    smlal2 v1.4s, v2.8h, v2.8h
    ld1 {v17.16b}, [x2], x3
    smlal v0.4s, v3.4h, v3.4h
    smlal2 v1.4s, v3.8h, v3.8h

    usubl v2.8h, v16.8b, v17.8b
    usubl2 v3.8h, v16.16b, v17.16b
    ld1 {v16.16b}, [x0], x1
    smlal v0.4s, v2.4h, v2.4h
    smlal2 v1.4s, v2.8h, v2.8h
    ld1 {v17.16b}, [x2], x3
    smlal v0.4s, v3.4h, v3.4h
    smlal2 v1.4s, v3.8h, v3.8h

    usubl v2.8h, v16.8b, v17.8b
    usubl2 v3.8h, v16.16b, v17.16b
    ld1 {v16.16b}, [x0], x1
    smlal v0.4s, v2.4h, v2.4h
    smlal2 v1.4s, v2.8h, v2.8h
    ld1 {v17.16b}, [x2], x3
    smlal v0.4s, v3.4h, v3.4h
    smlal2 v1.4s, v3.8h, v3.8h

    usubl v2.8h, v16.8b, v17.8b
    usubl2 v3.8h, v16.16b, v17.16b
    ld1 {v16.16b}, [x0], x1
    smlal v0.4s, v2.4h, v2.4h
    smlal2 v1.4s, v2.8h, v2.8h
    ld1 {v17.16b}, [x2], x3
    smlal v0.4s, v3.4h, v3.4h
    smlal2 v1.4s, v3.8h, v3.8h

    usubl v2.8h, v16.8b, v17.8b
    usubl2 v3.8h, v16.16b, v17.16b
    ld1 {v16.16b}, [x0], x1
    smlal v0.4s, v2.4h, v2.4h
    smlal2 v1.4s, v2.8h, v2.8h
    ld1 {v17.16b}, [x2], x3
    smlal v0.4s, v3.4h, v3.4h
    smlal2 v1.4s, v3.8h, v3.8h

    usubl v2.8h, v16.8b, v17.8b
    usubl2 v3.8h, v16.16b, v17.16b
    ld1 {v16.16b}, [x0], x1
    smlal v0.4s, v2.4h, v2.4h
    smlal2 v1.4s, v2.8h, v2.8h
    ld1 {v17.16b}, [x2], x3
    smlal v0.4s, v3.4h, v3.4h
    smlal2 v1.4s, v3.8h, v3.8h
    usubl v2.8h, v16.8b, v17.8b
    usubl2 v3.8h, v16.16b, v17.16b
    smlal v0.4s, v2.4h, v2.4h
    smlal2 v1.4s, v2.8h, v2.8h
    smlal v0.4s, v3.4h, v3.4h
    smlal2 v1.4s, v3.8h, v3.8h
    add v0.4s, v0.4s, v1.4s

    addv s0, v0.4s
    mov w0, v0.s[0]
    ret
 # //.size x264_pixel_ssd_16x16_neon, . - x264_pixel_ssd_16x16_neon
        //.endfunc


        .text
        .align 2
        .globl _x264_pixel_var_8x8_neon
 # //.type _x264_pixel_var_8x8_neon, %function
        //.func _x264_pixel_var_8x8_neon
_x264_pixel_var_8x8_neon:
    ld1 {v16.8b}, [x0], x1
    ld1 {v17.8b}, [x0], x1
    mov x2, 8 - 4
    umull v1.8h, v16.8b, v16.8b
        ushll v0.8h, v16.8b, #0
    umull v2.8h, v17.8b, v17.8b
    uaddw v0.8h, v0.8h, v17.8b
    ld1 {v18.8b}, [x0], x1
    uaddlp v1.4s, v1.8h
    uaddlp v2.4s, v2.8h
    ld1 {v19.8b}, [x0], x1

1: subs x2, x2, #4
    uaddw v0.8h, v0.8h, v18.8b
    umull v24.8h, v18.8b, v18.8b
    ld1 {v20.8b}, [x0], x1
    uaddw v0.8h, v0.8h, v19.8b
    umull v25.8h, v19.8b, v19.8b
    uadalp v1.4s, v24.8h
    ld1 {v21.8b}, [x0], x1
    uaddw v0.8h, v0.8h, v20.8b
    umull v26.8h, v20.8b, v20.8b
    uadalp v2.4s, v25.8h
    ld1 {v18.8b}, [x0], x1
    uaddw v0.8h, v0.8h, v21.8b
    umull v27.8h, v21.8b, v21.8b
    uadalp v1.4s, v26.8h
    ld1 {v19.8b}, [x0], x1
    uadalp v2.4s, v27.8h
    b.gt 1b

    uaddw v0.8h, v0.8h, v18.8b
    umull v28.8h, v18.8b, v18.8b
    uaddw v0.8h, v0.8h, v19.8b
    umull v29.8h, v19.8b, v19.8b
    uadalp v1.4s, v28.8h
    uadalp v2.4s, v29.8h

    b x264_var_end
 # //.size x264_pixel_var_8x8_neon, . - x264_pixel_var_8x8_neon
        //.endfunc
        .text
        .align 2
        .globl _x264_pixel_var_8x16_neon
 # //.type _x264_pixel_var_8x16_neon, %function
        //.func _x264_pixel_var_8x16_neon
_x264_pixel_var_8x16_neon:
    ld1 {v16.8b}, [x0], x1
    ld1 {v17.8b}, [x0], x1
    mov x2, 16 - 4
    umull v1.8h, v16.8b, v16.8b
        ushll v0.8h, v16.8b, #0
    umull v2.8h, v17.8b, v17.8b
    uaddw v0.8h, v0.8h, v17.8b
    ld1 {v18.8b}, [x0], x1
    uaddlp v1.4s, v1.8h
    uaddlp v2.4s, v2.8h
    ld1 {v19.8b}, [x0], x1

1: subs x2, x2, #4
    uaddw v0.8h, v0.8h, v18.8b
    umull v24.8h, v18.8b, v18.8b
    ld1 {v20.8b}, [x0], x1
    uaddw v0.8h, v0.8h, v19.8b
    umull v25.8h, v19.8b, v19.8b
    uadalp v1.4s, v24.8h
    ld1 {v21.8b}, [x0], x1
    uaddw v0.8h, v0.8h, v20.8b
    umull v26.8h, v20.8b, v20.8b
    uadalp v2.4s, v25.8h
    ld1 {v18.8b}, [x0], x1
    uaddw v0.8h, v0.8h, v21.8b
    umull v27.8h, v21.8b, v21.8b
    uadalp v1.4s, v26.8h
    ld1 {v19.8b}, [x0], x1
    uadalp v2.4s, v27.8h
    b.gt 1b

    uaddw v0.8h, v0.8h, v18.8b
    umull v28.8h, v18.8b, v18.8b
    uaddw v0.8h, v0.8h, v19.8b
    umull v29.8h, v19.8b, v19.8b
    uadalp v1.4s, v28.8h
    uadalp v2.4s, v29.8h

    b x264_var_end
 # //.size x264_pixel_var_8x16_neon, . - x264_pixel_var_8x16_neon
        //.endfunc

        .text
        .align 2
        .globl _x264_pixel_var_16x16_neon
 # //.type _x264_pixel_var_16x16_neon, %function
        //.func _x264_pixel_var_16x16_neon
_x264_pixel_var_16x16_neon:
    ld1 {v16.16b}, [x0], x1
    ld1 {v17.16b}, [x0], x1
    mov x2, #14
    umull v1.8h, v16.8b, v16.8b
    umull2 v2.8h, v16.16b, v16.16b
        ushll v0.8h, v16.8b, #0
    uaddlp v1.4s, v1.8h
    uaddlp v2.4s, v2.8h
    uaddw2 v0.8h, v0.8h, v16.16b

1: subs x2, x2, #2
    ld1 {v18.16b}, [x0], x1
    uaddw v0.8h, v0.8h, v17.8b
    umull v3.8h, v17.8b, v17.8b
    uaddw2 v0.8h, v0.8h, v17.16b
    umull2 v4.8h, v17.16b, v17.16b
    uadalp v1.4s, v3.8h
    uadalp v2.4s, v4.8h

    ld1 {v17.16b}, [x0], x1
    uaddw v0.8h, v0.8h, v18.8b
    umull v5.8h, v18.8b, v18.8b
    uaddw2 v0.8h, v0.8h, v18.16b
    umull2 v6.8h, v18.16b, v18.16b
    uadalp v1.4s, v5.8h
    uadalp v2.4s, v6.8h
    b.gt 1b

    uaddw v0.8h, v0.8h, v17.8b
    umull v3.8h, v17.8b, v17.8b
    uaddw2 v0.8h, v0.8h, v17.16b
    umull2 v4.8h, v17.16b, v17.16b
    uadalp v1.4s, v3.8h
    uadalp v2.4s, v4.8h
 # //.size x264_pixel_var_16x16_neon, . - x264_pixel_var_16x16_neon
        //.endfunc

        .text
        .align 2
 # //.type x264_var_end, %function
        //.func x264_var_end
x264_var_end:
    add v1.4s, v1.4s, v2.4s
    uaddlv s0, v0.8h
    uaddlv d1, v1.4s
    mov w0, v0.s[0]
    mov x1, v1.d[0]
    orr x0, x0, x1, lsl #32
    ret
 # //.size x264_var_end, . - x264_var_end
        //.endfunc



        .text
        .align 2
        .globl _x264_pixel_var2_8x8_neon
 # //.type _x264_pixel_var2_8x8_neon, %function
        //.func _x264_pixel_var2_8x8_neon
_x264_pixel_var2_8x8_neon:
    ld1 {v16.8b}, [x0], x1
    ld1 {v18.8b}, [x2], x3
    ld1 {v17.8b}, [x0], x1
    ld1 {v19.8b}, [x2], x3
    mov x5, 8 - 4
    usubl v6.8h, v16.8b, v18.8b
    usubl v7.8h, v17.8b, v19.8b
    ld1 {v16.8b}, [x0], x1
    ld1 {v18.8b}, [x2], x3
    smull v2.4s, v6.4h, v6.4h
    smull2 v3.4s, v6.8h, v6.8h
    add v0.8h, v6.8h, v7.8h
    smlal v2.4s, v7.4h, v7.4h
    smlal2 v3.4s, v7.8h, v7.8h

    usubl v6.8h, v16.8b, v18.8b

1: subs x5, x5, #2
    ld1 {v17.8b}, [x0], x1
    ld1 {v19.8b}, [x2], x3
    smlal v2.4s, v6.4h, v6.4h
    smlal2 v3.4s, v6.8h, v6.8h
    usubl v7.8h, v17.8b, v19.8b
    add v0.8h, v0.8h, v6.8h
    ld1 {v16.8b}, [x0], x1
    ld1 {v18.8b}, [x2], x3
    smlal v2.4s, v7.4h, v7.4h
    smlal2 v3.4s, v7.8h, v7.8h
    usubl v6.8h, v16.8b, v18.8b
    add v0.8h, v0.8h, v7.8h
    b.gt 1b

    ld1 {v17.8b}, [x0], x1
    ld1 {v19.8b}, [x2], x3
    smlal v2.4s, v6.4h, v6.4h
    smlal2 v3.4s, v6.8h, v6.8h
    usubl v7.8h, v17.8b, v19.8b
    add v0.8h, v0.8h, v6.8h
    smlal v2.4s, v7.4h, v7.4h
    add v0.8h, v0.8h, v7.8h
    smlal2 v3.4s, v7.8h, v7.8h

    saddlv s0, v0.8h
    add v2.4s, v2.4s, v3.4s
    mov w0, v0.s[0]
    addv s1, v2.4s
    sxtw x0, w0
    mov w1, v1.s[0]
    mul x0, x0, x0
    str w1, [x4]
    sub x0, x1, x0, lsr # 6 + (8 >> 4)

    ret
 # //.size x264_pixel_var2_8x8_neon, . - x264_pixel_var2_8x8_neon
        //.endfunc
        .text
        .align 2
        .globl _x264_pixel_var2_8x16_neon
 # //.type _x264_pixel_var2_8x16_neon, %function
        //.func _x264_pixel_var2_8x16_neon
_x264_pixel_var2_8x16_neon:
    ld1 {v16.8b}, [x0], x1
    ld1 {v18.8b}, [x2], x3
    ld1 {v17.8b}, [x0], x1
    ld1 {v19.8b}, [x2], x3
    mov x5, 16 - 4
    usubl v6.8h, v16.8b, v18.8b
    usubl v7.8h, v17.8b, v19.8b
    ld1 {v16.8b}, [x0], x1
    ld1 {v18.8b}, [x2], x3
    smull v2.4s, v6.4h, v6.4h
    smull2 v3.4s, v6.8h, v6.8h
    add v0.8h, v6.8h, v7.8h
    smlal v2.4s, v7.4h, v7.4h
    smlal2 v3.4s, v7.8h, v7.8h

    usubl v6.8h, v16.8b, v18.8b

1: subs x5, x5, #2
    ld1 {v17.8b}, [x0], x1
    ld1 {v19.8b}, [x2], x3
    smlal v2.4s, v6.4h, v6.4h
    smlal2 v3.4s, v6.8h, v6.8h
    usubl v7.8h, v17.8b, v19.8b
    add v0.8h, v0.8h, v6.8h
    ld1 {v16.8b}, [x0], x1
    ld1 {v18.8b}, [x2], x3
    smlal v2.4s, v7.4h, v7.4h
    smlal2 v3.4s, v7.8h, v7.8h
    usubl v6.8h, v16.8b, v18.8b
    add v0.8h, v0.8h, v7.8h
    b.gt 1b

    ld1 {v17.8b}, [x0], x1
    ld1 {v19.8b}, [x2], x3
    smlal v2.4s, v6.4h, v6.4h
    smlal2 v3.4s, v6.8h, v6.8h
    usubl v7.8h, v17.8b, v19.8b
    add v0.8h, v0.8h, v6.8h
    smlal v2.4s, v7.4h, v7.4h
    add v0.8h, v0.8h, v7.8h
    smlal2 v3.4s, v7.8h, v7.8h

    saddlv s0, v0.8h
    add v2.4s, v2.4s, v3.4s
    mov w0, v0.s[0]
    addv s1, v2.4s
    sxtw x0, w0
    mov w1, v1.s[0]
    mul x0, x0, x0
    str w1, [x4]
    sub x0, x1, x0, lsr # 6 + (16 >> 4)

    ret
 # //.size x264_pixel_var2_8x16_neon, . - x264_pixel_var2_8x16_neon
        //.endfunc


        .text
        .align 2
        .globl _x264_pixel_satd_4x4_neon
 # //.type _x264_pixel_satd_4x4_neon, %function
        //.func _x264_pixel_satd_4x4_neon
_x264_pixel_satd_4x4_neon:
    ld1 {v1.s}[0], [x2], x3
    ld1 {v0.s}[0], [x0], x1
    ld1 {v3.s}[0], [x2], x3
    ld1 {v2.s}[0], [x0], x1
    ld1 {v1.s}[1], [x2], x3
    ld1 {v0.s}[1], [x0], x1
    ld1 {v3.s}[1], [x2], x3
    ld1 {v2.s}[1], [x0], x1

    usubl v0.8h, v0.8b, v1.8b
    usubl v1.8h, v2.8b, v3.8b
    add v2.8h, v0.8h, v1.8h
    sub v3.8h, v0.8h, v1.8h

    zip1 v0.2d, v2.2d, v3.2d
    zip2 v1.2d, v2.2d, v3.2d
    add v2.8h, v0.8h, v1.8h
    sub v3.8h, v0.8h, v1.8h

    trn1 v0.8h, v2.8h, v3.8h
    trn2 v1.8h, v2.8h, v3.8h
    add v2.8h, v0.8h, v1.8h
    sub v3.8h, v0.8h, v1.8h

    trn1 v0.4s, v2.4s, v3.4s
    trn2 v1.4s, v2.4s, v3.4s
    abs v0.8h, v0.8h
    abs v1.8h, v1.8h
    umax v0.8h, v0.8h, v1.8h

    uaddlv s0, v0.8h
    mov w0, v0.s[0]
    ret
 # //.size x264_pixel_satd_4x4_neon, . - x264_pixel_satd_4x4_neon
        //.endfunc

        .text
        .align 2
        .globl _x264_pixel_satd_4x8_neon
 # //.type _x264_pixel_satd_4x8_neon, %function
        //.func _x264_pixel_satd_4x8_neon
_x264_pixel_satd_4x8_neon:
    ld1 {v1.s}[0], [x2], x3
    ld1 {v0.s}[0], [x0], x1
    ld1 {v3.s}[0], [x2], x3
    ld1 {v2.s}[0], [x0], x1
    ld1 {v5.s}[0], [x2], x3
    ld1 {v4.s}[0], [x0], x1
    ld1 {v7.s}[0], [x2], x3
    ld1 {v6.s}[0], [x0], x1
    ld1 {v1.s}[1], [x2], x3
    ld1 {v0.s}[1], [x0], x1
    ld1 {v3.s}[1], [x2], x3
    ld1 {v2.s}[1], [x0], x1
    ld1 {v5.s}[1], [x2], x3
    ld1 {v4.s}[1], [x0], x1
    ld1 {v7.s}[1], [x2], x3
    ld1 {v6.s}[1], [x0], x1
    b x264_satd_4x8_8x4_end_neon
 # //.size x264_pixel_satd_4x8_neon, . - x264_pixel_satd_4x8_neon
        //.endfunc

        .text
        .align 2
        .globl _x264_pixel_satd_8x4_neon
 # //.type _x264_pixel_satd_8x4_neon, %function
        //.func _x264_pixel_satd_8x4_neon
_x264_pixel_satd_8x4_neon:
    ld1 {v1.8b}, [x2], x3
    ld1 {v0.8b}, [x0], x1
    ld1 {v3.8b}, [x2], x3
    ld1 {v2.8b}, [x0], x1
    ld1 {v5.8b}, [x2], x3
    ld1 {v4.8b}, [x0], x1
    ld1 {v7.8b}, [x2], x3
    ld1 {v6.8b}, [x0], x1
 # //.size x264_pixel_satd_8x4_neon, . - x264_pixel_satd_8x4_neon
        //.endfunc

        .text
        .align 2
 # //.type x264_satd_4x8_8x4_end_neon, %function
        //.func x264_satd_4x8_8x4_end_neon
x264_satd_4x8_8x4_end_neon:
    usubl v0.8h, v0.8b, v1.8b
    usubl v1.8h, v2.8b, v3.8b
    usubl v2.8h, v4.8b, v5.8b
    usubl v3.8h, v6.8b, v7.8b

    add v16.8h, v0.8h, v1.8h
    sub v17.8h, v0.8h, v1.8h
    add v18.8h, v2.8h, v3.8h
    sub v19.8h, v2.8h, v3.8h

    add v4.8h, v16.8h, v18.8h
    sub v6.8h, v16.8h, v18.8h
    add v5.8h, v17.8h, v19.8h
    sub v7.8h, v17.8h, v19.8h

    trn1 v0.8h, v4.8h, v5.8h
    trn2 v1.8h, v4.8h, v5.8h
    trn1 v2.8h, v6.8h, v7.8h
    trn2 v3.8h, v6.8h, v7.8h

    add v16.8h, v0.8h, v1.8h
    sub v17.8h, v0.8h, v1.8h
    add v18.8h, v2.8h, v3.8h
    sub v19.8h, v2.8h, v3.8h

    trn1 v0.4s, v16.4s, v18.4s
    trn2 v1.4s, v16.4s, v18.4s
    trn1 v2.4s, v17.4s, v19.4s
    trn2 v3.4s, v17.4s, v19.4s
    abs v0.8h, v0.8h
    abs v1.8h, v1.8h
    abs v2.8h, v2.8h
    abs v3.8h, v3.8h
    umax v0.8h, v0.8h, v1.8h
    umax v1.8h, v2.8h, v3.8h
    add v0.8h, v0.8h, v1.8h
    uaddlv s0, v0.8h
    mov w0, v0.s[0]
    ret
 # //.size x264_satd_4x8_8x4_end_neon, . - x264_satd_4x8_8x4_end_neon
        //.endfunc

        .text
        .align 2
        .globl _x264_pixel_satd_8x8_neon
 # //.type _x264_pixel_satd_8x8_neon, %function
        //.func _x264_pixel_satd_8x8_neon
_x264_pixel_satd_8x8_neon:
    mov x4, x30

    bl x264_satd_8x8_neon
    add v0.8h, v0.8h, v1.8h
    add v1.8h, v2.8h, v3.8h
    add v0.8h, v0.8h, v1.8h
    uaddlv s0, v0.8h
    mov w0, v0.s[0]
    ret x4
 # //.size x264_pixel_satd_8x8_neon, . - x264_pixel_satd_8x8_neon
        //.endfunc

        .text
        .align 2
        .globl _x264_pixel_satd_8x16_neon
 # //.type _x264_pixel_satd_8x16_neon, %function
        //.func _x264_pixel_satd_8x16_neon
_x264_pixel_satd_8x16_neon:
    mov x4, x30

    bl x264_satd_8x8_neon
    add v0.8h, v0.8h, v1.8h
    add v1.8h, v2.8h, v3.8h
    add v30.8h, v0.8h, v1.8h

    bl x264_satd_8x8_neon
    add v0.8h, v0.8h, v1.8h
    add v1.8h, v2.8h, v3.8h
    add v31.8h, v0.8h, v1.8h
    add v0.8h, v30.8h, v31.8h
    uaddlv s0, v0.8h
    mov w0, v0.s[0]
    ret x4
 # //.size x264_pixel_satd_8x16_neon, . - x264_pixel_satd_8x16_neon
        //.endfunc





        .text
        .align 2
 # //.type x264_satd_8x8_neon, %function
        //.func x264_satd_8x8_neon
x264_satd_8x8_neon:
    ld1 {v1.8b}, [x2], x3
    ld1 {v0.8b}, [x0], x1
    ld1 {v3.8b}, [x2], x3
    ld1 {v2.8b}, [x0], x1
    usubl v16.8h, v0.8b, v1.8b
    ld1 {v5.8b}, [x2], x3
    ld1 {v4.8b}, [x0], x1
    usubl v17.8h, v2.8b, v3.8b
    ld1 {v7.8b}, [x2], x3
    ld1 {v6.8b}, [x0], x1
    usubl v18.8h, v4.8b, v5.8b
    ld1 {v1.8b}, [x2], x3
    ld1 {v0.8b}, [x0], x1
    usubl v19.8h, v6.8b, v7.8b
    ld1 {v3.8b}, [x2], x3
    ld1 {v2.8b}, [x0], x1
    usubl v20.8h, v0.8b, v1.8b
    ld1 {v5.8b}, [x2], x3
    ld1 {v4.8b}, [x0], x1
    usubl v21.8h, v2.8b, v3.8b
    ld1 {v7.8b}, [x2], x3
    ld1 {v6.8b}, [x0], x1

    add v0.8h, v16.8h, v17.8h
    sub v1.8h, v16.8h, v17.8h
    add v2.8h, v18.8h, v19.8h
    sub v3.8h, v18.8h, v19.8h

    usubl v22.8h, v4.8b, v5.8b
    usubl v23.8h, v6.8b, v7.8b
 # //.size x264_satd_8x8_neon, . - x264_satd_8x8_neon
        //.endfunc


        .text
        .align 2
 # //.type x264_satd_8x4v_8x8h_neon, %function
        //.func x264_satd_8x4v_8x8h_neon
x264_satd_8x4v_8x8h_neon:
    add v16.8h, v0.8h, v2.8h
    sub v18.8h, v0.8h, v2.8h
    add v17.8h, v1.8h, v3.8h
    sub v19.8h, v1.8h, v3.8h

    add v0.8h, v20.8h, v21.8h
    sub v1.8h, v20.8h, v21.8h
    add v2.8h, v22.8h, v23.8h
    sub v3.8h, v22.8h, v23.8h
    add v20.8h, v0.8h, v2.8h
    sub v22.8h, v0.8h, v2.8h
    add v21.8h, v1.8h, v3.8h
    sub v23.8h, v1.8h, v3.8h

    trn1 v0.8h, v16.8h, v17.8h
    trn2 v1.8h, v16.8h, v17.8h
    trn1 v2.8h, v18.8h, v19.8h
    trn2 v3.8h, v18.8h, v19.8h
    trn1 v4.8h, v20.8h, v21.8h
    trn2 v5.8h, v20.8h, v21.8h
    trn1 v6.8h, v22.8h, v23.8h
    trn2 v7.8h, v22.8h, v23.8h

    add v16.8h, v0.8h, v1.8h
    sub v17.8h, v0.8h, v1.8h
    add v18.8h, v2.8h, v3.8h
    sub v19.8h, v2.8h, v3.8h
    add v20.8h, v4.8h, v5.8h
    sub v21.8h, v4.8h, v5.8h
    add v22.8h, v6.8h, v7.8h
    sub v23.8h, v6.8h, v7.8h

    trn1 v0.4s, v16.4s, v18.4s
    trn2 v2.4s, v16.4s, v18.4s
    trn1 v1.4s, v17.4s, v19.4s
    trn2 v3.4s, v17.4s, v19.4s
    trn1 v4.4s, v20.4s, v22.4s
    trn2 v6.4s, v20.4s, v22.4s
    trn1 v5.4s, v21.4s, v23.4s
    trn2 v7.4s, v21.4s, v23.4s

    abs v0.8h, v0.8h
    abs v1.8h, v1.8h
    abs v2.8h, v2.8h
    abs v3.8h, v3.8h
    abs v4.8h, v4.8h
    abs v5.8h, v5.8h
    abs v6.8h, v6.8h
    abs v7.8h, v7.8h

    umax v0.8h, v0.8h, v2.8h
    umax v1.8h, v1.8h, v3.8h
    umax v2.8h, v4.8h, v6.8h
    umax v3.8h, v5.8h, v7.8h

    ret
 # //.size x264_satd_8x4v_8x8h_neon, . - x264_satd_8x4v_8x8h_neon
        //.endfunc

        .text
        .align 2
        .globl _x264_pixel_satd_16x8_neon
 # //.type _x264_pixel_satd_16x8_neon, %function
        //.func _x264_pixel_satd_16x8_neon
_x264_pixel_satd_16x8_neon:
    mov x4, x30

    bl x264_satd_16x4_neon
    add v30.8h, v0.8h, v1.8h
    add v31.8h, v2.8h, v3.8h

    bl x264_satd_16x4_neon
    add v0.8h, v0.8h, v1.8h
    add v1.8h, v2.8h, v3.8h
    add v30.8h, v30.8h, v0.8h
    add v31.8h, v31.8h, v1.8h

    add v0.8h, v30.8h, v31.8h
    uaddlv s0, v0.8h
    mov w0, v0.s[0]
    ret x4
 # //.size x264_pixel_satd_16x8_neon, . - x264_pixel_satd_16x8_neon
        //.endfunc

        .text
        .align 2
        .globl _x264_pixel_satd_16x16_neon
 # //.type _x264_pixel_satd_16x16_neon, %function
        //.func _x264_pixel_satd_16x16_neon
_x264_pixel_satd_16x16_neon:
    mov x4, x30

    bl x264_satd_16x4_neon
    add v30.8h, v0.8h, v1.8h
    add v31.8h, v2.8h, v3.8h

    bl x264_satd_16x4_neon
    add v0.8h, v0.8h, v1.8h
    add v1.8h, v2.8h, v3.8h
    add v30.8h, v30.8h, v0.8h
    add v31.8h, v31.8h, v1.8h

    bl x264_satd_16x4_neon
    add v0.8h, v0.8h, v1.8h
    add v1.8h, v2.8h, v3.8h
    add v30.8h, v30.8h, v0.8h
    add v31.8h, v31.8h, v1.8h

    bl x264_satd_16x4_neon
    add v0.8h, v0.8h, v1.8h
    add v1.8h, v2.8h, v3.8h
    add v30.8h, v30.8h, v0.8h
    add v31.8h, v31.8h, v1.8h

    add v0.8h, v30.8h, v31.8h
    uaddlv s0, v0.8h
    mov w0, v0.s[0]
    ret x4
 # //.size x264_pixel_satd_16x16_neon, . - x264_pixel_satd_16x16_neon
        //.endfunc

        .text
        .align 2
 # //.type x264_satd_16x4_neon, %function
        //.func x264_satd_16x4_neon
x264_satd_16x4_neon:
    ld1 {v1.16b}, [x2], x3
    ld1 {v0.16b}, [x0], x1
    ld1 {v3.16b}, [x2], x3
    ld1 {v2.16b}, [x0], x1
    usubl v16.8h, v0.8b, v1.8b
    usubl2 v20.8h, v0.16b, v1.16b
    ld1 {v5.16b}, [x2], x3
    ld1 {v4.16b}, [x0], x1
    usubl v17.8h, v2.8b, v3.8b
    usubl2 v21.8h, v2.16b, v3.16b
    ld1 {v7.16b}, [x2], x3
    ld1 {v6.16b}, [x0], x1

    usubl v18.8h, v4.8b, v5.8b
    usubl2 v22.8h, v4.16b, v5.16b
    usubl v19.8h, v6.8b, v7.8b
    usubl2 v23.8h, v6.16b, v7.16b

    add v0.8h, v16.8h, v17.8h
    sub v1.8h, v16.8h, v17.8h
    add v2.8h, v18.8h, v19.8h
    sub v3.8h, v18.8h, v19.8h

    b x264_satd_8x4v_8x8h_neon
 # //.size x264_satd_16x4_neon, . - x264_satd_16x4_neon
        //.endfunc


        .text
        .align 2
        .globl _x264_pixel_sa8d_8x8_neon
 # //.type _x264_pixel_sa8d_8x8_neon, %function
        //.func _x264_pixel_sa8d_8x8_neon
_x264_pixel_sa8d_8x8_neon:
    mov x4, x30
    bl x264_sa8d_8x8_neon
    add v0.8h, v0.8h, v1.8h
    uaddlv s0, v0.8h
    mov w0, v0.s[0]
    add w0, w0, #1
    lsr w0, w0, #1
    ret x4
 # //.size x264_pixel_sa8d_8x8_neon, . - x264_pixel_sa8d_8x8_neon
        //.endfunc

        .text
        .align 2
        .globl _x264_pixel_sa8d_16x16_neon
 # //.type _x264_pixel_sa8d_16x16_neon, %function
        //.func _x264_pixel_sa8d_16x16_neon
_x264_pixel_sa8d_16x16_neon:
    mov x4, x30
    bl x264_sa8d_8x8_neon
    uaddlp v30.4s, v0.8h
    uaddlp v31.4s, v1.8h
    bl x264_sa8d_8x8_neon
    uadalp v30.4s, v0.8h
    uadalp v31.4s, v1.8h
    sub x0, x0, x1, lsl #4
    sub x2, x2, x3, lsl #4
    add x0, x0, #8
    add x2, x2, #8
    bl x264_sa8d_8x8_neon
    uadalp v30.4s, v0.8h
    uadalp v31.4s, v1.8h
    bl x264_sa8d_8x8_neon
    uadalp v30.4s, v0.8h
    uadalp v31.4s, v1.8h
    add v0.4s, v30.4s, v31.4s
    addv s0, v0.4s
    mov w0, v0.s[0]
    add w0, w0, #1
    lsr w0, w0, #1
    ret x4
 # //.size x264_pixel_sa8d_16x16_neon, . - x264_pixel_sa8d_16x16_neon
        //.endfunc

        .text
        .align 2
 # //.type x264_sa8d_8x8_neon, %function
        //.func x264_sa8d_8x8_neon
x264_sa8d_8x8_neon:
    ld1 {v1.8b}, [x2], x3
    ld1 {v0.8b}, [x0], x1
    ld1 {v3.8b}, [x2], x3
    ld1 {v2.8b}, [x0], x1
    usubl v16.8h, v0.8b, v1.8b
    ld1 {v5.8b}, [x2], x3
    ld1 {v4.8b}, [x0], x1
    usubl v17.8h, v2.8b, v3.8b
    ld1 {v7.8b}, [x2], x3
    ld1 {v6.8b}, [x0], x1
    usubl v18.8h, v4.8b, v5.8b
    ld1 {v1.8b}, [x2], x3
    ld1 {v0.8b}, [x0], x1
    usubl v19.8h, v6.8b, v7.8b
    ld1 {v3.8b}, [x2], x3
    ld1 {v2.8b}, [x0], x1
    usubl v20.8h, v0.8b, v1.8b
    ld1 {v5.8b}, [x2], x3
    ld1 {v4.8b}, [x0], x1
    usubl v21.8h, v2.8b, v3.8b
    ld1 {v7.8b}, [x2], x3
    ld1 {v6.8b}, [x0], x1

    add v0.8h, v16.8h, v17.8h
    sub v1.8h, v16.8h, v17.8h
    add v2.8h, v18.8h, v19.8h
    sub v3.8h, v18.8h, v19.8h

    usubl v22.8h, v4.8b, v5.8b
    usubl v23.8h, v6.8b, v7.8b

    add v16.8h, v0.8h, v2.8h
    sub v18.8h, v0.8h, v2.8h
    add v17.8h, v1.8h, v3.8h
    sub v19.8h, v1.8h, v3.8h

    add v0.8h, v20.8h, v21.8h
    sub v1.8h, v20.8h, v21.8h
    add v2.8h, v22.8h, v23.8h
    sub v3.8h, v22.8h, v23.8h
    add v20.8h, v0.8h, v2.8h
    sub v22.8h, v0.8h, v2.8h
    add v21.8h, v1.8h, v3.8h
    sub v23.8h, v1.8h, v3.8h
    add v0.8h, v16.8h, v20.8h
    sub v16.8h, v16.8h, v20.8h
    add v1.8h, v17.8h, v21.8h
    sub v17.8h, v17.8h, v21.8h
    add v2.8h, v18.8h, v22.8h
    sub v18.8h, v18.8h, v22.8h
    add v3.8h, v19.8h, v23.8h
    sub v19.8h, v19.8h, v23.8h

    trn1 v20.8h, v16.8h, v17.8h
    trn2 v21.8h, v16.8h, v17.8h
    trn1 v4.8h, v0.8h, v1.8h
    trn2 v5.8h, v0.8h, v1.8h
    trn1 v22.8h, v18.8h, v19.8h
    trn2 v23.8h, v18.8h, v19.8h
    trn1 v6.8h, v2.8h, v3.8h
    trn2 v7.8h, v2.8h, v3.8h

    add v28.8h, v20.8h, v21.8h
    sub v29.8h, v20.8h, v21.8h
    add v24.8h, v4.8h, v5.8h
    sub v25.8h, v4.8h, v5.8h
    add v0.8h, v22.8h, v23.8h
    sub v1.8h, v22.8h, v23.8h
    add v26.8h, v6.8h, v7.8h
    sub v27.8h, v6.8h, v7.8h

    trn1 v20.4s, v28.4s, v0.4s
    trn2 v22.4s, v28.4s, v0.4s
    trn1 v21.4s, v29.4s, v1.4s
    trn2 v23.4s, v29.4s, v1.4s
    trn1 v16.4s, v24.4s, v26.4s
    trn2 v18.4s, v24.4s, v26.4s
    trn1 v17.4s, v25.4s, v27.4s
    trn2 v19.4s, v25.4s, v27.4s

    add v0.8h, v20.8h, v22.8h
    sub v2.8h, v20.8h, v22.8h
    add v1.8h, v21.8h, v23.8h
    sub v3.8h, v21.8h, v23.8h
    add v4.8h, v16.8h, v18.8h
    sub v6.8h, v16.8h, v18.8h
    add v5.8h, v17.8h, v19.8h
    sub v7.8h, v17.8h, v19.8h

    trn1 v16.2d, v0.2d, v4.2d
    trn2 v20.2d, v0.2d, v4.2d
    trn1 v17.2d, v1.2d, v5.2d
    trn2 v21.2d, v1.2d, v5.2d
    trn1 v18.2d, v2.2d, v6.2d
    trn2 v22.2d, v2.2d, v6.2d
    trn1 v19.2d, v3.2d, v7.2d
    trn2 v23.2d, v3.2d, v7.2d

    abs v16.8h, v16.8h
    abs v20.8h, v20.8h
    abs v17.8h, v17.8h
    abs v21.8h, v21.8h
    abs v18.8h, v18.8h
    abs v22.8h, v22.8h
    abs v19.8h, v19.8h
    abs v23.8h, v23.8h

    umax v16.8h, v16.8h, v20.8h
    umax v17.8h, v17.8h, v21.8h
    umax v18.8h, v18.8h, v22.8h
    umax v19.8h, v19.8h, v23.8h

    add v0.8h, v16.8h, v17.8h
    add v1.8h, v18.8h, v19.8h

    ret
 # //.size x264_sa8d_8x8_neon, . - x264_sa8d_8x8_neon
        //.endfunc



        .text
        .align 2
        .globl _x264_pixel_hadamard_ac_8x8_neon
 # //.type _x264_pixel_hadamard_ac_8x8_neon, %function
        //.func _x264_pixel_hadamard_ac_8x8_neon
_x264_pixel_hadamard_ac_8x8_neon:

        adrp x5, mask_ac_4_8@PAGE
        add x5, x5, mask_ac_4_8@PAGEOFF






    mov x4, x30
    ld1 {v30.8h,v31.8h}, [x5]
    movi v28.16b, #0
    movi v29.16b, #0

    bl x264_hadamard_ac_8x8_neon

    addv s1, v29.4s
    addv s0, v28.4s
    mov w1, v1.s[0]
    mov w0, v0.s[0]
    lsr w1, w1, #2
    lsr w0, w0, #1
    orr x0, x0, x1, lsl #32
    ret x4
 # //.size x264_pixel_hadamard_ac_8x8_neon, . - x264_pixel_hadamard_ac_8x8_neon
        //.endfunc
        .text
        .align 2
        .globl _x264_pixel_hadamard_ac_8x16_neon
 # //.type _x264_pixel_hadamard_ac_8x16_neon, %function
        //.func _x264_pixel_hadamard_ac_8x16_neon
_x264_pixel_hadamard_ac_8x16_neon:

        adrp x5, mask_ac_4_8@PAGE
        add x5, x5, mask_ac_4_8@PAGEOFF






    mov x4, x30
    ld1 {v30.8h,v31.8h}, [x5]
    movi v28.16b, #0
    movi v29.16b, #0

    bl x264_hadamard_ac_8x8_neon
    bl x264_hadamard_ac_8x8_neon

    addv s1, v29.4s
    addv s0, v28.4s
    mov w1, v1.s[0]
    mov w0, v0.s[0]
    lsr w1, w1, #2
    lsr w0, w0, #1
    orr x0, x0, x1, lsl #32
    ret x4
 # //.size x264_pixel_hadamard_ac_8x16_neon, . - x264_pixel_hadamard_ac_8x16_neon
        //.endfunc
        .text
        .align 2
        .globl _x264_pixel_hadamard_ac_16x8_neon
 # //.type _x264_pixel_hadamard_ac_16x8_neon, %function
        //.func _x264_pixel_hadamard_ac_16x8_neon
_x264_pixel_hadamard_ac_16x8_neon:

        adrp x5, mask_ac_4_8@PAGE
        add x5, x5, mask_ac_4_8@PAGEOFF






    mov x4, x30
    ld1 {v30.8h,v31.8h}, [x5]
    movi v28.16b, #0
    movi v29.16b, #0

    bl x264_hadamard_ac_8x8_neon
    sub x0, x0, x1, lsl #3
    add x0, x0, #8
    bl x264_hadamard_ac_8x8_neon

    addv s1, v29.4s
    addv s0, v28.4s
    mov w1, v1.s[0]
    mov w0, v0.s[0]
    lsr w1, w1, #2
    lsr w0, w0, #1
    orr x0, x0, x1, lsl #32
    ret x4
 # //.size x264_pixel_hadamard_ac_16x8_neon, . - x264_pixel_hadamard_ac_16x8_neon
        //.endfunc
        .text
        .align 2
        .globl _x264_pixel_hadamard_ac_16x16_neon
 # //.type _x264_pixel_hadamard_ac_16x16_neon, %function
        //.func _x264_pixel_hadamard_ac_16x16_neon
_x264_pixel_hadamard_ac_16x16_neon:

        adrp x5, mask_ac_4_8@PAGE
        add x5, x5, mask_ac_4_8@PAGEOFF






    mov x4, x30
    ld1 {v30.8h,v31.8h}, [x5]
    movi v28.16b, #0
    movi v29.16b, #0

    bl x264_hadamard_ac_8x8_neon
    bl x264_hadamard_ac_8x8_neon
    sub x0, x0, x1, lsl #3
    add x0, x0, #8
    bl x264_hadamard_ac_8x8_neon
    sub x0, x0, x1, lsl #4
    bl x264_hadamard_ac_8x8_neon

    addv s1, v29.4s
    addv s0, v28.4s
    mov w1, v1.s[0]
    mov w0, v0.s[0]
    lsr w1, w1, #2
    lsr w0, w0, #1
    orr x0, x0, x1, lsl #32
    ret x4
 # //.size x264_pixel_hadamard_ac_16x16_neon, . - x264_pixel_hadamard_ac_16x16_neon
        //.endfunc


        .text
        .align 2
 # //.type x264_hadamard_ac_8x8_neon, %function
        //.func x264_hadamard_ac_8x8_neon
x264_hadamard_ac_8x8_neon:
    ld1 {v16.8b}, [x0], x1
    ld1 {v17.8b}, [x0], x1
    ld1 {v18.8b}, [x0], x1
    ld1 {v19.8b}, [x0], x1
    uaddl v0.8h, v16.8b, v17.8b
    usubl v1.8h, v16.8b, v17.8b
    ld1 {v20.8b}, [x0], x1
    ld1 {v21.8b}, [x0], x1
    uaddl v2.8h, v18.8b, v19.8b
    usubl v3.8h, v18.8b, v19.8b
    ld1 {v22.8b}, [x0], x1
    ld1 {v23.8b}, [x0], x1
    uaddl v4.8h, v20.8b, v21.8b
    usubl v5.8h, v20.8b, v21.8b
    uaddl v6.8h, v22.8b, v23.8b
    usubl v7.8h, v22.8b, v23.8b

    add v16.8h, v0.8h, v2.8h
    sub v18.8h, v0.8h, v2.8h
    add v17.8h, v1.8h, v3.8h
    sub v19.8h, v1.8h, v3.8h
    add v20.8h, v4.8h, v6.8h
    sub v22.8h, v4.8h, v6.8h
    add v21.8h, v5.8h, v7.8h
    sub v23.8h, v5.8h, v7.8h

    trn1 v0.8h, v16.8h, v17.8h
    trn2 v1.8h, v16.8h, v17.8h
    trn1 v2.8h, v18.8h, v19.8h
    trn2 v3.8h, v18.8h, v19.8h
    trn1 v4.8h, v20.8h, v21.8h
    trn2 v5.8h, v20.8h, v21.8h
    trn1 v6.8h, v22.8h, v23.8h
    trn2 v7.8h, v22.8h, v23.8h

    add v16.8h, v0.8h, v1.8h
    sub v17.8h, v0.8h, v1.8h
    add v18.8h, v2.8h, v3.8h
    sub v19.8h, v2.8h, v3.8h
    add v20.8h, v4.8h, v5.8h
    sub v21.8h, v4.8h, v5.8h
    add v22.8h, v6.8h, v7.8h
    sub v23.8h, v6.8h, v7.8h

    trn1 v0.4s, v16.4s, v18.4s
    trn2 v2.4s, v16.4s, v18.4s
    trn1 v1.4s, v17.4s, v19.4s
    trn2 v3.4s, v17.4s, v19.4s
    trn1 v4.4s, v20.4s, v22.4s
    trn2 v6.4s, v20.4s, v22.4s
    trn1 v5.4s, v21.4s, v23.4s
    trn2 v7.4s, v21.4s, v23.4s

    add v16.8h, v0.8h, v2.8h
    sub v18.8h, v0.8h, v2.8h
    add v17.8h, v1.8h, v3.8h
    sub v19.8h, v1.8h, v3.8h
    add v20.8h, v4.8h, v6.8h
    sub v22.8h, v4.8h, v6.8h
    add v21.8h, v5.8h, v7.8h
    sub v23.8h, v5.8h, v7.8h

    abs v0.8h, v16.8h
    abs v4.8h, v20.8h
    abs v1.8h, v17.8h
    abs v5.8h, v21.8h
    abs v2.8h, v18.8h
    abs v6.8h, v22.8h
    abs v3.8h, v19.8h
    abs v7.8h, v23.8h

    add v0.8h, v0.8h, v4.8h
    add v1.8h, v1.8h, v5.8h
    and v0.16b, v0.16b, v30.16b
    add v2.8h, v2.8h, v6.8h
    add v3.8h, v3.8h, v7.8h
    add v0.8h, v0.8h, v2.8h
    add v1.8h, v1.8h, v3.8h
    uadalp v28.4s, v0.8h
    uadalp v28.4s, v1.8h

    add v6.8h, v23.8h, v19.8h
    sub v7.8h, v23.8h, v19.8h
    add v4.8h, v22.8h, v18.8h
    sub v5.8h, v22.8h, v18.8h
    add v2.8h, v21.8h, v17.8h
    sub v3.8h, v21.8h, v17.8h
    add v1.8h, v16.8h, v20.8h
    sub v0.8h, v16.8h, v20.8h

    trn1 v16.2d, v6.2d, v7.2d
    trn2 v17.2d, v6.2d, v7.2d
    trn1 v18.2d, v4.2d, v5.2d
    trn2 v19.2d, v4.2d, v5.2d
    trn1 v20.2d, v2.2d, v3.2d
    trn2 v21.2d, v2.2d, v3.2d

    abs v16.8h, v16.8h
    abs v17.8h, v17.8h
    abs v18.8h, v18.8h
    abs v19.8h, v19.8h
    abs v20.8h, v20.8h
    abs v21.8h, v21.8h

    trn1 v7.2d, v1.2d, v0.2d
    trn2 v6.2d, v1.2d, v0.2d

    umax v3.8h, v16.8h, v17.8h
    umax v2.8h, v18.8h, v19.8h
    umax v1.8h, v20.8h, v21.8h

    add v4.8h, v7.8h, v6.8h
    sub v5.8h, v7.8h, v6.8h

    add v2.8h, v2.8h, v3.8h
    add v2.8h, v2.8h, v1.8h
    and v4.16b, v4.16b, v31.16b
    add v2.8h, v2.8h, v2.8h
    abs v5.8h, v5.8h
    abs v4.8h, v4.8h
    add v2.8h, v2.8h, v5.8h
    add v2.8h, v2.8h, v4.8h
    uadalp v29.4s, v2.8h
    ret
 # //.size x264_hadamard_ac_8x8_neon, . - x264_hadamard_ac_8x8_neon
        //.endfunc


        .text
        .align 2
        .globl _x264_pixel_ssim_4x4x2_core_neon
 # //.type _x264_pixel_ssim_4x4x2_core_neon, %function
        //.func _x264_pixel_ssim_4x4x2_core_neon
_x264_pixel_ssim_4x4x2_core_neon:
    ld1 {v0.8b}, [x0], x1
    ld1 {v2.8b}, [x2], x3
    umull v16.8h, v0.8b, v0.8b
    umull v17.8h, v0.8b, v2.8b
    umull v18.8h, v2.8b, v2.8b

    ld1 {v28.8b}, [x0], x1
    ld1 {v29.8b}, [x2], x3
    umull v20.8h, v28.8b, v28.8b
    umull v21.8h, v28.8b, v29.8b
    umull v22.8h, v29.8b, v29.8b

    uaddlp v16.4s, v16.8h
    uaddlp v17.4s, v17.8h
    uaddl v0.8h, v0.8b, v28.8b
    uadalp v16.4s, v18.8h
    uaddl v1.8h, v2.8b, v29.8b

    ld1 {v26.8b}, [x0], x1
    ld1 {v27.8b}, [x2], x3
    umull v23.8h, v26.8b, v26.8b
    umull v24.8h, v26.8b, v27.8b
    umull v25.8h, v27.8b, v27.8b

    uadalp v16.4s, v20.8h
    uaddw v0.8h, v0.8h, v26.8b
    uadalp v17.4s, v21.8h
    uaddw v1.8h, v1.8h, v27.8b
    uadalp v16.4s, v22.8h

    ld1 {v28.8b}, [x0], x1
    ld1 {v29.8b}, [x2], x3
    umull v20.8h, v28.8b, v28.8b
    umull v21.8h, v28.8b, v29.8b
    umull v22.8h, v29.8b, v29.8b

    uadalp v16.4s, v23.8h
    uaddw v0.8h, v0.8h, v28.8b
    uadalp v17.4s, v24.8h
    uaddw v1.8h, v1.8h, v29.8b
    uadalp v16.4s, v25.8h

    uadalp v16.4s, v20.8h
    uadalp v17.4s, v21.8h
    uadalp v16.4s, v22.8h

    uaddlp v0.4s, v0.8h
    uaddlp v1.4s, v1.8h

    addp v0.4s, v0.4s, v0.4s
    addp v1.4s, v1.4s, v1.4s
    addp v2.4s, v16.4s, v16.4s
    addp v3.4s, v17.4s, v17.4s

    st4 {v0.2s,v1.2s,v2.2s,v3.2s}, [x4]
    ret
 # //.size x264_pixel_ssim_4x4x2_core_neon, . - x264_pixel_ssim_4x4x2_core_neon
        //.endfunc

        .text
        .align 2
        .globl _x264_pixel_ssim_end4_neon
 # //.type _x264_pixel_ssim_end4_neon, %function
        //.func _x264_pixel_ssim_end4_neon
_x264_pixel_ssim_end4_neon:
    mov x5, #4
    ld1 {v16.4s,v17.4s}, [x0], #32
    ld1 {v18.4s,v19.4s}, [x1], #32
    mov w4, #0x99bb
    subs x2, x5, w2, uxtw
    mov w3, #416
    movk w4, #0x03, lsl #16
    add v0.4s, v16.4s, v18.4s
    add v1.4s, v17.4s, v19.4s
    add v0.4s, v0.4s, v1.4s
    ld1 {v20.4s,v21.4s}, [x0], #32
    ld1 {v22.4s,v23.4s}, [x1], #32
    add v2.4s, v20.4s, v22.4s
    add v3.4s, v21.4s, v23.4s
    add v1.4s, v1.4s, v2.4s
    ld1 {v16.4s}, [x0], #16
    ld1 {v18.4s}, [x1], #16
    add v16.4s, v16.4s, v18.4s
    add v2.4s, v2.4s, v3.4s
    add v3.4s, v3.4s, v16.4s

    dup v30.4s, w3
    dup v31.4s, w4

    trn1 v4.4s, v0.4s, v1.4s
    trn2 v5.4s, v0.4s, v1.4s
    trn1 v6.4s, v2.4s, v3.4s
    trn2 v7.4s, v2.4s, v3.4s
    trn1 v0.2d, v4.2d, v6.2d
    trn2 v2.2d, v4.2d, v6.2d
    trn1 v1.2d, v5.2d, v7.2d
    trn2 v3.2d, v5.2d, v7.2d

    mul v16.4s, v0.4s, v1.4s
    mul v0.4s, v0.4s, v0.4s
    mla v0.4s, v1.4s, v1.4s

    shl v3.4s, v3.4s, #7
    shl v2.4s, v2.4s, #6
    add v1.4s, v16.4s, v16.4s

    sub v2.4s, v2.4s, v0.4s
    sub v3.4s, v3.4s, v1.4s
    add v0.4s, v0.4s, v30.4s
    add v2.4s, v2.4s, v31.4s
    add v1.4s, v1.4s, v30.4s
    add v3.4s, v3.4s, v31.4s

    scvtf v0.4s, v0.4s
    scvtf v2.4s, v2.4s
    scvtf v1.4s, v1.4s
    scvtf v3.4s, v3.4s

    fmul v0.4s, v0.4s, v2.4s
    fmul v1.4s, v1.4s, v3.4s

    fdiv v0.4s, v1.4s, v0.4s

    b.eq 1f

        adrp x3, mask@PAGE
        add x3, x3, mask@PAGEOFF


    add x3, x3, x2, lsl #2
    ld1 {v29.4s}, [x3]
    and v0.16b, v0.16b, v29.16b
1:
    faddp v0.4s, v0.4s, v0.4s
    faddp s0, v0.2s
    ret
 # //.size x264_pixel_ssim_end4_neon, . - x264_pixel_ssim_end4_neon
        //.endfunc


#endif