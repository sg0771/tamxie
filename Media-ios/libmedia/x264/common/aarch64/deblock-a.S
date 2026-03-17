
#ifdef __arm64__

        .text
        .align 2
        .globl _x264_deblock_v_luma_neon
 # //.type _x264_deblock_v_luma_neon, %function
        //.func _x264_deblock_v_luma_neon
_x264_deblock_v_luma_neon:
    cmp w2, #0
    ldr w6, [x4]
    ccmp w3, #0, #0, ne
    mov v24.s[0], w6
    and w6, w6, w6, lsl #16
    b.eq 1f
    ands w6, w6, w6, lsl #8
    b.ge 2f
1:
    ret
2:

    ld1 {v0.16b}, [x0], x1
    ld1 {v2.16b}, [x0], x1
    ld1 {v4.16b}, [x0], x1
    sub x0, x0, x1, lsl #2
    sub x0, x0, x1, lsl #1
    ld1 {v20.16b}, [x0], x1
    ld1 {v18.16b}, [x0], x1
    ld1 {v16.16b}, [x0], x1

    dup v22.16b, w2
        ushll v24.8h, v24.8b, #0
    uabd v21.16b, v16.16b, v0.16b
        ushll v24.4s, v24.4h, #0
    uabd v28.16b, v18.16b, v16.16b
    sli v24.8h, v24.8h, #8
    uabd v30.16b, v2.16b, v0.16b
    sli v24.4s, v24.4s, #16
    cmhi v21.16b, v22.16b, v21.16b
    dup v22.16b, w3
    cmlt v23.16b, v24.16b, #0
    cmhi v28.16b, v22.16b, v28.16b
    cmhi v30.16b, v22.16b, v30.16b
    bic v21.16b, v21.16b, v23.16b
    uabd v17.16b, v20.16b, v16.16b
    and v21.16b, v21.16b, v28.16b
    uabd v19.16b, v4.16b, v0.16b
    cmhi v17.16b, v22.16b, v17.16b
    and v21.16b, v21.16b, v30.16b
    cmhi v19.16b, v22.16b, v19.16b
    and v17.16b, v17.16b, v21.16b
    and v19.16b, v19.16b, v21.16b
    and v24.16b, v24.16b, v21.16b
    urhadd v28.16b, v16.16b, v0.16b
    sub v21.16b, v24.16b, v17.16b
    uqadd v23.16b, v18.16b, v24.16b
    uhadd v20.16b, v20.16b, v28.16b
    sub v21.16b, v21.16b, v19.16b
    uhadd v28.16b, v4.16b, v28.16b
    umin v23.16b, v23.16b, v20.16b
    uqsub v22.16b, v18.16b, v24.16b
    uqadd v4.16b, v2.16b, v24.16b
    umax v23.16b, v23.16b, v22.16b
    uqsub v22.16b, v2.16b, v24.16b
    umin v28.16b, v4.16b, v28.16b
        ushll v4.8h, v0.8b, #0
    umax v28.16b, v28.16b, v22.16b
        ushll2 v20.8h, v0.16b, #0
    usubw v4.8h, v4.8h, v16.8b
    usubw2 v20.8h, v20.8h, v16.16b
    shl v4.8h, v4.8h, #2
    shl v20.8h, v20.8h, #2
    uaddw v4.8h, v4.8h, v18.8b
    uaddw2 v20.8h, v20.8h, v18.16b
    usubw v4.8h, v4.8h, v2.8b
    usubw2 v20.8h, v20.8h, v2.16b
    rshrn v4.8b, v4.8h, #3
    rshrn2 v4.16b, v20.8h, #3
    bsl v17.16b, v23.16b, v18.16b
    bsl v19.16b, v28.16b, v2.16b
    neg v23.16b, v21.16b
        ushll v28.8h, v16.8b, #0
    smin v4.16b, v4.16b, v21.16b
        ushll2 v21.8h, v16.16b, #0
    smax v4.16b, v4.16b, v23.16b
        ushll v22.8h, v0.8b, #0
        ushll2 v24.8h, v0.16b, #0
    saddw v28.8h, v28.8h, v4.8b
    saddw2 v21.8h, v21.8h, v4.16b
    ssubw v22.8h, v22.8h, v4.8b
    ssubw2 v24.8h, v24.8h, v4.16b
    sqxtun v16.8b, v28.8h
    sqxtun2 v16.16b, v21.8h
    sqxtun v0.8b, v22.8h
    sqxtun2 v0.16b, v24.8h

    sub x0, x0, x1, lsl #1
    st1 {v17.16b}, [x0], x1
    st1 {v16.16b}, [x0], x1
    st1 {v0.16b}, [x0], x1
    st1 {v19.16b}, [x0]

    ret
 # //.size x264_deblock_v_luma_neon, . - x264_deblock_v_luma_neon
        //.endfunc

        .text
        .align 2
        .globl _x264_deblock_h_luma_neon
 # //.type _x264_deblock_h_luma_neon, %function
        //.func _x264_deblock_h_luma_neon
_x264_deblock_h_luma_neon:
    cmp w2, #0
    ldr w6, [x4]
    ccmp w3, #0, #0, ne
    mov v24.s[0], w6
    and w6, w6, w6, lsl #16
    b.eq 1f
    ands w6, w6, w6, lsl #8
    b.ge 2f
1:
    ret
2:

    sub x0, x0, #4
    ld1 {v6.8b}, [x0], x1
    ld1 {v20.8b}, [x0], x1
    ld1 {v18.8b}, [x0], x1
    ld1 {v16.8b}, [x0], x1
    ld1 {v0.8b}, [x0], x1
    ld1 {v2.8b}, [x0], x1
    ld1 {v4.8b}, [x0], x1
    ld1 {v26.8b}, [x0], x1
    ld1 {v6.d}[1], [x0], x1
    ld1 {v20.d}[1], [x0], x1
    ld1 {v18.d}[1], [x0], x1
    ld1 {v16.d}[1], [x0], x1
    ld1 {v0.d}[1], [x0], x1
    ld1 {v2.d}[1], [x0], x1
    ld1 {v4.d}[1], [x0], x1
    ld1 {v26.d}[1], [x0], x1

    trn1 v21.16b, v6.16b, v20.16b
    trn2 v23.16b, v6.16b, v20.16b
    trn1 v20.16b, v18.16b, v16.16b
    trn2 v16.16b, v18.16b, v16.16b
    trn1 v6.16b, v0.16b, v2.16b
    trn2 v2.16b, v0.16b, v2.16b
    trn1 v18.16b, v4.16b, v26.16b
    trn2 v26.16b, v4.16b, v26.16b

    trn1 v0.8h, v6.8h, v18.8h
    trn2 v18.8h, v6.8h, v18.8h
    trn1 v4.8h, v2.8h, v26.8h
    trn2 v26.8h, v2.8h, v26.8h
    trn1 v2.8h, v23.8h, v16.8h
    trn2 v23.8h, v23.8h, v16.8h
    trn1 v16.8h, v21.8h, v20.8h
    trn2 v21.8h, v21.8h, v20.8h

    trn1 v6.4s, v16.4s, v0.4s
    trn2 v0.4s, v16.4s, v0.4s

    trn1 v20.4s, v2.4s, v4.4s
    trn2 v2.4s, v2.4s, v4.4s

    trn2 v4.4s, v21.4s, v18.4s
    trn1 v18.4s, v21.4s, v18.4s

    trn1 v16.4s, v23.4s, v26.4s
    trn2 v26.4s, v23.4s, v26.4s

    dup v22.16b, w2
        ushll v24.8h, v24.8b, #0
    uabd v21.16b, v16.16b, v0.16b
        ushll v24.4s, v24.4h, #0
    uabd v28.16b, v18.16b, v16.16b
    sli v24.8h, v24.8h, #8
    uabd v30.16b, v2.16b, v0.16b
    sli v24.4s, v24.4s, #16
    cmhi v21.16b, v22.16b, v21.16b
    dup v22.16b, w3
    cmlt v23.16b, v24.16b, #0
    cmhi v28.16b, v22.16b, v28.16b
    cmhi v30.16b, v22.16b, v30.16b
    bic v21.16b, v21.16b, v23.16b
    uabd v17.16b, v20.16b, v16.16b
    and v21.16b, v21.16b, v28.16b
    uabd v19.16b, v4.16b, v0.16b
    cmhi v17.16b, v22.16b, v17.16b
    and v21.16b, v21.16b, v30.16b
    cmhi v19.16b, v22.16b, v19.16b
    and v17.16b, v17.16b, v21.16b
    and v19.16b, v19.16b, v21.16b
    and v24.16b, v24.16b, v21.16b
    urhadd v28.16b, v16.16b, v0.16b
    sub v21.16b, v24.16b, v17.16b
    uqadd v23.16b, v18.16b, v24.16b
    uhadd v20.16b, v20.16b, v28.16b
    sub v21.16b, v21.16b, v19.16b
    uhadd v28.16b, v4.16b, v28.16b
    umin v23.16b, v23.16b, v20.16b
    uqsub v22.16b, v18.16b, v24.16b
    uqadd v4.16b, v2.16b, v24.16b
    umax v23.16b, v23.16b, v22.16b
    uqsub v22.16b, v2.16b, v24.16b
    umin v28.16b, v4.16b, v28.16b
        ushll v4.8h, v0.8b, #0
    umax v28.16b, v28.16b, v22.16b
        ushll2 v20.8h, v0.16b, #0
    usubw v4.8h, v4.8h, v16.8b
    usubw2 v20.8h, v20.8h, v16.16b
    shl v4.8h, v4.8h, #2
    shl v20.8h, v20.8h, #2
    uaddw v4.8h, v4.8h, v18.8b
    uaddw2 v20.8h, v20.8h, v18.16b
    usubw v4.8h, v4.8h, v2.8b
    usubw2 v20.8h, v20.8h, v2.16b
    rshrn v4.8b, v4.8h, #3
    rshrn2 v4.16b, v20.8h, #3
    bsl v17.16b, v23.16b, v18.16b
    bsl v19.16b, v28.16b, v2.16b
    neg v23.16b, v21.16b
        ushll v28.8h, v16.8b, #0
    smin v4.16b, v4.16b, v21.16b
        ushll2 v21.8h, v16.16b, #0
    smax v4.16b, v4.16b, v23.16b
        ushll v22.8h, v0.8b, #0
        ushll2 v24.8h, v0.16b, #0
    saddw v28.8h, v28.8h, v4.8b
    saddw2 v21.8h, v21.8h, v4.16b
    ssubw v22.8h, v22.8h, v4.8b
    ssubw2 v24.8h, v24.8h, v4.16b
    sqxtun v16.8b, v28.8h
    sqxtun2 v16.16b, v21.8h
    sqxtun v0.8b, v22.8h
    sqxtun2 v0.16b, v24.8h

    trn1 v21.16b, v17.16b, v16.16b
    trn2 v23.16b, v17.16b, v16.16b
    trn1 v25.16b, v0.16b, v19.16b
    trn2 v27.16b, v0.16b, v19.16b

    trn1 v17.8h, v21.8h, v25.8h
    trn2 v0.8h, v21.8h, v25.8h
    trn1 v16.8h, v23.8h, v27.8h
    trn2 v19.8h, v23.8h, v27.8h

    sub x0, x0, x1, lsl #4
    add x0, x0, #2
    st1 {v17.s}[0], [x0], x1
    st1 {v16.s}[0], [x0], x1
    st1 {v0.s}[0], [x0], x1
    st1 {v19.s}[0], [x0], x1
    st1 {v17.s}[1], [x0], x1
    st1 {v16.s}[1], [x0], x1
    st1 {v0.s}[1], [x0], x1
    st1 {v19.s}[1], [x0], x1
    st1 {v17.s}[2], [x0], x1
    st1 {v16.s}[2], [x0], x1
    st1 {v0.s}[2], [x0], x1
    st1 {v19.s}[2], [x0], x1
    st1 {v17.s}[3], [x0], x1
    st1 {v16.s}[3], [x0], x1
    st1 {v0.s}[3], [x0], x1
    st1 {v19.s}[3], [x0], x1

    ret
 # //.size x264_deblock_h_luma_neon, . - x264_deblock_h_luma_neon
        //.endfunc


        .text
        .align 2
        .globl _x264_deblock_v_chroma_neon
 # //.type _x264_deblock_v_chroma_neon, %function
        //.func _x264_deblock_v_chroma_neon
_x264_deblock_v_chroma_neon:
    cmp w2, #0
    ldr w6, [x4]
    ccmp w3, #0, #0, ne
    mov v24.s[0], w6
    and w6, w6, w6, lsl #16
    b.eq 1f
    ands w6, w6, w6, lsl #8
    b.ge 2f
1:
    ret
2:

    sub x0, x0, x1, lsl #1
    ld1 {v18.16b}, [x0], x1
    ld1 {v16.16b}, [x0], x1
    ld1 {v0.16b}, [x0], x1
    ld1 {v2.16b}, [x0]

    dup v22.16b, w2
        ushll v24.8h, v24.8b, #0
    uabd v26.16b, v16.16b, v0.16b
        ushll v4.8h, v0.8b, #0
        ushll2 v5.8h, v0.16b, #0
    uabd v28.16b, v18.16b, v16.16b
    usubw v4.8h, v4.8h, v16.8b
    usubw2 v5.8h, v5.8h, v16.16b
    sli v24.8h, v24.8h, #8
    shl v4.8h, v4.8h, #2
    shl v5.8h, v5.8h, #2
    uabd v30.16b, v2.16b, v0.16b
        ushll v24.4s, v24.4h, #0
    uaddw v4.8h, v4.8h, v18.8b
    uaddw2 v5.8h, v5.8h, v18.16b
    cmhi v26.16b, v22.16b, v26.16b
    usubw v4.8h, v4.8h, v2.8b
    usubw2 v5.8h, v5.8h, v2.16b
    sli v24.4s, v24.4s, #16
    dup v22.16b, w3
    rshrn v4.8b, v4.8h, #3
    rshrn2 v4.16b, v5.8h, #3
    cmhi v28.16b, v22.16b, v28.16b
    cmhi v30.16b, v22.16b, v30.16b
    smin v4.16b, v4.16b, v24.16b
    neg v25.16b, v24.16b
    and v26.16b, v26.16b, v28.16b
    smax v4.16b, v4.16b, v25.16b
    and v26.16b, v26.16b, v30.16b
        ushll v22.8h, v0.8b, #0
        ushll2 v23.8h, v0.16b, #0
    and v4.16b, v4.16b, v26.16b
        ushll v28.8h, v16.8b, #0
        ushll2 v29.8h, v16.16b, #0
    saddw v28.8h, v28.8h, v4.8b
    saddw2 v29.8h, v29.8h, v4.16b
    ssubw v22.8h, v22.8h, v4.8b
    ssubw2 v23.8h, v23.8h, v4.16b
    sqxtun v16.8b, v28.8h
    sqxtun v0.8b, v22.8h
    sqxtun2 v16.16b, v29.8h
    sqxtun2 v0.16b, v23.8h

    sub x0, x0, x1, lsl #1
    st1 {v16.16b}, [x0], x1
    st1 {v0.16b}, [x0], x1

    ret
 # //.size x264_deblock_v_chroma_neon, . - x264_deblock_v_chroma_neon
        //.endfunc

        .text
        .align 2
        .globl _x264_deblock_h_chroma_neon
 # //.type _x264_deblock_h_chroma_neon, %function
        //.func _x264_deblock_h_chroma_neon
_x264_deblock_h_chroma_neon:
    cmp w2, #0
    ldr w6, [x4]
    ccmp w3, #0, #0, ne
    mov v24.s[0], w6
    and w6, w6, w6, lsl #16
    b.eq 1f
    ands w6, w6, w6, lsl #8
    b.ge 2f
1:
    ret
2:

    sub x0, x0, #4
    ld1 {v18.d}[0], [x0], x1
    ld1 {v16.d}[0], [x0], x1
    ld1 {v0.d}[0], [x0], x1
    ld1 {v2.d}[0], [x0], x1
    ld1 {v18.d}[1], [x0], x1
    ld1 {v16.d}[1], [x0], x1
    ld1 {v0.d}[1], [x0], x1
    ld1 {v2.d}[1], [x0], x1

    trn1 v28.4s, v18.4s, v0.4s
    trn2 v30.4s, v18.4s, v0.4s
    trn1 v29.4s, v16.4s, v2.4s
    trn2 v31.4s, v16.4s, v2.4s
    trn1 v18.8h, v28.8h, v29.8h
    trn2 v16.8h, v28.8h, v29.8h
    trn1 v0.8h, v30.8h, v31.8h
    trn2 v2.8h, v30.8h, v31.8h

    dup v22.16b, w2
        ushll v24.8h, v24.8b, #0
    uabd v26.16b, v16.16b, v0.16b
        ushll v4.8h, v0.8b, #0
        ushll2 v5.8h, v0.16b, #0
    uabd v28.16b, v18.16b, v16.16b
    usubw v4.8h, v4.8h, v16.8b
    usubw2 v5.8h, v5.8h, v16.16b
    sli v24.8h, v24.8h, #8
    shl v4.8h, v4.8h, #2
    shl v5.8h, v5.8h, #2
    uabd v30.16b, v2.16b, v0.16b
        ushll v24.4s, v24.4h, #0
    uaddw v4.8h, v4.8h, v18.8b
    uaddw2 v5.8h, v5.8h, v18.16b
    cmhi v26.16b, v22.16b, v26.16b
    usubw v4.8h, v4.8h, v2.8b
    usubw2 v5.8h, v5.8h, v2.16b
    sli v24.4s, v24.4s, #16
    dup v22.16b, w3
    rshrn v4.8b, v4.8h, #3
    rshrn2 v4.16b, v5.8h, #3
    cmhi v28.16b, v22.16b, v28.16b
    cmhi v30.16b, v22.16b, v30.16b
    smin v4.16b, v4.16b, v24.16b
    neg v25.16b, v24.16b
    and v26.16b, v26.16b, v28.16b
    smax v4.16b, v4.16b, v25.16b
    and v26.16b, v26.16b, v30.16b
        ushll v22.8h, v0.8b, #0
        ushll2 v23.8h, v0.16b, #0
    and v4.16b, v4.16b, v26.16b
        ushll v28.8h, v16.8b, #0
        ushll2 v29.8h, v16.16b, #0
    saddw v28.8h, v28.8h, v4.8b
    saddw2 v29.8h, v29.8h, v4.16b
    ssubw v22.8h, v22.8h, v4.8b
    ssubw2 v23.8h, v23.8h, v4.16b
    sqxtun v16.8b, v28.8h
    sqxtun v0.8b, v22.8h
    sqxtun2 v16.16b, v29.8h
    sqxtun2 v0.16b, v23.8h

    trn1 v28.4s, v18.4s, v0.4s
    trn2 v30.4s, v18.4s, v0.4s
    trn1 v29.4s, v16.4s, v2.4s
    trn2 v31.4s, v16.4s, v2.4s
    trn1 v18.8h, v28.8h, v29.8h
    trn2 v16.8h, v28.8h, v29.8h
    trn1 v0.8h, v30.8h, v31.8h
    trn2 v2.8h, v30.8h, v31.8h

    sub x0, x0, x1, lsl #3
    st1 {v18.d}[0], [x0], x1
    st1 {v16.d}[0], [x0], x1
    st1 {v0.d}[0], [x0], x1
    st1 {v2.d}[0], [x0], x1
    st1 {v18.d}[1], [x0], x1
    st1 {v16.d}[1], [x0], x1
    st1 {v0.d}[1], [x0], x1
    st1 {v2.d}[1], [x0], x1

    ret
 # //.size x264_deblock_h_chroma_neon, . - x264_deblock_h_chroma_neon
        //.endfunc







        .text
        .align 2
        .globl _x264_deblock_strength_neon
 # //.type _x264_deblock_strength_neon, %function
        //.func _x264_deblock_strength_neon
_x264_deblock_strength_neon:
    movi v4.16b, #0
    lsl w4, w4, #8
    add x3, x3, #32
    sub w4, w4, #(1<<8)-3
    movi v5.16b, #0
    dup v6.8h, w4
    mov x6, #-32

bframe:

    add x2, x2, #16
    ld1 {v31.d}[1], [x1], #8
    ld1 {v1.16b}, [x1], #16
    movi v0.16b, #0
    ld1 {v2.16b}, [x1], #16
    ext v3.16b, v0.16b, v1.16b, #15
    ext v0.16b, v0.16b, v2.16b, #15
    uzp1 v21.4s, v1.4s, v2.4s
    uzp2 v22.4s, v1.4s, v2.4s
    uzp1 v23.4s, v3.4s, v0.4s
    uzp2 v20.4s, v3.4s, v0.4s
    ext v21.16b, v31.16b, v22.16b, #12

    eor v0.16b, v20.16b, v22.16b
    eor v1.16b, v21.16b, v22.16b
    orr v4.16b, v4.16b, v0.16b
    orr v5.16b, v5.16b, v1.16b

    ld1 {v21.8h}, [x2], #16
    ld1 {v19.8h}, [x2], #16
    ld1 {v22.8h}, [x2], #16
    ld1 {v18.8h}, [x2], #16
    ld1 {v23.8h}, [x2], #16
    ext v19.16b, v19.16b, v22.16b, #12
    ext v18.16b, v18.16b, v23.16b, #12
    sabd v0.8h, v22.8h, v19.8h
    ld1 {v19.8h}, [x2], #16
    sabd v1.8h, v23.8h, v18.8h
    ld1 {v24.8h}, [x2], #16
    uqxtn v0.8b, v0.8h
    ld1 {v18.8h}, [x2], #16
    ld1 {v25.8h}, [x2], #16
    uqxtn2 v0.16b, v1.8h
    ext v19.16b, v19.16b, v24.16b, #12
    ext v18.16b, v18.16b, v25.16b, #12
    sabd v1.8h, v24.8h, v19.8h
    sabd v2.8h, v25.8h, v18.8h
    uqxtn v1.8b, v1.8h
    uqxtn2 v1.16b, v2.8h

    uqsub v0.16b, v0.16b, v6.16b
    uqsub v1.16b, v1.16b, v6.16b
    uqxtn v0.8b, v0.8h
    uqxtn2 v0.16b, v1.8h

    sabd v1.8h, v22.8h, v23.8h
    orr v4.16b, v4.16b, v0.16b

    sabd v0.8h, v21.8h, v22.8h
    sabd v2.8h, v23.8h, v24.8h
    sabd v3.8h, v24.8h, v25.8h
    uqxtn v0.8b, v0.8h
    uqxtn2 v0.16b, v1.8h
    uqxtn v1.8b, v2.8h
    uqxtn2 v1.16b, v3.8h

    uqsub v0.16b, v0.16b, v6.16b
    uqsub v1.16b, v1.16b, v6.16b
    uqxtn v0.8b, v0.8h
    uqxtn2 v0.16b, v1.8h
    subs w5, w5, #1
    orr v5.16b, v5.16b, v0.16b
    b.eq bframe

    movi v6.16b, #1

    ld1 {v31.d}[1], [x0], #8
    ld1 {v1.16b}, [x0], #16
    movi v0.16b, #0
    ld1 {v2.16b}, [x0], #16
    ext v3.16b, v0.16b, v1.16b, #15
    ext v0.16b, v0.16b, v2.16b, #15
    uzp1 v21.4s, v1.4s, v2.4s
    uzp2 v22.4s, v1.4s, v2.4s
    uzp1 v23.4s, v3.4s, v0.4s
    uzp2 v20.4s, v3.4s, v0.4s
    ext v21.16b, v31.16b, v22.16b, #12


        adrp x7, transpose_table@PAGE
        add x7, x7, transpose_table@PAGEOFF






    ld1 {v7.16b}, [x7]
    orr v0.16b, v20.16b, v22.16b
    orr v1.16b, v21.16b, v22.16b
    umin v0.16b, v0.16b, v6.16b
    umin v1.16b, v1.16b, v6.16b
    umin v4.16b, v4.16b, v6.16b
    umin v5.16b, v5.16b, v6.16b
    add v0.16b, v0.16b, v0.16b
    add v1.16b, v1.16b, v1.16b
    umax v4.16b, v4.16b, v0.16b
    umax v5.16b, v5.16b, v1.16b
    tbl v6.16b, {v4.16b}, v7.16b
    st1 {v5.16b}, [x3], x6
    st1 {v6.16b}, [x3]
    ret
 # //.size x264_deblock_strength_neon, . - x264_deblock_strength_neon
        //.endfunc

.const_data
        .const_data
        .align 2
transpose_table:
    .byte 0, 4, 8, 12
    .byte 1, 5, 9, 13
    .byte 2, 6, 10, 14
    .byte 3, 7, 11, 15
 # //.size transpose_table, . - transpose_table

 #endif
