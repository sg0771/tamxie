#ifdef __arm64__
        .text
        .align 2
        .globl _ff_h264_idct_add_neon
 # //.type _ff_h264_idct_add_neon, %function
 # //.func _ff_h264_idct_add_neon
_ff_h264_idct_add_neon:
        ld1 {v0.4H, v1.4H, v2.4H, v3.4H}, [x1]
        sxtw x2, w2
        movi v30.8H, #0, lsl #0

        add v4.4H, v0.4H, v2.4H
        sshr v16.4H, v1.4H, #1
        st1 {v30.8H}, [x1], #16
        sshr v17.4H, v3.4H, #1
        st1 {v30.8H}, [x1], #16
        sub v5.4H, v0.4H, v2.4H
        add v6.4H, v1.4H, v17.4H
        sub v7.4H, v16.4H, v3.4H
        add v0.4H, v4.4H, v6.4H
        add v1.4H, v5.4H, v7.4H
        sub v3.4H, v4.4H, v6.4H
        sub v2.4H, v5.4H, v7.4H

        trn1 v4.4H, v0.4H, v1.4H
        trn2 v5.4H, v0.4H, v1.4H
        trn1 v7.4H, v2.4H, v3.4H
        trn2 v6.4H, v2.4H, v3.4H
        trn1 v0.2S, v4.2S, v7.2S
        trn2 v3.2S, v4.2S, v7.2S
        trn1 v1.2S, v5.2S, v6.2S
        trn2 v2.2S, v5.2S, v6.2S

        add v4.4H, v0.4H, v3.4H
        ld1 {v18.S}[0], [x0], x2
        sshr v16.4H, v2.4H, #1
        sshr v17.4H, v1.4H, #1
        ld1 {v19.S}[1], [x0], x2
        sub v5.4H, v0.4H, v3.4H
        ld1 {v18.S}[1], [x0], x2
        add v6.4H, v16.4H, v1.4H
        ins v4.D[1], v5.D[0]
        sub v7.4H, v2.4H, v17.4H
        ld1 {v19.S}[0], [x0], x2
        ins v6.D[1], v7.D[0]
        sub x0, x0, x2, lsl #2
        add v0.8H, v4.8H, v6.8H
        sub v1.8H, v4.8H, v6.8H

        srshr v0.8H, v0.8H, #6
        srshr v1.8H, v1.8H, #6

        uaddw v0.8H, v0.8H, v18.8B
        uaddw v1.8H, v1.8H, v19.8B

        sqxtun v0.8B, v0.8H
        sqxtun v1.8B, v1.8H

        st1 {v0.S}[0], [x0], x2
        st1 {v1.S}[1], [x0], x2
        st1 {v0.S}[1], [x0], x2
        st1 {v1.S}[0], [x0], x2

        sub x1, x1, #32
        ret
 # //.size ff_h264_idct_add_neon, . - ff_h264_idct_add_neon
 # //.endfunc

        .text
        .align 2
        .globl _ff_h264_idct_dc_add_neon
 # //.type _ff_h264_idct_dc_add_neon, %function
 # //.func _ff_h264_idct_dc_add_neon
_ff_h264_idct_dc_add_neon:
        sxtw x2, w2
        mov w3, #0
        ld1r {v2.8H}, [x1]
        strh w3, [x1]
        srshr v2.8H, v2.8H, #6
        ld1 {v0.S}[0], [x0], x2
        ld1 {v0.S}[1], [x0], x2
        uaddw v3.8H, v2.8H, v0.8B
        ld1 {v1.S}[0], [x0], x2
        ld1 {v1.S}[1], [x0], x2
        uaddw v4.8H, v2.8H, v1.8B
        sqxtun v0.8B, v3.8H
        sqxtun v1.8B, v4.8H
        sub x0, x0, x2, lsl #2
        st1 {v0.S}[0], [x0], x2
        st1 {v0.S}[1], [x0], x2
        st1 {v1.S}[0], [x0], x2
        st1 {v1.S}[1], [x0], x2
        ret
 # //.size ff_h264_idct_dc_add_neon, . - ff_h264_idct_dc_add_neon
 # //.endfunc

        .text
        .align 2
        .globl _ff_h264_idct_add16_neon
 # //.type _ff_h264_idct_add16_neon, %function
 # //.func _ff_h264_idct_add16_neon
_ff_h264_idct_add16_neon:
        mov x12, x30
        mov x6, x0
        mov x5, x1
        mov x1, x2
        mov w9, w3

        adrp x7, scan8@PAGE
        add x7, x7, scan8@PAGEOFF


        mov x10, #16

        adrp x13, _ff_h264_idct_dc_add_neon@PAGE
        add x13, x13, _ff_h264_idct_dc_add_neon@PAGEOFF
        adrp x14, _ff_h264_idct_add_neon@PAGE
        add x14, x14, _ff_h264_idct_add_neon@PAGEOFF


1: mov w2, w9
        ldrb w3, [x7], #1
        ldrsw x0, [x5], #4
        ldrb w3, [x4, w3, uxtw]
        subs w3, w3, #1
        b.lt 2f
        ldrsh w3, [x1]
        add x0, x0, x6
        ccmp w3, #0, #4, eq
        csel x15, x13, x14, ne
        blr x15
2: subs x10, x10, #1
        add x1, x1, #32
        b.ne 1b
        ret x12
 # //.size ff_h264_idct_add16_neon, . - ff_h264_idct_add16_neon
 # //.endfunc

        .text
        .align 2
        .globl _ff_h264_idct_add16intra_neon
 # //.type _ff_h264_idct_add16intra_neon, %function
 # //.func _ff_h264_idct_add16intra_neon
_ff_h264_idct_add16intra_neon:
        mov x12, x30
        mov x6, x0
        mov x5, x1
        mov x1, x2
        mov w9, w3

        adrp x7, scan8@PAGE
        add x7, x7, scan8@PAGEOFF


        mov x10, #16

        adrp x13, _ff_h264_idct_dc_add_neon@PAGE
        add x13, x13, _ff_h264_idct_dc_add_neon@PAGEOFF
        adrp x14, _ff_h264_idct_add_neon@PAGE
        add x14, x14, _ff_h264_idct_add_neon@PAGEOFF


1: mov w2, w9
        ldrb w3, [x7], #1
        ldrsw x0, [x5], #4
        ldrb w3, [x4, w3, uxtw]
        add x0, x0, x6
        cmp w3, #0
        ldrsh w3, [x1]
        csel x15, x13, x14, eq
        ccmp w3, #0, #0, eq
        b.eq 2f
        blr x15
2: subs x10, x10, #1
        add x1, x1, #32
        b.ne 1b
        ret x12
 # //.size ff_h264_idct_add16intra_neon, . - ff_h264_idct_add16intra_neon
 # //.endfunc

        .text
        .align 2
        .globl _ff_h264_idct_add8_neon
 # //.type _ff_h264_idct_add8_neon, %function
 # //.func _ff_h264_idct_add8_neon
_ff_h264_idct_add8_neon:
        sub sp, sp, #0x40
        stp x19, x20, [sp]
        mov x12, x30
        ldp x6, x15, [x0]
        add x5, x1, #16*4
        add x9, x2, #16*32
        mov w19, w3

        adrp x13, _ff_h264_idct_dc_add_neon@PAGE
        add x13, x13, _ff_h264_idct_dc_add_neon@PAGEOFF
        adrp x14, _ff_h264_idct_add_neon@PAGE
        add x14, x14, _ff_h264_idct_add_neon@PAGEOFF
        adrp x7, scan8+16@PAGE
        add x7, x7, scan8+16@PAGEOFF


        mov x10, #0
        mov x11, #16
1: mov w2, w19
        ldrb w3, [x7, x10]
        ldrsw x0, [x5, x10, lsl #2]
        ldrb w3, [x4, w3, uxtw]
        add x0, x0, x6
        add x1, x9, x10, lsl #5
        cmp w3, #0
        ldrsh w3, [x1]
        csel x20, x13, x14, eq
        ccmp w3, #0, #0, eq
        b.eq 2f
        blr x20
2: add x10, x10, #1
        cmp x10, #4
        csel x10, x11, x10, eq
        csel x6, x15, x6, eq
        cmp x10, #20
        b.lt 1b
        ldp x19, x20, [sp]
        add sp, sp, #0x40
        ret x12
 # //.size ff_h264_idct_add8_neon, . - ff_h264_idct_add8_neon
 # //.endfunc


        .text
        .align 2
        .globl _ff_h264_idct8_add_neon
 # //.type _ff_h264_idct8_add_neon, %function
 # //.func _ff_h264_idct8_add_neon
_ff_h264_idct8_add_neon:
        movi v19.8H, #0, lsl #0
        ld1 {v24.8H, v25.8H}, [x1]
        st1 {v19.8H}, [x1], #16
        st1 {v19.8H}, [x1], #16
        ld1 {v26.8H, v27.8H}, [x1]
        st1 {v19.8H}, [x1], #16
        st1 {v19.8H}, [x1], #16
        ld1 {v28.8H, v29.8H}, [x1]
        st1 {v19.8H}, [x1], #16
        st1 {v19.8H}, [x1], #16

        sshr v18.8H, v26.8H, #1
        add v16.8H, v24.8H, v28.8H
        ld1 {v30.8H, v31.8H}, [x1]
        st1 {v19.8H}, [x1], #16
        st1 {v19.8H}, [x1], #16
        sub v17.8H, v24.8H, v28.8H
        sshr v19.8H, v30.8H, #1
        sub v18.8H, v18.8H, v30.8H
        add v19.8H, v19.8H, v26.8H
        add v26.8H, v17.8H, v18.8H
        sub v28.8H, v17.8H, v18.8H
        add v24.8H, v16.8H, v19.8H
        sub v30.8H, v16.8H, v19.8H
        sub v16.8H, v29.8H, v27.8H
        add v17.8H, v31.8H, v25.8H
        sub v18.8H, v31.8H, v25.8H
        add v19.8H, v29.8H, v27.8H
        sub v16.8H, v16.8H, v31.8H
        sub v17.8H, v17.8H, v27.8H
        add v18.8H, v18.8H, v29.8H
        add v19.8H, v19.8H, v25.8H
        sshr v25.8H, v25.8H, #1
        sshr v27.8H, v27.8H, #1
        sshr v29.8H, v29.8H, #1
        sshr v31.8H, v31.8H, #1
        sub v16.8H, v16.8H, v31.8H
        sub v17.8H, v17.8H, v27.8H
        add v18.8H, v18.8H, v29.8H
        add v19.8H, v19.8H, v25.8H
        sshr v25.8H, v16.8H, #2
        sshr v27.8H, v17.8H, #2
        sshr v29.8H, v18.8H, #2
        sshr v31.8H, v19.8H, #2
        sub v19.8H, v19.8H, v25.8H
        sub v18.8H, v27.8H, v18.8H
        add v17.8H, v17.8H, v29.8H
        add v16.8H, v16.8H, v31.8H
        sub v31.8H, v24.8H, v19.8H
        add v24.8H, v24.8H, v19.8H
        add v25.8H, v26.8H, v18.8H
        sub v18.8H, v26.8H, v18.8H
        add v26.8H, v28.8H, v17.8H
        add v27.8H, v30.8H, v16.8H
        sub v29.8H, v28.8H, v17.8H
        sub v28.8H, v30.8H, v16.8H
        trn1 v6.8H, v24.8H, v25.8H
        trn2 v7.8H, v24.8H, v25.8H
        trn1 v25.8H, v26.8H, v27.8H
        trn2 v27.8H, v26.8H, v27.8H
        trn1 v24.8H, v28.8H, v29.8H
        trn2 v29.8H, v28.8H, v29.8H
        trn1 v26.8H, v18.8H, v31.8H
        trn2 v31.8H, v18.8H, v31.8H

        trn1 v28.4S, v24.4S, v26.4S
        trn2 v26.4S, v24.4S, v26.4S
        trn1 v18.4S, v29.4S, v31.4S
        trn2 v31.4S, v29.4S, v31.4S
        trn1 v29.4S, v7.4S, v27.4S
        trn2 v7.4S, v7.4S, v27.4S
        trn1 v27.4S, v6.4S, v25.4S
        trn2 v6.4S, v6.4S, v25.4S

        trn1 v24.2D, v27.2D, v28.2D
        trn2 v28.2D, v27.2D, v28.2D

        trn1 v25.2D, v29.2D, v18.2D
        trn2 v29.2D, v29.2D, v18.2D

        trn2 v18.2D, v6.2D, v26.2D
        trn1 v26.2D, v6.2D, v26.2D

        trn1 v27.2D, v7.2D, v31.2D
        trn2 v31.2D, v7.2D, v31.2D

        sshr v30.8H, v26.8H, #1
        sshr v19.8H, v18.8H, #1
        add v16.8H, v24.8H, v28.8H
        sub v17.8H, v24.8H, v28.8H
        sub v30.8H, v30.8H, v18.8H
        add v19.8H, v19.8H, v26.8H
        add v26.8H, v17.8H, v30.8H
        sub v28.8H, v17.8H, v30.8H
        add v24.8H, v16.8H, v19.8H
        sub v18.8H, v16.8H, v19.8H
        sub v16.8H, v29.8H, v27.8H
        add v17.8H, v31.8H, v25.8H
        sub v30.8H, v31.8H, v25.8H
        add v19.8H, v29.8H, v27.8H
        sub v16.8H, v16.8H, v31.8H
        sub v17.8H, v17.8H, v27.8H
        add v30.8H, v30.8H, v29.8H
        add v19.8H, v19.8H, v25.8H
        sshr v25.8H, v25.8H, #1
        sshr v27.8H, v27.8H, #1
        sshr v29.8H, v29.8H, #1
        sshr v31.8H, v31.8H, #1
        sub v16.8H, v16.8H, v31.8H
        sub v17.8H, v17.8H, v27.8H
        add v30.8H, v30.8H, v29.8H
        add v19.8H, v19.8H, v25.8H
        sshr v25.8H, v16.8H, #2
        sshr v27.8H, v17.8H, #2
        sshr v29.8H, v30.8H, #2
        sshr v31.8H, v19.8H, #2
        sub v19.8H, v19.8H, v25.8H
        sub v30.8H, v27.8H, v30.8H
        add v17.8H, v17.8H, v29.8H
        add v16.8H, v16.8H, v31.8H
        sub v31.8H, v24.8H, v19.8H
        add v24.8H, v24.8H, v19.8H
        add v25.8H, v26.8H, v30.8H
        sub v30.8H, v26.8H, v30.8H
        add v26.8H, v28.8H, v17.8H
        sub v29.8H, v28.8H, v17.8H
        add v27.8H, v18.8H, v16.8H
        sub v28.8H, v18.8H, v16.8H

        mov x3, x0
        srshr v24.8H, v24.8H, #6
        ld1 {v0.8B}, [x0], x2
        srshr v25.8H, v25.8H, #6
        ld1 {v1.8B}, [x0], x2
        srshr v26.8H, v26.8H, #6
        ld1 {v2.8B}, [x0], x2
        srshr v27.8H, v27.8H, #6
        ld1 {v3.8B}, [x0], x2
        srshr v28.8H, v28.8H, #6
        ld1 {v4.8B}, [x0], x2
        srshr v29.8H, v29.8H, #6
        ld1 {v5.8B}, [x0], x2
        srshr v30.8H, v30.8H, #6
        ld1 {v6.8B}, [x0], x2
        srshr v31.8H, v31.8H, #6
        ld1 {v7.8B}, [x0], x2
        uaddw v24.8H, v24.8H, v0.8B
        uaddw v25.8H, v25.8H, v1.8B
        uaddw v26.8H, v26.8H, v2.8B
        sqxtun v0.8B, v24.8H
        uaddw v27.8H, v27.8H, v3.8B
        sqxtun v1.8B, v25.8H
        uaddw v28.8H, v28.8H, v4.8B
        sqxtun v2.8B, v26.8H
        st1 {v0.8B}, [x3], x2
        uaddw v29.8H, v29.8H, v5.8B
        sqxtun v3.8B, v27.8H
        st1 {v1.8B}, [x3], x2
        uaddw v30.8H, v30.8H, v6.8B
        sqxtun v4.8B, v28.8H
        st1 {v2.8B}, [x3], x2
        uaddw v31.8H, v31.8H, v7.8B
        sqxtun v5.8B, v29.8H
        st1 {v3.8B}, [x3], x2
        sqxtun v6.8B, v30.8H
        sqxtun v7.8B, v31.8H
        st1 {v4.8B}, [x3], x2
        st1 {v5.8B}, [x3], x2
        st1 {v6.8B}, [x3], x2
        st1 {v7.8B}, [x3], x2

        sub x1, x1, #128
        ret
 # //.size ff_h264_idct8_add_neon, . - ff_h264_idct8_add_neon
 # //.endfunc

        .text
        .align 2
        .globl _ff_h264_idct8_dc_add_neon
 # //.type _ff_h264_idct8_dc_add_neon, %function
 # //.func _ff_h264_idct8_dc_add_neon
_ff_h264_idct8_dc_add_neon:
        mov w3, #0
        sxtw x2, w2
        ld1r {v31.8H}, [x1]
        strh w3, [x1]
        ld1 {v0.8B}, [x0], x2
        srshr v31.8H, v31.8H, #6
        ld1 {v1.8B}, [x0], x2
        ld1 {v2.8B}, [x0], x2
        uaddw v24.8H, v31.8H, v0.8B
        ld1 {v3.8B}, [x0], x2
        uaddw v25.8H, v31.8H, v1.8B
        ld1 {v4.8B}, [x0], x2
        uaddw v26.8H, v31.8H, v2.8B
        ld1 {v5.8B}, [x0], x2
        uaddw v27.8H, v31.8H, v3.8B
        ld1 {v6.8B}, [x0], x2
        uaddw v28.8H, v31.8H, v4.8B
        ld1 {v7.8B}, [x0], x2
        uaddw v29.8H, v31.8H, v5.8B
        uaddw v30.8H, v31.8H, v6.8B
        uaddw v31.8H, v31.8H, v7.8B
        sqxtun v0.8B, v24.8H
        sqxtun v1.8B, v25.8H
        sqxtun v2.8B, v26.8H
        sqxtun v3.8B, v27.8H
        sub x0, x0, x2, lsl #3
        st1 {v0.8B}, [x0], x2
        sqxtun v4.8B, v28.8H
        st1 {v1.8B}, [x0], x2
        sqxtun v5.8B, v29.8H
        st1 {v2.8B}, [x0], x2
        sqxtun v6.8B, v30.8H
        st1 {v3.8B}, [x0], x2
        sqxtun v7.8B, v31.8H
        st1 {v4.8B}, [x0], x2
        st1 {v5.8B}, [x0], x2
        st1 {v6.8B}, [x0], x2
        st1 {v7.8B}, [x0], x2
        ret
 # //.size ff_h264_idct8_dc_add_neon, . - ff_h264_idct8_dc_add_neon
 # //.endfunc

        .text
        .align 2
        .globl _ff_h264_idct8_add4_neon
 # //.type _ff_h264_idct8_add4_neon, %function
 # //.func _ff_h264_idct8_add4_neon
_ff_h264_idct8_add4_neon:
        mov x12, x30
        mov x6, x0
        mov x5, x1
        mov x1, x2
        mov w2, w3

        adrp x7, scan8@PAGE
        add x7, x7, scan8@PAGEOFF


        mov w10, #16

        adrp x13, _ff_h264_idct8_dc_add_neon@PAGE
        add x13, x13, _ff_h264_idct8_dc_add_neon@PAGEOFF
        adrp x14, _ff_h264_idct8_add_neon@PAGE
        add x14, x14, _ff_h264_idct8_add_neon@PAGEOFF


1: ldrb w9, [x7], #4
        ldrsw x0, [x5], #16
        ldrb w9, [x4, w9, UXTW]
        subs w9, w9, #1
        b.lt 2f
        ldrsh w11, [x1]
        add x0, x6, x0
        ccmp w11, #0, #4, eq
        csel x15, x13, x14, ne
        blr x15
2: subs w10, w10, #4
        add x1, x1, #128
        b.ne 1b
        ret x12
 # //.size ff_h264_idct8_add4_neon, . - ff_h264_idct8_add4_neon
 # //.endfunc


        .const_data

        .align 2
scan8:
        .byte 4+ 1*8, 5+ 1*8, 4+ 2*8, 5+ 2*8
        .byte 6+ 1*8, 7+ 1*8, 6+ 2*8, 7+ 2*8
        .byte 4+ 3*8, 5+ 3*8, 4+ 4*8, 5+ 4*8
        .byte 6+ 3*8, 7+ 3*8, 6+ 4*8, 7+ 4*8
        .byte 4+ 6*8, 5+ 6*8, 4+ 7*8, 5+ 7*8
        .byte 6+ 6*8, 7+ 6*8, 6+ 7*8, 7+ 7*8
        .byte 4+ 8*8, 5+ 8*8, 4+ 9*8, 5+ 9*8
        .byte 6+ 8*8, 7+ 8*8, 6+ 9*8, 7+ 9*8
        .byte 4+11*8, 5+11*8, 4+12*8, 5+12*8
        .byte 6+11*8, 7+11*8, 6+12*8, 7+12*8
        .byte 4+13*8, 5+13*8, 4+14*8, 5+14*8
        .byte 6+13*8, 7+13*8, 6+14*8, 7+14*8
 # //.size scan8, . - scan8
.text
.align 2
#endif