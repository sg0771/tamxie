#ifdef __arm64__
        .text
        .align 2
        .globl _ff_h264_v_loop_filter_luma_neon
 # //.type _ff_h264_v_loop_filter_luma_neon, %function
 # //.func _ff_h264_v_loop_filter_luma_neon
_ff_h264_v_loop_filter_luma_neon:
        cmp w2, #0
        ldr w6, [x4]
        ccmp w3, #0, #0, ne
        mov v24.S[0], w6
        and w6, w6, w6, lsl #16
        b.eq 1f
        ands w6, w6, w6, lsl #8
        b.ge 2f
1:
        ret
2:
        sxtw x1, w1

        ld1 {v0.16B}, [x0], x1
        ld1 {v2.16B}, [x0], x1
        ld1 {v4.16B}, [x0], x1
        sub x0, x0, x1, lsl #2
        sub x0, x0, x1, lsl #1
        ld1 {v20.16B}, [x0], x1
        ld1 {v18.16B}, [x0], x1
        ld1 {v16.16B}, [x0], x1

        dup v22.16B, w2
        ushll v24.8H, v24.8B, #0
        uabd v21.16B, v16.16B, v0.16B
        ushll v24.4S, v24.4H, #0
        uabd v28.16B, v18.16B, v16.16B
        sli v24.8H, v24.8H, #8
        uabd v30.16B, v2.16B, v0.16B
        sli v24.4S, v24.4S, #16
        cmhi v21.16B, v22.16B, v21.16B
        dup v22.16B, w3
        cmlt v23.16B, v24.16B, #0
        cmhi v28.16B, v22.16B, v28.16B
        cmhi v30.16B, v22.16B, v30.16B
        bic v21.16B, v21.16B, v23.16B
        uabd v17.16B, v20.16B, v16.16B
        and v21.16B, v21.16B, v28.16B
        uabd v19.16B, v4.16B, v0.16B
        cmhi v17.16B, v22.16B, v17.16B
        and v21.16B, v21.16B, v30.16B
        cmhi v19.16B, v22.16B, v19.16B
        and v17.16B, v17.16B, v21.16B
        and v19.16B, v19.16B, v21.16B
        and v24.16B, v24.16B, v21.16B
        urhadd v28.16B, v16.16B, v0.16B
        sub v21.16B, v24.16B, v17.16B
        uqadd v23.16B, v18.16B, v24.16B
        uhadd v20.16B, v20.16B, v28.16B
        sub v21.16B, v21.16B, v19.16B
        uhadd v28.16B, v4.16B, v28.16B
        umin v23.16B, v23.16B, v20.16B
        uqsub v22.16B, v18.16B, v24.16B
        uqadd v4.16B, v2.16B, v24.16B
        umax v23.16B, v23.16B, v22.16B
        uqsub v22.16B, v2.16B, v24.16B
        umin v28.16B, v4.16B, v28.16B
        ushll v4.8H, v0.8B, #0
        umax v28.16B, v28.16B, v22.16B
        ushll2 v20.8H, v0.16B, #0
        usubw v4.8H, v4.8H, v16.8B
        usubw2 v20.8H, v20.8H, v16.16B
        shl v4.8H, v4.8H, #2
        shl v20.8H, v20.8H, #2
        uaddw v4.8H, v4.8H, v18.8B
        uaddw2 v20.8H, v20.8H, v18.16B
        usubw v4.8H, v4.8H, v2.8B
        usubw2 v20.8H, v20.8H, v2.16B
        rshrn v4.8B, v4.8H, #3
        rshrn2 v4.16B, v20.8H, #3
        bsl v17.16B, v23.16B, v18.16B
        bsl v19.16B, v28.16B, v2.16B
        neg v23.16B, v21.16B
        ushll v28.8H, v16.8B, #0
        smin v4.16B, v4.16B, v21.16B
        ushll2 v21.8H, v16.16B, #0
        smax v4.16B, v4.16B, v23.16B
        ushll v22.8H, v0.8B, #0
        ushll2 v24.8H, v0.16B, #0
        saddw v28.8H, v28.8H, v4.8B
        saddw2 v21.8H, v21.8H, v4.16B
        ssubw v22.8H, v22.8H, v4.8B
        ssubw2 v24.8H, v24.8H, v4.16B
        sqxtun v16.8B, v28.8H
        sqxtun2 v16.16B, v21.8H
        sqxtun v0.8B, v22.8H
        sqxtun2 v0.16B, v24.8H

        sub x0, x0, x1, lsl #1
        st1 {v17.16B}, [x0], x1
        st1 {v16.16B}, [x0], x1
        st1 {v0.16B}, [x0], x1
        st1 {v19.16B}, [x0]

        ret
 # //.size ff_h264_v_loop_filter_luma_neon, . - ff_h264_v_loop_filter_luma_neon
 # //.endfunc

        .text
        .align 2
        .globl _ff_h264_h_loop_filter_luma_neon
 # //.type _ff_h264_h_loop_filter_luma_neon, %function
 # //.func _ff_h264_h_loop_filter_luma_neon
_ff_h264_h_loop_filter_luma_neon:
        cmp w2, #0
        ldr w6, [x4]
        ccmp w3, #0, #0, ne
        mov v24.S[0], w6
        and w6, w6, w6, lsl #16
        b.eq 1f
        ands w6, w6, w6, lsl #8
        b.ge 2f
1:
        ret
2:

        sub x0, x0, #4
        ld1 {v6.8B}, [x0], x1
        ld1 {v20.8B}, [x0], x1
        ld1 {v18.8B}, [x0], x1
        ld1 {v16.8B}, [x0], x1
        ld1 {v0.8B}, [x0], x1
        ld1 {v2.8B}, [x0], x1
        ld1 {v4.8B}, [x0], x1
        ld1 {v26.8B}, [x0], x1
        ld1 {v6.D}[1], [x0], x1
        ld1 {v20.D}[1], [x0], x1
        ld1 {v18.D}[1], [x0], x1
        ld1 {v16.D}[1], [x0], x1
        ld1 {v0.D}[1], [x0], x1
        ld1 {v2.D}[1], [x0], x1
        ld1 {v4.D}[1], [x0], x1
        ld1 {v26.D}[1], [x0], x1

        trn1 v21.16B, v6.16B, v20.16B
        trn2 v23.16B, v6.16B, v20.16B
        trn1 v20.16B, v18.16B, v16.16B
        trn2 v16.16B, v18.16B, v16.16B
        trn1 v6.16B, v0.16B, v2.16B
        trn2 v2.16B, v0.16B, v2.16B
        trn1 v18.16B, v4.16B, v26.16B
        trn2 v26.16B, v4.16B, v26.16B

        trn1 v0.8H, v6.8H, v18.8H
        trn2 v18.8H, v6.8H, v18.8H
        trn1 v4.8H, v2.8H, v26.8H
        trn2 v26.8H, v2.8H, v26.8H
        trn1 v2.8H, v23.8H, v16.8H
        trn2 v23.8H, v23.8H, v16.8H
        trn1 v16.8H, v21.8H, v20.8H
        trn2 v21.8H, v21.8H, v20.8H

        trn1 v6.4S, v16.4S, v0.4S
        trn2 v0.4S, v16.4S, v0.4S

        trn1 v20.4S, v2.4S, v4.4S
        trn2 v2.4S, v2.4S, v4.4S

        trn2 v4.4S, v21.4S, v18.4S
        trn1 v18.4S, v21.4S, v18.4S

        trn1 v16.4S, v23.4S, v26.4S
        trn2 v26.4S, v23.4S, v26.4S

        dup v22.16B, w2
        ushll v24.8H, v24.8B, #0
        uabd v21.16B, v16.16B, v0.16B
        ushll v24.4S, v24.4H, #0
        uabd v28.16B, v18.16B, v16.16B
        sli v24.8H, v24.8H, #8
        uabd v30.16B, v2.16B, v0.16B
        sli v24.4S, v24.4S, #16
        cmhi v21.16B, v22.16B, v21.16B
        dup v22.16B, w3
        cmlt v23.16B, v24.16B, #0
        cmhi v28.16B, v22.16B, v28.16B
        cmhi v30.16B, v22.16B, v30.16B
        bic v21.16B, v21.16B, v23.16B
        uabd v17.16B, v20.16B, v16.16B
        and v21.16B, v21.16B, v28.16B
        uabd v19.16B, v4.16B, v0.16B
        cmhi v17.16B, v22.16B, v17.16B
        and v21.16B, v21.16B, v30.16B
        cmhi v19.16B, v22.16B, v19.16B
        and v17.16B, v17.16B, v21.16B
        and v19.16B, v19.16B, v21.16B
        and v24.16B, v24.16B, v21.16B
        urhadd v28.16B, v16.16B, v0.16B
        sub v21.16B, v24.16B, v17.16B
        uqadd v23.16B, v18.16B, v24.16B
        uhadd v20.16B, v20.16B, v28.16B
        sub v21.16B, v21.16B, v19.16B
        uhadd v28.16B, v4.16B, v28.16B
        umin v23.16B, v23.16B, v20.16B
        uqsub v22.16B, v18.16B, v24.16B
        uqadd v4.16B, v2.16B, v24.16B
        umax v23.16B, v23.16B, v22.16B
        uqsub v22.16B, v2.16B, v24.16B
        umin v28.16B, v4.16B, v28.16B
        ushll v4.8H, v0.8B, #0
        umax v28.16B, v28.16B, v22.16B
        ushll2 v20.8H, v0.16B, #0
        usubw v4.8H, v4.8H, v16.8B
        usubw2 v20.8H, v20.8H, v16.16B
        shl v4.8H, v4.8H, #2
        shl v20.8H, v20.8H, #2
        uaddw v4.8H, v4.8H, v18.8B
        uaddw2 v20.8H, v20.8H, v18.16B
        usubw v4.8H, v4.8H, v2.8B
        usubw2 v20.8H, v20.8H, v2.16B
        rshrn v4.8B, v4.8H, #3
        rshrn2 v4.16B, v20.8H, #3
        bsl v17.16B, v23.16B, v18.16B
        bsl v19.16B, v28.16B, v2.16B
        neg v23.16B, v21.16B
        ushll v28.8H, v16.8B, #0
        smin v4.16B, v4.16B, v21.16B
        ushll2 v21.8H, v16.16B, #0
        smax v4.16B, v4.16B, v23.16B
        ushll v22.8H, v0.8B, #0
        ushll2 v24.8H, v0.16B, #0
        saddw v28.8H, v28.8H, v4.8B
        saddw2 v21.8H, v21.8H, v4.16B
        ssubw v22.8H, v22.8H, v4.8B
        ssubw2 v24.8H, v24.8H, v4.16B
        sqxtun v16.8B, v28.8H
        sqxtun2 v16.16B, v21.8H
        sqxtun v0.8B, v22.8H
        sqxtun2 v0.16B, v24.8H

        trn1 v21.16B, v17.16B, v16.16B
        trn2 v23.16B, v17.16B, v16.16B
        trn1 v25.16B, v0.16B, v19.16B
        trn2 v27.16B, v0.16B, v19.16B

        trn1 v17.8H, v21.8H, v25.8H
        trn2 v0.8H, v21.8H, v25.8H
        trn1 v16.8H, v23.8H, v27.8H
        trn2 v19.8H, v23.8H, v27.8H

        sub x0, x0, x1, lsl #4
        add x0, x0, #2
        st1 {v17.S}[0], [x0], x1
        st1 {v16.S}[0], [x0], x1
        st1 {v0.S}[0], [x0], x1
        st1 {v19.S}[0], [x0], x1
        st1 {v17.S}[1], [x0], x1
        st1 {v16.S}[1], [x0], x1
        st1 {v0.S}[1], [x0], x1
        st1 {v19.S}[1], [x0], x1
        st1 {v17.S}[2], [x0], x1
        st1 {v16.S}[2], [x0], x1
        st1 {v0.S}[2], [x0], x1
        st1 {v19.S}[2], [x0], x1
        st1 {v17.S}[3], [x0], x1
        st1 {v16.S}[3], [x0], x1
        st1 {v0.S}[3], [x0], x1
        st1 {v19.S}[3], [x0], x1

        ret
 # //.size ff_h264_h_loop_filter_luma_neon, . - ff_h264_h_loop_filter_luma_neon
 # //.endfunc


        .text
        .align 2
        .globl _ff_h264_v_loop_filter_chroma_neon
 # //.type _ff_h264_v_loop_filter_chroma_neon, %function
 # //.func _ff_h264_v_loop_filter_chroma_neon
_ff_h264_v_loop_filter_chroma_neon:
        cmp w2, #0
        ldr w6, [x4]
        ccmp w3, #0, #0, ne
        mov v24.S[0], w6
        and w6, w6, w6, lsl #16
        b.eq 1f
        ands w6, w6, w6, lsl #8
        b.ge 2f
1:
        ret
2:

        sub x0, x0, x1, lsl #1
        ld1 {v18.8B}, [x0], x1
        ld1 {v16.8B}, [x0], x1
        ld1 {v0.8B}, [x0], x1
        ld1 {v2.8B}, [x0]

        dup v22.8B, w2
        ushll v24.8H, v24.8B, #0
        uabd v26.8B, v16.8B, v0.8B
        ushll v4.8H, v0.8B, #0
        uabd v28.8B, v18.8B, v16.8B
        usubw v4.8H, v4.8H, v16.8B
        sli v24.8H, v24.8H, #8
        shl v4.8H, v4.8H, #2
        uabd v30.8B, v2.8B, v0.8B
        uaddw v4.8H, v4.8H, v18.8B
        cmhi v26.8B, v22.8B, v26.8B
        usubw v4.8H, v4.8H, v2.8B
        dup v22.8B, w3
        rshrn v4.8B, v4.8H, #3
        cmhi v28.8B, v22.8B, v28.8B
        cmhi v30.8B, v22.8B, v30.8B
        smin v4.8B, v4.8B, v24.8B
        neg v25.8B, v24.8B
        and v26.8B, v26.8B, v28.8B
        smax v4.8B, v4.8B, v25.8B
        and v26.8B, v26.8B, v30.8B
        ushll v22.8H, v0.8B, #0
        and v4.8B, v4.8B, v26.8B
        ushll v28.8H, v16.8B, #0
        saddw v28.8H, v28.8H, v4.8B
        ssubw v22.8H, v22.8H, v4.8B
        sqxtun v16.8B, v28.8H
        sqxtun v0.8B, v22.8H

        sub x0, x0, x1, lsl #1
        st1 {v16.8B}, [x0], x1
        st1 {v0.8B}, [x0], x1

        ret
 # //.size ff_h264_v_loop_filter_chroma_neon, . - ff_h264_v_loop_filter_chroma_neon
 # //.endfunc

        .text
        .align 2
        .globl _ff_h264_h_loop_filter_chroma_neon
 # //.type _ff_h264_h_loop_filter_chroma_neon, %function
 # //.func _ff_h264_h_loop_filter_chroma_neon
_ff_h264_h_loop_filter_chroma_neon:
        cmp w2, #0
        ldr w6, [x4]
        ccmp w3, #0, #0, ne
        mov v24.S[0], w6
        and w6, w6, w6, lsl #16
        b.eq 1f
        ands w6, w6, w6, lsl #8
        b.ge 2f
1:
        ret
2:

        sub x0, x0, #2
        ld1 {v18.S}[0], [x0], x1
        ld1 {v16.S}[0], [x0], x1
        ld1 {v0.S}[0], [x0], x1
        ld1 {v2.S}[0], [x0], x1
        ld1 {v18.S}[1], [x0], x1
        ld1 {v16.S}[1], [x0], x1
        ld1 {v0.S}[1], [x0], x1
        ld1 {v2.S}[1], [x0], x1

        trn1 v28.8B, v18.8B, v16.8B
        trn2 v29.8B, v18.8B, v16.8B
        trn1 v30.8B, v0.8B, v2.8B
        trn2 v31.8B, v0.8B, v2.8B

        trn1 v18.4H, v28.4H, v30.4H
        trn2 v0.4H, v28.4H, v30.4H
        trn1 v16.4H, v29.4H, v31.4H
        trn2 v2.4H, v29.4H, v31.4H

        dup v22.8B, w2
        ushll v24.8H, v24.8B, #0
        uabd v26.8B, v16.8B, v0.8B
        ushll v4.8H, v0.8B, #0
        uabd v28.8B, v18.8B, v16.8B
        usubw v4.8H, v4.8H, v16.8B
        sli v24.8H, v24.8H, #8
        shl v4.8H, v4.8H, #2
        uabd v30.8B, v2.8B, v0.8B
        uaddw v4.8H, v4.8H, v18.8B
        cmhi v26.8B, v22.8B, v26.8B
        usubw v4.8H, v4.8H, v2.8B
        dup v22.8B, w3
        rshrn v4.8B, v4.8H, #3
        cmhi v28.8B, v22.8B, v28.8B
        cmhi v30.8B, v22.8B, v30.8B
        smin v4.8B, v4.8B, v24.8B
        neg v25.8B, v24.8B
        and v26.8B, v26.8B, v28.8B
        smax v4.8B, v4.8B, v25.8B
        and v26.8B, v26.8B, v30.8B
        ushll v22.8H, v0.8B, #0
        and v4.8B, v4.8B, v26.8B
        ushll v28.8H, v16.8B, #0
        saddw v28.8H, v28.8H, v4.8B
        ssubw v22.8H, v22.8H, v4.8B
        sqxtun v16.8B, v28.8H
        sqxtun v0.8B, v22.8H

        trn1 v28.8B, v18.8B, v16.8B
        trn2 v29.8B, v18.8B, v16.8B
        trn1 v30.8B, v0.8B, v2.8B
        trn2 v31.8B, v0.8B, v2.8B

        trn1 v18.4H, v28.4H, v30.4H
        trn2 v0.4H, v28.4H, v30.4H
        trn1 v16.4H, v29.4H, v31.4H
        trn2 v2.4H, v29.4H, v31.4H

        sub x0, x0, x1, lsl #3
        st1 {v18.S}[0], [x0], x1
        st1 {v16.S}[0], [x0], x1
        st1 {v0.S}[0], [x0], x1
        st1 {v2.S}[0], [x0], x1
        st1 {v18.S}[1], [x0], x1
        st1 {v16.S}[1], [x0], x1
        st1 {v0.S}[1], [x0], x1
        st1 {v2.S}[1], [x0], x1

        ret
 # //.size ff_h264_h_loop_filter_chroma_neon, . - ff_h264_h_loop_filter_chroma_neon
 # //.endfunc

        .text
        .align 2
        .globl _ff_biweight_h264_pixels_16_neon
 # //.type _ff_biweight_h264_pixels_16_neon, %function
 # //.func _ff_biweight_h264_pixels_16_neon
_ff_biweight_h264_pixels_16_neon:
        sxtw x2, w2
        lsr w8, w5, #31
        add w7, w7, #1
        eor w8, w8, w6, lsr #30
        orr w7, w7, #1
        dup v18.8H, w4
        lsl w7, w7, w4
        not v18.16B, v18.16B
        dup v16.8H, w7
        mov x7, x0
        cbz w8, 10f
        subs w8, w8, #1
        b.eq 20f
        subs w8, w8, #1
        b.eq 30f
        b 40f
10:        dup v0.16B, w5
        dup v1.16B, w6
        orr v4.16B, v16.16B, v16.16B
        orr v6.16B, v16.16B, v16.16B
1: subs w3, w3, #2
        ld1 {v20.16B}, [x0], x2
        umlal v4.8H, v0.8B, v20.8B
        umlal2 v6.8H, v0.16B, v20.16B
        ld1 {v22.16B}, [x1], x2
        umlal v4.8H, v1.8B, v22.8B
        umlal2 v6.8H, v1.16B, v22.16B
        orr v24.16B, v16.16B, v16.16B
        ld1 {v28.16B}, [x0], x2
        orr v26.16B, v16.16B, v16.16B
        umlal v24.8H, v0.8B, v28.8B
        umlal2 v26.8H, v0.16B, v28.16B
        ld1 {v30.16B}, [x1], x2
        umlal v24.8H, v1.8B, v30.8B
        umlal2 v26.8H, v1.16B, v30.16B
        sshl v4.8H, v4.8H, v18.8H
        sshl v6.8H, v6.8H, v18.8H
        sqxtun v4.8B, v4.8H
        sqxtun2 v4.16B, v6.8H
        sshl v24.8H, v24.8H, v18.8H
        sshl v26.8H, v26.8H, v18.8H
        sqxtun v24.8B, v24.8H
        sqxtun2 v24.16B, v26.8H
        orr v6.16B, v16.16B, v16.16B
        st1 {v4.16B}, [x7], x2
        orr v4.16B, v16.16B, v16.16B
        st1 {v24.16B}, [x7], x2
        b.ne 1b
        ret
20: neg w5, w5
        dup v0.16B, w5
        dup v1.16B, w6
        orr v4.16B, v16.16B, v16.16B
        orr v6.16B, v16.16B, v16.16B
1: subs w3, w3, #2
        ld1 {v20.16B}, [x0], x2
        umlsl v4.8H, v0.8B, v20.8B
        umlsl2 v6.8H, v0.16B, v20.16B
        ld1 {v22.16B}, [x1], x2
        umlal v4.8H, v1.8B, v22.8B
        umlal2 v6.8H, v1.16B, v22.16B
        orr v24.16B, v16.16B, v16.16B
        ld1 {v28.16B}, [x0], x2
        orr v26.16B, v16.16B, v16.16B
        umlsl v24.8H, v0.8B, v28.8B
        umlsl2 v26.8H, v0.16B, v28.16B
        ld1 {v30.16B}, [x1], x2
        umlal v24.8H, v1.8B, v30.8B
        umlal2 v26.8H, v1.16B, v30.16B
        sshl v4.8H, v4.8H, v18.8H
        sshl v6.8H, v6.8H, v18.8H
        sqxtun v4.8B, v4.8H
        sqxtun2 v4.16B, v6.8H
        sshl v24.8H, v24.8H, v18.8H
        sshl v26.8H, v26.8H, v18.8H
        sqxtun v24.8B, v24.8H
        sqxtun2 v24.16B, v26.8H
        orr v6.16B, v16.16B, v16.16B
        st1 {v4.16B}, [x7], x2
        orr v4.16B, v16.16B, v16.16B
        st1 {v24.16B}, [x7], x2
        b.ne 1b
        ret
30: neg w5, w5
        neg w6, w6
        dup v0.16B, w5
        dup v1.16B, w6
        orr v4.16B, v16.16B, v16.16B
        orr v6.16B, v16.16B, v16.16B
1: subs w3, w3, #2
        ld1 {v20.16B}, [x0], x2
        umlsl v4.8H, v0.8B, v20.8B
        umlsl2 v6.8H, v0.16B, v20.16B
        ld1 {v22.16B}, [x1], x2
        umlsl v4.8H, v1.8B, v22.8B
        umlsl2 v6.8H, v1.16B, v22.16B
        orr v24.16B, v16.16B, v16.16B
        ld1 {v28.16B}, [x0], x2
        orr v26.16B, v16.16B, v16.16B
        umlsl v24.8H, v0.8B, v28.8B
        umlsl2 v26.8H, v0.16B, v28.16B
        ld1 {v30.16B}, [x1], x2
        umlsl v24.8H, v1.8B, v30.8B
        umlsl2 v26.8H, v1.16B, v30.16B
        sshl v4.8H, v4.8H, v18.8H
        sshl v6.8H, v6.8H, v18.8H
        sqxtun v4.8B, v4.8H
        sqxtun2 v4.16B, v6.8H
        sshl v24.8H, v24.8H, v18.8H
        sshl v26.8H, v26.8H, v18.8H
        sqxtun v24.8B, v24.8H
        sqxtun2 v24.16B, v26.8H
        orr v6.16B, v16.16B, v16.16B
        st1 {v4.16B}, [x7], x2
        orr v4.16B, v16.16B, v16.16B
        st1 {v24.16B}, [x7], x2
        b.ne 1b
        ret
40: neg w6, w6
        dup v0.16B, w5
        dup v1.16B, w6
        orr v4.16B, v16.16B, v16.16B
        orr v6.16B, v16.16B, v16.16B
1: subs w3, w3, #2
        ld1 {v20.16B}, [x0], x2
        umlal v4.8H, v0.8B, v20.8B
        umlal2 v6.8H, v0.16B, v20.16B
        ld1 {v22.16B}, [x1], x2
        umlsl v4.8H, v1.8B, v22.8B
        umlsl2 v6.8H, v1.16B, v22.16B
        orr v24.16B, v16.16B, v16.16B
        ld1 {v28.16B}, [x0], x2
        orr v26.16B, v16.16B, v16.16B
        umlal v24.8H, v0.8B, v28.8B
        umlal2 v26.8H, v0.16B, v28.16B
        ld1 {v30.16B}, [x1], x2
        umlsl v24.8H, v1.8B, v30.8B
        umlsl2 v26.8H, v1.16B, v30.16B
        sshl v4.8H, v4.8H, v18.8H
        sshl v6.8H, v6.8H, v18.8H
        sqxtun v4.8B, v4.8H
        sqxtun2 v4.16B, v6.8H
        sshl v24.8H, v24.8H, v18.8H
        sshl v26.8H, v26.8H, v18.8H
        sqxtun v24.8B, v24.8H
        sqxtun2 v24.16B, v26.8H
        orr v6.16B, v16.16B, v16.16B
        st1 {v4.16B}, [x7], x2
        orr v4.16B, v16.16B, v16.16B
        st1 {v24.16B}, [x7], x2
        b.ne 1b
        ret
 # //.size ff_biweight_h264_pixels_16_neon, . - ff_biweight_h264_pixels_16_neon
 # //.endfunc
        .text
        .align 2
        .globl _ff_biweight_h264_pixels_8_neon
 # //.type _ff_biweight_h264_pixels_8_neon, %function
 # //.func _ff_biweight_h264_pixels_8_neon
_ff_biweight_h264_pixels_8_neon:
        sxtw x2, w2
        lsr w8, w5, #31
        add w7, w7, #1
        eor w8, w8, w6, lsr #30
        orr w7, w7, #1
        dup v18.8H, w4
        lsl w7, w7, w4
        not v18.16B, v18.16B
        dup v16.8H, w7
        mov x7, x0
        cbz w8, 10f
        subs w8, w8, #1
        b.eq 20f
        subs w8, w8, #1
        b.eq 30f
        b 40f
10:        dup v0.8B, w5
        dup v1.8B, w6
        orr v2.16B, v16.16B, v16.16B
        orr v20.16B, v16.16B, v16.16B
1: subs w3, w3, #2
        ld1 {v4.8B}, [x0], x2
        umlal v2.8H, v0.8B, v4.8B
        ld1 {v5.8B}, [x1], x2
        umlal v2.8H, v1.8B, v5.8B
        ld1 {v6.8B}, [x0], x2
        umlal v20.8H, v0.8B, v6.8B
        ld1 {v7.8B}, [x1], x2
        umlal v20.8H, v1.8B, v7.8B
        sshl v2.8H, v2.8H, v18.8H
        sqxtun v2.8B, v2.8H
        sshl v20.8H, v20.8H, v18.8H
        sqxtun v4.8B, v20.8H
        orr v20.16B, v16.16B, v16.16B
        st1 {v2.8B}, [x7], x2
        orr v2.16B, v16.16B, v16.16B
        st1 {v4.8B}, [x7], x2
        b.ne 1b
        ret
20: neg w5, w5
        dup v0.8B, w5
        dup v1.8B, w6
        orr v2.16B, v16.16B, v16.16B
        orr v20.16B, v16.16B, v16.16B
1: subs w3, w3, #2
        ld1 {v4.8B}, [x0], x2
        umlsl v2.8H, v0.8B, v4.8B
        ld1 {v5.8B}, [x1], x2
        umlal v2.8H, v1.8B, v5.8B
        ld1 {v6.8B}, [x0], x2
        umlsl v20.8H, v0.8B, v6.8B
        ld1 {v7.8B}, [x1], x2
        umlal v20.8H, v1.8B, v7.8B
        sshl v2.8H, v2.8H, v18.8H
        sqxtun v2.8B, v2.8H
        sshl v20.8H, v20.8H, v18.8H
        sqxtun v4.8B, v20.8H
        orr v20.16B, v16.16B, v16.16B
        st1 {v2.8B}, [x7], x2
        orr v2.16B, v16.16B, v16.16B
        st1 {v4.8B}, [x7], x2
        b.ne 1b
        ret
30: neg w5, w5
        neg w6, w6
        dup v0.8B, w5
        dup v1.8B, w6
        orr v2.16B, v16.16B, v16.16B
        orr v20.16B, v16.16B, v16.16B
1: subs w3, w3, #2
        ld1 {v4.8B}, [x0], x2
        umlsl v2.8H, v0.8B, v4.8B
        ld1 {v5.8B}, [x1], x2
        umlsl v2.8H, v1.8B, v5.8B
        ld1 {v6.8B}, [x0], x2
        umlsl v20.8H, v0.8B, v6.8B
        ld1 {v7.8B}, [x1], x2
        umlsl v20.8H, v1.8B, v7.8B
        sshl v2.8H, v2.8H, v18.8H
        sqxtun v2.8B, v2.8H
        sshl v20.8H, v20.8H, v18.8H
        sqxtun v4.8B, v20.8H
        orr v20.16B, v16.16B, v16.16B
        st1 {v2.8B}, [x7], x2
        orr v2.16B, v16.16B, v16.16B
        st1 {v4.8B}, [x7], x2
        b.ne 1b
        ret
40: neg w6, w6
        dup v0.8B, w5
        dup v1.8B, w6
        orr v2.16B, v16.16B, v16.16B
        orr v20.16B, v16.16B, v16.16B
1: subs w3, w3, #2
        ld1 {v4.8B}, [x0], x2
        umlal v2.8H, v0.8B, v4.8B
        ld1 {v5.8B}, [x1], x2
        umlsl v2.8H, v1.8B, v5.8B
        ld1 {v6.8B}, [x0], x2
        umlal v20.8H, v0.8B, v6.8B
        ld1 {v7.8B}, [x1], x2
        umlsl v20.8H, v1.8B, v7.8B
        sshl v2.8H, v2.8H, v18.8H
        sqxtun v2.8B, v2.8H
        sshl v20.8H, v20.8H, v18.8H
        sqxtun v4.8B, v20.8H
        orr v20.16B, v16.16B, v16.16B
        st1 {v2.8B}, [x7], x2
        orr v2.16B, v16.16B, v16.16B
        st1 {v4.8B}, [x7], x2
        b.ne 1b
        ret
 # //.size ff_biweight_h264_pixels_8_neon, . - ff_biweight_h264_pixels_8_neon
 # //.endfunc
        .text
        .align 2
        .globl _ff_biweight_h264_pixels_4_neon
 # //.type _ff_biweight_h264_pixels_4_neon, %function
 # //.func _ff_biweight_h264_pixels_4_neon
_ff_biweight_h264_pixels_4_neon:
        sxtw x2, w2
        lsr w8, w5, #31
        add w7, w7, #1
        eor w8, w8, w6, lsr #30
        orr w7, w7, #1
        dup v18.8H, w4
        lsl w7, w7, w4
        not v18.16B, v18.16B
        dup v16.8H, w7
        mov x7, x0
        cbz w8, 10f
        subs w8, w8, #1
        b.eq 20f
        subs w8, w8, #1
        b.eq 30f
        b 40f
10:        dup v0.8B, w5
        dup v1.8B, w6
        orr v2.16B, v16.16B, v16.16B
        orr v20.16B, v16.16B, v16.16B
1: subs w3, w3, #4
        ld1 {v4.S}[0], [x0], x2
        ld1 {v4.S}[1], [x0], x2
        umlal v2.8H, v0.8B, v4.8B
        ld1 {v5.S}[0], [x1], x2
        ld1 {v5.S}[1], [x1], x2
        umlal v2.8H, v1.8B, v5.8B
        b.lt 2f
        ld1 {v6.S}[0], [x0], x2
        ld1 {v6.S}[1], [x0], x2
        umlal v20.8H, v0.8B, v6.8B
        ld1 {v7.S}[0], [x1], x2
        ld1 {v7.S}[1], [x1], x2
        umlal v20.8H, v1.8B, v7.8B
        sshl v2.8H, v2.8H, v18.8H
        sqxtun v2.8B, v2.8H
        sshl v20.8H, v20.8H, v18.8H
        sqxtun v4.8B, v20.8H
        orr v20.16B, v16.16B, v16.16B
        st1 {v2.S}[0], [x7], x2
        st1 {v2.S}[1], [x7], x2
        orr v2.16B, v16.16B, v16.16B
        st1 {v4.S}[0], [x7], x2
        st1 {v4.S}[1], [x7], x2
        b.ne 1b
        ret
2: sshl v2.8H, v2.8H, v18.8H
        sqxtun v2.8B, v2.8H
        st1 {v2.S}[0], [x7], x2
        st1 {v2.S}[1], [x7], x2
        ret
20: neg w5, w5
        dup v0.8B, w5
        dup v1.8B, w6
        orr v2.16B, v16.16B, v16.16B
        orr v20.16B, v16.16B, v16.16B
1: subs w3, w3, #4
        ld1 {v4.S}[0], [x0], x2
        ld1 {v4.S}[1], [x0], x2
        umlsl v2.8H, v0.8B, v4.8B
        ld1 {v5.S}[0], [x1], x2
        ld1 {v5.S}[1], [x1], x2
        umlal v2.8H, v1.8B, v5.8B
        b.lt 2f
        ld1 {v6.S}[0], [x0], x2
        ld1 {v6.S}[1], [x0], x2
        umlsl v20.8H, v0.8B, v6.8B
        ld1 {v7.S}[0], [x1], x2
        ld1 {v7.S}[1], [x1], x2
        umlal v20.8H, v1.8B, v7.8B
        sshl v2.8H, v2.8H, v18.8H
        sqxtun v2.8B, v2.8H
        sshl v20.8H, v20.8H, v18.8H
        sqxtun v4.8B, v20.8H
        orr v20.16B, v16.16B, v16.16B
        st1 {v2.S}[0], [x7], x2
        st1 {v2.S}[1], [x7], x2
        orr v2.16B, v16.16B, v16.16B
        st1 {v4.S}[0], [x7], x2
        st1 {v4.S}[1], [x7], x2
        b.ne 1b
        ret
2: sshl v2.8H, v2.8H, v18.8H
        sqxtun v2.8B, v2.8H
        st1 {v2.S}[0], [x7], x2
        st1 {v2.S}[1], [x7], x2
        ret
30: neg w5, w5
        neg w6, w6
        dup v0.8B, w5
        dup v1.8B, w6
        orr v2.16B, v16.16B, v16.16B
        orr v20.16B, v16.16B, v16.16B
1: subs w3, w3, #4
        ld1 {v4.S}[0], [x0], x2
        ld1 {v4.S}[1], [x0], x2
        umlsl v2.8H, v0.8B, v4.8B
        ld1 {v5.S}[0], [x1], x2
        ld1 {v5.S}[1], [x1], x2
        umlsl v2.8H, v1.8B, v5.8B
        b.lt 2f
        ld1 {v6.S}[0], [x0], x2
        ld1 {v6.S}[1], [x0], x2
        umlsl v20.8H, v0.8B, v6.8B
        ld1 {v7.S}[0], [x1], x2
        ld1 {v7.S}[1], [x1], x2
        umlsl v20.8H, v1.8B, v7.8B
        sshl v2.8H, v2.8H, v18.8H
        sqxtun v2.8B, v2.8H
        sshl v20.8H, v20.8H, v18.8H
        sqxtun v4.8B, v20.8H
        orr v20.16B, v16.16B, v16.16B
        st1 {v2.S}[0], [x7], x2
        st1 {v2.S}[1], [x7], x2
        orr v2.16B, v16.16B, v16.16B
        st1 {v4.S}[0], [x7], x2
        st1 {v4.S}[1], [x7], x2
        b.ne 1b
        ret
2: sshl v2.8H, v2.8H, v18.8H
        sqxtun v2.8B, v2.8H
        st1 {v2.S}[0], [x7], x2
        st1 {v2.S}[1], [x7], x2
        ret
40: neg w6, w6
        dup v0.8B, w5
        dup v1.8B, w6
        orr v2.16B, v16.16B, v16.16B
        orr v20.16B, v16.16B, v16.16B
1: subs w3, w3, #4
        ld1 {v4.S}[0], [x0], x2
        ld1 {v4.S}[1], [x0], x2
        umlal v2.8H, v0.8B, v4.8B
        ld1 {v5.S}[0], [x1], x2
        ld1 {v5.S}[1], [x1], x2
        umlsl v2.8H, v1.8B, v5.8B
        b.lt 2f
        ld1 {v6.S}[0], [x0], x2
        ld1 {v6.S}[1], [x0], x2
        umlal v20.8H, v0.8B, v6.8B
        ld1 {v7.S}[0], [x1], x2
        ld1 {v7.S}[1], [x1], x2
        umlsl v20.8H, v1.8B, v7.8B
        sshl v2.8H, v2.8H, v18.8H
        sqxtun v2.8B, v2.8H
        sshl v20.8H, v20.8H, v18.8H
        sqxtun v4.8B, v20.8H
        orr v20.16B, v16.16B, v16.16B
        st1 {v2.S}[0], [x7], x2
        st1 {v2.S}[1], [x7], x2
        orr v2.16B, v16.16B, v16.16B
        st1 {v4.S}[0], [x7], x2
        st1 {v4.S}[1], [x7], x2
        b.ne 1b
        ret
2: sshl v2.8H, v2.8H, v18.8H
        sqxtun v2.8B, v2.8H
        st1 {v2.S}[0], [x7], x2
        st1 {v2.S}[1], [x7], x2
        ret
 # //.size ff_biweight_h264_pixels_4_neon, . - ff_biweight_h264_pixels_4_neon
 # //.endfunc

        .text
        .align 2
        .globl _ff_weight_h264_pixels_16_neon
 # //.type _ff_weight_h264_pixels_16_neon, %function
 # //.func _ff_weight_h264_pixels_16_neon
_ff_weight_h264_pixels_16_neon:
        sxtw x1, w1
        cmp w3, #1
        mov w6, #1
        lsl w5, w5, w3
        dup v16.8H, w5
        mov x5, x0
        b.le 20f
        sub w6, w6, w3
        dup v18.8H, w6
        cmp w4, #0
        b.lt 10f
        dup v0.16B, w4
1: subs w2, w2, #2
        ld1 {v20.16B}, [x0], x1
        umull v4.8H, v0.8B, v20.8B
        umull2 v6.8H, v0.16B, v20.16B
        ld1 {v28.16B}, [x0], x1
        umull v24.8H, v0.8B, v28.8B
        umull2 v26.8H, v0.16B, v28.16B
        shadd v4.8H, v16.8H, v4.8H
        srshl v4.8H, v4.8H, v18.8H
        shadd v6.8H, v16.8H, v6.8H
        srshl v6.8H, v6.8H, v18.8H
        sqxtun v4.8B, v4.8H
        sqxtun2 v4.16B, v6.8H
        shadd v24.8H, v16.8H, v24.8H
        srshl v24.8H, v24.8H, v18.8H
        shadd v26.8H, v16.8H, v26.8H
        srshl v26.8H, v26.8H, v18.8H
        sqxtun v24.8B, v24.8H
        sqxtun2 v24.16B, v26.8H
        st1 {v4.16B}, [x5], x1
        st1 {v24.16B}, [x5], x1
        b.ne 1b
        ret
10: neg w4, w4
        dup v0.16B, w4
1: subs w2, w2, #2
        ld1 {v20.16B}, [x0], x1
        umull v4.8H, v0.8B, v20.8B
        umull2 v6.8H, v0.16B, v20.16B
        ld1 {v28.16B}, [x0], x1
        umull v24.8H, v0.8B, v28.8B
        umull2 v26.8H, v0.16B, v28.16B
        shsub v4.8H, v16.8H, v4.8H
        srshl v4.8H, v4.8H, v18.8H
        shsub v6.8H, v16.8H, v6.8H
        srshl v6.8H, v6.8H, v18.8H
        sqxtun v4.8B, v4.8H
        sqxtun2 v4.16B, v6.8H
        shsub v24.8H, v16.8H, v24.8H
        srshl v24.8H, v24.8H, v18.8H
        shsub v26.8H, v16.8H, v26.8H
        srshl v26.8H, v26.8H, v18.8H
        sqxtun v24.8B, v24.8H
        sqxtun2 v24.16B, v26.8H
        st1 {v4.16B}, [x5], x1
        st1 {v24.16B}, [x5], x1
        b.ne 1b
        ret
20: neg w6, w3
        dup v18.8H, w6
        cmp w4, #0
        b.lt 10f
        dup v0.16B, w4
1: subs w2, w2, #2
        ld1 {v20.16B}, [x0], x1
        umull v4.8H, v0.8B, v20.8B
        umull2 v6.8H, v0.16B, v20.16B
        ld1 {v28.16B}, [x0], x1
        umull v24.8H, v0.8B, v28.8B
        umull2 v26.8H, v0.16B, v28.16B
        add v4.8H, v16.8H, v4.8H
        srshl v4.8H, v4.8H, v18.8H
        add v6.8H, v16.8H, v6.8H
        srshl v6.8H, v6.8H, v18.8H
        sqxtun v4.8B, v4.8H
        sqxtun2 v4.16B, v6.8H
        add v24.8H, v16.8H, v24.8H
        srshl v24.8H, v24.8H, v18.8H
        add v26.8H, v16.8H, v26.8H
        srshl v26.8H, v26.8H, v18.8H
        sqxtun v24.8B, v24.8H
        sqxtun2 v24.16B, v26.8H
        st1 {v4.16B}, [x5], x1
        st1 {v24.16B}, [x5], x1
        b.ne 1b
        ret
10: neg w4, w4
        dup v0.16B, w4
1: subs w2, w2, #2
        ld1 {v20.16B}, [x0], x1
        umull v4.8H, v0.8B, v20.8B
        umull2 v6.8H, v0.16B, v20.16B
        ld1 {v28.16B}, [x0], x1
        umull v24.8H, v0.8B, v28.8B
        umull2 v26.8H, v0.16B, v28.16B
        sub v4.8H, v16.8H, v4.8H
        srshl v4.8H, v4.8H, v18.8H
        sub v6.8H, v16.8H, v6.8H
        srshl v6.8H, v6.8H, v18.8H
        sqxtun v4.8B, v4.8H
        sqxtun2 v4.16B, v6.8H
        sub v24.8H, v16.8H, v24.8H
        srshl v24.8H, v24.8H, v18.8H
        sub v26.8H, v16.8H, v26.8H
        srshl v26.8H, v26.8H, v18.8H
        sqxtun v24.8B, v24.8H
        sqxtun2 v24.16B, v26.8H
        st1 {v4.16B}, [x5], x1
        st1 {v24.16B}, [x5], x1
        b.ne 1b
        ret
 # //.size ff_weight_h264_pixels_16_neon, . - ff_weight_h264_pixels_16_neon
 # //.endfunc
        .text
        .align 2
        .globl _ff_weight_h264_pixels_8_neon
 # //.type _ff_weight_h264_pixels_8_neon, %function
 # //.func _ff_weight_h264_pixels_8_neon
_ff_weight_h264_pixels_8_neon:
        sxtw x1, w1
        cmp w3, #1
        mov w6, #1
        lsl w5, w5, w3
        dup v16.8H, w5
        mov x5, x0
        b.le 20f
        sub w6, w6, w3
        dup v18.8H, w6
        cmp w4, #0
        b.lt 10f
        dup v0.8B, w4
1: subs w2, w2, #2
        ld1 {v4.8B}, [x0], x1
        umull v2.8H, v0.8B, v4.8B
        ld1 {v6.8B}, [x0], x1
        umull v20.8H, v0.8B, v6.8B
        shadd v2.8H, v16.8H, v2.8H
        srshl v2.8H, v2.8H, v18.8H
        sqxtun v2.8B, v2.8H
        shadd v20.8H, v16.8H, v20.8H
        srshl v20.8H, v20.8H, v18.8H
        sqxtun v4.8B, v20.8H
        st1 {v2.8B}, [x5], x1
        st1 {v4.8B}, [x5], x1
        b.ne 1b
        ret
10: neg w4, w4
        dup v0.8B, w4
1: subs w2, w2, #2
        ld1 {v4.8B}, [x0], x1
        umull v2.8H, v0.8B, v4.8B
        ld1 {v6.8B}, [x0], x1
        umull v20.8H, v0.8B, v6.8B
        shsub v2.8H, v16.8H, v2.8H
        srshl v2.8H, v2.8H, v18.8H
        sqxtun v2.8B, v2.8H
        shsub v20.8H, v16.8H, v20.8H
        srshl v20.8H, v20.8H, v18.8H
        sqxtun v4.8B, v20.8H
        st1 {v2.8B}, [x5], x1
        st1 {v4.8B}, [x5], x1
        b.ne 1b
        ret
20: neg w6, w3
        dup v18.8H, w6
        cmp w4, #0
        b.lt 10f
        dup v0.8B, w4
1: subs w2, w2, #2
        ld1 {v4.8B}, [x0], x1
        umull v2.8H, v0.8B, v4.8B
        ld1 {v6.8B}, [x0], x1
        umull v20.8H, v0.8B, v6.8B
        add v2.8H, v16.8H, v2.8H
        srshl v2.8H, v2.8H, v18.8H
        sqxtun v2.8B, v2.8H
        add v20.8H, v16.8H, v20.8H
        srshl v20.8H, v20.8H, v18.8H
        sqxtun v4.8B, v20.8H
        st1 {v2.8B}, [x5], x1
        st1 {v4.8B}, [x5], x1
        b.ne 1b
        ret
10: neg w4, w4
        dup v0.8B, w4
1: subs w2, w2, #2
        ld1 {v4.8B}, [x0], x1
        umull v2.8H, v0.8B, v4.8B
        ld1 {v6.8B}, [x0], x1
        umull v20.8H, v0.8B, v6.8B
        sub v2.8H, v16.8H, v2.8H
        srshl v2.8H, v2.8H, v18.8H
        sqxtun v2.8B, v2.8H
        sub v20.8H, v16.8H, v20.8H
        srshl v20.8H, v20.8H, v18.8H
        sqxtun v4.8B, v20.8H
        st1 {v2.8B}, [x5], x1
        st1 {v4.8B}, [x5], x1
        b.ne 1b
        ret
 # //.size ff_weight_h264_pixels_8_neon, . - ff_weight_h264_pixels_8_neon
 # //.endfunc
        .text
        .align 2
        .globl _ff_weight_h264_pixels_4_neon
 # //.type _ff_weight_h264_pixels_4_neon, %function
 # //.func _ff_weight_h264_pixels_4_neon
_ff_weight_h264_pixels_4_neon:
        sxtw x1, w1
        cmp w3, #1
        mov w6, #1
        lsl w5, w5, w3
        dup v16.8H, w5
        mov x5, x0
        b.le 20f
        sub w6, w6, w3
        dup v18.8H, w6
        cmp w4, #0
        b.lt 10f
        dup v0.8B, w4
1: subs w2, w2, #4
        ld1 {v4.S}[0], [x0], x1
        ld1 {v4.S}[1], [x0], x1
        umull v2.8H, v0.8B, v4.8B
        b.lt 2f
        ld1 {v6.S}[0], [x0], x1
        ld1 {v6.S}[1], [x0], x1
        umull v20.8H, v0.8B, v6.8B
        shadd v2.8H, v16.8H, v2.8H
        srshl v2.8H, v2.8H, v18.8H
        sqxtun v2.8B, v2.8H
        shadd v20.8H, v16.8H, v20.8H
        srshl v20.8H, v20.8h, v18.8H
        sqxtun v4.8B, v20.8H
        st1 {v2.S}[0], [x5], x1
        st1 {v2.S}[1], [x5], x1
        st1 {v4.S}[0], [x5], x1
        st1 {v4.S}[1], [x5], x1
        b.ne 1b
        ret
2: shadd v2.8H, v16.8H, v2.8H
        srshl v2.8H, v2.8H, v18.8H
        sqxtun v2.8B, v2.8H
        st1 {v2.S}[0], [x5], x1
        st1 {v2.S}[1], [x5], x1
        ret
10: neg w4, w4
        dup v0.8B, w4
1: subs w2, w2, #4
        ld1 {v4.S}[0], [x0], x1
        ld1 {v4.S}[1], [x0], x1
        umull v2.8H, v0.8B, v4.8B
        b.lt 2f
        ld1 {v6.S}[0], [x0], x1
        ld1 {v6.S}[1], [x0], x1
        umull v20.8H, v0.8B, v6.8B
        shsub v2.8H, v16.8H, v2.8H
        srshl v2.8H, v2.8H, v18.8H
        sqxtun v2.8B, v2.8H
        shsub v20.8H, v16.8H, v20.8H
        srshl v20.8H, v20.8h, v18.8H
        sqxtun v4.8B, v20.8H
        st1 {v2.S}[0], [x5], x1
        st1 {v2.S}[1], [x5], x1
        st1 {v4.S}[0], [x5], x1
        st1 {v4.S}[1], [x5], x1
        b.ne 1b
        ret
2: shsub v2.8H, v16.8H, v2.8H
        srshl v2.8H, v2.8H, v18.8H
        sqxtun v2.8B, v2.8H
        st1 {v2.S}[0], [x5], x1
        st1 {v2.S}[1], [x5], x1
        ret
20: neg w6, w3
        dup v18.8H, w6
        cmp w4, #0
        b.lt 10f
        dup v0.8B, w4
1: subs w2, w2, #4
        ld1 {v4.S}[0], [x0], x1
        ld1 {v4.S}[1], [x0], x1
        umull v2.8H, v0.8B, v4.8B
        b.lt 2f
        ld1 {v6.S}[0], [x0], x1
        ld1 {v6.S}[1], [x0], x1
        umull v20.8H, v0.8B, v6.8B
        add v2.8H, v16.8H, v2.8H
        srshl v2.8H, v2.8H, v18.8H
        sqxtun v2.8B, v2.8H
        add v20.8H, v16.8H, v20.8H
        srshl v20.8H, v20.8h, v18.8H
        sqxtun v4.8B, v20.8H
        st1 {v2.S}[0], [x5], x1
        st1 {v2.S}[1], [x5], x1
        st1 {v4.S}[0], [x5], x1
        st1 {v4.S}[1], [x5], x1
        b.ne 1b
        ret
2: add v2.8H, v16.8H, v2.8H
        srshl v2.8H, v2.8H, v18.8H
        sqxtun v2.8B, v2.8H
        st1 {v2.S}[0], [x5], x1
        st1 {v2.S}[1], [x5], x1
        ret
10: neg w4, w4
        dup v0.8B, w4
1: subs w2, w2, #4
        ld1 {v4.S}[0], [x0], x1
        ld1 {v4.S}[1], [x0], x1
        umull v2.8H, v0.8B, v4.8B
        b.lt 2f
        ld1 {v6.S}[0], [x0], x1
        ld1 {v6.S}[1], [x0], x1
        umull v20.8H, v0.8B, v6.8B
        sub v2.8H, v16.8H, v2.8H
        srshl v2.8H, v2.8H, v18.8H
        sqxtun v2.8B, v2.8H
        sub v20.8H, v16.8H, v20.8H
        srshl v20.8H, v20.8h, v18.8H
        sqxtun v4.8B, v20.8H
        st1 {v2.S}[0], [x5], x1
        st1 {v2.S}[1], [x5], x1
        st1 {v4.S}[0], [x5], x1
        st1 {v4.S}[1], [x5], x1
        b.ne 1b
        ret
2: sub v2.8H, v16.8H, v2.8H
        srshl v2.8H, v2.8H, v18.8H
        sqxtun v2.8B, v2.8H
        st1 {v2.S}[0], [x5], x1
        st1 {v2.S}[1], [x5], x1
        ret
 # //.size ff_weight_h264_pixels_4_neon, . - ff_weight_h264_pixels_4_neon
 # //.endfunc
.text
.align 2
#endif
