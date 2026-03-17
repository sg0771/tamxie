#ifdef __arm64__
        .text
        .align 2
        .globl _x264_prefetch_ref_aarch64
 # //.type _x264_prefetch_ref_aarch64, %function
        //.func _x264_prefetch_ref_aarch64
_x264_prefetch_ref_aarch64:
    cmp w2, #1
    csel x2, xzr, x1, eq
    add x0, x0, #64
    add x0, x0, x2, lsl #3

    lsl x2, x1, #1
    add x3, x1, x1, lsl #1
    add x4, x0, x1, lsl #2

    prfm pldl1strm, [x0]
    prfm pldl1strm, [x0, x1]
    prfm pldl1strm, [x0, x2]
    prfm pldl1strm, [x0, x3]
    prfm pldl1strm, [x4]
    prfm pldl1strm, [x4, x1]
    prfm pldl1strm, [x4, x2]
    prfm pldl1strm, [x4, x3]
    ret
 # //.size x264_prefetch_ref_aarch64, . - x264_prefetch_ref_aarch64
        //.endfunc




        .text
        .align 2
        .globl _x264_prefetch_fenc_420_aarch64
 # //.type _x264_prefetch_fenc_420_aarch64, %function
        //.func _x264_prefetch_fenc_420_aarch64
_x264_prefetch_fenc_420_aarch64:
    and w6, w5, #3
    and w7, w5, #3
    mul x6, x6, x1
    mul x7, x7, x3
    add x0, x0, #64
    add x2, x2, #64

    add x0, x0, x6, lsl #2
    add x6, x0, x1, lsl #1
    prfm pldl1strm, [x0]
    prfm pldl1strm, [x0, x1]
    prfm pldl1strm, [x6]
    prfm pldl1strm, [x6, x1]

    add x2, x2, x7, lsl #1
    prfm pldl1strm, [x2]
    prfm pldl1strm, [x2, x3]
    ret
 # //.size x264_prefetch_fenc_420_aarch64, . - x264_prefetch_fenc_420_aarch64
        //.endfunc
        .text
        .align 2
        .globl _x264_prefetch_fenc_422_aarch64
 # //.type _x264_prefetch_fenc_422_aarch64, %function
        //.func _x264_prefetch_fenc_422_aarch64
_x264_prefetch_fenc_422_aarch64:
    and w6, w5, #3
    and w7, w5, #3
    mul x6, x6, x1
    mul x7, x7, x3
    add x0, x0, #64
    add x2, x2, #64

    add x0, x0, x6, lsl #2
    add x6, x0, x1, lsl #1
    prfm pldl1strm, [x0]
    prfm pldl1strm, [x0, x1]
    prfm pldl1strm, [x6]
    prfm pldl1strm, [x6, x1]

    add x2, x2, x7, lsl #1
    prfm pldl1strm, [x2]
    prfm pldl1strm, [x2, x3]
    add x7, x2, x3, lsl #1
    prfm pldl1strm, [x7]
    prfm pldl1strm, [x7, x3]
    ret
 # //.size x264_prefetch_fenc_422_aarch64, . - x264_prefetch_fenc_422_aarch64
        //.endfunc





        .text
        .align 2
        .globl _x264_pixel_avg_4x2_neon
 # //.type _x264_pixel_avg_4x2_neon, %function
        //.func _x264_pixel_avg_4x2_neon
_x264_pixel_avg_4x2_neon:
    mov w10, #64
    cmp w6, #32
    mov w9, #2
    b.eq pixel_avg_w4_neon
    subs w7, w10, w6
    b.lt pixel_avg_weight_w4_add_sub_neon
    cmp w6, #0
    b.ge pixel_avg_weight_w4_add_add_neon
    b pixel_avg_weight_w4_sub_add_neon
 # //.size x264_pixel_avg_4x2_neon, . - x264_pixel_avg_4x2_neon
        //.endfunc
        .text
        .align 2
        .globl _x264_pixel_avg_4x4_neon
 # //.type _x264_pixel_avg_4x4_neon, %function
        //.func _x264_pixel_avg_4x4_neon
_x264_pixel_avg_4x4_neon:
    mov w10, #64
    cmp w6, #32
    mov w9, #4
    b.eq pixel_avg_w4_neon
    subs w7, w10, w6
    b.lt pixel_avg_weight_w4_add_sub_neon
    cmp w6, #0
    b.ge pixel_avg_weight_w4_add_add_neon
    b pixel_avg_weight_w4_sub_add_neon
 # //.size x264_pixel_avg_4x4_neon, . - x264_pixel_avg_4x4_neon
        //.endfunc
        .text
        .align 2
        .globl _x264_pixel_avg_4x8_neon
 # //.type _x264_pixel_avg_4x8_neon, %function
        //.func _x264_pixel_avg_4x8_neon
_x264_pixel_avg_4x8_neon:
    mov w10, #64
    cmp w6, #32
    mov w9, #8
    b.eq pixel_avg_w4_neon
    subs w7, w10, w6
    b.lt pixel_avg_weight_w4_add_sub_neon
    cmp w6, #0
    b.ge pixel_avg_weight_w4_add_add_neon
    b pixel_avg_weight_w4_sub_add_neon
 # //.size x264_pixel_avg_4x8_neon, . - x264_pixel_avg_4x8_neon
        //.endfunc
        .text
        .align 2
        .globl _x264_pixel_avg_4x16_neon
 # //.type _x264_pixel_avg_4x16_neon, %function
        //.func _x264_pixel_avg_4x16_neon
_x264_pixel_avg_4x16_neon:
    mov w10, #64
    cmp w6, #32
    mov w9, #16
    b.eq pixel_avg_w4_neon
    subs w7, w10, w6
    b.lt pixel_avg_weight_w4_add_sub_neon
    cmp w6, #0
    b.ge pixel_avg_weight_w4_add_add_neon
    b pixel_avg_weight_w4_sub_add_neon
 # //.size x264_pixel_avg_4x16_neon, . - x264_pixel_avg_4x16_neon
        //.endfunc
        .text
        .align 2
        .globl _x264_pixel_avg_8x4_neon
 # //.type _x264_pixel_avg_8x4_neon, %function
        //.func _x264_pixel_avg_8x4_neon
_x264_pixel_avg_8x4_neon:
    mov w10, #64
    cmp w6, #32
    mov w9, #4
    b.eq pixel_avg_w8_neon
    subs w7, w10, w6
    b.lt pixel_avg_weight_w8_add_sub_neon
    cmp w6, #0
    b.ge pixel_avg_weight_w8_add_add_neon
    b pixel_avg_weight_w8_sub_add_neon
 # //.size x264_pixel_avg_8x4_neon, . - x264_pixel_avg_8x4_neon
        //.endfunc
        .text
        .align 2
        .globl _x264_pixel_avg_8x8_neon
 # //.type _x264_pixel_avg_8x8_neon, %function
        //.func _x264_pixel_avg_8x8_neon
_x264_pixel_avg_8x8_neon:
    mov w10, #64
    cmp w6, #32
    mov w9, #8
    b.eq pixel_avg_w8_neon
    subs w7, w10, w6
    b.lt pixel_avg_weight_w8_add_sub_neon
    cmp w6, #0
    b.ge pixel_avg_weight_w8_add_add_neon
    b pixel_avg_weight_w8_sub_add_neon
 # //.size x264_pixel_avg_8x8_neon, . - x264_pixel_avg_8x8_neon
        //.endfunc
        .text
        .align 2
        .globl _x264_pixel_avg_8x16_neon
 # //.type _x264_pixel_avg_8x16_neon, %function
        //.func _x264_pixel_avg_8x16_neon
_x264_pixel_avg_8x16_neon:
    mov w10, #64
    cmp w6, #32
    mov w9, #16
    b.eq pixel_avg_w8_neon
    subs w7, w10, w6
    b.lt pixel_avg_weight_w8_add_sub_neon
    cmp w6, #0
    b.ge pixel_avg_weight_w8_add_add_neon
    b pixel_avg_weight_w8_sub_add_neon
 # //.size x264_pixel_avg_8x16_neon, . - x264_pixel_avg_8x16_neon
        //.endfunc
        .text
        .align 2
        .globl _x264_pixel_avg_16x8_neon
 # //.type _x264_pixel_avg_16x8_neon, %function
        //.func _x264_pixel_avg_16x8_neon
_x264_pixel_avg_16x8_neon:
    mov w10, #64
    cmp w6, #32
    mov w9, #8
    b.eq pixel_avg_w16_neon
    subs w7, w10, w6
    b.lt pixel_avg_weight_w16_add_sub_neon
    cmp w6, #0
    b.ge pixel_avg_weight_w16_add_add_neon
    b pixel_avg_weight_w16_sub_add_neon
 # //.size x264_pixel_avg_16x8_neon, . - x264_pixel_avg_16x8_neon
        //.endfunc
        .text
        .align 2
        .globl _x264_pixel_avg_16x16_neon
 # //.type _x264_pixel_avg_16x16_neon, %function
        //.func _x264_pixel_avg_16x16_neon
_x264_pixel_avg_16x16_neon:
    mov w10, #64
    cmp w6, #32
    mov w9, #16
    b.eq pixel_avg_w16_neon
    subs w7, w10, w6
    b.lt pixel_avg_weight_w16_add_sub_neon
    cmp w6, #0
    b.ge pixel_avg_weight_w16_add_add_neon
    b pixel_avg_weight_w16_sub_add_neon
 # //.size x264_pixel_avg_16x16_neon, . - x264_pixel_avg_16x16_neon
        //.endfunc








        .text
        .align 2
 # //.type pixel_avg_weight_w4_add_add_neon, %function
        //.func pixel_avg_weight_w4_add_add_neon
pixel_avg_weight_w4_add_add_neon:
    mov w6, w6
    dup v30.8b, w6
    dup v31.8b, w7
1:
    subs w9, w9, #2
    ld1 {v0.s}[0], [x2], x3
    ld1 {v1.s}[0], [x4], x5
    umull v4.8h, v0.8b, v30.8b
    umlal v4.8h, v1.8b, v31.8b
    ld1 {v2.s}[0], [x2], x3
    ld1 {v3.s}[0], [x4], x5
    sqrshrun v0.8b, v4.8h, #6
    umull v5.8h, v2.8b, v30.8b
    umlal v5.8h, v3.8b, v31.8b
    st1 {v0.s}[0], [x0], x1
    sqrshrun v1.8b, v5.8h, #6
    st1 {v1.s}[0], [x0], x1
    b.gt 1b
    ret
 # //.size pixel_avg_weight_w4_add_add_neon, . - pixel_avg_weight_w4_add_add_neon
        //.endfunc

        .text
        .align 2
 # //.type pixel_avg_weight_w8_add_add_neon, %function
        //.func pixel_avg_weight_w8_add_add_neon
pixel_avg_weight_w8_add_add_neon:
    mov w6, w6
    dup v30.8b, w6
    dup v31.8b, w7
1:
    subs w9, w9, #4
    ld1 {v0.8b}, [x2], x3
    ld1 {v1.8b}, [x4], x5
    umull v16.8h, v0.8b, v30.8b
    umlal v16.8h, v1.8b, v31.8b
    ld1 {v2.8b}, [x2], x3
    ld1 {v3.8b}, [x4], x5
    umull v17.8h, v2.8b, v30.8b
    umlal v17.8h, v3.8b, v31.8b
    ld1 {v4.8b}, [x2], x3
    ld1 {v5.8b}, [x4], x5
    umull v18.8h, v4.8b, v30.8b
    umlal v18.8h, v5.8b, v31.8b
    ld1 {v6.8b}, [x2], x3
    ld1 {v7.8b}, [x4], x5
    umull v19.8h, v6.8b, v30.8b
    umlal v19.8h, v7.8b, v31.8b
    sqrshrun v0.8b, v16.8h, #6
    sqrshrun v1.8b, v17.8h, #6
    sqrshrun v2.8b, v18.8h, #6
    sqrshrun v3.8b, v19.8h, #6
    st1 {v0.8b}, [x0], x1
    st1 {v1.8b}, [x0], x1
    st1 {v2.8b}, [x0], x1
    st1 {v3.8b}, [x0], x1
    b.gt 1b
    ret
 # //.size pixel_avg_weight_w8_add_add_neon, . - pixel_avg_weight_w8_add_add_neon
        //.endfunc

        .text
        .align 2
 # //.type pixel_avg_weight_w16_add_add_neon, %function
        //.func pixel_avg_weight_w16_add_add_neon
pixel_avg_weight_w16_add_add_neon:
    mov w6, w6
    dup v30.16b, w6
    dup v31.16b, w7
1:
    subs w9, w9, #2
    ld1 {v0.16b}, [x2], x3
    ld1 {v1.16b}, [x4], x5
    umull v16.8h, v0.8b, v30.8b
    umlal v16.8h, v1.8b, v31.8b
    umull2 v17.8h, v0.16b, v30.16b
    umlal2 v17.8h, v1.16b, v31.16b
    ld1 {v2.16b}, [x2], x3
    ld1 {v3.16b}, [x4], x5
    umull v18.8h, v2.8b, v30.8b
    umlal v18.8h, v3.8b, v31.8b
    umull2 v19.8h, v2.16b, v30.16b
    umlal2 v19.8h, v3.16b, v31.16b
    sqrshrun v0.8b, v16.8h, #6
    sqrshrun v1.8b, v18.8h, #6
    sqrshrun2 v0.16b, v17.8h, #6
    sqrshrun2 v1.16b, v19.8h, #6
    st1 {v0.16b}, [x0], x1
    st1 {v1.16b}, [x0], x1
    b.gt 1b
    ret
 # //.size pixel_avg_weight_w16_add_add_neon, . - pixel_avg_weight_w16_add_add_neon
        //.endfunc
        .text
        .align 2
 # //.type pixel_avg_weight_w4_add_sub_neon, %function
        //.func pixel_avg_weight_w4_add_sub_neon
pixel_avg_weight_w4_add_sub_neon:
    neg w7, w7
    dup v30.8b, w6
    dup v31.8b, w7
1:
    subs w9, w9, #2
    ld1 {v0.s}[0], [x2], x3
    ld1 {v1.s}[0], [x4], x5
    umull v4.8h, v0.8b, v30.8b
    umlsl v4.8h, v1.8b, v31.8b
    ld1 {v2.s}[0], [x2], x3
    ld1 {v3.s}[0], [x4], x5
    sqrshrun v0.8b, v4.8h, #6
    umull v5.8h, v2.8b, v30.8b
    umlsl v5.8h, v3.8b, v31.8b
    st1 {v0.s}[0], [x0], x1
    sqrshrun v1.8b, v5.8h, #6
    st1 {v1.s}[0], [x0], x1
    b.gt 1b
    ret
 # //.size pixel_avg_weight_w4_add_sub_neon, . - pixel_avg_weight_w4_add_sub_neon
        //.endfunc

        .text
        .align 2
 # //.type pixel_avg_weight_w8_add_sub_neon, %function
        //.func pixel_avg_weight_w8_add_sub_neon
pixel_avg_weight_w8_add_sub_neon:
    neg w7, w7
    dup v30.8b, w6
    dup v31.8b, w7
1:
    subs w9, w9, #4
    ld1 {v0.8b}, [x2], x3
    ld1 {v1.8b}, [x4], x5
    umull v16.8h, v0.8b, v30.8b
    umlsl v16.8h, v1.8b, v31.8b
    ld1 {v2.8b}, [x2], x3
    ld1 {v3.8b}, [x4], x5
    umull v17.8h, v2.8b, v30.8b
    umlsl v17.8h, v3.8b, v31.8b
    ld1 {v4.8b}, [x2], x3
    ld1 {v5.8b}, [x4], x5
    umull v18.8h, v4.8b, v30.8b
    umlsl v18.8h, v5.8b, v31.8b
    ld1 {v6.8b}, [x2], x3
    ld1 {v7.8b}, [x4], x5
    umull v19.8h, v6.8b, v30.8b
    umlsl v19.8h, v7.8b, v31.8b
    sqrshrun v0.8b, v16.8h, #6
    sqrshrun v1.8b, v17.8h, #6
    sqrshrun v2.8b, v18.8h, #6
    sqrshrun v3.8b, v19.8h, #6
    st1 {v0.8b}, [x0], x1
    st1 {v1.8b}, [x0], x1
    st1 {v2.8b}, [x0], x1
    st1 {v3.8b}, [x0], x1
    b.gt 1b
    ret
 # //.size pixel_avg_weight_w8_add_sub_neon, . - pixel_avg_weight_w8_add_sub_neon
        //.endfunc

        .text
        .align 2
 # //.type pixel_avg_weight_w16_add_sub_neon, %function
        //.func pixel_avg_weight_w16_add_sub_neon
pixel_avg_weight_w16_add_sub_neon:
    neg w7, w7
    dup v30.16b, w6
    dup v31.16b, w7
1:
    subs w9, w9, #2
    ld1 {v0.16b}, [x2], x3
    ld1 {v1.16b}, [x4], x5
    umull v16.8h, v0.8b, v30.8b
    umlsl v16.8h, v1.8b, v31.8b
    umull2 v17.8h, v0.16b, v30.16b
    umlsl2 v17.8h, v1.16b, v31.16b
    ld1 {v2.16b}, [x2], x3
    ld1 {v3.16b}, [x4], x5
    umull v18.8h, v2.8b, v30.8b
    umlsl v18.8h, v3.8b, v31.8b
    umull2 v19.8h, v2.16b, v30.16b
    umlsl2 v19.8h, v3.16b, v31.16b
    sqrshrun v0.8b, v16.8h, #6
    sqrshrun v1.8b, v18.8h, #6
    sqrshrun2 v0.16b, v17.8h, #6
    sqrshrun2 v1.16b, v19.8h, #6
    st1 {v0.16b}, [x0], x1
    st1 {v1.16b}, [x0], x1
    b.gt 1b
    ret
 # //.size pixel_avg_weight_w16_add_sub_neon, . - pixel_avg_weight_w16_add_sub_neon
        //.endfunc
        .text
        .align 2
 # //.type pixel_avg_weight_w4_sub_add_neon, %function
        //.func pixel_avg_weight_w4_sub_add_neon
pixel_avg_weight_w4_sub_add_neon:
    neg w6, w6
    dup v30.8b, w6
    dup v31.8b, w7
1:
    subs w9, w9, #2
    ld1 {v0.s}[0], [x2], x3
    ld1 {v1.s}[0], [x4], x5
    umull v4.8h, v1.8b, v31.8b
    umlsl v4.8h, v0.8b, v30.8b
    ld1 {v2.s}[0], [x2], x3
    ld1 {v3.s}[0], [x4], x5
    sqrshrun v0.8b, v4.8h, #6
    umull v5.8h, v3.8b, v31.8b
    umlsl v5.8h, v2.8b, v30.8b
    st1 {v0.s}[0], [x0], x1
    sqrshrun v1.8b, v5.8h, #6
    st1 {v1.s}[0], [x0], x1
    b.gt 1b
    ret
 # //.size pixel_avg_weight_w4_sub_add_neon, . - pixel_avg_weight_w4_sub_add_neon
        //.endfunc

        .text
        .align 2
 # //.type pixel_avg_weight_w8_sub_add_neon, %function
        //.func pixel_avg_weight_w8_sub_add_neon
pixel_avg_weight_w8_sub_add_neon:
    neg w6, w6
    dup v30.8b, w6
    dup v31.8b, w7
1:
    subs w9, w9, #4
    ld1 {v0.8b}, [x2], x3
    ld1 {v1.8b}, [x4], x5
    umull v16.8h, v1.8b, v31.8b
    umlsl v16.8h, v0.8b, v30.8b
    ld1 {v2.8b}, [x2], x3
    ld1 {v3.8b}, [x4], x5
    umull v17.8h, v3.8b, v31.8b
    umlsl v17.8h, v2.8b, v30.8b
    ld1 {v4.8b}, [x2], x3
    ld1 {v5.8b}, [x4], x5
    umull v18.8h, v5.8b, v31.8b
    umlsl v18.8h, v4.8b, v30.8b
    ld1 {v6.8b}, [x2], x3
    ld1 {v7.8b}, [x4], x5
    umull v19.8h, v7.8b, v31.8b
    umlsl v19.8h, v6.8b, v30.8b
    sqrshrun v0.8b, v16.8h, #6
    sqrshrun v1.8b, v17.8h, #6
    sqrshrun v2.8b, v18.8h, #6
    sqrshrun v3.8b, v19.8h, #6
    st1 {v0.8b}, [x0], x1
    st1 {v1.8b}, [x0], x1
    st1 {v2.8b}, [x0], x1
    st1 {v3.8b}, [x0], x1
    b.gt 1b
    ret
 # //.size pixel_avg_weight_w8_sub_add_neon, . - pixel_avg_weight_w8_sub_add_neon
        //.endfunc

        .text
        .align 2
 # //.type pixel_avg_weight_w16_sub_add_neon, %function
        //.func pixel_avg_weight_w16_sub_add_neon
pixel_avg_weight_w16_sub_add_neon:
    neg w6, w6
    dup v30.16b, w6
    dup v31.16b, w7
1:
    subs w9, w9, #2
    ld1 {v0.16b}, [x2], x3
    ld1 {v1.16b}, [x4], x5
    umull v16.8h, v1.8b, v31.8b
    umlsl v16.8h, v0.8b, v30.8b
    umull2 v17.8h, v1.16b, v31.16b
    umlsl2 v17.8h, v0.16b, v30.16b
    ld1 {v2.16b}, [x2], x3
    ld1 {v3.16b}, [x4], x5
    umull v18.8h, v3.8b, v31.8b
    umlsl v18.8h, v2.8b, v30.8b
    umull2 v19.8h, v3.16b, v31.16b
    umlsl2 v19.8h, v2.16b, v30.16b
    sqrshrun v0.8b, v16.8h, #6
    sqrshrun v1.8b, v18.8h, #6
    sqrshrun2 v0.16b, v17.8h, #6
    sqrshrun2 v1.16b, v19.8h, #6
    st1 {v0.16b}, [x0], x1
    st1 {v1.16b}, [x0], x1
    b.gt 1b
    ret
 # //.size pixel_avg_weight_w16_sub_add_neon, . - pixel_avg_weight_w16_sub_add_neon
        //.endfunc

        .text
        .align 2
 # //.type pixel_avg_w4_neon, %function
        //.func pixel_avg_w4_neon
pixel_avg_w4_neon:
1: subs w9, w9, #2
    ld1 {v0.s}[0], [x2], x3
    ld1 {v2.s}[0], [x4], x5
    urhadd v0.8b, v0.8b, v2.8b
    ld1 {v1.s}[0], [x2], x3
    ld1 {v3.s}[0], [x4], x5
    urhadd v1.8b, v1.8b, v3.8b
    st1 {v0.s}[0], [x0], x1
    st1 {v1.s}[0], [x0], x1
    b.gt 1b
    ret
 # //.size pixel_avg_w4_neon, . - pixel_avg_w4_neon
        //.endfunc

        .text
        .align 2
 # //.type pixel_avg_w8_neon, %function
        //.func pixel_avg_w8_neon
pixel_avg_w8_neon:
1: subs w9, w9, #4
    ld1 {v0.8b}, [x2], x3
    ld1 {v1.8b}, [x4], x5
    ld1 {v2.8b}, [x2], x3
    urhadd v0.8b, v0.8b, v1.8b
    ld1 {v3.8b}, [x4], x5
    st1 {v0.8b}, [x0], x1
    ld1 {v4.8b}, [x2], x3
    urhadd v1.8b, v2.8b, v3.8b
    ld1 {v5.8b}, [x4], x5
    st1 {v1.8b}, [x0], x1
    ld1 {v6.8b}, [x2], x3
    ld1 {v7.8b}, [x4], x5
    urhadd v2.8b, v4.8b, v5.8b
    urhadd v3.8b, v6.8b, v7.8b
    st1 {v2.8b}, [x0], x1
    st1 {v3.8b}, [x0], x1
    b.gt 1b
    ret
 # //.size pixel_avg_w8_neon, . - pixel_avg_w8_neon
        //.endfunc

        .text
        .align 2
 # //.type pixel_avg_w16_neon, %function
        //.func pixel_avg_w16_neon
pixel_avg_w16_neon:
1: subs w9, w9, #4
    ld1 {v0.16b}, [x2], x3
    ld1 {v1.16b}, [x4], x5
    ld1 {v2.16b}, [x2], x3
    urhadd v0.16b, v0.16b, v1.16b
    ld1 {v3.16b}, [x4], x5
    st1 {v0.16b}, [x0], x1
    ld1 {v4.16b}, [x2], x3
    urhadd v1.16b, v2.16b, v3.16b
    ld1 {v5.16b}, [x4], x5
    st1 {v1.16b}, [x0], x1
    ld1 {v6.16b}, [x2], x3
    ld1 {v7.16b}, [x4], x5
    urhadd v2.16b, v4.16b, v5.16b
    urhadd v3.16b, v6.16b, v7.16b
    st1 {v2.16b}, [x0], x1
    st1 {v3.16b}, [x0], x1
    b.gt 1b
    ret
 # //.size pixel_avg_w16_neon, . - pixel_avg_w16_neon
        //.endfunc

        .text
        .align 2
        .globl _x264_pixel_avg2_w4_neon
 # //.type _x264_pixel_avg2_w4_neon, %function
        //.func _x264_pixel_avg2_w4_neon
_x264_pixel_avg2_w4_neon:
1:
    subs w5, w5, #2
    ld1 {v0.s}[0], [x2], x3
    ld1 {v2.s}[0], [x4], x3
    urhadd v0.8b, v0.8b, v2.8b
    ld1 {v1.s}[0], [x2], x3
    ld1 {v3.s}[0], [x4], x3
    urhadd v1.8b, v1.8b, v3.8b
    st1 {v0.s}[0], [x0], x1
    st1 {v1.s}[0], [x0], x1
    b.gt 1b
    ret
 # //.size x264_pixel_avg2_w4_neon, . - x264_pixel_avg2_w4_neon
        //.endfunc

        .text
        .align 2
        .globl _x264_pixel_avg2_w8_neon
 # //.type _x264_pixel_avg2_w8_neon, %function
        //.func _x264_pixel_avg2_w8_neon
_x264_pixel_avg2_w8_neon:
1:
    subs w5, w5, #2
    ld1 {v0.8b}, [x2], x3
    ld1 {v2.8b}, [x4], x3
    urhadd v0.8b, v0.8b, v2.8b
    ld1 {v1.8b}, [x2], x3
    ld1 {v3.8b}, [x4], x3
    urhadd v1.8b, v1.8b, v3.8b
    st1 {v0.8b}, [x0], x1
    st1 {v1.8b}, [x0], x1
    b.gt 1b
    ret
 # //.size x264_pixel_avg2_w8_neon, . - x264_pixel_avg2_w8_neon
        //.endfunc

        .text
        .align 2
        .globl _x264_pixel_avg2_w16_neon
 # //.type _x264_pixel_avg2_w16_neon, %function
        //.func _x264_pixel_avg2_w16_neon
_x264_pixel_avg2_w16_neon:
1:
    subs w5, w5, #2
    ld1 {v0.16b}, [x2], x3
    ld1 {v2.16b}, [x4], x3
    urhadd v0.16b, v0.16b, v2.16b
    ld1 {v1.16b}, [x2], x3
    ld1 {v3.16b}, [x4], x3
    urhadd v1.16b, v1.16b, v3.16b
    st1 {v0.16b}, [x0], x1
    st1 {v1.16b}, [x0], x1
    b.gt 1b
    ret
 # //.size x264_pixel_avg2_w16_neon, . - x264_pixel_avg2_w16_neon
        //.endfunc

        .text
        .align 2
        .globl _x264_pixel_avg2_w20_neon
 # //.type _x264_pixel_avg2_w20_neon, %function
        //.func _x264_pixel_avg2_w20_neon
_x264_pixel_avg2_w20_neon:
    sub x1, x1, #16
1:
    subs w5, w5, #2
    ld1 {v0.16b,v1.16b}, [x2], x3
    ld1 {v2.16b,v3.16b}, [x4], x3
    urhadd v0.16b, v0.16b, v2.16b
    urhadd v1.8b, v1.8b, v3.8b
    ld1 {v4.16b,v5.16b}, [x2], x3
    ld1 {v6.16b,v7.16b}, [x4], x3
    urhadd v4.16b, v4.16b, v6.16b
    urhadd v5.8b, v5.8b, v7.8b
    st1 {v0.16b}, [x0], #16
    st1 {v1.s}[0], [x0], x1
    st1 {v4.16b}, [x0], #16
    st1 {v5.s}[0], [x0], x1
    b.gt 1b
    ret
 # //.size x264_pixel_avg2_w20_neon, . - x264_pixel_avg2_w20_neon
        //.endfunc




        .text
        .align 2
        .globl _x264_mc_weight_w20_neon
 # //.type _x264_mc_weight_w20_neon, %function
        //.func _x264_mc_weight_w20_neon
_x264_mc_weight_w20_neon:
    mov w9, w5
    ldr w12, [x4, #32]
    ldp w4, w5, [x4, #32+4]
    dup v0.16b, w4
    dup v1.8h, w5
    neg w12, w12
    dup v2.8h, w12
    sub x1, x1, #16
1:
    subs w9, w9, #2
    ld1 {v16.8b,v17.8b,v18.8b}, [x2], x3
    ld1 {v19.8b,v20.8b,v21.8b}, [x2], x3
    umull v22.8h, v16.8b, v0.8b
    umull v23.8h, v17.8b, v0.8b
    zip1 v18.2s, v18.2s, v21.2s
    umull v25.8h, v19.8b, v0.8b
    umull v26.8h, v20.8b, v0.8b
    umull v24.8h, v18.8b, v0.8b
    srshl v22.8h, v22.8h, v2.8h
    srshl v23.8h, v23.8h, v2.8h
    srshl v24.8h, v24.8h, v2.8h
    srshl v25.8h, v25.8h, v2.8h
    srshl v26.8h, v26.8h, v2.8h
    add v22.8h, v22.8h, v1.8h
    add v23.8h, v23.8h, v1.8h
    add v24.8h, v24.8h, v1.8h
    add v25.8h, v25.8h, v1.8h
    add v26.8h, v26.8h, v1.8h
    sqxtun v4.8b, v22.8h
    sqxtun2 v4.16b, v23.8h
    sqxtun v6.8b, v24.8h
    sqxtun v5.8b, v25.8h
    sqxtun2 v5.16b, v26.8h
    st1 {v4.16b}, [x0], #16
    st1 {v6.s}[0], [x0], x1
    st1 {v5.16b}, [x0], #16
    st1 {v6.s}[1], [x0], x1
    b.gt 1b
    ret
 # //.size x264_mc_weight_w20_neon, . - x264_mc_weight_w20_neon
        //.endfunc

        .text
        .align 2
        .globl _x264_mc_weight_w16_neon
 # //.type _x264_mc_weight_w16_neon, %function
        //.func _x264_mc_weight_w16_neon
_x264_mc_weight_w16_neon:
    mov w9, w5
    ldr w12, [x4, #32]
    ldp w4, w5, [x4, #32+4]
    dup v0.16b, w4
    dup v1.8h, w5
    neg w12, w12
    dup v2.8h, w12
weight16_loop:
1:
    subs w9, w9, #2
    ld1 {v4.16b}, [x2], x3
    ld1 {v5.16b}, [x2], x3
    umull v22.8h, v4.8b, v0.8b
    umull2 v23.8h, v4.16b, v0.16b
    umull v24.8h, v5.8b, v0.8b
    umull2 v25.8h, v5.16b, v0.16b
    srshl v22.8h, v22.8h, v2.8h
    srshl v23.8h, v23.8h, v2.8h
    srshl v24.8h, v24.8h, v2.8h
    srshl v25.8h, v25.8h, v2.8h
    add v22.8h, v22.8h, v1.8h
    add v23.8h, v23.8h, v1.8h
    add v24.8h, v24.8h, v1.8h
    add v25.8h, v25.8h, v1.8h
    sqxtun v4.8b, v22.8h
    sqxtun2 v4.16b, v23.8h
    sqxtun v5.8b, v24.8h
    sqxtun2 v5.16b, v25.8h
    st1 {v4.16b}, [x0], x1
    st1 {v5.16b}, [x0], x1
    b.gt 1b
    ret
 # //.size x264_mc_weight_w16_neon, . - x264_mc_weight_w16_neon
        //.endfunc

        .text
        .align 2
        .globl _x264_mc_weight_w8_neon
 # //.type _x264_mc_weight_w8_neon, %function
        //.func _x264_mc_weight_w8_neon
_x264_mc_weight_w8_neon:
    mov w9, w5
    ldr w12, [x4, #32]
    ldp w4, w5, [x4, #32+4]
    dup v0.16b, w4
    dup v1.8h, w5
    neg w12, w12
    dup v2.8h, w12
1:
    subs w9, w9, #2
    ld1 {v16.8b}, [x2], x3
    ld1 {v17.8b}, [x2], x3
    umull v4.8h, v16.8b, v0.8b
    umull v5.8h, v17.8b, v0.8b
    srshl v4.8h, v4.8h, v2.8h
    srshl v5.8h, v5.8h, v2.8h
    add v4.8h, v4.8h, v1.8h
    add v5.8h, v5.8h, v1.8h
    sqxtun v16.8b, v4.8h
    sqxtun v17.8b, v5.8h
    st1 {v16.8b}, [x0], x1
    st1 {v17.8b}, [x0], x1
    b.gt 1b
    ret
 # //.size x264_mc_weight_w8_neon, . - x264_mc_weight_w8_neon
        //.endfunc

        .text
        .align 2
        .globl _x264_mc_weight_w4_neon
 # //.type _x264_mc_weight_w4_neon, %function
        //.func _x264_mc_weight_w4_neon
_x264_mc_weight_w4_neon:
    mov w9, w5
    ldr w12, [x4, #32]
    ldp w4, w5, [x4, #32+4]
    dup v0.16b, w4
    dup v1.8h, w5
    neg w12, w12
    dup v2.8h, w12
1:
    subs w9, w9, #2
    ld1 {v16.s}[0], [x2], x3
    ld1 {v16.s}[1], [x2], x3
    umull v4.8h, v16.8b, v0.8b
    srshl v4.8h, v4.8h, v2.8h
    add v4.8h, v4.8h, v1.8h
    sqxtun v16.8b, v4.8h
    st1 {v16.s}[0], [x0], x1
    st1 {v16.s}[1], [x0], x1
    b.gt 1b
    ret
 # //.size x264_mc_weight_w4_neon, . - x264_mc_weight_w4_neon
        //.endfunc

        .text
        .align 2
        .globl _x264_mc_weight_w20_nodenom_neon
 # //.type _x264_mc_weight_w20_nodenom_neon, %function
        //.func _x264_mc_weight_w20_nodenom_neon
_x264_mc_weight_w20_nodenom_neon:
    mov w9, w5
    ldp w4, w5, [x4, #32+4]
    dup v0.16b, w4
    dup v1.8h, w5
    sub x1, x1, #16
1:
    subs w9, w9, #2
    ld1 {v16.8b,v17.8b,v18.8b}, [x2], x3
        orr v27.16b, v1.16b, v1.16b
        orr v28.16b, v1.16b, v1.16b
    ld1 {v19.8b,v20.8b,v21.8b}, [x2], x3
        orr v31.16b, v1.16b, v1.16b
        orr v29.16b, v1.16b, v1.16b
        orr v30.16b, v1.16b, v1.16b
    zip1 v18.2s, v18.2s, v21.2s
    umlal v27.8h, v16.8b, v0.8b
    umlal v28.8h, v17.8b, v0.8b
    umlal v31.8h, v18.8b, v0.8b
    umlal v29.8h, v19.8b, v0.8b
    umlal v30.8h, v20.8b, v0.8b
    sqxtun v4.8b, v27.8h
    sqxtun2 v4.16b, v28.8h
    sqxtun v5.8b, v29.8h
    sqxtun2 v5.16b, v30.8h
    sqxtun v6.8b, v31.8h
    st1 {v4.16b}, [x0], #16
    st1 {v6.s}[0], [x0], x1
    st1 {v5.16b}, [x0], #16
    st1 {v6.s}[1], [x0], x1
    b.gt 1b
    ret
 # //.size x264_mc_weight_w20_nodenom_neon, . - x264_mc_weight_w20_nodenom_neon
        //.endfunc

        .text
        .align 2
        .globl _x264_mc_weight_w16_nodenom_neon
 # //.type _x264_mc_weight_w16_nodenom_neon, %function
        //.func _x264_mc_weight_w16_nodenom_neon
_x264_mc_weight_w16_nodenom_neon:
    mov w9, w5
    ldp w4, w5, [x4, #32+4]
    dup v0.16b, w4
    dup v1.8h, w5
1:
    subs w9, w9, #2
    ld1 {v6.16b}, [x2], x3
        orr v27.16b, v1.16b, v1.16b
        orr v28.16b, v1.16b, v1.16b
    ld1 {v7.16b}, [x2], x3
        orr v29.16b, v1.16b, v1.16b
        orr v30.16b, v1.16b, v1.16b
    umlal v27.8h, v6.8b, v0.8b
    umlal2 v28.8h, v6.16b, v0.16b
    umlal v29.8h, v7.8b, v0.8b
    umlal2 v30.8h, v7.16b, v0.16b
    sqxtun v4.8b, v27.8h
    sqxtun2 v4.16b, v28.8h
    sqxtun v5.8b, v29.8h
    sqxtun2 v5.16b, v30.8h
    st1 {v4.16b}, [x0], x1
    st1 {v5.16b}, [x0], x1
    b.gt 1b
    ret
 # //.size x264_mc_weight_w16_nodenom_neon, . - x264_mc_weight_w16_nodenom_neon
        //.endfunc

        .text
        .align 2
        .globl _x264_mc_weight_w8_nodenom_neon
 # //.type _x264_mc_weight_w8_nodenom_neon, %function
        //.func _x264_mc_weight_w8_nodenom_neon
_x264_mc_weight_w8_nodenom_neon:
    mov w9, w5
    ldp w4, w5, [x4, #32+4]
    dup v0.16b, w4
    dup v1.8h, w5
1:
    subs w9, w9, #2
    ld1 {v16.8b}, [x2], x3
        orr v27.16b, v1.16b, v1.16b
    ld1 {v17.8b}, [x2], x3
        orr v29.16b, v1.16b, v1.16b
    umlal v27.8h, v16.8b, v0.8b
    umlal v29.8h, v17.8b, v0.8b
    sqxtun v4.8b, v27.8h
    sqxtun v5.8b, v29.8h
    st1 {v4.8b}, [x0], x1
    st1 {v5.8b}, [x0], x1
    b.gt 1b
    ret
 # //.size x264_mc_weight_w8_nodenom_neon, . - x264_mc_weight_w8_nodenom_neon
        //.endfunc

        .text
        .align 2
        .globl _x264_mc_weight_w4_nodenom_neon
 # //.type _x264_mc_weight_w4_nodenom_neon, %function
        //.func _x264_mc_weight_w4_nodenom_neon
_x264_mc_weight_w4_nodenom_neon:
    mov w9, w5
    ldp w4, w5, [x4, #32+4]
    dup v0.16b, w4
    dup v1.8h, w5
1:
    subs w9, w9, #2
    ld1 {v16.s}[0], [x2], x3
    ld1 {v16.s}[1], [x2], x3
        orr v27.16b, v1.16b, v1.16b
    umlal v27.8h, v16.8b, v0.8b
    sqxtun v4.8b, v27.8h
    st1 {v4.s}[0], [x0], x1
    st1 {v4.s}[1], [x0], x1
    b.gt 1b
    ret
 # //.size x264_mc_weight_w4_nodenom_neon, . - x264_mc_weight_w4_nodenom_neon
        //.endfunc



        .text
        .align 2
        .globl _x264_mc_weight_w20_offsetadd_neon
 # //.type _x264_mc_weight_w20_offsetadd_neon, %function
        //.func _x264_mc_weight_w20_offsetadd_neon
_x264_mc_weight_w20_offsetadd_neon:
    ldr w6, [x4]
    dup v1.16b, w6
1:
    subs w5, w5, #2
    ldr s18, [x2, #16]
    ld1 {v16.16b}, [x2], x3
    ldr s19, [x2, #16]
    ld1 {v17.16b}, [x2], x3
    uqadd v18.8b, v18.8b, v1.8b
    uqadd v16.16b, v16.16b, v1.16b
    uqadd v19.8b, v19.8b, v1.8b
    uqadd v17.16b, v17.16b, v1.16b
    str s18, [x0, #16]
    st1 {v16.16b}, [x0], x1
    str s19, [x0, #16]
    st1 {v17.16b}, [x0], x1
    b.gt 1b
    ret
 # //.size x264_mc_weight_w20_offsetadd_neon, . - x264_mc_weight_w20_offsetadd_neon
        //.endfunc

        .text
        .align 2
        .globl _x264_mc_weight_w16_offsetadd_neon
 # //.type _x264_mc_weight_w16_offsetadd_neon, %function
        //.func _x264_mc_weight_w16_offsetadd_neon
_x264_mc_weight_w16_offsetadd_neon:
    ldr w6, [x4]
    dup v1.16b, w6
1:
    subs w5, w5, #2
    ld1 {v16.16b}, [x2], x3
    ld1 {v17.16b}, [x2], x3
    uqadd v16.16b, v16.16b, v1.16b
    uqadd v17.16b, v17.16b, v1.16b
    st1 {v16.16b}, [x0], x1
    st1 {v17.16b}, [x0], x1
    b.gt 1b
    ret
 # //.size x264_mc_weight_w16_offsetadd_neon, . - x264_mc_weight_w16_offsetadd_neon
        //.endfunc

        .text
        .align 2
        .globl _x264_mc_weight_w8_offsetadd_neon
 # //.type _x264_mc_weight_w8_offsetadd_neon, %function
        //.func _x264_mc_weight_w8_offsetadd_neon
_x264_mc_weight_w8_offsetadd_neon:
    ldr w6, [x4]
    dup v1.16b, w6
1:
    subs w5, w5, #2
    ld1 {v16.8b}, [x2], x3
    ld1 {v17.8b}, [x2], x3
    uqadd v16.8b, v16.8b, v1.8b
    uqadd v17.8b, v17.8b, v1.8b
    st1 {v16.8b}, [x0], x1
    st1 {v17.8b}, [x0], x1
    b.gt 1b
    ret
 # //.size x264_mc_weight_w8_offsetadd_neon, . - x264_mc_weight_w8_offsetadd_neon
        //.endfunc

        .text
        .align 2
        .globl _x264_mc_weight_w4_offsetadd_neon
 # //.type _x264_mc_weight_w4_offsetadd_neon, %function
        //.func _x264_mc_weight_w4_offsetadd_neon
_x264_mc_weight_w4_offsetadd_neon:
    ldr w6, [x4]
    dup v1.16b, w6
1:
    subs w5, w5, #2
    ld1 {v16.s}[0], [x2], x3
    ld1 {v16.s}[1], [x2], x3
    uqadd v16.8b, v16.8b, v1.8b
    st1 {v16.s}[0], [x0], x1
    st1 {v16.s}[1], [x0], x1
    b.gt 1b
    ret
 # //.size x264_mc_weight_w4_offsetadd_neon, . - x264_mc_weight_w4_offsetadd_neon
        //.endfunc
        .text
        .align 2
        .globl _x264_mc_weight_w20_offsetsub_neon
 # //.type _x264_mc_weight_w20_offsetsub_neon, %function
        //.func _x264_mc_weight_w20_offsetsub_neon
_x264_mc_weight_w20_offsetsub_neon:
    ldr w6, [x4]
    dup v1.16b, w6
1:
    subs w5, w5, #2
    ldr s18, [x2, #16]
    ld1 {v16.16b}, [x2], x3
    ldr s19, [x2, #16]
    ld1 {v17.16b}, [x2], x3
    uqsub v18.8b, v18.8b, v1.8b
    uqsub v16.16b, v16.16b, v1.16b
    uqsub v19.8b, v19.8b, v1.8b
    uqsub v17.16b, v17.16b, v1.16b
    str s18, [x0, #16]
    st1 {v16.16b}, [x0], x1
    str s19, [x0, #16]
    st1 {v17.16b}, [x0], x1
    b.gt 1b
    ret
 # //.size x264_mc_weight_w20_offsetsub_neon, . - x264_mc_weight_w20_offsetsub_neon
        //.endfunc

        .text
        .align 2
        .globl _x264_mc_weight_w16_offsetsub_neon
 # //.type _x264_mc_weight_w16_offsetsub_neon, %function
        //.func _x264_mc_weight_w16_offsetsub_neon
_x264_mc_weight_w16_offsetsub_neon:
    ldr w6, [x4]
    dup v1.16b, w6
1:
    subs w5, w5, #2
    ld1 {v16.16b}, [x2], x3
    ld1 {v17.16b}, [x2], x3
    uqsub v16.16b, v16.16b, v1.16b
    uqsub v17.16b, v17.16b, v1.16b
    st1 {v16.16b}, [x0], x1
    st1 {v17.16b}, [x0], x1
    b.gt 1b
    ret
 # //.size x264_mc_weight_w16_offsetsub_neon, . - x264_mc_weight_w16_offsetsub_neon
        //.endfunc

        .text
        .align 2
        .globl _x264_mc_weight_w8_offsetsub_neon
 # //.type _x264_mc_weight_w8_offsetsub_neon, %function
        //.func _x264_mc_weight_w8_offsetsub_neon
_x264_mc_weight_w8_offsetsub_neon:
    ldr w6, [x4]
    dup v1.16b, w6
1:
    subs w5, w5, #2
    ld1 {v16.8b}, [x2], x3
    ld1 {v17.8b}, [x2], x3
    uqsub v16.8b, v16.8b, v1.8b
    uqsub v17.8b, v17.8b, v1.8b
    st1 {v16.8b}, [x0], x1
    st1 {v17.8b}, [x0], x1
    b.gt 1b
    ret
 # //.size x264_mc_weight_w8_offsetsub_neon, . - x264_mc_weight_w8_offsetsub_neon
        //.endfunc

        .text
        .align 2
        .globl _x264_mc_weight_w4_offsetsub_neon
 # //.type _x264_mc_weight_w4_offsetsub_neon, %function
        //.func _x264_mc_weight_w4_offsetsub_neon
_x264_mc_weight_w4_offsetsub_neon:
    ldr w6, [x4]
    dup v1.16b, w6
1:
    subs w5, w5, #2
    ld1 {v16.s}[0], [x2], x3
    ld1 {v16.s}[1], [x2], x3
    uqsub v16.8b, v16.8b, v1.8b
    st1 {v16.s}[0], [x0], x1
    st1 {v16.s}[1], [x0], x1
    b.gt 1b
    ret
 # //.size x264_mc_weight_w4_offsetsub_neon, . - x264_mc_weight_w4_offsetsub_neon
        //.endfunc



        .text
        .align 2
        .globl _x264_mc_copy_w4_neon
 # //.type _x264_mc_copy_w4_neon, %function
        //.func _x264_mc_copy_w4_neon
_x264_mc_copy_w4_neon:
1:
    subs w4, w4, #4
    ld1 {v0.s}[0], [x2], x3
    ld1 {v1.s}[0], [x2], x3
    ld1 {v2.s}[0], [x2], x3
    ld1 {v3.s}[0], [x2], x3
    st1 {v0.s}[0], [x0], x1
    st1 {v1.s}[0], [x0], x1
    st1 {v2.s}[0], [x0], x1
    st1 {v3.s}[0], [x0], x1
    b.gt 1b
    ret
 # //.size x264_mc_copy_w4_neon, . - x264_mc_copy_w4_neon
        //.endfunc

        .text
        .align 2
        .globl _x264_mc_copy_w8_neon
 # //.type _x264_mc_copy_w8_neon, %function
        //.func _x264_mc_copy_w8_neon
_x264_mc_copy_w8_neon:
1: subs w4, w4, #4
    ld1 {v0.8b}, [x2], x3
    ld1 {v1.8b}, [x2], x3
    ld1 {v2.8b}, [x2], x3
    ld1 {v3.8b}, [x2], x3
    st1 {v0.8b}, [x0], x1
    st1 {v1.8b}, [x0], x1
    st1 {v2.8b}, [x0], x1
    st1 {v3.8b}, [x0], x1
    b.gt 1b
    ret
 # //.size x264_mc_copy_w8_neon, . - x264_mc_copy_w8_neon
        //.endfunc

        .text
        .align 2
        .globl _x264_mc_copy_w16_neon
 # //.type _x264_mc_copy_w16_neon, %function
        //.func _x264_mc_copy_w16_neon
_x264_mc_copy_w16_neon:
1: subs w4, w4, #4
    ld1 {v0.16b}, [x2], x3
    ld1 {v1.16b}, [x2], x3
    ld1 {v2.16b}, [x2], x3
    ld1 {v3.16b}, [x2], x3
    st1 {v0.16b}, [x0], x1
    st1 {v1.16b}, [x0], x1
    st1 {v2.16b}, [x0], x1
    st1 {v3.16b}, [x0], x1
    b.gt 1b
    ret
 # //.size x264_mc_copy_w16_neon, . - x264_mc_copy_w16_neon
        //.endfunc





        .text
        .align 2
        .globl _x264_mc_chroma_neon
 # //.type _x264_mc_chroma_neon, %function
        //.func _x264_mc_chroma_neon
_x264_mc_chroma_neon:
    ldr w15, [sp]
    sbfx x12, x6, #3, #29
    sbfx x11, x5, #3, #29
    cmp w7, #4
    mul x12, x12, x4
    add x3, x3, x11, lsl #1

    and w5, w5, #7
    and w6, w6, #7

    add x3, x3, x12




    b.gt mc_chroma_w8_neon
    b.eq mc_chroma_w4_neon
 # //.size x264_mc_chroma_neon, . - x264_mc_chroma_neon
        //.endfunc



        .text
        .align 2
 # //.type mc_chroma_w2_neon, %function
        //.func mc_chroma_w2_neon
mc_chroma_w2_neon:

    .set st2, 2
    mul w12, w5, w6
    lsl w13, w5, #3
    add w9, w12, #64
    lsl w14, w6, #3
    tst w12, w12
    sub w9, w9, w13
    sub w10, w13, w12
    sub w11, w14, w12
    sub w9, w9, w14
    b.eq 2f

    ld2 {v28.8b,v29.8b}, [x3], x4
    dup v0.8b, w9
    dup v1.8b, w10

    ext v6.8b, v28.8b, v6.8b, #1
    ext v7.8b, v29.8b, v7.8b, #1

    ld2 {v30.8b,v31.8b}, [x3], x4
    dup v2.8b, w11
    dup v3.8b, w12

    ext v22.8b, v30.8b, v22.8b, #1
    ext v23.8b, v31.8b, v23.8b, #1

    trn1 v0.2s, v0.2s, v1.2s
    trn1 v2.2s, v2.2s, v3.2s

    trn1 v4.2s, v28.2s, v6.2s
    trn1 v5.2s, v29.2s, v7.2s
    trn1 v20.2s, v30.2s, v22.2s
    trn1 v21.2s, v31.2s, v23.2s
1:
    subs w15, w15, #2
    umull v16.8h, v4.8b, v0.8b
    umlal v16.8h, v20.8b, v2.8b
    umull v17.8h, v5.8b, v0.8b
    umlal v17.8h, v21.8b, v2.8b

    ld2 {v28.8b,v29.8b}, [x3], x4
    trn1 v24.2d, v16.2d, v17.2d
    trn2 v25.2d, v16.2d, v17.2d

    ext v6.8b, v28.8b, v6.8b, #1
    ext v7.8b, v29.8b, v7.8b, #1

    trn1 v4.2s, v28.2s, v6.2s
    trn1 v5.2s, v29.2s, v7.2s

    add v16.8h, v24.8h, v25.8h

    umull v18.8h, v20.8b, v0.8b
    umlal v18.8h, v4.8b, v2.8b
    umull v19.8h, v21.8b, v0.8b
    umlal v19.8h, v5.8b, v2.8b

    ld2 {v30.8b,v31.8b}, [x3], x4
    trn1 v26.2d, v18.2d, v19.2d
    trn2 v27.2d, v18.2d, v19.2d

    ext v22.8b, v30.8b, v22.8b, #1
    ext v23.8b, v31.8b, v23.8b, #1
    trn1 v20.2s, v30.2s, v22.2s
    trn1 v21.2s, v31.2s, v23.2s

    add v17.8h, v26.8h, v27.8h

    rshrn v16.8b, v16.8h, #6
    rshrn v17.8b, v17.8h, #6




    st1 {v16.h}[0], [x0], x2
    st1 {v16.h}[st2], [x1], x2
    st1 {v17.h}[0], [x0], x2
    st1 {v17.h}[st2], [x1], x2
    b.gt 1b

    ret
2:
    tst w11, w11
    add w10, w10, w11
    dup v0.8b, w9
    dup v1.8b, w10

    b.eq 4f

    ld1 {v4.8b}, [x3], x4
    ld1 {v6.8b}, [x3], x4
3:
    subs w15, w15, #2
    umull v16.8h, v4.8b, v0.8b
    ld1 {v4.8b}, [x3], x4
    umlal v16.8h, v6.8b, v1.8b
    umull v17.8h, v6.8b, v0.8b
    ld1 {v6.8b}, [x3], x4
    umlal v17.8h, v4.8b, v1.8b

    rshrn v20.8b, v16.8h, #6
    rshrn v21.8b, v17.8h, #6

    uzp1 v16.8b, v20.8b, v21.8b
    uzp2 v17.8b, v20.8b, v21.8b




    st1 {v16.h}[0], [x0], x2
    st1 {v16.h}[st2], [x0], x2
    st1 {v17.h}[0], [x1], x2
    st1 {v17.h}[st2], [x1], x2
    b.gt 3b

    ret

4:
    ld1 {v4.8b,v5.8b}, [x3], x4
    ld1 {v6.8b,v7.8b}, [x3], x4

    ext v5.8b, v4.8b, v5.8b, #2
    ext v7.8b, v6.8b, v7.8b, #2
5:
    subs w15, w15, #2
    umull v16.8h, v4.8b, v0.8b
    umlal v16.8h, v5.8b, v1.8b
    umull v17.8h, v6.8b, v0.8b
    umlal v17.8h, v7.8b, v1.8b

    ld1 {v4.8b,v5.8b}, [x3], x4
    ld1 {v6.8b,v7.8b}, [x3], x4
    rshrn v20.8b, v16.8h, #6
    rshrn v21.8b, v17.8h, #6
    ext v5.8b, v4.8b, v5.8b, #2
    ext v7.8b, v6.8b, v7.8b, #2
    uzp1 v16.8b, v20.8b, v21.8b
    uzp2 v17.8b, v20.8b, v21.8b




    st1 {v16.h}[0], [x0], x2
    st1 {v16.h}[st2], [x0], x2
    st1 {v17.h}[0], [x1], x2
    st1 {v17.h}[st2], [x1], x2
    b.gt 5b

    ret
 # //.size mc_chroma_w2_neon, . - mc_chroma_w2_neon
        //.endfunc
        .text
        .align 2
 # //.type mc_chroma_w4_neon, %function
        //.func mc_chroma_w4_neon
mc_chroma_w4_neon:

    .set st2, 1
    mul w12, w5, w6
    lsl w13, w5, #3
    add w9, w12, #64
    lsl w14, w6, #3
    tst w12, w12
    sub w9, w9, w13
    sub w10, w13, w12
    sub w11, w14, w12
    sub w9, w9, w14
    b.eq 2f

    ld2 {v28.8b,v29.8b}, [x3], x4
    dup v0.8b, w9
    dup v1.8b, w10

    ext v6.8b, v28.8b, v6.8b, #1
    ext v7.8b, v29.8b, v7.8b, #1

    ld2 {v30.8b,v31.8b}, [x3], x4
    dup v2.8b, w11
    dup v3.8b, w12

    ext v22.8b, v30.8b, v22.8b, #1
    ext v23.8b, v31.8b, v23.8b, #1

    trn1 v0.2s, v0.2s, v1.2s
    trn1 v2.2s, v2.2s, v3.2s

    trn1 v4.2s, v28.2s, v6.2s
    trn1 v5.2s, v29.2s, v7.2s
    trn1 v20.2s, v30.2s, v22.2s
    trn1 v21.2s, v31.2s, v23.2s
1:
    subs w15, w15, #2
    umull v16.8h, v4.8b, v0.8b
    umlal v16.8h, v20.8b, v2.8b
    umull v17.8h, v5.8b, v0.8b
    umlal v17.8h, v21.8b, v2.8b

    ld2 {v28.8b,v29.8b}, [x3], x4
    trn1 v24.2d, v16.2d, v17.2d
    trn2 v25.2d, v16.2d, v17.2d

    ext v6.8b, v28.8b, v6.8b, #1
    ext v7.8b, v29.8b, v7.8b, #1

    trn1 v4.2s, v28.2s, v6.2s
    trn1 v5.2s, v29.2s, v7.2s

    add v16.8h, v24.8h, v25.8h

    umull v18.8h, v20.8b, v0.8b
    umlal v18.8h, v4.8b, v2.8b
    umull v19.8h, v21.8b, v0.8b
    umlal v19.8h, v5.8b, v2.8b

    ld2 {v30.8b,v31.8b}, [x3], x4
    trn1 v26.2d, v18.2d, v19.2d
    trn2 v27.2d, v18.2d, v19.2d

    ext v22.8b, v30.8b, v22.8b, #1
    ext v23.8b, v31.8b, v23.8b, #1
    trn1 v20.2s, v30.2s, v22.2s
    trn1 v21.2s, v31.2s, v23.2s

    add v17.8h, v26.8h, v27.8h

    rshrn v16.8b, v16.8h, #6
    rshrn v17.8b, v17.8h, #6




    st1 {v16.s}[0], [x0], x2
    st1 {v16.s}[st2], [x1], x2
    st1 {v17.s}[0], [x0], x2
    st1 {v17.s}[st2], [x1], x2
    b.gt 1b

    ret
2:
    tst w11, w11
    add w10, w10, w11
    dup v0.8b, w9
    dup v1.8b, w10

    b.eq 4f

    ld1 {v4.8b}, [x3], x4
    ld1 {v6.8b}, [x3], x4
3:
    subs w15, w15, #2
    umull v16.8h, v4.8b, v0.8b
    ld1 {v4.8b}, [x3], x4
    umlal v16.8h, v6.8b, v1.8b
    umull v17.8h, v6.8b, v0.8b
    ld1 {v6.8b}, [x3], x4
    umlal v17.8h, v4.8b, v1.8b

    rshrn v20.8b, v16.8h, #6
    rshrn v21.8b, v17.8h, #6

    uzp1 v16.8b, v20.8b, v21.8b
    uzp2 v17.8b, v20.8b, v21.8b




    st1 {v16.s}[0], [x0], x2
    st1 {v16.s}[st2], [x0], x2
    st1 {v17.s}[0], [x1], x2
    st1 {v17.s}[st2], [x1], x2
    b.gt 3b

    ret

4:
    ld1 {v4.8b,v5.8b}, [x3], x4
    ld1 {v6.8b,v7.8b}, [x3], x4

    ext v5.8b, v4.8b, v5.8b, #2
    ext v7.8b, v6.8b, v7.8b, #2
5:
    subs w15, w15, #2
    umull v16.8h, v4.8b, v0.8b
    umlal v16.8h, v5.8b, v1.8b
    umull v17.8h, v6.8b, v0.8b
    umlal v17.8h, v7.8b, v1.8b

    ld1 {v4.8b,v5.8b}, [x3], x4
    ld1 {v6.8b,v7.8b}, [x3], x4
    rshrn v20.8b, v16.8h, #6
    rshrn v21.8b, v17.8h, #6
    ext v5.8b, v4.8b, v5.8b, #2
    ext v7.8b, v6.8b, v7.8b, #2
    uzp1 v16.8b, v20.8b, v21.8b
    uzp2 v17.8b, v20.8b, v21.8b




    st1 {v16.s}[0], [x0], x2
    st1 {v16.s}[st2], [x0], x2
    st1 {v17.s}[0], [x1], x2
    st1 {v17.s}[st2], [x1], x2
    b.gt 5b

    ret
 # //.size mc_chroma_w4_neon, . - mc_chroma_w4_neon
        //.endfunc

        .text
        .align 2
 # //.type mc_chroma_w8_neon, %function
        //.func mc_chroma_w8_neon
mc_chroma_w8_neon:
    mul w12, w5, w6
    lsl w13, w5, #3
    add w9, w12, #64
    lsl w14, w6, #3
    tst w12, w12
    sub w9, w9, w13
    sub w10, w13, w12
    sub w11, w14, w12
    sub w9, w9, w14
    b.eq 2f
    ld2 {v4.16b,v5.16b}, [x3], x4
    ld2 {v20.16b,v21.16b}, [x3], x4
    dup v0.8b, w9
    dup v1.8b, w10

    ext v6.16b, v4.16b, v4.16b, #1
    ext v7.16b, v5.16b, v5.16b, #1

    dup v2.8b, w11
    dup v3.8b, w12

    ext v22.16b, v20.16b, v20.16b, #1
    ext v23.16b, v21.16b, v21.16b, #1

1:
    subs w15, w15, #2
    umull v16.8h, v4.8b, v0.8b
    umlal v16.8h, v6.8b, v1.8b
    umlal v16.8h, v20.8b, v2.8b
    umlal v16.8h, v22.8b, v3.8b

    umull v17.8h, v5.8b, v0.8b
    umlal v17.8h, v7.8b, v1.8b
    umlal v17.8h, v21.8b, v2.8b
    umlal v17.8h, v23.8b, v3.8b

    ld2 {v4.16b,v5.16b}, [x3], x4

    ext v6.16b, v4.16b, v4.16b, #1
    ext v7.16b, v5.16b, v5.16b, #1

    umull v18.8h, v20.8b, v0.8b
    umlal v18.8h, v22.8b, v1.8b
    umlal v18.8h, v4.8b, v2.8b
    umlal v18.8h, v6.8b, v3.8b

    umull v19.8h, v21.8b, v0.8b
    umlal v19.8h, v23.8b, v1.8b
    umlal v19.8h, v5.8b, v2.8b
    umlal v19.8h, v7.8b, v3.8b

    ld2 {v20.16b,v21.16b}, [x3], x4

    rshrn v16.8b, v16.8h, #6
    rshrn v17.8b, v17.8h, #6
    rshrn v18.8b, v18.8h, #6
    rshrn v19.8b, v19.8h, #6

    ext v22.16b, v20.16b, v20.16b, #1
    ext v23.16b, v21.16b, v21.16b, #1




    st1 {v16.8b}, [x0], x2
    st1 {v17.8b}, [x1], x2
    st1 {v18.8b}, [x0], x2
    st1 {v19.8b}, [x1], x2
    b.gt 1b

    ret
2:
    tst w11, w11
    add w10, w10, w11
    dup v0.8b, w9
    dup v1.8b, w10

    b.eq 4f

    ld2 {v4.8b,v5.8b}, [x3], x4
    ld2 {v6.8b,v7.8b}, [x3], x4
3:
    subs w15, w15, #2
    umull v16.8h, v4.8b, v0.8b
    umlal v16.8h, v6.8b, v1.8b
    umull v17.8h, v5.8b, v0.8b
    umlal v17.8h, v7.8b, v1.8b

    ld2 {v4.8b,v5.8b}, [x3], x4

    umull v18.8h, v6.8b, v0.8b
    umlal v18.8h, v4.8b, v1.8b
    umull v19.8h, v7.8b, v0.8b
    umlal v19.8h, v5.8b, v1.8b

    ld2 {v6.8b,v7.8b}, [x3], x4

    rshrn v16.8b, v16.8h, #6
    rshrn v17.8b, v17.8h, #6
    rshrn v18.8b, v18.8h, #6
    rshrn v19.8b, v19.8h, #6




    st1 {v16.8b}, [x0], x2
    st1 {v17.8b}, [x1], x2
    st1 {v18.8b}, [x0], x2
    st1 {v19.8b}, [x1], x2
    b.gt 3b

    ret
4:
    ld2 {v4.16b,v5.16b}, [x3], x4
    ext v6.16b, v4.16b, v4.16b, #1
    ext v7.16b, v5.16b, v5.16b, #1
    ld2 {v20.16b,v21.16b}, [x3], x4
    ext v22.16b, v20.16b, v20.16b, #1
    ext v23.16b, v21.16b, v21.16b, #1
5:
    subs w15, w15, #2
    umull v16.8h, v4.8b, v0.8b
    umlal v16.8h, v6.8b, v1.8b
    umull v17.8h, v5.8b, v0.8b
    umlal v17.8h, v7.8b, v1.8b

    ld2 {v4.16b,v5.16b}, [x3], x4

    umull v18.8h, v20.8b, v0.8b
    umlal v18.8h, v22.8b, v1.8b
    umull v19.8h, v21.8b, v0.8b
    umlal v19.8h, v23.8b, v1.8b

    ld2 {v20.16b,v21.16b}, [x3], x4

    rshrn v16.8b, v16.8h, #6
    rshrn v17.8b, v17.8h, #6
    rshrn v18.8b, v18.8h, #6
    rshrn v19.8b, v19.8h, #6

    ext v6.16b, v4.16b, v4.16b, #1
    ext v7.16b, v5.16b, v5.16b, #1
    ext v22.16b, v20.16b, v20.16b, #1
    ext v23.16b, v21.16b, v21.16b, #1




    st1 {v16.8b}, [x0], x2
    st1 {v17.8b}, [x1], x2
    st1 {v18.8b}, [x0], x2
    st1 {v19.8b}, [x1], x2
    b.gt 5b

    ret
 # //.size mc_chroma_w8_neon, . - mc_chroma_w8_neon
        //.endfunc



        .text
        .align 2
        .globl _x264_hpel_filter_neon
 # //.type _x264_hpel_filter_neon, %function
        //.func _x264_hpel_filter_neon
_x264_hpel_filter_neon:
    ubfm x9, x3, #0, #3
    add w15, w5, w9
    sub x13, x3, x9
    sub x10, x0, x9
    sub x11, x1, x9
    sub x12, x2, x9
    movi v30.16b, #5
    movi v31.16b, #20
1:
    mov x3, x13
    mov x2, x12
    mov x1, x11
    mov x0, x10
    add x7, x3, #16
    mov x5, x15
    sub x3, x3, x4, lsl #1
    ld1 {v28.16b}, [x7], #16

    add x9, x3, x5

    ld1 {v16.16b}, [x3], x4
    ld1 {v17.16b}, [x3], x4
    ld1 {v18.16b}, [x3], x4
    ld1 {v19.16b}, [x3], x4
    ld1 {v20.16b}, [x3], x4
    ld1 {v21.16b}, [x3], x4

    ext v22.16b, v7.16b, v18.16b, #14
    uaddl v1.8h, v16.8b, v21.8b
    ext v26.16b, v18.16b, v28.16b, #3
    umlsl v1.8h, v17.8b, v30.8b
    ext v23.16b, v7.16b, v18.16b, #15
    umlal v1.8h, v18.8b, v31.8b
    ext v24.16b, v18.16b, v28.16b, #1
    umlal v1.8h, v19.8b, v31.8b
    ext v25.16b, v18.16b, v28.16b, #2
    umlsl v1.8h, v20.8b, v30.8b
2:
    subs x5, x5, #16
    sub x3, x9, x5

    uaddl v4.8h, v22.8b, v26.8b
    uaddl2 v5.8h, v22.16b, v26.16b
    sqrshrun v6.8b, v1.8h, #5
    umlsl v4.8h, v23.8b, v30.8b
    umlsl2 v5.8h, v23.16b, v30.16b
    umlal v4.8h, v18.8b, v31.8b
    umlal2 v5.8h, v18.16b, v31.16b
    umlal v4.8h, v24.8b, v31.8b
    umlal2 v5.8h, v24.16b, v31.16b
    umlsl v4.8h, v25.8b, v30.8b
    umlsl2 v5.8h, v25.16b, v30.16b

    uaddl2 v2.8h, v16.16b, v21.16b
    sqrshrun v4.8b, v4.8h, #5
        orr v7.16b, v18.16b, v18.16b
    sqrshrun2 v4.16b, v5.8h, #5

    umlsl2 v2.8h, v17.16b, v30.16b
    ld1 {v16.16b}, [x3], x4
    umlal2 v2.8h, v18.16b, v31.16b
    ld1 {v17.16b}, [x3], x4
    umlal2 v2.8h, v19.16b, v31.16b
    ld1 {v18.16b}, [x3], x4
    umlsl2 v2.8h, v20.16b, v30.16b
    ld1 {v19.16b}, [x3], x4
    st1 {v4.16b}, [x0], #16
    sqrshrun2 v6.16b, v2.8h, #5
    ld1 {v20.16b}, [x3], x4
    ld1 {v21.16b}, [x3], x4

    ext v22.16b, v0.16b, v1.16b, #12
    ext v26.16b, v1.16b, v2.16b, #6
    ext v23.16b, v0.16b, v1.16b, #14
    st1 {v6.16b}, [x1], #16
    uaddl v3.8h, v16.8b, v21.8b
    ext v25.16b, v1.16b, v2.16b, #4
    umlsl v3.8h, v17.8b, v30.8b
    ext v24.16b, v1.16b, v2.16b, #2

    umlal v3.8h, v18.8b, v31.8b
    add v4.8h, v22.8h, v26.8h
    umlal v3.8h, v19.8b, v31.8b
    add v5.8h, v23.8h, v25.8h
    umlsl v3.8h, v20.8b, v30.8b
    add v6.8h, v24.8h, v1.8h

    ext v22.16b, v1.16b, v2.16b, #12
    ext v26.16b, v2.16b, v3.16b, #6
    ext v23.16b, v1.16b, v2.16b, #14
    ext v25.16b, v2.16b, v3.16b, #4
    ext v24.16b, v2.16b, v3.16b, #2

    add v22.8h, v22.8h, v26.8h
    add v23.8h, v23.8h, v25.8h
    add v24.8h, v24.8h, v2.8h

    sub v4.8h, v4.8h, v5.8h
    sub v5.8h, v5.8h, v6.8h

    sub v22.8h, v22.8h, v23.8h
    sub v23.8h, v23.8h, v24.8h

    sshr v4.8h, v4.8h, #2
    sshr v22.8h, v22.8h, #2
    sub v4.8h, v4.8h, v5.8h
    sub v22.8h, v22.8h, v23.8h
    sshr v4.8h, v4.8h, #2
    sshr v22.8h, v22.8h, #2
    add v4.8h, v4.8h, v6.8h
    add v22.8h, v22.8h, v24.8h

    sqrshrun v4.8b, v4.8h, #6
    ld1 {v28.16b}, [x7], #16
        orr v0.16b, v2.16b, v2.16b
    ext v23.16b, v7.16b, v18.16b, #15
    sqrshrun2 v4.16b, v22.8h, #6
        orr v1.16b, v3.16b, v3.16b
    ext v22.16b, v7.16b, v18.16b, #14
    ext v24.16b, v18.16b, v28.16b, #1
    ext v25.16b, v18.16b, v28.16b, #2
    ext v26.16b, v18.16b, v28.16b, #3

    st1 {v4.16b}, [x2], #16
    b.gt 2b

    subs w6, w6, #1
    add x10, x10, x4
    add x11, x11, x4
    add x12, x12, x4
    add x13, x13, x4
    b.gt 1b

    ret
 # //.size x264_hpel_filter_neon, . - x264_hpel_filter_neon
        //.endfunc




        .text
        .align 2
        .globl _x264_frame_init_lowres_core_neon
 # //.type _x264_frame_init_lowres_core_neon, %function
        //.func _x264_frame_init_lowres_core_neon
_x264_frame_init_lowres_core_neon:
    ldr w8, [sp]
    sub x10, x6, w7, uxtw
    and x10, x10, #~15

1:
    mov w9, w7
    mov x11, x0
    add x12, x0, x5
    add x13, x0, x5, lsl #1

    ld2 {v0.16b,v1.16b}, [x11], #32
    ld2 {v2.16b,v3.16b}, [x12], #32
    ld2 {v4.16b,v5.16b}, [x13], #32

    urhadd v20.16b, v0.16b, v2.16b
    urhadd v22.16b, v2.16b, v4.16b
2:
    subs w9, w9, #16
    urhadd v21.16b, v1.16b, v3.16b
    urhadd v23.16b, v3.16b, v5.16b

    ld2 {v0.16b,v1.16b}, [x11], #32
    ld2 {v2.16b,v3.16b}, [x12], #32
    ld2 {v4.16b,v5.16b}, [x13], #32
    urhadd v30.16b, v0.16b, v2.16b
    urhadd v31.16b, v2.16b, v4.16b
    ext v24.16b, v20.16b, v30.16b, #1
    ext v25.16b, v22.16b, v31.16b, #1

    urhadd v16.16b, v20.16b, v21.16b
    urhadd v18.16b, v22.16b, v23.16b
    urhadd v17.16b, v21.16b, v24.16b
    urhadd v19.16b, v23.16b, v25.16b

    st1 {v16.16b}, [x1], #16
    st1 {v18.16b}, [x3], #16
    st1 {v17.16b}, [x2], #16
    st1 {v19.16b}, [x4], #16
    b.le 3f

    subs w9, w9, #16
    urhadd v21.16b, v1.16b, v3.16b
    urhadd v23.16b, v3.16b, v5.16b

    ld2 {v0.16b,v1.16b}, [x11], #32
    ld2 {v2.16b,v3.16b}, [x12], #32
    ld2 {v4.16b,v5.16b}, [x13], #32
    urhadd v20.16b, v0.16b, v2.16b
    urhadd v22.16b, v2.16b, v4.16b
    ext v24.16b, v30.16b, v20.16b, #1
    ext v25.16b, v31.16b, v22.16b, #1

    urhadd v16.16b, v30.16b, v21.16b
    urhadd v18.16b, v31.16b, v23.16b
    urhadd v17.16b, v21.16b, v24.16b
    urhadd v19.16b, v23.16b, v25.16b

    st1 {v16.16b}, [x1], #16
    st1 {v18.16b}, [x3], #16
    st1 {v17.16b}, [x2], #16
    st1 {v19.16b}, [x4], #16
    b.gt 2b
3:
    subs w8, w8, #1
    add x0, x0, x5, lsl #1
    add x1, x1, x10
    add x2, x2, x10
    add x3, x3, x10
    add x4, x4, x10
    b.gt 1b

    ret
 # //.size x264_frame_init_lowres_core_neon, . - x264_frame_init_lowres_core_neon
        //.endfunc

        .text
        .align 2
        .globl _x264_load_deinterleave_chroma_fenc_neon
 # //.type _x264_load_deinterleave_chroma_fenc_neon, %function
        //.func _x264_load_deinterleave_chroma_fenc_neon
_x264_load_deinterleave_chroma_fenc_neon:
    mov x4, #16/2
    b load_deinterleave_chroma
 # //.size x264_load_deinterleave_chroma_fenc_neon, . - x264_load_deinterleave_chroma_fenc_neon
        //.endfunc

        .text
        .align 2
        .globl _x264_load_deinterleave_chroma_fdec_neon
 # //.type _x264_load_deinterleave_chroma_fdec_neon, %function
        //.func _x264_load_deinterleave_chroma_fdec_neon
_x264_load_deinterleave_chroma_fdec_neon:
    mov x4, #32/2
load_deinterleave_chroma:
    ld2 {v0.8b,v1.8b}, [x1], x2
    ld2 {v2.8b,v3.8b}, [x1], x2
    subs w3, w3, #2
    st1 {v0.8b}, [x0], x4
    st1 {v1.8b}, [x0], x4
    st1 {v2.8b}, [x0], x4
    st1 {v3.8b}, [x0], x4
    b.gt load_deinterleave_chroma

    ret
 # //.size x264_load_deinterleave_chroma_fdec_neon, . - x264_load_deinterleave_chroma_fdec_neon
        //.endfunc

        .text
        .align 2
        .globl _x264_plane_copy_deinterleave_neon
 # //.type _x264_plane_copy_deinterleave_neon, %function
        //.func _x264_plane_copy_deinterleave_neon
_x264_plane_copy_deinterleave_neon:
    add w9, w6, #15
    and w9, w9, #0xfffffff0
    sub x1, x1, x9
    sub x3, x3, x9
    sub x5, x5, x9, lsl #1
1:
    ld2 {v0.16b,v1.16b}, [x4], #32
    subs w9, w9, #16
    st1 {v0.16b}, [x0], #16
    st1 {v1.16b}, [x2], #16
    b.gt 1b

    add x4, x4, x5
    subs w7, w7, #1
    add x0, x0, x1
    add x2, x2, x3
    mov w9, w6
    b.gt 1b

    ret
 # //.size x264_plane_copy_deinterleave_neon, . - x264_plane_copy_deinterleave_neon
        //.endfunc


        .text
        .align 2
        .globl _x264_plane_copy_deinterleave_rgb_neon
 # //.type _x264_plane_copy_deinterleave_rgb_neon, %function
        //.func _x264_plane_copy_deinterleave_rgb_neon
_x264_plane_copy_deinterleave_rgb_neon:

    ldr w8, [sp]
    ldp w9, w10, [sp, #4]




    cmp w8, #3
    uxtw x9, w9
    add x11, x9, #7
    and x11, x11, #~7
    sub x1, x1, x11
    sub x3, x3, x11
    sub x5, x5, x11
    b.ne 4f
    sub x7, x7, x11, lsl #1
    sub x7, x7, x11
1:
    ld3 {v0.8b,v1.8b,v2.8b}, [x6], #24
    subs x11, x11, #8
    st1 {v0.8b}, [x0], #8
    st1 {v1.8b}, [x2], #8
    st1 {v2.8b}, [x4], #8
    b.gt 1b

    subs w10, w10, #1
    add x0, x0, x1
    add x2, x2, x3
    add x4, x4, x5
    add x6, x6, x7
    mov x11, x9
    b.gt 1b

    ret
4:
    sub x7, x7, x11, lsl #2
1:
    ld4 {v0.8b,v1.8b,v2.8b,v3.8b}, [x6], #32
    subs x11, x11, #8
    st1 {v0.8b}, [x0], #8
    st1 {v1.8b}, [x2], #8
    st1 {v2.8b}, [x4], #8
    b.gt 1b

    subs w10, w10, #1
    add x0, x0, x1
    add x2, x2, x3
    add x4, x4, x5
    add x6, x6, x7
    mov x11, x9
    b.gt 1b

    ret
 # //.size x264_plane_copy_deinterleave_rgb_neon, . - x264_plane_copy_deinterleave_rgb_neon
        //.endfunc

        .text
        .align 2
        .globl _x264_plane_copy_interleave_neon
 # //.type _x264_plane_copy_interleave_neon, %function
        //.func _x264_plane_copy_interleave_neon
_x264_plane_copy_interleave_neon:
    add w9, w6, #15
    and w9, w9, #0xfffffff0
    sub x1, x1, x9, lsl #1
    sub x3, x3, x9
    sub x5, x5, x9
1:
    ld1 {v0.16b}, [x2], #16
    ld1 {v1.16b}, [x4], #16
    subs w9, w9, #16
    st2 {v0.16b,v1.16b}, [x0], #32
    b.gt 1b

    subs w7, w7, #1
    add x0, x0, x1
    add x2, x2, x3
    add x4, x4, x5
    mov w9, w6
    b.gt 1b

    ret
 # //.size x264_plane_copy_interleave_neon, . - x264_plane_copy_interleave_neon
        //.endfunc

        .text
        .align 2
        .globl _x264_store_interleave_chroma_neon
 # //.type _x264_store_interleave_chroma_neon, %function
        //.func _x264_store_interleave_chroma_neon
_x264_store_interleave_chroma_neon:
    mov x5, #32
1:
    ld1 {v0.8b}, [x2], x5
    ld1 {v1.8b}, [x3], x5
    ld1 {v2.8b}, [x2], x5
    ld1 {v3.8b}, [x3], x5
    subs w4, w4, #2
    zip1 v4.16b, v0.16b, v1.16b
    zip1 v5.16b, v2.16b, v3.16b
    st1 {v4.16b}, [x0], x1
    st1 {v5.16b}, [x0], x1
    b.gt 1b

    ret
 # //.size x264_store_interleave_chroma_neon, . - x264_store_interleave_chroma_neon
        //.endfunc

#endif