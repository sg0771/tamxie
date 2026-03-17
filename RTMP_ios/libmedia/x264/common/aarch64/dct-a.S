#ifdef __arm64__

.const_data
        .const_data
        .align 4
scan4x4_frame:
.byte 0,1, 8,9, 2,3, 4,5
.byte 10,11, 16,17, 24,25, 18,19
.byte 12,13, 6,7, 14,15, 20,21
.byte 26,27, 28,29, 22,23, 30,31
 # //.size scan4x4_frame, . - scan4x4_frame


        .text
        .align 2
        .globl _x264_dct4x4dc_neon
 # //.type _x264_dct4x4dc_neon, %function
        //.func _x264_dct4x4dc_neon
_x264_dct4x4dc_neon:
    ld1 {v0.4h,v1.4h,v2.4h,v3.4h}, [x0]
        movi v31.4h, #1, lsl #0
    add v4.4h, v0.4h, v1.4h
    sub v5.4h, v0.4h, v1.4h
    add v6.4h, v2.4h, v3.4h
    sub v7.4h, v2.4h, v3.4h
    add v0.4h, v4.4h, v6.4h
    sub v2.4h, v4.4h, v6.4h
    add v3.4h, v5.4h, v7.4h
    sub v1.4h, v5.4h, v7.4h
    trn1 v4.4h, v0.4h, v2.4h
    trn2 v6.4h, v0.4h, v2.4h
    trn1 v5.4h, v1.4h, v3.4h
    trn2 v7.4h, v1.4h, v3.4h
    add v0.4h, v4.4h, v6.4h
    sub v2.4h, v4.4h, v6.4h
    add v1.4h, v5.4h, v7.4h
    sub v3.4h, v5.4h, v7.4h
    trn1 v4.2s, v0.2s, v1.2s
    trn2 v5.2s, v0.2s, v1.2s
    trn1 v6.2s, v2.2s, v3.2s
    trn2 v7.2s, v2.2s, v3.2s
    add v16.4h, v4.4h, v31.4h
    add v17.4h, v6.4h, v31.4h
    srhadd v0.4h, v4.4h, v5.4h
    shsub v1.4h, v16.4h, v5.4h
    shsub v2.4h, v17.4h, v7.4h
    srhadd v3.4h, v6.4h, v7.4h
    st1 {v0.4h,v1.4h,v2.4h,v3.4h}, [x0]
    ret
 # //.size x264_dct4x4dc_neon, . - x264_dct4x4dc_neon
        //.endfunc

        .text
        .align 2
        .globl _x264_idct4x4dc_neon
 # //.type _x264_idct4x4dc_neon, %function
        //.func _x264_idct4x4dc_neon
_x264_idct4x4dc_neon:
    ld1 {v0.4h,v1.4h,v2.4h,v3.4h}, [x0]
    add v4.4h, v0.4h, v1.4h
    sub v5.4h, v0.4h, v1.4h
    add v6.4h, v2.4h, v3.4h
    sub v7.4h, v2.4h, v3.4h
    add v0.4h, v4.4h, v6.4h
    sub v2.4h, v4.4h, v6.4h
    add v3.4h, v5.4h, v7.4h
    sub v1.4h, v5.4h, v7.4h
    trn1 v4.4h, v0.4h, v2.4h
    trn2 v6.4h, v0.4h, v2.4h
    trn1 v5.4h, v1.4h, v3.4h
    trn2 v7.4h, v1.4h, v3.4h
    add v0.4h, v4.4h, v6.4h
    sub v2.4h, v4.4h, v6.4h
    add v1.4h, v5.4h, v7.4h
    sub v3.4h, v5.4h, v7.4h
    trn1 v4.2s, v0.2s, v1.2s
    trn2 v5.2s, v0.2s, v1.2s
    trn1 v6.2s, v2.2s, v3.2s
    trn2 v7.2s, v2.2s, v3.2s
    add v0.4h, v4.4h, v5.4h
    sub v1.4h, v4.4h, v5.4h
    add v3.4h, v6.4h, v7.4h
    sub v2.4h, v6.4h, v7.4h
    st1 {v0.4h,v1.4h,v2.4h,v3.4h}, [x0]
    ret
 # //.size x264_idct4x4dc_neon, . - x264_idct4x4dc_neon
        //.endfunc


        .text
        .align 2
        .globl _x264_sub4x4_dct_neon
 # //.type _x264_sub4x4_dct_neon, %function
        //.func _x264_sub4x4_dct_neon
_x264_sub4x4_dct_neon:
    mov x3, #16
    mov x4, #32
    ld1 {v0.s}[0], [x1], x3
    ld1 {v1.s}[0], [x2], x4
    ld1 {v2.s}[0], [x1], x3
    usubl v16.8h, v0.8b, v1.8b
    ld1 {v3.s}[0], [x2], x4
    ld1 {v4.s}[0], [x1], x3
    usubl v17.8h, v2.8b, v3.8b
    ld1 {v5.s}[0], [x2], x4
    ld1 {v6.s}[0], [x1], x3
    usubl v18.8h, v4.8b, v5.8b
    ld1 {v7.s}[0], [x2], x4
    usubl v19.8h, v6.8b, v7.8b

    add v1.4h, v17.4h, v18.4h
    sub v18.4h, v17.4h, v18.4h
    add v3.4h, v16.4h, v19.4h
    sub v19.4h, v16.4h, v19.4h
    add v0.4h, v3.4h, v1.4h
    add v16.4h, v19.4h, v19.4h
    add v17.4h, v18.4h, v18.4h
    sub v2.4h, v3.4h, v1.4h
    add v1.4h, v16.4h, v18.4h
    sub v3.4h, v19.4h, v17.4h
    trn1 v4.2s, v0.2s, v2.2s
    trn2 v6.2s, v0.2s, v2.2s
    trn1 v5.2s, v1.2s, v3.2s
    trn2 v7.2s, v1.2s, v3.2s
    trn1 v0.4h, v4.4h, v5.4h
    trn2 v1.4h, v4.4h, v5.4h
    trn1 v2.4h, v6.4h, v7.4h
    trn2 v3.4h, v6.4h, v7.4h
    add v5.4h, v1.4h, v2.4h
    sub v2.4h, v1.4h, v2.4h
    add v7.4h, v0.4h, v3.4h
    sub v3.4h, v0.4h, v3.4h
    add v4.4h, v7.4h, v5.4h
    add v0.4h, v3.4h, v3.4h
    add v1.4h, v2.4h, v2.4h
    sub v6.4h, v7.4h, v5.4h
    add v5.4h, v0.4h, v2.4h
    sub v7.4h, v3.4h, v1.4h
    st1 {v4.4h,v5.4h,v6.4h,v7.4h}, [x0]
    ret
 # //.size x264_sub4x4_dct_neon, . - x264_sub4x4_dct_neon
        //.endfunc

        .text
        .align 2
 # //.type x264_sub8x4_dct_neon, %function
        //.func x264_sub8x4_dct_neon
x264_sub8x4_dct_neon:
    ld1 {v0.8b}, [x1], x3
    ld1 {v1.8b}, [x2], x4
    usubl v16.8h, v0.8b, v1.8b
    ld1 {v2.8b}, [x1], x3
    ld1 {v3.8b}, [x2], x4
    usubl v17.8h, v2.8b, v3.8b
    ld1 {v4.8b}, [x1], x3
    ld1 {v5.8b}, [x2], x4
    usubl v18.8h, v4.8b, v5.8b
    ld1 {v6.8b}, [x1], x3
    ld1 {v7.8b}, [x2], x4
    usubl v19.8h, v6.8b, v7.8b

    add v1.8h, v17.8h, v18.8h
    sub v18.8h, v17.8h, v18.8h
    add v3.8h, v16.8h, v19.8h
    sub v19.8h, v16.8h, v19.8h
    add v0.8h, v3.8h, v1.8h
    add v16.8h, v19.8h, v19.8h
    add v17.8h, v18.8h, v18.8h
    sub v2.8h, v3.8h, v1.8h
    add v1.8h, v16.8h, v18.8h
    sub v3.8h, v19.8h, v17.8h
    trn1 v4.4s, v0.4s, v2.4s
    trn2 v6.4s, v0.4s, v2.4s
    trn1 v5.4s, v1.4s, v3.4s
    trn2 v7.4s, v1.4s, v3.4s
    trn1 v0.8h, v4.8h, v5.8h
    trn2 v1.8h, v4.8h, v5.8h
    trn1 v2.8h, v6.8h, v7.8h
    trn2 v3.8h, v6.8h, v7.8h

    add v16.8h, v0.8h, v3.8h
    sub v19.8h, v0.8h, v3.8h
    add v17.8h, v1.8h, v2.8h
    sub v18.8h, v1.8h, v2.8h
    add v22.8h, v19.8h, v19.8h
    add v21.8h, v18.8h, v18.8h
    add v0.8h, v16.8h, v17.8h
    sub v1.8h, v16.8h, v17.8h

    add v2.8h, v22.8h, v18.8h
    sub v3.8h, v19.8h, v21.8h

    zip1 v4.2d, v0.2d, v2.2d
    zip2 v6.2d, v0.2d, v2.2d
    zip1 v5.2d, v1.2d, v3.2d
    zip2 v7.2d, v1.2d, v3.2d

    st1 {v4.8h}, [x0], #16
    st1 {v5.8h}, [x0], #16
    st1 {v6.8h}, [x0], #16
    st1 {v7.8h}, [x0], #16
    ret
 # //.size x264_sub8x4_dct_neon, . - x264_sub8x4_dct_neon
        //.endfunc

        .text
        .align 2
        .globl _x264_sub8x8_dct_neon
 # //.type _x264_sub8x8_dct_neon, %function
        //.func _x264_sub8x8_dct_neon
_x264_sub8x8_dct_neon:
    mov x5, x30
    mov x3, #16
    mov x4, #32
    bl x264_sub8x4_dct_neon
    mov x30, x5
    b x264_sub8x4_dct_neon
 # //.size x264_sub8x8_dct_neon, . - x264_sub8x8_dct_neon
        //.endfunc

        .text
        .align 2
        .globl _x264_sub16x16_dct_neon
 # //.type _x264_sub16x16_dct_neon, %function
        //.func _x264_sub16x16_dct_neon
_x264_sub16x16_dct_neon:
    mov x5, x30
    mov x3, #16
    mov x4, #32
    bl x264_sub8x4_dct_neon
    bl x264_sub8x4_dct_neon
    sub x1, x1, #8*16 -8
    sub x2, x2, #8*32 -8
    bl x264_sub8x4_dct_neon
    bl x264_sub8x4_dct_neon
    sub x1, x1, #8
    sub x2, x2, #8
    bl x264_sub8x4_dct_neon
    bl x264_sub8x4_dct_neon
    sub x1, x1, #8*16 -8
    sub x2, x2, #8*32 -8
    bl x264_sub8x4_dct_neon
    mov x30, x5
    b x264_sub8x4_dct_neon
 # //.size x264_sub16x16_dct_neon, . - x264_sub16x16_dct_neon
        //.endfunc



        .text
        .align 2
        .globl _x264_sub8x8_dct8_neon
 # //.type _x264_sub8x8_dct8_neon, %function
        //.func _x264_sub8x8_dct8_neon
_x264_sub8x8_dct8_neon:
    mov x3, #16
    mov x4, #32
    ld1 {v16.8b}, [x1], x3
    ld1 {v17.8b}, [x2], x4
    ld1 {v18.8b}, [x1], x3
    ld1 {v19.8b}, [x2], x4
    usubl v0.8h, v16.8b, v17.8b
    ld1 {v20.8b}, [x1], x3
    ld1 {v21.8b}, [x2], x4
    usubl v1.8h, v18.8b, v19.8b
    ld1 {v22.8b}, [x1], x3
    ld1 {v23.8b}, [x2], x4
    usubl v2.8h, v20.8b, v21.8b
    ld1 {v24.8b}, [x1], x3
    ld1 {v25.8b}, [x2], x4
    usubl v3.8h, v22.8b, v23.8b
    ld1 {v26.8b}, [x1], x3
    ld1 {v27.8b}, [x2], x4
    usubl v4.8h, v24.8b, v25.8b
    ld1 {v28.8b}, [x1], x3
    ld1 {v29.8b}, [x2], x4
    usubl v5.8h, v26.8b, v27.8b
    ld1 {v30.8b}, [x1], x3
    ld1 {v31.8b}, [x2], x4
    usubl v6.8h, v28.8b, v29.8b
    usubl v7.8h, v30.8b, v31.8b

    add v18.8h, v3.8h, v4.8h
    sub v17.8h, v3.8h, v4.8h
    add v19.8h, v2.8h, v5.8h
    sub v16.8h, v2.8h, v5.8h
    add v22.8h, v1.8h, v6.8h
    sub v21.8h, v1.8h, v6.8h
    add v23.8h, v0.8h, v7.8h
    sub v20.8h, v0.8h, v7.8h

    add v24.8h, v23.8h, v18.8h
    sub v26.8h, v23.8h, v18.8h
    add v25.8h, v22.8h, v19.8h
    sub v27.8h, v22.8h, v19.8h

    add v30.8h, v20.8h, v17.8h
    sub v29.8h, v20.8h, v17.8h
    sshr v23.8h, v21.8h, #1
    sshr v18.8h, v16.8h, #1
    add v23.8h, v23.8h, v21.8h
    add v18.8h, v18.8h, v16.8h
    sub v30.8h, v30.8h, v23.8h
    sub v29.8h, v29.8h, v18.8h

    add v28.8h, v21.8h, v16.8h
    sub v31.8h, v21.8h, v16.8h
    sshr v22.8h, v20.8h, #1
    sshr v19.8h, v17.8h, #1
    add v22.8h, v22.8h, v20.8h
    add v19.8h, v19.8h, v17.8h
    add v22.8h, v28.8h, v22.8h
    add v31.8h, v31.8h, v19.8h

    add v0.8h, v24.8h, v25.8h
    sub v4.8h, v24.8h, v25.8h
    sshr v16.8h, v31.8h, #2
    sshr v17.8h, v22.8h, #2
    add v1.8h, v22.8h, v16.8h
    sub v7.8h, v17.8h, v31.8h
    sshr v18.8h, v27.8h, #1
    sshr v19.8h, v26.8h, #1
    add v2.8h, v26.8h, v18.8h
    sub v6.8h, v19.8h, v27.8h
    sshr v20.8h, v30.8h, #2
    sshr v21.8h, v29.8h, #2
    add v3.8h, v20.8h, v29.8h
    sub v5.8h, v30.8h, v21.8h
    trn1 v30.8H, v0.8H, v1.8H
    trn2 v31.8H, v0.8H, v1.8H
    trn1 v1.8H, v2.8H, v3.8H
    trn2 v3.8H, v2.8H, v3.8H
    trn1 v0.8H, v4.8H, v5.8H
    trn2 v5.8H, v4.8H, v5.8H
    trn1 v2.8H, v6.8H, v7.8H
    trn2 v7.8H, v6.8H, v7.8H

    trn1 v4.4S, v0.4S, v2.4S
    trn2 v2.4S, v0.4S, v2.4S
    trn1 v6.4S, v5.4S, v7.4S
    trn2 v7.4S, v5.4S, v7.4S
    trn1 v5.4S, v31.4S, v3.4S
    trn2 v31.4S, v31.4S, v3.4S
    trn1 v3.4S, v30.4S, v1.4S
    trn2 v30.4S, v30.4S, v1.4S

    trn1 v0.2D, v3.2D, v4.2D
    trn2 v4.2D, v3.2D, v4.2D

    trn1 v1.2D, v5.2D, v6.2D
    trn2 v5.2D, v5.2D, v6.2D

    trn2 v6.2D, v30.2D, v2.2D
    trn1 v2.2D, v30.2D, v2.2D

    trn1 v3.2D, v31.2D, v7.2D
    trn2 v7.2D, v31.2D, v7.2D
    add v18.8h, v3.8h, v4.8h
    sub v17.8h, v3.8h, v4.8h
    add v19.8h, v2.8h, v5.8h
    sub v16.8h, v2.8h, v5.8h
    add v22.8h, v1.8h, v6.8h
    sub v21.8h, v1.8h, v6.8h
    add v23.8h, v0.8h, v7.8h
    sub v20.8h, v0.8h, v7.8h

    add v24.8h, v23.8h, v18.8h
    sub v26.8h, v23.8h, v18.8h
    add v25.8h, v22.8h, v19.8h
    sub v27.8h, v22.8h, v19.8h

    add v30.8h, v20.8h, v17.8h
    sub v29.8h, v20.8h, v17.8h
    sshr v23.8h, v21.8h, #1
    sshr v18.8h, v16.8h, #1
    add v23.8h, v23.8h, v21.8h
    add v18.8h, v18.8h, v16.8h
    sub v30.8h, v30.8h, v23.8h
    sub v29.8h, v29.8h, v18.8h

    add v28.8h, v21.8h, v16.8h
    sub v31.8h, v21.8h, v16.8h
    sshr v22.8h, v20.8h, #1
    sshr v19.8h, v17.8h, #1
    add v22.8h, v22.8h, v20.8h
    add v19.8h, v19.8h, v17.8h
    add v22.8h, v28.8h, v22.8h
    add v31.8h, v31.8h, v19.8h

    add v0.8h, v24.8h, v25.8h
    sub v4.8h, v24.8h, v25.8h
    sshr v16.8h, v31.8h, #2
    sshr v17.8h, v22.8h, #2
    add v1.8h, v22.8h, v16.8h
    sub v7.8h, v17.8h, v31.8h
    sshr v18.8h, v27.8h, #1
    sshr v19.8h, v26.8h, #1
    add v2.8h, v26.8h, v18.8h
    sub v6.8h, v19.8h, v27.8h
    sshr v20.8h, v30.8h, #2
    sshr v21.8h, v29.8h, #2
    add v3.8h, v20.8h, v29.8h
    sub v5.8h, v30.8h, v21.8h

    st1 {v0.8h,v1.8h,v2.8h,v3.8h}, [x0], #64
    st1 {v4.8h,v5.8h,v6.8h,v7.8h}, [x0], #64
    ret
 # //.size x264_sub8x8_dct8_neon, . - x264_sub8x8_dct8_neon
        //.endfunc

        .text
        .align 2
        .globl _x264_sub16x16_dct8_neon
 # //.type _x264_sub16x16_dct8_neon, %function
        //.func _x264_sub16x16_dct8_neon
_x264_sub16x16_dct8_neon:
    mov x7, x30
    bl _x264_sub8x8_dct8_neon
    sub x1, x1, #16*8 - 8
    sub x2, x2, #32*8 - 8
    bl _x264_sub8x8_dct8_neon
    sub x1, x1, #8
    sub x2, x2, #8
    bl _x264_sub8x8_dct8_neon
    mov x30, x7
    sub x1, x1, #16*8 - 8
    sub x2, x2, #32*8 - 8
    b _x264_sub8x8_dct8_neon
 # //.size x264_sub16x16_dct8_neon, . - x264_sub16x16_dct8_neon
        //.endfunc




        .text
        .align 2
        .globl _x264_add4x4_idct_neon
 # //.type _x264_add4x4_idct_neon, %function
        //.func _x264_add4x4_idct_neon
_x264_add4x4_idct_neon:
    mov x2, #32
    ld1 {v0.4h,v1.4h,v2.4h,v3.4h}, [x1]

    add v4.4h, v0.4h, v2.4h
    sub v5.4h, v0.4h, v2.4h
    sshr v7.4h, v1.4h, #1
    sshr v6.4h, v3.4h, #1
    sub v7.4h, v7.4h, v3.4h
    add v6.4h, v6.4h, v1.4h
    ld1 {v28.s}[0], [x0], x2
    add v0.4h, v4.4h, v6.4h
    sub v2.4h, v4.4h, v6.4h
    add v1.4h, v5.4h, v7.4h
    sub v3.4h, v5.4h, v7.4h

    trn1 v16.2s, v0.2s, v3.2s
    trn2 v18.2s, v0.2s, v3.2s
    trn1 v17.2s, v1.2s, v2.2s
    trn2 v19.2s, v1.2s, v2.2s
    trn1 v0.4h, v16.4h, v17.4h
    trn2 v1.4h, v16.4h, v17.4h
    trn1 v3.4h, v18.4h, v19.4h
    trn2 v2.4h, v18.4h, v19.4h

    add v4.4h, v0.4h, v3.4h
    sub v5.4h, v0.4h, v3.4h
    sshr v7.4h, v1.4h, #1
    sshr v6.4h, v2.4h, #1
    sub v7.4h, v7.4h, v2.4h
    add v6.4h, v6.4h, v1.4h
    ld1 {v29.s}[0], [x0], x2
    add v0.4h, v4.4h, v6.4h
    sub v2.4h, v4.4h, v6.4h
    add v1.4h, v5.4h, v7.4h
    sub v3.4h, v5.4h, v7.4h

    srshr v0.4h, v0.4h, #6
    srshr v1.4h, v1.4h, #6
    ld1 {v31.s}[0], [x0], x2
    srshr v2.4h, v2.4h, #6
    srshr v3.4h, v3.4h, #6
    ld1 {v30.s}[0], [x0], x2

    sub x0, x0, x2, lsl #2
    uaddw v0.8h, v0.8h, v28.8b
    uaddw v1.8h, v1.8h, v29.8b
    uaddw v2.8h, v2.8h, v30.8b
    uaddw v3.8h, v3.8h, v31.8b
    sqxtun v0.8b, v0.8h
    sqxtun v1.8b, v1.8h
    sqxtun v2.8b, v2.8h
    sqxtun v3.8b, v3.8h

    st1 {v0.s}[0], [x0], x2
    st1 {v1.s}[0], [x0], x2
    st1 {v3.s}[0], [x0], x2
    st1 {v2.s}[0], [x0], x2
    ret
 # //.size x264_add4x4_idct_neon, . - x264_add4x4_idct_neon
        //.endfunc

        .text
        .align 2
        .globl _x264_add8x4_idct_neon
 # //.type _x264_add8x4_idct_neon, %function
        //.func _x264_add8x4_idct_neon
_x264_add8x4_idct_neon:
    ld1 {v0.8h,v1.8h}, [x1], #32
    ld1 {v2.8h,v3.8h}, [x1], #32
    trn1 v20.2d, v0.2d, v2.2d
    trn2 v21.2d, v0.2d, v2.2d
    trn1 v22.2d, v1.2d, v3.2d
    trn2 v23.2d, v1.2d, v3.2d
    add v16.8h, v20.8h, v22.8h
    sub v17.8h, v20.8h, v22.8h
    sshr v19.8h, v21.8h, #1
    sshr v18.8h, v23.8h, #1
    sub v19.8h, v19.8h, v23.8h
    add v18.8h, v18.8h, v21.8h
    add v0.8h, v16.8h, v18.8h
    sub v3.8h, v16.8h, v18.8h
    add v1.8h, v17.8h, v19.8h
    sub v2.8h, v17.8h, v19.8h

    trn1 v4.4s, v0.4s, v2.4s
    trn2 v6.4s, v0.4s, v2.4s
    trn1 v5.4s, v1.4s, v3.4s
    trn2 v7.4s, v1.4s, v3.4s
    trn1 v0.8h, v4.8h, v5.8h
    trn2 v1.8h, v4.8h, v5.8h
    trn1 v2.8h, v6.8h, v7.8h
    trn2 v3.8h, v6.8h, v7.8h

    add v16.8h, v0.8h, v2.8h
    sub v17.8h, v0.8h, v2.8h
    sshr v19.8h, v1.8h, #1
    sshr v18.8h, v3.8h, #1
    sub v19.8h, v19.8h, v3.8h
    add v18.8h, v18.8h, v1.8h
    add v0.8h, v16.8h, v18.8h
    sub v3.8h, v16.8h, v18.8h
    add v1.8h, v17.8h, v19.8h
    sub v2.8h, v17.8h, v19.8h

    srshr v0.8h, v0.8h, #6
    ld1 {v28.8b}, [x0], x2
    srshr v1.8h, v1.8h, #6
    ld1 {v29.8b}, [x0], x2
    srshr v2.8h, v2.8h, #6
    ld1 {v30.8b}, [x0], x2
    srshr v3.8h, v3.8h, #6
    ld1 {v31.8b}, [x0], x2

    sub x0, x0, x2, lsl #2
    uaddw v0.8h, v0.8h, v28.8b
    uaddw v1.8h, v1.8h, v29.8b
    uaddw v2.8h, v2.8h, v30.8b
    uaddw v3.8h, v3.8h, v31.8b

    sqxtun v0.8b, v0.8h
    sqxtun v1.8b, v1.8h
    st1 {v0.8b}, [x0], x2
    sqxtun v2.8b, v2.8h
    st1 {v1.8b}, [x0], x2
    sqxtun v3.8b, v3.8h
    st1 {v2.8b}, [x0], x2
    st1 {v3.8b}, [x0], x2
    ret
 # //.size x264_add8x4_idct_neon, . - x264_add8x4_idct_neon
        //.endfunc

        .text
        .align 2
        .globl _x264_add8x8_idct_neon
 # //.type _x264_add8x8_idct_neon, %function
        //.func _x264_add8x8_idct_neon
_x264_add8x8_idct_neon:
    mov x2, #32
    mov x5, x30
    bl _x264_add8x4_idct_neon
    mov x30, x5
    b _x264_add8x4_idct_neon
 # //.size x264_add8x8_idct_neon, . - x264_add8x8_idct_neon
        //.endfunc

        .text
        .align 2
        .globl _x264_add16x16_idct_neon
 # //.type _x264_add16x16_idct_neon, %function
        //.func _x264_add16x16_idct_neon
_x264_add16x16_idct_neon:
    mov x2, #32
    mov x5, x30
    bl _x264_add8x4_idct_neon
    bl _x264_add8x4_idct_neon
    sub x0, x0, #8*32 -8
    bl _x264_add8x4_idct_neon
    bl _x264_add8x4_idct_neon
    sub x0, x0, #8
    bl _x264_add8x4_idct_neon
    bl _x264_add8x4_idct_neon
    sub x0, x0, #8*32 -8
    bl _x264_add8x4_idct_neon
    mov x30, x5
    b _x264_add8x4_idct_neon
 # //.size x264_add16x16_idct_neon, . - x264_add16x16_idct_neon
        //.endfunc


        .text
        .align 2
        .globl _x264_add8x8_idct8_neon
 # //.type _x264_add8x8_idct8_neon, %function
        //.func _x264_add8x8_idct8_neon
_x264_add8x8_idct8_neon:
    mov x2, #32
    ld1 {v16.8h,v17.8h}, [x1], #32
    ld1 {v18.8h,v19.8h}, [x1], #32
    ld1 {v20.8h,v21.8h}, [x1], #32

    add v0.8h, v16.8h, v20.8h
    sub v1.8h, v16.8h, v20.8h
    ld1 {v22.8h,v23.8h}, [x1], #32
    sshr v16.8h, v22.8h, #1
    sshr v20.8h, v18.8h, #1
    add v2.8h, v18.8h, v16.8h
    sub v3.8h, v20.8h, v22.8h
    add v16.8h, v21.8h, v19.8h
    sub v18.8h, v21.8h, v19.8h
    sshr v20.8h, v17.8h, #1
    sshr v22.8h, v23.8h, #1
    add v20.8h, v20.8h, v17.8h
    add v22.8h, v22.8h, v23.8h
    add v16.8h, v16.8h, v20.8h
    sub v18.8h, v18.8h, v22.8h
    add v22.8h, v23.8h, v17.8h
    sub v23.8h, v23.8h, v17.8h
    sshr v20.8h, v21.8h, #1
    sshr v17.8h, v19.8h, #1
    add v20.8h, v20.8h, v21.8h
    add v17.8h, v17.8h, v19.8h
    add v23.8h, v23.8h, v20.8h
    sub v22.8h, v22.8h, v17.8h

    sshr v19.8h, v23.8h, #2
    sshr v17.8h, v22.8h, #2
    add v21.8h, v22.8h, v19.8h
    sub v22.8h, v17.8h, v23.8h
    sshr v19.8h, v16.8h, #2
    sshr v17.8h, v18.8h, #2
    add v20.8h, v19.8h, v18.8h
    sub v23.8h, v16.8h, v17.8h

    add v18.8h, v0.8h, v2.8h
    sub v2.8h, v0.8h, v2.8h
    add v19.8h, v1.8h, v3.8h
    sub v3.8h, v1.8h, v3.8h

    add v16.8h, v18.8h, v23.8h
    sub v23.8h, v18.8h, v23.8h
    add v17.8h, v19.8h, v22.8h
    sub v22.8h, v19.8h, v22.8h
    add v18.8h, v3.8h, v21.8h
    sub v21.8h, v3.8h, v21.8h
    add v19.8h, v2.8h, v20.8h
    sub v20.8h, v2.8h, v20.8h

    trn1 v30.8H, v16.8H, v17.8H
    trn2 v31.8H, v16.8H, v17.8H
    trn1 v17.8H, v18.8H, v19.8H
    trn2 v19.8H, v18.8H, v19.8H
    trn1 v16.8H, v20.8H, v21.8H
    trn2 v21.8H, v20.8H, v21.8H
    trn1 v18.8H, v22.8H, v23.8H
    trn2 v23.8H, v22.8H, v23.8H

    trn1 v20.4S, v16.4S, v18.4S
    trn2 v18.4S, v16.4S, v18.4S
    trn1 v22.4S, v21.4S, v23.4S
    trn2 v23.4S, v21.4S, v23.4S
    trn1 v21.4S, v31.4S, v19.4S
    trn2 v31.4S, v31.4S, v19.4S
    trn1 v19.4S, v30.4S, v17.4S
    trn2 v30.4S, v30.4S, v17.4S

    trn1 v16.2D, v19.2D, v20.2D
    trn2 v20.2D, v19.2D, v20.2D

    trn1 v17.2D, v21.2D, v22.2D
    trn2 v21.2D, v21.2D, v22.2D

    trn2 v22.2D, v30.2D, v18.2D
    trn1 v18.2D, v30.2D, v18.2D

    trn1 v19.2D, v31.2D, v23.2D
    trn2 v23.2D, v31.2D, v23.2D

    add v0.8h, v16.8h, v20.8h
    sub v1.8h, v16.8h, v20.8h
    sshr v16.8h, v22.8h, #1
    sshr v20.8h, v18.8h, #1
    add v2.8h, v18.8h, v16.8h
    sub v3.8h, v20.8h, v22.8h
    add v16.8h, v21.8h, v19.8h
    sub v18.8h, v21.8h, v19.8h
    sshr v20.8h, v17.8h, #1
    sshr v22.8h, v23.8h, #1
    add v20.8h, v20.8h, v17.8h
    add v22.8h, v22.8h, v23.8h
    add v16.8h, v16.8h, v20.8h
    sub v18.8h, v18.8h, v22.8h
    add v22.8h, v23.8h, v17.8h
    sub v23.8h, v23.8h, v17.8h
    sshr v20.8h, v21.8h, #1
    sshr v17.8h, v19.8h, #1
    add v20.8h, v20.8h, v21.8h
    add v17.8h, v17.8h, v19.8h
    add v23.8h, v23.8h, v20.8h
    sub v22.8h, v22.8h, v17.8h

    sshr v19.8h, v23.8h, #2
    sshr v17.8h, v22.8h, #2
    add v21.8h, v22.8h, v19.8h
    sub v22.8h, v17.8h, v23.8h
    sshr v19.8h, v16.8h, #2
    sshr v17.8h, v18.8h, #2
    add v20.8h, v19.8h, v18.8h
    sub v23.8h, v16.8h, v17.8h

    add v18.8h, v0.8h, v2.8h
    sub v2.8h, v0.8h, v2.8h
    add v19.8h, v1.8h, v3.8h
    sub v3.8h, v1.8h, v3.8h

    add v16.8h, v18.8h, v23.8h
    sub v23.8h, v18.8h, v23.8h
    add v17.8h, v19.8h, v22.8h
    sub v22.8h, v19.8h, v22.8h
    add v18.8h, v3.8h, v21.8h
    sub v21.8h, v3.8h, v21.8h
    add v19.8h, v2.8h, v20.8h
    sub v20.8h, v2.8h, v20.8h

    ld1 {v0.8b}, [x0], x2
    srshr v16.8h, v16.8h, #6
    ld1 {v1.8b}, [x0], x2
    srshr v17.8h, v17.8h, #6
    ld1 {v2.8b}, [x0], x2
    srshr v18.8h, v18.8h, #6
    ld1 {v3.8b}, [x0], x2
    srshr v19.8h, v19.8h, #6
    ld1 {v4.8b}, [x0], x2
    srshr v20.8h, v20.8h, #6
    ld1 {v5.8b}, [x0], x2
    srshr v21.8h, v21.8h, #6
    ld1 {v6.8b}, [x0], x2
    srshr v22.8h, v22.8h, #6
    ld1 {v7.8b}, [x0], x2
    srshr v23.8h, v23.8h, #6
    sub x0, x0, x2, lsl #3

    uaddw v16.8h, v16.8h, v0.8b
    uaddw v17.8h, v17.8h, v1.8b
    uaddw v18.8h, v18.8h, v2.8b
    sqxtun v0.8b, v16.8h
    sqxtun v1.8b, v17.8h
    sqxtun v2.8b, v18.8h
    uaddw v19.8h, v19.8h, v3.8b
    st1 {v0.8b}, [x0], x2
    uaddw v20.8h, v20.8h, v4.8b
    st1 {v1.8b}, [x0], x2
    uaddw v21.8h, v21.8h, v5.8b
    st1 {v2.8b}, [x0], x2
    sqxtun v3.8b, v19.8h
    sqxtun v4.8b, v20.8h
    uaddw v22.8h, v22.8h, v6.8b
    uaddw v23.8h, v23.8h, v7.8b
    st1 {v3.8b}, [x0], x2
    sqxtun v5.8b, v21.8h
    st1 {v4.8b}, [x0], x2
    sqxtun v6.8b, v22.8h
    sqxtun v7.8b, v23.8h
    st1 {v5.8b}, [x0], x2
    st1 {v6.8b}, [x0], x2
    st1 {v7.8b}, [x0], x2
    ret
 # //.size x264_add8x8_idct8_neon, . - x264_add8x8_idct8_neon
        //.endfunc

        .text
        .align 2
        .globl _x264_add16x16_idct8_neon
 # //.type _x264_add16x16_idct8_neon, %function
        //.func _x264_add16x16_idct8_neon
_x264_add16x16_idct8_neon:
    mov x7, x30
    bl _x264_add8x8_idct8_neon
    sub x0, x0, #8*32 -8
    bl _x264_add8x8_idct8_neon
    sub x0, x0, #8
    bl _x264_add8x8_idct8_neon
    sub x0, x0, #8*32 -8
    mov x30, x7
    b _x264_add8x8_idct8_neon
 # //.size x264_add16x16_idct8_neon, . - x264_add16x16_idct8_neon
        //.endfunc

        .text
        .align 2
        .globl _x264_add8x8_idct_dc_neon
 # //.type _x264_add8x8_idct_dc_neon, %function
        //.func _x264_add8x8_idct_dc_neon
_x264_add8x8_idct_dc_neon:
    mov x2, #32
    ld1 {v16.4h}, [x1]
    ld1 {v0.8b}, [x0], x2
    srshr v16.4h, v16.4h, #6
    ld1 {v1.8b}, [x0], x2
    dup v20.8h, v16.h[0]
    dup v21.8h, v16.h[1]
    ld1 {v2.8b}, [x0], x2
    dup v22.8h, v16.h[2]
    dup v23.8h, v16.h[3]
    ld1 {v3.8b}, [x0], x2
    trn1 v20.2d, v20.2d, v21.2d
    ld1 {v4.8b}, [x0], x2
    trn1 v21.2d, v22.2d, v23.2d
    ld1 {v5.8b}, [x0], x2
    neg v22.8h, v20.8h
    ld1 {v6.8b}, [x0], x2
    neg v23.8h, v21.8h
    ld1 {v7.8b}, [x0], x2

    sub x0, x0, #8*32

    sqxtun v20.8b, v20.8h
    sqxtun v21.8b, v21.8h
    sqxtun v22.8b, v22.8h
    sqxtun v23.8b, v23.8h

    uqadd v0.8b, v0.8b, v20.8b
    uqadd v1.8b, v1.8b, v20.8b
    uqadd v2.8b, v2.8b, v20.8b
    uqadd v3.8b, v3.8b, v20.8b
    uqadd v4.8b, v4.8b, v21.8b
    uqadd v5.8b, v5.8b, v21.8b
    uqadd v6.8b, v6.8b, v21.8b
    uqadd v7.8b, v7.8b, v21.8b
    uqsub v0.8b, v0.8b, v22.8b
    uqsub v1.8b, v1.8b, v22.8b
    uqsub v2.8b, v2.8b, v22.8b
    uqsub v3.8b, v3.8b, v22.8b
    uqsub v4.8b, v4.8b, v23.8b
    uqsub v5.8b, v5.8b, v23.8b
    uqsub v6.8b, v6.8b, v23.8b
    uqsub v7.8b, v7.8b, v23.8b

    st1 {v0.8b}, [x0], x2
    st1 {v1.8b}, [x0], x2
    st1 {v2.8b}, [x0], x2
    st1 {v3.8b}, [x0], x2
    st1 {v4.8b}, [x0], x2
    st1 {v5.8b}, [x0], x2
    st1 {v6.8b}, [x0], x2
    st1 {v7.8b}, [x0], x2
    ret
 # //.size x264_add8x8_idct_dc_neon, . - x264_add8x8_idct_dc_neon
        //.endfunc


        .text
        .align 2
        .globl _x264_add16x16_idct_dc_neon
 # //.type _x264_add16x16_idct_dc_neon, %function
        //.func _x264_add16x16_idct_dc_neon
_x264_add16x16_idct_dc_neon:
    mov x2, x0
    mov x3, #32

    ld1 {v0.4h,v1.4h,v2.4h,v3.4h}, [x1]
    srshr v0.4h, v0.4h, #6
    srshr v1.4h, v1.4h, #6

    ld1 {v4.16b}, [x0], x3
    dup v24.8h, v0.h[0]
    dup v25.8h, v0.h[1]
    ld1 {v5.16b}, [x0], x3
    dup v26.8h, v0.h[2]
    dup v27.8h, v0.h[3]
    ld1 {v6.16b}, [x0], x3
    trn1 v24.2d, v24.2d, v25.2d
    ld1 {v7.16b}, [x0], x3
    trn1 v25.2d, v26.2d, v27.2d
    neg v26.8h, v24.8h
    neg v27.8h, v25.8h

    sqxtun v20.8b, v24.8h
    sqxtun v21.8b, v26.8h
    sqxtun2 v20.16b, v25.8h
    sqxtun2 v21.16b, v27.8h

    uqadd v4.16b, v4.16b, v20.16b
    uqadd v5.16b, v5.16b, v20.16b
    uqadd v6.16b, v6.16b, v20.16b
    uqadd v7.16b, v7.16b, v20.16b

    uqsub v4.16b, v4.16b, v21.16b
    uqsub v5.16b, v5.16b, v21.16b
    uqsub v6.16b, v6.16b, v21.16b
    st1 {v4.16b}, [x2], x3
    uqsub v7.16b, v7.16b, v21.16b
    st1 {v5.16b}, [x2], x3
    st1 {v6.16b}, [x2], x3
    st1 {v7.16b}, [x2], x3
    srshr v2.4h, v2.4h, #6
    ld1 {v4.16b}, [x0], x3
    dup v24.8h, v1.h[0]
    dup v25.8h, v1.h[1]
    ld1 {v5.16b}, [x0], x3
    dup v26.8h, v1.h[2]
    dup v27.8h, v1.h[3]
    ld1 {v6.16b}, [x0], x3
    trn1 v24.2d, v24.2d, v25.2d
    ld1 {v7.16b}, [x0], x3
    trn1 v25.2d, v26.2d, v27.2d
    neg v26.8h, v24.8h
    neg v27.8h, v25.8h

    sqxtun v20.8b, v24.8h
    sqxtun v21.8b, v26.8h
    sqxtun2 v20.16b, v25.8h
    sqxtun2 v21.16b, v27.8h

    uqadd v4.16b, v4.16b, v20.16b
    uqadd v5.16b, v5.16b, v20.16b
    uqadd v6.16b, v6.16b, v20.16b
    uqadd v7.16b, v7.16b, v20.16b

    uqsub v4.16b, v4.16b, v21.16b
    uqsub v5.16b, v5.16b, v21.16b
    uqsub v6.16b, v6.16b, v21.16b
    st1 {v4.16b}, [x2], x3
    uqsub v7.16b, v7.16b, v21.16b
    st1 {v5.16b}, [x2], x3
    st1 {v6.16b}, [x2], x3
    st1 {v7.16b}, [x2], x3
    srshr v3.4h, v3.4h, #6
    ld1 {v4.16b}, [x0], x3
    dup v24.8h, v2.h[0]
    dup v25.8h, v2.h[1]
    ld1 {v5.16b}, [x0], x3
    dup v26.8h, v2.h[2]
    dup v27.8h, v2.h[3]
    ld1 {v6.16b}, [x0], x3
    trn1 v24.2d, v24.2d, v25.2d
    ld1 {v7.16b}, [x0], x3
    trn1 v25.2d, v26.2d, v27.2d
    neg v26.8h, v24.8h
    neg v27.8h, v25.8h

    sqxtun v20.8b, v24.8h
    sqxtun v21.8b, v26.8h
    sqxtun2 v20.16b, v25.8h
    sqxtun2 v21.16b, v27.8h

    uqadd v4.16b, v4.16b, v20.16b
    uqadd v5.16b, v5.16b, v20.16b
    uqadd v6.16b, v6.16b, v20.16b
    uqadd v7.16b, v7.16b, v20.16b

    uqsub v4.16b, v4.16b, v21.16b
    uqsub v5.16b, v5.16b, v21.16b
    uqsub v6.16b, v6.16b, v21.16b
    st1 {v4.16b}, [x2], x3
    uqsub v7.16b, v7.16b, v21.16b
    st1 {v5.16b}, [x2], x3
    st1 {v6.16b}, [x2], x3
    st1 {v7.16b}, [x2], x3
    ld1 {v4.16b}, [x0], x3
    dup v24.8h, v3.h[0]
    dup v25.8h, v3.h[1]
    ld1 {v5.16b}, [x0], x3
    dup v26.8h, v3.h[2]
    dup v27.8h, v3.h[3]
    ld1 {v6.16b}, [x0], x3
    trn1 v24.2d, v24.2d, v25.2d
    ld1 {v7.16b}, [x0], x3
    trn1 v25.2d, v26.2d, v27.2d
    neg v26.8h, v24.8h
    neg v27.8h, v25.8h

    sqxtun v20.8b, v24.8h
    sqxtun v21.8b, v26.8h
    sqxtun2 v20.16b, v25.8h
    sqxtun2 v21.16b, v27.8h

    uqadd v4.16b, v4.16b, v20.16b
    uqadd v5.16b, v5.16b, v20.16b
    uqadd v6.16b, v6.16b, v20.16b
    uqadd v7.16b, v7.16b, v20.16b

    uqsub v4.16b, v4.16b, v21.16b
    uqsub v5.16b, v5.16b, v21.16b
    uqsub v6.16b, v6.16b, v21.16b
    st1 {v4.16b}, [x2], x3
    uqsub v7.16b, v7.16b, v21.16b
    st1 {v5.16b}, [x2], x3
    st1 {v6.16b}, [x2], x3
    st1 {v7.16b}, [x2], x3
    ret
 # //.size x264_add16x16_idct_dc_neon, . - x264_add16x16_idct_dc_neon
        //.endfunc

        .text
        .align 2
        .globl _x264_sub8x8_dct_dc_neon
 # //.type _x264_sub8x8_dct_dc_neon, %function
        //.func _x264_sub8x8_dct_dc_neon
_x264_sub8x8_dct_dc_neon:
    mov x3, #16
    mov x4, #32
    ld1 {v16.8b}, [x1], x3
    ld1 {v17.8b}, [x2], x4
    usubl v16.8h, v16.8b, v17.8b
    ld1 {v18.8b}, [x1], x3
    ld1 {v19.8b}, [x2], x4
    usubl v17.8h, v18.8b, v19.8b
    ld1 {v20.8b}, [x1], x3
    ld1 {v21.8b}, [x2], x4
    usubl v18.8h, v20.8b, v21.8b
    ld1 {v22.8b}, [x1], x3
    add v0.8h, v16.8h, v17.8h
    ld1 {v23.8b}, [x2], x4
    usubl v19.8h, v22.8b, v23.8b
    ld1 {v24.8b}, [x1], x3
    add v0.8h, v0.8h, v18.8h
    ld1 {v25.8b}, [x2], x4
    usubl v20.8h, v24.8b, v25.8b
    ld1 {v26.8b}, [x1], x3
    add v0.8h, v0.8h, v19.8h
    ld1 {v27.8b}, [x2], x4
    usubl v21.8h, v26.8b, v27.8b
    ld1 {v28.8b}, [x1], x3
    ld1 {v29.8b}, [x2], x4
    usubl v22.8h, v28.8b, v29.8b
    ld1 {v30.8b}, [x1], x3
    add v1.8h, v20.8h, v21.8h
    ld1 {v31.8b}, [x2], x4
    usubl v23.8h, v30.8b, v31.8b
    add v1.8h, v1.8h, v22.8h
    add v1.8h, v1.8h, v23.8h

    trn1 v2.2d, v0.2d, v1.2d
    trn2 v3.2d, v0.2d, v1.2d

    add v0.8h, v2.8h, v3.8h
    sub v1.8h, v2.8h, v3.8h

    trn1 v2.2d, v0.2d, v1.2d
    trn2 v3.2d, v0.2d, v1.2d

    add v0.8h, v2.8h, v3.8h
    sub v1.8h, v2.8h, v3.8h

    trn1 v2.2d, v0.2d, v1.2d
    trn2 v3.2d, v0.2d, v1.2d

    addp v0.8h, v2.8h, v3.8h
    addp v0.8h, v0.8h, v0.8h

    st1 {v0.4h}, [x0]
    ret
 # //.size x264_sub8x8_dct_dc_neon, . - x264_sub8x8_dct_dc_neon
        //.endfunc

        .text
        .align 2
        .globl _x264_zigzag_scan_4x4_frame_neon
 # //.type _x264_zigzag_scan_4x4_frame_neon, %function
        //.func _x264_zigzag_scan_4x4_frame_neon
_x264_zigzag_scan_4x4_frame_neon:

        adrp x2, scan4x4_frame@PAGE
        add x2, x2, scan4x4_frame@PAGEOFF






    ld1 {v0.16b,v1.16b}, [x1]
    ld1 {v16.16b,v17.16b}, [x2]
    tbl v2.16b, {v0.16b,v1.16b}, v16.16b
    tbl v3.16b, {v0.16b,v1.16b}, v17.16b
    st1 {v2.16b,v3.16b}, [x0]
    ret
 # //.size x264_zigzag_scan_4x4_frame_neon, . - x264_zigzag_scan_4x4_frame_neon
        //.endfunc

 #endif
