#ifdef __arm64__
        .text
        .align 2
 # //.type put_h264_qpel16_h_lowpass_neon_packed, %function
 # //.func put_h264_qpel16_h_lowpass_neon_packed
put_h264_qpel16_h_lowpass_neon_packed:
        mov x4, x30
        mov x12, #16
        mov x3, #8
        bl put_h264_qpel8_h_lowpass_neon
        sub x1, x1, x2, lsl #4
        add x1, x1, #8
        mov x12, #16
        mov x30, x4
        b put_h264_qpel8_h_lowpass_neon
 # //.size put_h264_qpel16_h_lowpass_neon_packed, . - put_h264_qpel16_h_lowpass_neon_packed
 # //.endfunc


        .text
        .align 2
 # //.type put_h264_qpel16_h_lowpass_neon, %function
 # //.func put_h264_qpel16_h_lowpass_neon
put_h264_qpel16_h_lowpass_neon:
        mov x13, x30
        mov x12, #16
        bl put_h264_qpel8_h_lowpass_neon
        sub x0, x0, x3, lsl #4
        sub x1, x1, x2, lsl #4
        add x0, x0, #8
        add x1, x1, #8
        mov x12, #16
        mov x30, x13
 # //.size put_h264_qpel16_h_lowpass_neon, . - put_h264_qpel16_h_lowpass_neon
 # //.endfunc

        .text
        .align 2
 # //.type put_h264_qpel8_h_lowpass_neon, %function
 # //.func put_h264_qpel8_h_lowpass_neon
put_h264_qpel8_h_lowpass_neon:
1: ld1 {v28.8B, v29.8B}, [x1], x2
        ld1 {v16.8B, v17.8B}, [x1], x2
        subs x12, x12, #2
        ext v2.8B, v28.8B, v29.8B, #2
        ext v3.8B, v28.8B, v29.8B, #3
        uaddl v2.8H, v2.8B, v3.8B
        ext v4.8B, v28.8B, v29.8B, #1
        ext v5.8B, v28.8B, v29.8B, #4
        uaddl v4.8H, v4.8B, v5.8B
        ext v1.8B, v28.8B, v29.8B, #5
        uaddl v28.8H, v28.8B, v1.8B
        ext v0.8B, v16.8B, v17.8B, #2
        mla v28.8H, v2.8H, v6.H[1]
        ext v1.8B, v16.8B, v17.8B, #3
        uaddl v0.8H, v0.8B, v1.8B
        ext v1.8B, v16.8B, v17.8B, #1
        mls v28.8H, v4.8H, v6.H[0]
        ext v3.8B, v16.8B, v17.8B, #4
        uaddl v1.8H, v1.8B, v3.8B
        ext v2.8B, v16.8B, v17.8B, #5
        uaddl v16.8H, v16.8B, v2.8B
        mla v16.8H, v0.8H, v6.H[1]
        mls v16.8H, v1.8H, v6.H[0]
        sqrshrun v28.8B, v28.8H, #5
        sqrshrun v16.8B, v16.8H, #5
        st1 {v28.8B}, [x0], x3
        st1 {v16.8B}, [x0], x3
        b.ne 1b
        ret
 # //.size put_h264_qpel8_h_lowpass_neon, . - put_h264_qpel8_h_lowpass_neon
 # //.endfunc
        .text
        .align 2
 # //.type avg_h264_qpel16_h_lowpass_neon, %function
 # //.func avg_h264_qpel16_h_lowpass_neon
avg_h264_qpel16_h_lowpass_neon:
        mov x13, x30
        mov x12, #16
        bl avg_h264_qpel8_h_lowpass_neon
        sub x0, x0, x3, lsl #4
        sub x1, x1, x2, lsl #4
        add x0, x0, #8
        add x1, x1, #8
        mov x12, #16
        mov x30, x13
 # //.size avg_h264_qpel16_h_lowpass_neon, . - avg_h264_qpel16_h_lowpass_neon
 # //.endfunc

        .text
        .align 2
 # //.type avg_h264_qpel8_h_lowpass_neon, %function
 # //.func avg_h264_qpel8_h_lowpass_neon
avg_h264_qpel8_h_lowpass_neon:
1: ld1 {v28.8B, v29.8B}, [x1], x2
        ld1 {v16.8B, v17.8B}, [x1], x2
        subs x12, x12, #2
        ext v2.8B, v28.8B, v29.8B, #2
        ext v3.8B, v28.8B, v29.8B, #3
        uaddl v2.8H, v2.8B, v3.8B
        ext v4.8B, v28.8B, v29.8B, #1
        ext v5.8B, v28.8B, v29.8B, #4
        uaddl v4.8H, v4.8B, v5.8B
        ext v1.8B, v28.8B, v29.8B, #5
        uaddl v28.8H, v28.8B, v1.8B
        ext v0.8B, v16.8B, v17.8B, #2
        mla v28.8H, v2.8H, v6.H[1]
        ext v1.8B, v16.8B, v17.8B, #3
        uaddl v0.8H, v0.8B, v1.8B
        ext v1.8B, v16.8B, v17.8B, #1
        mls v28.8H, v4.8H, v6.H[0]
        ext v3.8B, v16.8B, v17.8B, #4
        uaddl v1.8H, v1.8B, v3.8B
        ext v2.8B, v16.8B, v17.8B, #5
        uaddl v16.8H, v16.8B, v2.8B
        mla v16.8H, v0.8H, v6.H[1]
        mls v16.8H, v1.8H, v6.H[0]
        sqrshrun v28.8B, v28.8H, #5
        sqrshrun v16.8B, v16.8H, #5
        ld1 {v2.8B}, [x0], x3
        urhadd v28.8B, v28.8B, v2.8B
        ld1 {v3.8B}, [x0]
        urhadd v16.8B, v16.8B, v3.8B
        sub x0, x0, x3
        st1 {v28.8B}, [x0], x3
        st1 {v16.8B}, [x0], x3
        b.ne 1b
        ret
 # //.size avg_h264_qpel8_h_lowpass_neon, . - avg_h264_qpel8_h_lowpass_neon
 # //.endfunc


        .text
        .align 2
 # //.type put_h264_qpel16_h_lowpass_l2_neon, %function
 # //.func put_h264_qpel16_h_lowpass_l2_neon
put_h264_qpel16_h_lowpass_l2_neon:
        mov x13, x30
        mov x12, #16
        bl put_h264_qpel8_h_lowpass_l2_neon
        sub x0, x0, x2, lsl #4
        sub x1, x1, x2, lsl #4
        sub x3, x3, x2, lsl #4
        add x0, x0, #8
        add x1, x1, #8
        add x3, x3, #8
        mov x12, #16
        mov x30, x13
 # //.size put_h264_qpel16_h_lowpass_l2_neon, . - put_h264_qpel16_h_lowpass_l2_neon
 # //.endfunc

        .text
        .align 2
 # //.type put_h264_qpel8_h_lowpass_l2_neon, %function
 # //.func put_h264_qpel8_h_lowpass_l2_neon
put_h264_qpel8_h_lowpass_l2_neon:
1: ld1 {v26.8B, v27.8B}, [x1], x2
        ld1 {v16.8B, v17.8B}, [x1], x2
        ld1 {v28.8B}, [x3], x2
        ld1 {v29.8B}, [x3], x2
        subs x12, x12, #2
        ext v2.8B, v26.8B, v27.8B, #2
        ext v3.8B, v26.8B, v27.8B, #3
        uaddl v2.8H, v2.8B, v3.8B
        ext v4.8B, v26.8B, v27.8B, #1
        ext v5.8B, v26.8B, v27.8B, #4
        uaddl v4.8H, v4.8B, v5.8B
        ext v1.8B, v26.8B, v27.8B, #5
        uaddl v26.8H, v26.8B, v1.8B
        ext v0.8B, v16.8B, v17.8B, #2
        mla v26.8H, v2.8H, v6.H[1]
        ext v1.8B, v16.8B, v17.8B, #3
        uaddl v0.8H, v0.8B, v1.8B
        ext v1.8B, v16.8B, v17.8B, #1
        mls v26.8H, v4.8H, v6.H[0]
        ext v3.8B, v16.8B, v17.8B, #4
        uaddl v1.8H, v1.8B, v3.8B
        ext v2.8B, v16.8B, v17.8B, #5
        uaddl v27.8H, v16.8B, v2.8B
        mla v27.8H, v0.8H, v6.H[1]
        mls v27.8H, v1.8H, v6.H[0]
        sqrshrun v26.8B, v26.8H, #5
        sqrshrun v27.8B, v27.8H, #5
        urhadd v26.8B, v26.8B, v28.8B
        urhadd v27.8B, v27.8B, v29.8B
        st1 {v26.8B}, [x0], x2
        st1 {v27.8B}, [x0], x2
        b.ne 1b
        ret
 # //.size put_h264_qpel8_h_lowpass_l2_neon, . - put_h264_qpel8_h_lowpass_l2_neon
 # //.endfunc
        .text
        .align 2
 # //.type avg_h264_qpel16_h_lowpass_l2_neon, %function
 # //.func avg_h264_qpel16_h_lowpass_l2_neon
avg_h264_qpel16_h_lowpass_l2_neon:
        mov x13, x30
        mov x12, #16
        bl avg_h264_qpel8_h_lowpass_l2_neon
        sub x0, x0, x2, lsl #4
        sub x1, x1, x2, lsl #4
        sub x3, x3, x2, lsl #4
        add x0, x0, #8
        add x1, x1, #8
        add x3, x3, #8
        mov x12, #16
        mov x30, x13
 # //.size avg_h264_qpel16_h_lowpass_l2_neon, . - avg_h264_qpel16_h_lowpass_l2_neon
 # //.endfunc

        .text
        .align 2
 # //.type avg_h264_qpel8_h_lowpass_l2_neon, %function
 # //.func avg_h264_qpel8_h_lowpass_l2_neon
avg_h264_qpel8_h_lowpass_l2_neon:
1: ld1 {v26.8B, v27.8B}, [x1], x2
        ld1 {v16.8B, v17.8B}, [x1], x2
        ld1 {v28.8B}, [x3], x2
        ld1 {v29.8B}, [x3], x2
        subs x12, x12, #2
        ext v2.8B, v26.8B, v27.8B, #2
        ext v3.8B, v26.8B, v27.8B, #3
        uaddl v2.8H, v2.8B, v3.8B
        ext v4.8B, v26.8B, v27.8B, #1
        ext v5.8B, v26.8B, v27.8B, #4
        uaddl v4.8H, v4.8B, v5.8B
        ext v1.8B, v26.8B, v27.8B, #5
        uaddl v26.8H, v26.8B, v1.8B
        ext v0.8B, v16.8B, v17.8B, #2
        mla v26.8H, v2.8H, v6.H[1]
        ext v1.8B, v16.8B, v17.8B, #3
        uaddl v0.8H, v0.8B, v1.8B
        ext v1.8B, v16.8B, v17.8B, #1
        mls v26.8H, v4.8H, v6.H[0]
        ext v3.8B, v16.8B, v17.8B, #4
        uaddl v1.8H, v1.8B, v3.8B
        ext v2.8B, v16.8B, v17.8B, #5
        uaddl v27.8H, v16.8B, v2.8B
        mla v27.8H, v0.8H, v6.H[1]
        mls v27.8H, v1.8H, v6.H[0]
        sqrshrun v26.8B, v26.8H, #5
        sqrshrun v27.8B, v27.8H, #5
        urhadd v26.8B, v26.8B, v28.8B
        urhadd v27.8B, v27.8B, v29.8B
        ld1 {v2.8B}, [x0], x2
        urhadd v26.8B, v26.8B, v2.8B
        ld1 {v3.8B}, [x0]
        urhadd v27.8B, v27.8B, v3.8B
        sub x0, x0, x2
        st1 {v26.8B}, [x0], x2
        st1 {v27.8B}, [x0], x2
        b.ne 1b
        ret
 # //.size avg_h264_qpel8_h_lowpass_l2_neon, . - avg_h264_qpel8_h_lowpass_l2_neon
 # //.endfunc

        .text
        .align 2
 # //.type put_h264_qpel16_v_lowpass_neon_packed, %function
 # //.func put_h264_qpel16_v_lowpass_neon_packed
put_h264_qpel16_v_lowpass_neon_packed:
        mov x4, x30
        mov x2, #8
        bl put_h264_qpel8_v_lowpass_neon
        sub x1, x1, x3, lsl #2
        bl put_h264_qpel8_v_lowpass_neon
        sub x1, x1, x3, lsl #4
        sub x1, x1, x3, lsl #2
        add x1, x1, #8
        bl put_h264_qpel8_v_lowpass_neon
        sub x1, x1, x3, lsl #2
        mov x30, x4
        b put_h264_qpel8_v_lowpass_neon
 # //.size put_h264_qpel16_v_lowpass_neon_packed, . - put_h264_qpel16_v_lowpass_neon_packed
 # //.endfunc


        .text
        .align 2
 # //.type put_h264_qpel16_v_lowpass_neon, %function
 # //.func put_h264_qpel16_v_lowpass_neon
put_h264_qpel16_v_lowpass_neon:
        mov x4, x30
        bl put_h264_qpel8_v_lowpass_neon
        sub x1, x1, x3, lsl #2
        bl put_h264_qpel8_v_lowpass_neon
        sub x0, x0, x2, lsl #4
        add x0, x0, #8
        sub x1, x1, x3, lsl #4
        sub x1, x1, x3, lsl #2
        add x1, x1, #8
        bl put_h264_qpel8_v_lowpass_neon
        sub x1, x1, x3, lsl #2
        mov x30, x4
 # //.size put_h264_qpel16_v_lowpass_neon, . - put_h264_qpel16_v_lowpass_neon
 # //.endfunc

        .text
        .align 2
 # //.type put_h264_qpel8_v_lowpass_neon, %function
 # //.func put_h264_qpel8_v_lowpass_neon
put_h264_qpel8_v_lowpass_neon:
        ld1 {v16.8B}, [x1], x3
        ld1 {v18.8B}, [x1], x3
        ld1 {v20.8B}, [x1], x3
        ld1 {v22.8B}, [x1], x3
        ld1 {v24.8B}, [x1], x3
        ld1 {v26.8B}, [x1], x3
        ld1 {v28.8B}, [x1], x3
        ld1 {v30.8B}, [x1], x3
        ld1 {v17.8B}, [x1], x3
        ld1 {v19.8B}, [x1], x3
        ld1 {v21.8B}, [x1], x3
        ld1 {v23.8B}, [x1], x3
        ld1 {v25.8B}, [x1]

        trn1 v0.8B, v16.8B, v18.8B
        trn2 v1.8B, v16.8B, v18.8B
        trn1 v18.8B, v20.8B, v22.8B
        trn2 v22.8B, v20.8B, v22.8B
        trn1 v16.8B, v24.8B, v26.8B
        trn2 v26.8B, v24.8B, v26.8B
        trn1 v20.8B, v28.8B, v30.8B
        trn2 v30.8B, v28.8B, v30.8B

        trn1 v24.4H, v16.4H, v20.4H
        trn2 v20.4H, v16.4H, v20.4H
        trn1 v28.4H, v26.4H, v30.4H
        trn2 v30.4H, v26.4H, v30.4H
        trn1 v26.4H, v1.4H, v22.4H
        trn2 v1.4H, v1.4H, v22.4H
        trn1 v22.4H, v0.4H, v18.4H
        trn2 v0.4H, v0.4H, v18.4H

        trn1 v16.2S, v22.2S, v24.2S
        trn2 v24.2S, v22.2S, v24.2S

        trn1 v18.2S, v26.2S, v28.2S
        trn2 v26.2S, v26.2S, v28.2S

        trn2 v28.2S, v0.2S, v20.2S
        trn1 v20.2S, v0.2S, v20.2S

        trn1 v22.2S, v1.2S, v30.2S
        trn2 v30.2S, v1.2S, v30.2S
        trn1 v0.8B, v17.8B, v19.8B
        trn2 v1.8B, v17.8B, v19.8B
        trn1 v19.8B, v21.8B, v23.8B
        trn2 v23.8B, v21.8B, v23.8B
        trn1 v17.8B, v25.8B, v27.8B
        trn2 v27.8B, v25.8B, v27.8B
        trn1 v21.8B, v29.8B, v31.8B
        trn2 v31.8B, v29.8B, v31.8B

        trn1 v25.4H, v17.4H, v21.4H
        trn2 v21.4H, v17.4H, v21.4H
        trn1 v29.4H, v27.4H, v31.4H
        trn2 v31.4H, v27.4H, v31.4H
        trn1 v27.4H, v1.4H, v23.4H
        trn2 v1.4H, v1.4H, v23.4H
        trn1 v23.4H, v0.4H, v19.4H
        trn2 v0.4H, v0.4H, v19.4H

        trn1 v17.2S, v23.2S, v25.2S
        trn2 v25.2S, v23.2S, v25.2S

        trn1 v19.2S, v27.2S, v29.2S
        trn2 v27.2S, v27.2S, v29.2S

        trn2 v29.2S, v0.2S, v21.2S
        trn1 v21.2S, v0.2S, v21.2S

        trn1 v23.2S, v1.2S, v31.2S
        trn2 v31.2S, v1.2S, v31.2S
        ext v2.8B, v16.8B, v17.8B, #2
        ext v3.8B, v16.8B, v17.8B, #3
        uaddl v2.8H, v2.8B, v3.8B
        ext v4.8B, v16.8B, v17.8B, #1
        ext v5.8B, v16.8B, v17.8B, #4
        uaddl v4.8H, v4.8B, v5.8B
        ext v1.8B, v16.8B, v17.8B, #5
        uaddl v16.8H, v16.8B, v1.8B
        ext v0.8B, v18.8B, v19.8B, #2
        mla v16.8H, v2.8H, v6.H[1]
        ext v1.8B, v18.8B, v19.8B, #3
        uaddl v0.8H, v0.8B, v1.8B
        ext v1.8B, v18.8B, v19.8B, #1
        mls v16.8H, v4.8H, v6.H[0]
        ext v3.8B, v18.8B, v19.8B, #4
        uaddl v1.8H, v1.8B, v3.8B
        ext v2.8B, v18.8B, v19.8B, #5
        uaddl v17.8H, v18.8B, v2.8B
        mla v17.8H, v0.8H, v6.H[1]
        mls v17.8H, v1.8H, v6.H[0]
        sqrshrun v16.8B, v16.8H, #5
        sqrshrun v17.8B, v17.8H, #5
        ext v2.8B, v20.8B, v21.8B, #2
        ext v3.8B, v20.8B, v21.8B, #3
        uaddl v2.8H, v2.8B, v3.8B
        ext v4.8B, v20.8B, v21.8B, #1
        ext v5.8B, v20.8B, v21.8B, #4
        uaddl v4.8H, v4.8B, v5.8B
        ext v1.8B, v20.8B, v21.8B, #5
        uaddl v18.8H, v20.8B, v1.8B
        ext v0.8B, v22.8B, v23.8B, #2
        mla v18.8H, v2.8H, v6.H[1]
        ext v1.8B, v22.8B, v23.8B, #3
        uaddl v0.8H, v0.8B, v1.8B
        ext v1.8B, v22.8B, v23.8B, #1
        mls v18.8H, v4.8H, v6.H[0]
        ext v3.8B, v22.8B, v23.8B, #4
        uaddl v1.8H, v1.8B, v3.8B
        ext v2.8B, v22.8B, v23.8B, #5
        uaddl v19.8H, v22.8B, v2.8B
        mla v19.8H, v0.8H, v6.H[1]
        mls v19.8H, v1.8H, v6.H[0]
        sqrshrun v18.8B, v18.8H, #5
        sqrshrun v19.8B, v19.8H, #5
        ext v2.8B, v24.8B, v25.8B, #2
        ext v3.8B, v24.8B, v25.8B, #3
        uaddl v2.8H, v2.8B, v3.8B
        ext v4.8B, v24.8B, v25.8B, #1
        ext v5.8B, v24.8B, v25.8B, #4
        uaddl v4.8H, v4.8B, v5.8B
        ext v1.8B, v24.8B, v25.8B, #5
        uaddl v20.8H, v24.8B, v1.8B
        ext v0.8B, v26.8B, v27.8B, #2
        mla v20.8H, v2.8H, v6.H[1]
        ext v1.8B, v26.8B, v27.8B, #3
        uaddl v0.8H, v0.8B, v1.8B
        ext v1.8B, v26.8B, v27.8B, #1
        mls v20.8H, v4.8H, v6.H[0]
        ext v3.8B, v26.8B, v27.8B, #4
        uaddl v1.8H, v1.8B, v3.8B
        ext v2.8B, v26.8B, v27.8B, #5
        uaddl v21.8H, v26.8B, v2.8B
        mla v21.8H, v0.8H, v6.H[1]
        mls v21.8H, v1.8H, v6.H[0]
        sqrshrun v20.8B, v20.8H, #5
        sqrshrun v21.8B, v21.8H, #5
        ext v2.8B, v28.8B, v29.8B, #2
        ext v3.8B, v28.8B, v29.8B, #3
        uaddl v2.8H, v2.8B, v3.8B
        ext v4.8B, v28.8B, v29.8B, #1
        ext v5.8B, v28.8B, v29.8B, #4
        uaddl v4.8H, v4.8B, v5.8B
        ext v1.8B, v28.8B, v29.8B, #5
        uaddl v22.8H, v28.8B, v1.8B
        ext v0.8B, v30.8B, v31.8B, #2
        mla v22.8H, v2.8H, v6.H[1]
        ext v1.8B, v30.8B, v31.8B, #3
        uaddl v0.8H, v0.8B, v1.8B
        ext v1.8B, v30.8B, v31.8B, #1
        mls v22.8H, v4.8H, v6.H[0]
        ext v3.8B, v30.8B, v31.8B, #4
        uaddl v1.8H, v1.8B, v3.8B
        ext v2.8B, v30.8B, v31.8B, #5
        uaddl v23.8H, v30.8B, v2.8B
        mla v23.8H, v0.8H, v6.H[1]
        mls v23.8H, v1.8H, v6.H[0]
        sqrshrun v22.8B, v22.8H, #5
        sqrshrun v23.8B, v23.8H, #5
        trn1 v0.8B, v16.8B, v17.8B
        trn2 v1.8B, v16.8B, v17.8B
        trn1 v17.8B, v18.8B, v19.8B
        trn2 v19.8B, v18.8B, v19.8B
        trn1 v16.8B, v20.8B, v21.8B
        trn2 v21.8B, v20.8B, v21.8B
        trn1 v18.8B, v22.8B, v23.8B
        trn2 v23.8B, v22.8B, v23.8B

        trn1 v20.4H, v16.4H, v18.4H
        trn2 v18.4H, v16.4H, v18.4H
        trn1 v22.4H, v21.4H, v23.4H
        trn2 v23.4H, v21.4H, v23.4H
        trn1 v21.4H, v1.4H, v19.4H
        trn2 v1.4H, v1.4H, v19.4H
        trn1 v19.4H, v0.4H, v17.4H
        trn2 v0.4H, v0.4H, v17.4H

        trn1 v16.2S, v19.2S, v20.2S
        trn2 v20.2S, v19.2S, v20.2S

        trn1 v17.2S, v21.2S, v22.2S
        trn2 v21.2S, v21.2S, v22.2S

        trn2 v22.2S, v0.2S, v18.2S
        trn1 v18.2S, v0.2S, v18.2S

        trn1 v19.2S, v1.2S, v23.2S
        trn2 v23.2S, v1.2S, v23.2S


        st1 {v16.8B}, [x0], x2
        st1 {v17.8B}, [x0], x2
        st1 {v18.8B}, [x0], x2
        st1 {v19.8B}, [x0], x2
        st1 {v20.8B}, [x0], x2
        st1 {v21.8B}, [x0], x2
        st1 {v22.8B}, [x0], x2
        st1 {v23.8B}, [x0], x2

        ret
 # //.size put_h264_qpel8_v_lowpass_neon, . - put_h264_qpel8_v_lowpass_neon
 # //.endfunc
        .text
        .align 2
 # //.type avg_h264_qpel16_v_lowpass_neon, %function
 # //.func avg_h264_qpel16_v_lowpass_neon
avg_h264_qpel16_v_lowpass_neon:
        mov x4, x30
        bl avg_h264_qpel8_v_lowpass_neon
        sub x1, x1, x3, lsl #2
        bl avg_h264_qpel8_v_lowpass_neon
        sub x0, x0, x2, lsl #4
        add x0, x0, #8
        sub x1, x1, x3, lsl #4
        sub x1, x1, x3, lsl #2
        add x1, x1, #8
        bl avg_h264_qpel8_v_lowpass_neon
        sub x1, x1, x3, lsl #2
        mov x30, x4
 # //.size avg_h264_qpel16_v_lowpass_neon, . - avg_h264_qpel16_v_lowpass_neon
 # //.endfunc

        .text
        .align 2
 # //.type avg_h264_qpel8_v_lowpass_neon, %function
 # //.func avg_h264_qpel8_v_lowpass_neon
avg_h264_qpel8_v_lowpass_neon:
        ld1 {v16.8B}, [x1], x3
        ld1 {v18.8B}, [x1], x3
        ld1 {v20.8B}, [x1], x3
        ld1 {v22.8B}, [x1], x3
        ld1 {v24.8B}, [x1], x3
        ld1 {v26.8B}, [x1], x3
        ld1 {v28.8B}, [x1], x3
        ld1 {v30.8B}, [x1], x3
        ld1 {v17.8B}, [x1], x3
        ld1 {v19.8B}, [x1], x3
        ld1 {v21.8B}, [x1], x3
        ld1 {v23.8B}, [x1], x3
        ld1 {v25.8B}, [x1]

        trn1 v0.8B, v16.8B, v18.8B
        trn2 v1.8B, v16.8B, v18.8B
        trn1 v18.8B, v20.8B, v22.8B
        trn2 v22.8B, v20.8B, v22.8B
        trn1 v16.8B, v24.8B, v26.8B
        trn2 v26.8B, v24.8B, v26.8B
        trn1 v20.8B, v28.8B, v30.8B
        trn2 v30.8B, v28.8B, v30.8B

        trn1 v24.4H, v16.4H, v20.4H
        trn2 v20.4H, v16.4H, v20.4H
        trn1 v28.4H, v26.4H, v30.4H
        trn2 v30.4H, v26.4H, v30.4H
        trn1 v26.4H, v1.4H, v22.4H
        trn2 v1.4H, v1.4H, v22.4H
        trn1 v22.4H, v0.4H, v18.4H
        trn2 v0.4H, v0.4H, v18.4H

        trn1 v16.2S, v22.2S, v24.2S
        trn2 v24.2S, v22.2S, v24.2S

        trn1 v18.2S, v26.2S, v28.2S
        trn2 v26.2S, v26.2S, v28.2S

        trn2 v28.2S, v0.2S, v20.2S
        trn1 v20.2S, v0.2S, v20.2S

        trn1 v22.2S, v1.2S, v30.2S
        trn2 v30.2S, v1.2S, v30.2S
        trn1 v0.8B, v17.8B, v19.8B
        trn2 v1.8B, v17.8B, v19.8B
        trn1 v19.8B, v21.8B, v23.8B
        trn2 v23.8B, v21.8B, v23.8B
        trn1 v17.8B, v25.8B, v27.8B
        trn2 v27.8B, v25.8B, v27.8B
        trn1 v21.8B, v29.8B, v31.8B
        trn2 v31.8B, v29.8B, v31.8B

        trn1 v25.4H, v17.4H, v21.4H
        trn2 v21.4H, v17.4H, v21.4H
        trn1 v29.4H, v27.4H, v31.4H
        trn2 v31.4H, v27.4H, v31.4H
        trn1 v27.4H, v1.4H, v23.4H
        trn2 v1.4H, v1.4H, v23.4H
        trn1 v23.4H, v0.4H, v19.4H
        trn2 v0.4H, v0.4H, v19.4H

        trn1 v17.2S, v23.2S, v25.2S
        trn2 v25.2S, v23.2S, v25.2S

        trn1 v19.2S, v27.2S, v29.2S
        trn2 v27.2S, v27.2S, v29.2S

        trn2 v29.2S, v0.2S, v21.2S
        trn1 v21.2S, v0.2S, v21.2S

        trn1 v23.2S, v1.2S, v31.2S
        trn2 v31.2S, v1.2S, v31.2S
        ext v2.8B, v16.8B, v17.8B, #2
        ext v3.8B, v16.8B, v17.8B, #3
        uaddl v2.8H, v2.8B, v3.8B
        ext v4.8B, v16.8B, v17.8B, #1
        ext v5.8B, v16.8B, v17.8B, #4
        uaddl v4.8H, v4.8B, v5.8B
        ext v1.8B, v16.8B, v17.8B, #5
        uaddl v16.8H, v16.8B, v1.8B
        ext v0.8B, v18.8B, v19.8B, #2
        mla v16.8H, v2.8H, v6.H[1]
        ext v1.8B, v18.8B, v19.8B, #3
        uaddl v0.8H, v0.8B, v1.8B
        ext v1.8B, v18.8B, v19.8B, #1
        mls v16.8H, v4.8H, v6.H[0]
        ext v3.8B, v18.8B, v19.8B, #4
        uaddl v1.8H, v1.8B, v3.8B
        ext v2.8B, v18.8B, v19.8B, #5
        uaddl v17.8H, v18.8B, v2.8B
        mla v17.8H, v0.8H, v6.H[1]
        mls v17.8H, v1.8H, v6.H[0]
        sqrshrun v16.8B, v16.8H, #5
        sqrshrun v17.8B, v17.8H, #5
        ext v2.8B, v20.8B, v21.8B, #2
        ext v3.8B, v20.8B, v21.8B, #3
        uaddl v2.8H, v2.8B, v3.8B
        ext v4.8B, v20.8B, v21.8B, #1
        ext v5.8B, v20.8B, v21.8B, #4
        uaddl v4.8H, v4.8B, v5.8B
        ext v1.8B, v20.8B, v21.8B, #5
        uaddl v18.8H, v20.8B, v1.8B
        ext v0.8B, v22.8B, v23.8B, #2
        mla v18.8H, v2.8H, v6.H[1]
        ext v1.8B, v22.8B, v23.8B, #3
        uaddl v0.8H, v0.8B, v1.8B
        ext v1.8B, v22.8B, v23.8B, #1
        mls v18.8H, v4.8H, v6.H[0]
        ext v3.8B, v22.8B, v23.8B, #4
        uaddl v1.8H, v1.8B, v3.8B
        ext v2.8B, v22.8B, v23.8B, #5
        uaddl v19.8H, v22.8B, v2.8B
        mla v19.8H, v0.8H, v6.H[1]
        mls v19.8H, v1.8H, v6.H[0]
        sqrshrun v18.8B, v18.8H, #5
        sqrshrun v19.8B, v19.8H, #5
        ext v2.8B, v24.8B, v25.8B, #2
        ext v3.8B, v24.8B, v25.8B, #3
        uaddl v2.8H, v2.8B, v3.8B
        ext v4.8B, v24.8B, v25.8B, #1
        ext v5.8B, v24.8B, v25.8B, #4
        uaddl v4.8H, v4.8B, v5.8B
        ext v1.8B, v24.8B, v25.8B, #5
        uaddl v20.8H, v24.8B, v1.8B
        ext v0.8B, v26.8B, v27.8B, #2
        mla v20.8H, v2.8H, v6.H[1]
        ext v1.8B, v26.8B, v27.8B, #3
        uaddl v0.8H, v0.8B, v1.8B
        ext v1.8B, v26.8B, v27.8B, #1
        mls v20.8H, v4.8H, v6.H[0]
        ext v3.8B, v26.8B, v27.8B, #4
        uaddl v1.8H, v1.8B, v3.8B
        ext v2.8B, v26.8B, v27.8B, #5
        uaddl v21.8H, v26.8B, v2.8B
        mla v21.8H, v0.8H, v6.H[1]
        mls v21.8H, v1.8H, v6.H[0]
        sqrshrun v20.8B, v20.8H, #5
        sqrshrun v21.8B, v21.8H, #5
        ext v2.8B, v28.8B, v29.8B, #2
        ext v3.8B, v28.8B, v29.8B, #3
        uaddl v2.8H, v2.8B, v3.8B
        ext v4.8B, v28.8B, v29.8B, #1
        ext v5.8B, v28.8B, v29.8B, #4
        uaddl v4.8H, v4.8B, v5.8B
        ext v1.8B, v28.8B, v29.8B, #5
        uaddl v22.8H, v28.8B, v1.8B
        ext v0.8B, v30.8B, v31.8B, #2
        mla v22.8H, v2.8H, v6.H[1]
        ext v1.8B, v30.8B, v31.8B, #3
        uaddl v0.8H, v0.8B, v1.8B
        ext v1.8B, v30.8B, v31.8B, #1
        mls v22.8H, v4.8H, v6.H[0]
        ext v3.8B, v30.8B, v31.8B, #4
        uaddl v1.8H, v1.8B, v3.8B
        ext v2.8B, v30.8B, v31.8B, #5
        uaddl v23.8H, v30.8B, v2.8B
        mla v23.8H, v0.8H, v6.H[1]
        mls v23.8H, v1.8H, v6.H[0]
        sqrshrun v22.8B, v22.8H, #5
        sqrshrun v23.8B, v23.8H, #5
        trn1 v0.8B, v16.8B, v17.8B
        trn2 v1.8B, v16.8B, v17.8B
        trn1 v17.8B, v18.8B, v19.8B
        trn2 v19.8B, v18.8B, v19.8B
        trn1 v16.8B, v20.8B, v21.8B
        trn2 v21.8B, v20.8B, v21.8B
        trn1 v18.8B, v22.8B, v23.8B
        trn2 v23.8B, v22.8B, v23.8B

        trn1 v20.4H, v16.4H, v18.4H
        trn2 v18.4H, v16.4H, v18.4H
        trn1 v22.4H, v21.4H, v23.4H
        trn2 v23.4H, v21.4H, v23.4H
        trn1 v21.4H, v1.4H, v19.4H
        trn2 v1.4H, v1.4H, v19.4H
        trn1 v19.4H, v0.4H, v17.4H
        trn2 v0.4H, v0.4H, v17.4H

        trn1 v16.2S, v19.2S, v20.2S
        trn2 v20.2S, v19.2S, v20.2S

        trn1 v17.2S, v21.2S, v22.2S
        trn2 v21.2S, v21.2S, v22.2S

        trn2 v22.2S, v0.2S, v18.2S
        trn1 v18.2S, v0.2S, v18.2S

        trn1 v19.2S, v1.2S, v23.2S
        trn2 v23.2S, v1.2S, v23.2S

        ld1 {v24.8B}, [x0], x2
        urhadd v16.8B, v16.8B, v24.8B
        ld1 {v25.8B}, [x0], x2
        urhadd v17.8B, v17.8B, v25.8B
        ld1 {v26.8B}, [x0], x2
        urhadd v18.8B, v18.8B, v26.8B
        ld1 {v27.8B}, [x0], x2
        urhadd v19.8B, v19.8B, v27.8B
        ld1 {v28.8B}, [x0], x2
        urhadd v20.8B, v20.8B, v28.8B
        ld1 {v29.8B}, [x0], x2
        urhadd v21.8B, v21.8B, v29.8B
        ld1 {v30.8B}, [x0], x2
        urhadd v22.8B, v22.8B, v30.8B
        ld1 {v31.8B}, [x0], x2
        urhadd v23.8B, v23.8B, v31.8B
        sub x0, x0, x2, lsl #3

        st1 {v16.8B}, [x0], x2
        st1 {v17.8B}, [x0], x2
        st1 {v18.8B}, [x0], x2
        st1 {v19.8B}, [x0], x2
        st1 {v20.8B}, [x0], x2
        st1 {v21.8B}, [x0], x2
        st1 {v22.8B}, [x0], x2
        st1 {v23.8B}, [x0], x2

        ret
 # //.size avg_h264_qpel8_v_lowpass_neon, . - avg_h264_qpel8_v_lowpass_neon
 # //.endfunc


        .text
        .align 2
 # //.type put_h264_qpel16_v_lowpass_l2_neon, %function
 # //.func put_h264_qpel16_v_lowpass_l2_neon
put_h264_qpel16_v_lowpass_l2_neon:
        mov x4, x30
        bl put_h264_qpel8_v_lowpass_l2_neon
        sub x1, x1, x3, lsl #2
        bl put_h264_qpel8_v_lowpass_l2_neon
        sub x0, x0, x3, lsl #4
        sub x12, x12, x2, lsl #4
        add x0, x0, #8
        add x12, x12, #8
        sub x1, x1, x3, lsl #4
        sub x1, x1, x3, lsl #2
        add x1, x1, #8
        bl put_h264_qpel8_v_lowpass_l2_neon
        sub x1, x1, x3, lsl #2
        mov x30, x4
 # //.size put_h264_qpel16_v_lowpass_l2_neon, . - put_h264_qpel16_v_lowpass_l2_neon
 # //.endfunc

        .text
        .align 2
 # //.type put_h264_qpel8_v_lowpass_l2_neon, %function
 # //.func put_h264_qpel8_v_lowpass_l2_neon
put_h264_qpel8_v_lowpass_l2_neon:
        ld1 {v16.8B}, [x1], x3
        ld1 {v18.8B}, [x1], x3
        ld1 {v20.8B}, [x1], x3
        ld1 {v22.8B}, [x1], x3
        ld1 {v24.8B}, [x1], x3
        ld1 {v26.8B}, [x1], x3
        ld1 {v28.8B}, [x1], x3
        ld1 {v30.8B}, [x1], x3
        ld1 {v17.8B}, [x1], x3
        ld1 {v19.8B}, [x1], x3
        ld1 {v21.8B}, [x1], x3
        ld1 {v23.8B}, [x1], x3
        ld1 {v25.8B}, [x1]

        trn1 v0.8B, v16.8B, v18.8B
        trn2 v1.8B, v16.8B, v18.8B
        trn1 v18.8B, v20.8B, v22.8B
        trn2 v22.8B, v20.8B, v22.8B
        trn1 v16.8B, v24.8B, v26.8B
        trn2 v26.8B, v24.8B, v26.8B
        trn1 v20.8B, v28.8B, v30.8B
        trn2 v30.8B, v28.8B, v30.8B

        trn1 v24.4H, v16.4H, v20.4H
        trn2 v20.4H, v16.4H, v20.4H
        trn1 v28.4H, v26.4H, v30.4H
        trn2 v30.4H, v26.4H, v30.4H
        trn1 v26.4H, v1.4H, v22.4H
        trn2 v1.4H, v1.4H, v22.4H
        trn1 v22.4H, v0.4H, v18.4H
        trn2 v0.4H, v0.4H, v18.4H

        trn1 v16.2S, v22.2S, v24.2S
        trn2 v24.2S, v22.2S, v24.2S

        trn1 v18.2S, v26.2S, v28.2S
        trn2 v26.2S, v26.2S, v28.2S

        trn2 v28.2S, v0.2S, v20.2S
        trn1 v20.2S, v0.2S, v20.2S

        trn1 v22.2S, v1.2S, v30.2S
        trn2 v30.2S, v1.2S, v30.2S
        trn1 v0.8B, v17.8B, v19.8B
        trn2 v1.8B, v17.8B, v19.8B
        trn1 v19.8B, v21.8B, v23.8B
        trn2 v23.8B, v21.8B, v23.8B
        trn1 v17.8B, v25.8B, v27.8B
        trn2 v27.8B, v25.8B, v27.8B
        trn1 v21.8B, v29.8B, v31.8B
        trn2 v31.8B, v29.8B, v31.8B

        trn1 v25.4H, v17.4H, v21.4H
        trn2 v21.4H, v17.4H, v21.4H
        trn1 v29.4H, v27.4H, v31.4H
        trn2 v31.4H, v27.4H, v31.4H
        trn1 v27.4H, v1.4H, v23.4H
        trn2 v1.4H, v1.4H, v23.4H
        trn1 v23.4H, v0.4H, v19.4H
        trn2 v0.4H, v0.4H, v19.4H

        trn1 v17.2S, v23.2S, v25.2S
        trn2 v25.2S, v23.2S, v25.2S

        trn1 v19.2S, v27.2S, v29.2S
        trn2 v27.2S, v27.2S, v29.2S

        trn2 v29.2S, v0.2S, v21.2S
        trn1 v21.2S, v0.2S, v21.2S

        trn1 v23.2S, v1.2S, v31.2S
        trn2 v31.2S, v1.2S, v31.2S
        ext v2.8B, v16.8B, v17.8B, #2
        ext v3.8B, v16.8B, v17.8B, #3
        uaddl v2.8H, v2.8B, v3.8B
        ext v4.8B, v16.8B, v17.8B, #1
        ext v5.8B, v16.8B, v17.8B, #4
        uaddl v4.8H, v4.8B, v5.8B
        ext v1.8B, v16.8B, v17.8B, #5
        uaddl v16.8H, v16.8B, v1.8B
        ext v0.8B, v18.8B, v19.8B, #2
        mla v16.8H, v2.8H, v6.H[1]
        ext v1.8B, v18.8B, v19.8B, #3
        uaddl v0.8H, v0.8B, v1.8B
        ext v1.8B, v18.8B, v19.8B, #1
        mls v16.8H, v4.8H, v6.H[0]
        ext v3.8B, v18.8B, v19.8B, #4
        uaddl v1.8H, v1.8B, v3.8B
        ext v2.8B, v18.8B, v19.8B, #5
        uaddl v17.8H, v18.8B, v2.8B
        mla v17.8H, v0.8H, v6.H[1]
        mls v17.8H, v1.8H, v6.H[0]
        sqrshrun v16.8B, v16.8H, #5
        sqrshrun v17.8B, v17.8H, #5
        ext v2.8B, v20.8B, v21.8B, #2
        ext v3.8B, v20.8B, v21.8B, #3
        uaddl v2.8H, v2.8B, v3.8B
        ext v4.8B, v20.8B, v21.8B, #1
        ext v5.8B, v20.8B, v21.8B, #4
        uaddl v4.8H, v4.8B, v5.8B
        ext v1.8B, v20.8B, v21.8B, #5
        uaddl v18.8H, v20.8B, v1.8B
        ext v0.8B, v22.8B, v23.8B, #2
        mla v18.8H, v2.8H, v6.H[1]
        ext v1.8B, v22.8B, v23.8B, #3
        uaddl v0.8H, v0.8B, v1.8B
        ext v1.8B, v22.8B, v23.8B, #1
        mls v18.8H, v4.8H, v6.H[0]
        ext v3.8B, v22.8B, v23.8B, #4
        uaddl v1.8H, v1.8B, v3.8B
        ext v2.8B, v22.8B, v23.8B, #5
        uaddl v19.8H, v22.8B, v2.8B
        mla v19.8H, v0.8H, v6.H[1]
        mls v19.8H, v1.8H, v6.H[0]
        sqrshrun v18.8B, v18.8H, #5
        sqrshrun v19.8B, v19.8H, #5
        ext v2.8B, v24.8B, v25.8B, #2
        ext v3.8B, v24.8B, v25.8B, #3
        uaddl v2.8H, v2.8B, v3.8B
        ext v4.8B, v24.8B, v25.8B, #1
        ext v5.8B, v24.8B, v25.8B, #4
        uaddl v4.8H, v4.8B, v5.8B
        ext v1.8B, v24.8B, v25.8B, #5
        uaddl v20.8H, v24.8B, v1.8B
        ext v0.8B, v26.8B, v27.8B, #2
        mla v20.8H, v2.8H, v6.H[1]
        ext v1.8B, v26.8B, v27.8B, #3
        uaddl v0.8H, v0.8B, v1.8B
        ext v1.8B, v26.8B, v27.8B, #1
        mls v20.8H, v4.8H, v6.H[0]
        ext v3.8B, v26.8B, v27.8B, #4
        uaddl v1.8H, v1.8B, v3.8B
        ext v2.8B, v26.8B, v27.8B, #5
        uaddl v21.8H, v26.8B, v2.8B
        mla v21.8H, v0.8H, v6.H[1]
        mls v21.8H, v1.8H, v6.H[0]
        sqrshrun v20.8B, v20.8H, #5
        sqrshrun v21.8B, v21.8H, #5
        ext v2.8B, v28.8B, v29.8B, #2
        ext v3.8B, v28.8B, v29.8B, #3
        uaddl v2.8H, v2.8B, v3.8B
        ext v4.8B, v28.8B, v29.8B, #1
        ext v5.8B, v28.8B, v29.8B, #4
        uaddl v4.8H, v4.8B, v5.8B
        ext v1.8B, v28.8B, v29.8B, #5
        uaddl v22.8H, v28.8B, v1.8B
        ext v0.8B, v30.8B, v31.8B, #2
        mla v22.8H, v2.8H, v6.H[1]
        ext v1.8B, v30.8B, v31.8B, #3
        uaddl v0.8H, v0.8B, v1.8B
        ext v1.8B, v30.8B, v31.8B, #1
        mls v22.8H, v4.8H, v6.H[0]
        ext v3.8B, v30.8B, v31.8B, #4
        uaddl v1.8H, v1.8B, v3.8B
        ext v2.8B, v30.8B, v31.8B, #5
        uaddl v23.8H, v30.8B, v2.8B
        mla v23.8H, v0.8H, v6.H[1]
        mls v23.8H, v1.8H, v6.H[0]
        sqrshrun v22.8B, v22.8H, #5
        sqrshrun v23.8B, v23.8H, #5
        trn1 v0.8B, v16.8B, v17.8B
        trn2 v1.8B, v16.8B, v17.8B
        trn1 v17.8B, v18.8B, v19.8B
        trn2 v19.8B, v18.8B, v19.8B
        trn1 v16.8B, v20.8B, v21.8B
        trn2 v21.8B, v20.8B, v21.8B
        trn1 v18.8B, v22.8B, v23.8B
        trn2 v23.8B, v22.8B, v23.8B

        trn1 v20.4H, v16.4H, v18.4H
        trn2 v18.4H, v16.4H, v18.4H
        trn1 v22.4H, v21.4H, v23.4H
        trn2 v23.4H, v21.4H, v23.4H
        trn1 v21.4H, v1.4H, v19.4H
        trn2 v1.4H, v1.4H, v19.4H
        trn1 v19.4H, v0.4H, v17.4H
        trn2 v0.4H, v0.4H, v17.4H

        trn1 v16.2S, v19.2S, v20.2S
        trn2 v20.2S, v19.2S, v20.2S

        trn1 v17.2S, v21.2S, v22.2S
        trn2 v21.2S, v21.2S, v22.2S

        trn2 v22.2S, v0.2S, v18.2S
        trn1 v18.2S, v0.2S, v18.2S

        trn1 v19.2S, v1.2S, v23.2S
        trn2 v23.2S, v1.2S, v23.2S

        ld1 {v24.8B}, [x12], x2
        ld1 {v25.8B}, [x12], x2
        ld1 {v26.8B}, [x12], x2
        ld1 {v27.8B}, [x12], x2
        ld1 {v28.8B}, [x12], x2
        urhadd v16.8B, v24.8B, v16.8B
        urhadd v17.8B, v25.8B, v17.8B
        ld1 {v29.8B}, [x12], x2
        urhadd v18.8B, v26.8B, v18.8B
        urhadd v19.8B, v27.8B, v19.8B
        ld1 {v30.8B}, [x12], x2
        urhadd v20.8B, v28.8B, v20.8B
        urhadd v21.8B, v29.8B, v21.8B
        ld1 {v31.8B}, [x12], x2
        urhadd v22.8B, v30.8B, v22.8B
        urhadd v23.8B, v31.8B, v23.8B


        st1 {v16.8B}, [x0], x3
        st1 {v17.8B}, [x0], x3
        st1 {v18.8B}, [x0], x3
        st1 {v19.8B}, [x0], x3
        st1 {v20.8B}, [x0], x3
        st1 {v21.8B}, [x0], x3
        st1 {v22.8B}, [x0], x3
        st1 {v23.8B}, [x0], x3

        ret
 # //.size put_h264_qpel8_v_lowpass_l2_neon, . - put_h264_qpel8_v_lowpass_l2_neon
 # //.endfunc
        .text
        .align 2
 # //.type avg_h264_qpel16_v_lowpass_l2_neon, %function
 # //.func avg_h264_qpel16_v_lowpass_l2_neon
avg_h264_qpel16_v_lowpass_l2_neon:
        mov x4, x30
        bl avg_h264_qpel8_v_lowpass_l2_neon
        sub x1, x1, x3, lsl #2
        bl avg_h264_qpel8_v_lowpass_l2_neon
        sub x0, x0, x3, lsl #4
        sub x12, x12, x2, lsl #4
        add x0, x0, #8
        add x12, x12, #8
        sub x1, x1, x3, lsl #4
        sub x1, x1, x3, lsl #2
        add x1, x1, #8
        bl avg_h264_qpel8_v_lowpass_l2_neon
        sub x1, x1, x3, lsl #2
        mov x30, x4
 # //.size avg_h264_qpel16_v_lowpass_l2_neon, . - avg_h264_qpel16_v_lowpass_l2_neon
 # //.endfunc

        .text
        .align 2
 # //.type avg_h264_qpel8_v_lowpass_l2_neon, %function
 # //.func avg_h264_qpel8_v_lowpass_l2_neon
avg_h264_qpel8_v_lowpass_l2_neon:
        ld1 {v16.8B}, [x1], x3
        ld1 {v18.8B}, [x1], x3
        ld1 {v20.8B}, [x1], x3
        ld1 {v22.8B}, [x1], x3
        ld1 {v24.8B}, [x1], x3
        ld1 {v26.8B}, [x1], x3
        ld1 {v28.8B}, [x1], x3
        ld1 {v30.8B}, [x1], x3
        ld1 {v17.8B}, [x1], x3
        ld1 {v19.8B}, [x1], x3
        ld1 {v21.8B}, [x1], x3
        ld1 {v23.8B}, [x1], x3
        ld1 {v25.8B}, [x1]

        trn1 v0.8B, v16.8B, v18.8B
        trn2 v1.8B, v16.8B, v18.8B
        trn1 v18.8B, v20.8B, v22.8B
        trn2 v22.8B, v20.8B, v22.8B
        trn1 v16.8B, v24.8B, v26.8B
        trn2 v26.8B, v24.8B, v26.8B
        trn1 v20.8B, v28.8B, v30.8B
        trn2 v30.8B, v28.8B, v30.8B

        trn1 v24.4H, v16.4H, v20.4H
        trn2 v20.4H, v16.4H, v20.4H
        trn1 v28.4H, v26.4H, v30.4H
        trn2 v30.4H, v26.4H, v30.4H
        trn1 v26.4H, v1.4H, v22.4H
        trn2 v1.4H, v1.4H, v22.4H
        trn1 v22.4H, v0.4H, v18.4H
        trn2 v0.4H, v0.4H, v18.4H

        trn1 v16.2S, v22.2S, v24.2S
        trn2 v24.2S, v22.2S, v24.2S

        trn1 v18.2S, v26.2S, v28.2S
        trn2 v26.2S, v26.2S, v28.2S

        trn2 v28.2S, v0.2S, v20.2S
        trn1 v20.2S, v0.2S, v20.2S

        trn1 v22.2S, v1.2S, v30.2S
        trn2 v30.2S, v1.2S, v30.2S
        trn1 v0.8B, v17.8B, v19.8B
        trn2 v1.8B, v17.8B, v19.8B
        trn1 v19.8B, v21.8B, v23.8B
        trn2 v23.8B, v21.8B, v23.8B
        trn1 v17.8B, v25.8B, v27.8B
        trn2 v27.8B, v25.8B, v27.8B
        trn1 v21.8B, v29.8B, v31.8B
        trn2 v31.8B, v29.8B, v31.8B

        trn1 v25.4H, v17.4H, v21.4H
        trn2 v21.4H, v17.4H, v21.4H
        trn1 v29.4H, v27.4H, v31.4H
        trn2 v31.4H, v27.4H, v31.4H
        trn1 v27.4H, v1.4H, v23.4H
        trn2 v1.4H, v1.4H, v23.4H
        trn1 v23.4H, v0.4H, v19.4H
        trn2 v0.4H, v0.4H, v19.4H

        trn1 v17.2S, v23.2S, v25.2S
        trn2 v25.2S, v23.2S, v25.2S

        trn1 v19.2S, v27.2S, v29.2S
        trn2 v27.2S, v27.2S, v29.2S

        trn2 v29.2S, v0.2S, v21.2S
        trn1 v21.2S, v0.2S, v21.2S

        trn1 v23.2S, v1.2S, v31.2S
        trn2 v31.2S, v1.2S, v31.2S
        ext v2.8B, v16.8B, v17.8B, #2
        ext v3.8B, v16.8B, v17.8B, #3
        uaddl v2.8H, v2.8B, v3.8B
        ext v4.8B, v16.8B, v17.8B, #1
        ext v5.8B, v16.8B, v17.8B, #4
        uaddl v4.8H, v4.8B, v5.8B
        ext v1.8B, v16.8B, v17.8B, #5
        uaddl v16.8H, v16.8B, v1.8B
        ext v0.8B, v18.8B, v19.8B, #2
        mla v16.8H, v2.8H, v6.H[1]
        ext v1.8B, v18.8B, v19.8B, #3
        uaddl v0.8H, v0.8B, v1.8B
        ext v1.8B, v18.8B, v19.8B, #1
        mls v16.8H, v4.8H, v6.H[0]
        ext v3.8B, v18.8B, v19.8B, #4
        uaddl v1.8H, v1.8B, v3.8B
        ext v2.8B, v18.8B, v19.8B, #5
        uaddl v17.8H, v18.8B, v2.8B
        mla v17.8H, v0.8H, v6.H[1]
        mls v17.8H, v1.8H, v6.H[0]
        sqrshrun v16.8B, v16.8H, #5
        sqrshrun v17.8B, v17.8H, #5
        ext v2.8B, v20.8B, v21.8B, #2
        ext v3.8B, v20.8B, v21.8B, #3
        uaddl v2.8H, v2.8B, v3.8B
        ext v4.8B, v20.8B, v21.8B, #1
        ext v5.8B, v20.8B, v21.8B, #4
        uaddl v4.8H, v4.8B, v5.8B
        ext v1.8B, v20.8B, v21.8B, #5
        uaddl v18.8H, v20.8B, v1.8B
        ext v0.8B, v22.8B, v23.8B, #2
        mla v18.8H, v2.8H, v6.H[1]
        ext v1.8B, v22.8B, v23.8B, #3
        uaddl v0.8H, v0.8B, v1.8B
        ext v1.8B, v22.8B, v23.8B, #1
        mls v18.8H, v4.8H, v6.H[0]
        ext v3.8B, v22.8B, v23.8B, #4
        uaddl v1.8H, v1.8B, v3.8B
        ext v2.8B, v22.8B, v23.8B, #5
        uaddl v19.8H, v22.8B, v2.8B
        mla v19.8H, v0.8H, v6.H[1]
        mls v19.8H, v1.8H, v6.H[0]
        sqrshrun v18.8B, v18.8H, #5
        sqrshrun v19.8B, v19.8H, #5
        ext v2.8B, v24.8B, v25.8B, #2
        ext v3.8B, v24.8B, v25.8B, #3
        uaddl v2.8H, v2.8B, v3.8B
        ext v4.8B, v24.8B, v25.8B, #1
        ext v5.8B, v24.8B, v25.8B, #4
        uaddl v4.8H, v4.8B, v5.8B
        ext v1.8B, v24.8B, v25.8B, #5
        uaddl v20.8H, v24.8B, v1.8B
        ext v0.8B, v26.8B, v27.8B, #2
        mla v20.8H, v2.8H, v6.H[1]
        ext v1.8B, v26.8B, v27.8B, #3
        uaddl v0.8H, v0.8B, v1.8B
        ext v1.8B, v26.8B, v27.8B, #1
        mls v20.8H, v4.8H, v6.H[0]
        ext v3.8B, v26.8B, v27.8B, #4
        uaddl v1.8H, v1.8B, v3.8B
        ext v2.8B, v26.8B, v27.8B, #5
        uaddl v21.8H, v26.8B, v2.8B
        mla v21.8H, v0.8H, v6.H[1]
        mls v21.8H, v1.8H, v6.H[0]
        sqrshrun v20.8B, v20.8H, #5
        sqrshrun v21.8B, v21.8H, #5
        ext v2.8B, v28.8B, v29.8B, #2
        ext v3.8B, v28.8B, v29.8B, #3
        uaddl v2.8H, v2.8B, v3.8B
        ext v4.8B, v28.8B, v29.8B, #1
        ext v5.8B, v28.8B, v29.8B, #4
        uaddl v4.8H, v4.8B, v5.8B
        ext v1.8B, v28.8B, v29.8B, #5
        uaddl v22.8H, v28.8B, v1.8B
        ext v0.8B, v30.8B, v31.8B, #2
        mla v22.8H, v2.8H, v6.H[1]
        ext v1.8B, v30.8B, v31.8B, #3
        uaddl v0.8H, v0.8B, v1.8B
        ext v1.8B, v30.8B, v31.8B, #1
        mls v22.8H, v4.8H, v6.H[0]
        ext v3.8B, v30.8B, v31.8B, #4
        uaddl v1.8H, v1.8B, v3.8B
        ext v2.8B, v30.8B, v31.8B, #5
        uaddl v23.8H, v30.8B, v2.8B
        mla v23.8H, v0.8H, v6.H[1]
        mls v23.8H, v1.8H, v6.H[0]
        sqrshrun v22.8B, v22.8H, #5
        sqrshrun v23.8B, v23.8H, #5
        trn1 v0.8B, v16.8B, v17.8B
        trn2 v1.8B, v16.8B, v17.8B
        trn1 v17.8B, v18.8B, v19.8B
        trn2 v19.8B, v18.8B, v19.8B
        trn1 v16.8B, v20.8B, v21.8B
        trn2 v21.8B, v20.8B, v21.8B
        trn1 v18.8B, v22.8B, v23.8B
        trn2 v23.8B, v22.8B, v23.8B

        trn1 v20.4H, v16.4H, v18.4H
        trn2 v18.4H, v16.4H, v18.4H
        trn1 v22.4H, v21.4H, v23.4H
        trn2 v23.4H, v21.4H, v23.4H
        trn1 v21.4H, v1.4H, v19.4H
        trn2 v1.4H, v1.4H, v19.4H
        trn1 v19.4H, v0.4H, v17.4H
        trn2 v0.4H, v0.4H, v17.4H

        trn1 v16.2S, v19.2S, v20.2S
        trn2 v20.2S, v19.2S, v20.2S

        trn1 v17.2S, v21.2S, v22.2S
        trn2 v21.2S, v21.2S, v22.2S

        trn2 v22.2S, v0.2S, v18.2S
        trn1 v18.2S, v0.2S, v18.2S

        trn1 v19.2S, v1.2S, v23.2S
        trn2 v23.2S, v1.2S, v23.2S

        ld1 {v24.8B}, [x12], x2
        ld1 {v25.8B}, [x12], x2
        ld1 {v26.8B}, [x12], x2
        ld1 {v27.8B}, [x12], x2
        ld1 {v28.8B}, [x12], x2
        urhadd v16.8B, v24.8B, v16.8B
        urhadd v17.8B, v25.8B, v17.8B
        ld1 {v29.8B}, [x12], x2
        urhadd v18.8B, v26.8B, v18.8B
        urhadd v19.8B, v27.8B, v19.8B
        ld1 {v30.8B}, [x12], x2
        urhadd v20.8B, v28.8B, v20.8B
        urhadd v21.8B, v29.8B, v21.8B
        ld1 {v31.8B}, [x12], x2
        urhadd v22.8B, v30.8B, v22.8B
        urhadd v23.8B, v31.8B, v23.8B

        ld1 {v24.8B}, [x0], x3
        urhadd v16.8B, v16.8B, v24.8B
        ld1 {v25.8B}, [x0], x3
        urhadd v17.8B, v17.8B, v25.8B
        ld1 {v26.8B}, [x0], x3
        urhadd v18.8B, v18.8B, v26.8B
        ld1 {v27.8B}, [x0], x3
        urhadd v19.8B, v19.8B, v27.8B
        ld1 {v28.8B}, [x0], x3
        urhadd v20.8B, v20.8B, v28.8B
        ld1 {v29.8B}, [x0], x3
        urhadd v21.8B, v21.8B, v29.8B
        ld1 {v30.8B}, [x0], x3
        urhadd v22.8B, v22.8B, v30.8B
        ld1 {v31.8B}, [x0], x3
        urhadd v23.8B, v23.8B, v31.8B
        sub x0, x0, x3, lsl #3

        st1 {v16.8B}, [x0], x3
        st1 {v17.8B}, [x0], x3
        st1 {v18.8B}, [x0], x3
        st1 {v19.8B}, [x0], x3
        st1 {v20.8B}, [x0], x3
        st1 {v21.8B}, [x0], x3
        st1 {v22.8B}, [x0], x3
        st1 {v23.8B}, [x0], x3

        ret
 # //.size avg_h264_qpel8_v_lowpass_l2_neon, . - avg_h264_qpel8_v_lowpass_l2_neon
 # //.endfunc

        .text
        .align 2
 # //.type put_h264_qpel8_hv_lowpass_neon_top, %function
 # //.func put_h264_qpel8_hv_lowpass_neon_top
put_h264_qpel8_hv_lowpass_neon_top:
        movz w12, #20, lsl #16
        movk w12, #5
        mov v6.S[0], w12
        ld1 {v16.8H}, [x1], x3
        ld1 {v17.8H}, [x1], x3
        ld1 {v18.8H}, [x1], x3
        ld1 {v19.8H}, [x1], x3
        ld1 {v20.8H}, [x1], x3
        ld1 {v21.8H}, [x1], x3
        ld1 {v22.8H}, [x1], x3
        ld1 {v23.8H}, [x1], x3
        ld1 {v24.8H}, [x1], x3
        ld1 {v25.8H}, [x1], x3
        ld1 {v26.8H}, [x1], x3
        ld1 {v27.8H}, [x1], x3
        ld1 {v28.8H}, [x1]
        ext v0.16B, v16.16B, v16.16B, #2
        ext v1.16B, v16.16B, v16.16B, #3
        uaddl v0.8H, v0.8B, v1.8B
        ext v2.16B, v16.16B, v16.16B, #1
        ext v3.16B, v16.16B, v16.16B, #4
        uaddl v2.8H, v2.8B, v3.8B
        ext v30.16B, v16.16B, v16.16B, #5
        uaddl v16.8H, v16.8B, v30.8B
        ext v4.16B, v17.16B, v17.16B, #2
        mla v16.8H, v0.8H, v6.H[1]
        ext v5.16B, v17.16B, v17.16B, #3
        uaddl v4.8H, v4.8B, v5.8B
        ext v7.16B, v17.16B, v17.16B, #1
        mls v16.8H, v2.8H, v6.H[0]
        ext v0.16B, v17.16B, v17.16B, #4
        uaddl v7.8H, v7.8B, v0.8B
        ext v31.16B, v17.16B, v17.16B, #5
        uaddl v17.8H, v17.8B, v31.8B
        mla v17.8H, v4.8H, v6.H[1]
        mls v17.8H, v7.8H, v6.H[0]
        ext v0.16B, v18.16B, v18.16B, #2
        ext v1.16B, v18.16B, v18.16B, #3
        uaddl v0.8H, v0.8B, v1.8B
        ext v2.16B, v18.16B, v18.16B, #1
        ext v3.16B, v18.16B, v18.16B, #4
        uaddl v2.8H, v2.8B, v3.8B
        ext v30.16B, v18.16B, v18.16B, #5
        uaddl v18.8H, v18.8B, v30.8B
        ext v4.16B, v19.16B, v19.16B, #2
        mla v18.8H, v0.8H, v6.H[1]
        ext v5.16B, v19.16B, v19.16B, #3
        uaddl v4.8H, v4.8B, v5.8B
        ext v7.16B, v19.16B, v19.16B, #1
        mls v18.8H, v2.8H, v6.H[0]
        ext v0.16B, v19.16B, v19.16B, #4
        uaddl v7.8H, v7.8B, v0.8B
        ext v31.16B, v19.16B, v19.16B, #5
        uaddl v19.8H, v19.8B, v31.8B
        mla v19.8H, v4.8H, v6.H[1]
        mls v19.8H, v7.8H, v6.H[0]
        ext v0.16B, v20.16B, v20.16B, #2
        ext v1.16B, v20.16B, v20.16B, #3
        uaddl v0.8H, v0.8B, v1.8B
        ext v2.16B, v20.16B, v20.16B, #1
        ext v3.16B, v20.16B, v20.16B, #4
        uaddl v2.8H, v2.8B, v3.8B
        ext v30.16B, v20.16B, v20.16B, #5
        uaddl v20.8H, v20.8B, v30.8B
        ext v4.16B, v21.16B, v21.16B, #2
        mla v20.8H, v0.8H, v6.H[1]
        ext v5.16B, v21.16B, v21.16B, #3
        uaddl v4.8H, v4.8B, v5.8B
        ext v7.16B, v21.16B, v21.16B, #1
        mls v20.8H, v2.8H, v6.H[0]
        ext v0.16B, v21.16B, v21.16B, #4
        uaddl v7.8H, v7.8B, v0.8B
        ext v31.16B, v21.16B, v21.16B, #5
        uaddl v21.8H, v21.8B, v31.8B
        mla v21.8H, v4.8H, v6.H[1]
        mls v21.8H, v7.8H, v6.H[0]
        ext v0.16B, v22.16B, v22.16B, #2
        ext v1.16B, v22.16B, v22.16B, #3
        uaddl v0.8H, v0.8B, v1.8B
        ext v2.16B, v22.16B, v22.16B, #1
        ext v3.16B, v22.16B, v22.16B, #4
        uaddl v2.8H, v2.8B, v3.8B
        ext v30.16B, v22.16B, v22.16B, #5
        uaddl v22.8H, v22.8B, v30.8B
        ext v4.16B, v23.16B, v23.16B, #2
        mla v22.8H, v0.8H, v6.H[1]
        ext v5.16B, v23.16B, v23.16B, #3
        uaddl v4.8H, v4.8B, v5.8B
        ext v7.16B, v23.16B, v23.16B, #1
        mls v22.8H, v2.8H, v6.H[0]
        ext v0.16B, v23.16B, v23.16B, #4
        uaddl v7.8H, v7.8B, v0.8B
        ext v31.16B, v23.16B, v23.16B, #5
        uaddl v23.8H, v23.8B, v31.8B
        mla v23.8H, v4.8H, v6.H[1]
        mls v23.8H, v7.8H, v6.H[0]
        ext v0.16B, v24.16B, v24.16B, #2
        ext v1.16B, v24.16B, v24.16B, #3
        uaddl v0.8H, v0.8B, v1.8B
        ext v2.16B, v24.16B, v24.16B, #1
        ext v3.16B, v24.16B, v24.16B, #4
        uaddl v2.8H, v2.8B, v3.8B
        ext v30.16B, v24.16B, v24.16B, #5
        uaddl v24.8H, v24.8B, v30.8B
        ext v4.16B, v25.16B, v25.16B, #2
        mla v24.8H, v0.8H, v6.H[1]
        ext v5.16B, v25.16B, v25.16B, #3
        uaddl v4.8H, v4.8B, v5.8B
        ext v7.16B, v25.16B, v25.16B, #1
        mls v24.8H, v2.8H, v6.H[0]
        ext v0.16B, v25.16B, v25.16B, #4
        uaddl v7.8H, v7.8B, v0.8B
        ext v31.16B, v25.16B, v25.16B, #5
        uaddl v25.8H, v25.8B, v31.8B
        mla v25.8H, v4.8H, v6.H[1]
        mls v25.8H, v7.8H, v6.H[0]
        ext v0.16B, v26.16B, v26.16B, #2
        ext v1.16B, v26.16B, v26.16B, #3
        uaddl v0.8H, v0.8B, v1.8B
        ext v2.16B, v26.16B, v26.16B, #1
        ext v3.16B, v26.16B, v26.16B, #4
        uaddl v2.8H, v2.8B, v3.8B
        ext v30.16B, v26.16B, v26.16B, #5
        uaddl v26.8H, v26.8B, v30.8B
        ext v4.16B, v27.16B, v27.16B, #2
        mla v26.8H, v0.8H, v6.H[1]
        ext v5.16B, v27.16B, v27.16B, #3
        uaddl v4.8H, v4.8B, v5.8B
        ext v7.16B, v27.16B, v27.16B, #1
        mls v26.8H, v2.8H, v6.H[0]
        ext v0.16B, v27.16B, v27.16B, #4
        uaddl v7.8H, v7.8B, v0.8B
        ext v31.16B, v27.16B, v27.16B, #5
        uaddl v27.8H, v27.8B, v31.8B
        mla v27.8H, v4.8H, v6.H[1]
        mls v27.8H, v7.8H, v6.H[0]
        ext v0.16B, v28.16B, v28.16B, #2
        ext v1.16B, v28.16B, v28.16B, #3
        uaddl v0.8H, v0.8B, v1.8B
        ext v2.16B, v28.16B, v28.16B, #1
        ext v3.16B, v28.16B, v28.16B, #4
        uaddl v2.8H, v2.8B, v3.8B
        ext v30.16B, v28.16B, v28.16B, #5
        uaddl v28.8H, v28.8B, v30.8B
        ext v4.16B, v29.16B, v29.16B, #2
        mla v28.8H, v0.8H, v6.H[1]
        ext v5.16B, v29.16B, v29.16B, #3
        uaddl v4.8H, v4.8B, v5.8B
        ext v7.16B, v29.16B, v29.16B, #1
        mls v28.8H, v2.8H, v6.H[0]
        ext v0.16B, v29.16B, v29.16B, #4
        uaddl v7.8H, v7.8B, v0.8B
        ext v31.16B, v29.16B, v29.16B, #5
        uaddl v29.8H, v29.8B, v31.8B
        mla v29.8H, v4.8H, v6.H[1]
        mls v29.8H, v7.8H, v6.H[0]

        trn1 v0.8H, v16.8H, v17.8H
        trn2 v1.8H, v16.8H, v17.8H
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
        trn1 v21.4S, v1.4S, v19.4S
        trn2 v1.4S, v1.4S, v19.4S
        trn1 v19.4S, v0.4S, v17.4S
        trn2 v0.4S, v0.4S, v17.4S

        trn1 v16.2D, v19.2D, v20.2D
        trn2 v20.2D, v19.2D, v20.2D

        trn1 v17.2D, v21.2D, v22.2D
        trn2 v21.2D, v21.2D, v22.2D

        trn2 v22.2D, v0.2D, v18.2D
        trn1 v18.2D, v0.2D, v18.2D

        trn1 v19.2D, v1.2D, v23.2D
        trn2 v23.2D, v1.2D, v23.2D

        trn1 v0.8H, v24.8H, v25.8H
        trn2 v1.8H, v24.8H, v25.8H
        trn1 v25.8H, v26.8H, v27.8H
        trn2 v27.8H, v26.8H, v27.8H
        trn1 v24.8H, v28.8H, v29.8H
        trn2 v29.8H, v28.8H, v29.8H
        trn1 v26.8H, v30.8H, v31.8H
        trn2 v31.8H, v30.8H, v31.8H

        trn1 v28.4S, v24.4S, v26.4S
        trn2 v26.4S, v24.4S, v26.4S
        trn1 v30.4S, v29.4S, v31.4S
        trn2 v31.4S, v29.4S, v31.4S
        trn1 v29.4S, v1.4S, v27.4S
        trn2 v1.4S, v1.4S, v27.4S
        trn1 v27.4S, v0.4S, v25.4S
        trn2 v0.4S, v0.4S, v25.4S

        trn1 v24.2D, v27.2D, v28.2D
        trn2 v28.2D, v27.2D, v28.2D

        trn1 v25.2D, v29.2D, v30.2D
        trn2 v29.2D, v29.2D, v30.2D

        trn2 v30.2D, v0.2D, v26.2D
        trn1 v26.2D, v0.2D, v26.2D

        trn1 v27.2D, v1.2D, v31.2D
        trn2 v31.2D, v1.2D, v31.2D


        ext v1.16B, v16.16B, v24.16B, #4
        ext v0.16B, v16.16B, v24.16B, #6
        saddl v5.4S, v1.4H, v0.4H
        ext v2.16B, v16.16B, v24.16B, #2
        saddl2 v1.4S, v1.8H, v0.8H
        ext v3.16B, v16.16B, v24.16B, #8
        saddl v6.4S, v2.4H, v3.4H
        ext v24.16B, v16.16B, v24.16B, #10
        saddl2 v2.4S, v2.8H, v3.8H
        saddl v0.4S, v16.4H, v24.4H
        saddl2 v4.4S, v16.8H, v24.8H

        shl v3.4S, v5.4S, #4
        shl v5.4S, v5.4S, #2
        shl v7.4S, v6.4S, #2
        add v5.4S, v5.4S, v3.4S
        add v6.4S, v6.4S, v7.4S

        shl v3.4S, v1.4S, #4
        shl v1.4S, v1.4S, #2
        shl v7.4S, v2.4S, #2
        add v1.4S, v1.4S, v3.4S
        add v2.4S, v2.4S, v7.4S

        add v5.4S, v5.4S, v0.4S
        sub v5.4S, v5.4S, v6.4S

        add v1.4S, v1.4S, v4.4S
        sub v1.4S, v1.4S, v2.4S

        rshrn v5.4H, v5.4S, #10
        rshrn2 v5.8H, v1.4S, #10

        sqxtun v16.8B, v5.8H
        ext v1.16B, v17.16B, v25.16B, #4
        ext v0.16B, v17.16B, v25.16B, #6
        saddl v5.4S, v1.4H, v0.4H
        ext v2.16B, v17.16B, v25.16B, #2
        saddl2 v1.4S, v1.8H, v0.8H
        ext v3.16B, v17.16B, v25.16B, #8
        saddl v6.4S, v2.4H, v3.4H
        ext v25.16B, v17.16B, v25.16B, #10
        saddl2 v2.4S, v2.8H, v3.8H
        saddl v0.4S, v17.4H, v25.4H
        saddl2 v4.4S, v17.8H, v25.8H

        shl v3.4S, v5.4S, #4
        shl v5.4S, v5.4S, #2
        shl v7.4S, v6.4S, #2
        add v5.4S, v5.4S, v3.4S
        add v6.4S, v6.4S, v7.4S

        shl v3.4S, v1.4S, #4
        shl v1.4S, v1.4S, #2
        shl v7.4S, v2.4S, #2
        add v1.4S, v1.4S, v3.4S
        add v2.4S, v2.4S, v7.4S

        add v5.4S, v5.4S, v0.4S
        sub v5.4S, v5.4S, v6.4S

        add v1.4S, v1.4S, v4.4S
        sub v1.4S, v1.4S, v2.4S

        rshrn v5.4H, v5.4S, #10
        rshrn2 v5.8H, v1.4S, #10

        sqxtun v17.8B, v5.8H

        ext v1.16B, v18.16B, v26.16B, #4
        ext v0.16B, v18.16B, v26.16B, #6
        saddl v5.4S, v1.4H, v0.4H
        ext v2.16B, v18.16B, v26.16B, #2
        saddl2 v1.4S, v1.8H, v0.8H
        ext v3.16B, v18.16B, v26.16B, #8
        saddl v6.4S, v2.4H, v3.4H
        ext v26.16B, v18.16B, v26.16B, #10
        saddl2 v2.4S, v2.8H, v3.8H
        saddl v0.4S, v18.4H, v26.4H
        saddl2 v4.4S, v18.8H, v26.8H

        shl v3.4S, v5.4S, #4
        shl v5.4S, v5.4S, #2
        shl v7.4S, v6.4S, #2
        add v5.4S, v5.4S, v3.4S
        add v6.4S, v6.4S, v7.4S

        shl v3.4S, v1.4S, #4
        shl v1.4S, v1.4S, #2
        shl v7.4S, v2.4S, #2
        add v1.4S, v1.4S, v3.4S
        add v2.4S, v2.4S, v7.4S

        add v5.4S, v5.4S, v0.4S
        sub v5.4S, v5.4S, v6.4S

        add v1.4S, v1.4S, v4.4S
        sub v1.4S, v1.4S, v2.4S

        rshrn v5.4H, v5.4S, #10
        rshrn2 v5.8H, v1.4S, #10

        sqxtun v18.8B, v5.8H
        ext v1.16B, v19.16B, v27.16B, #4
        ext v0.16B, v19.16B, v27.16B, #6
        saddl v5.4S, v1.4H, v0.4H
        ext v2.16B, v19.16B, v27.16B, #2
        saddl2 v1.4S, v1.8H, v0.8H
        ext v3.16B, v19.16B, v27.16B, #8
        saddl v6.4S, v2.4H, v3.4H
        ext v27.16B, v19.16B, v27.16B, #10
        saddl2 v2.4S, v2.8H, v3.8H
        saddl v0.4S, v19.4H, v27.4H
        saddl2 v4.4S, v19.8H, v27.8H

        shl v3.4S, v5.4S, #4
        shl v5.4S, v5.4S, #2
        shl v7.4S, v6.4S, #2
        add v5.4S, v5.4S, v3.4S
        add v6.4S, v6.4S, v7.4S

        shl v3.4S, v1.4S, #4
        shl v1.4S, v1.4S, #2
        shl v7.4S, v2.4S, #2
        add v1.4S, v1.4S, v3.4S
        add v2.4S, v2.4S, v7.4S

        add v5.4S, v5.4S, v0.4S
        sub v5.4S, v5.4S, v6.4S

        add v1.4S, v1.4S, v4.4S
        sub v1.4S, v1.4S, v2.4S

        rshrn v5.4H, v5.4S, #10
        rshrn2 v5.8H, v1.4S, #10

        sqxtun v19.8B, v5.8H

        ext v1.16B, v20.16B, v28.16B, #4
        ext v0.16B, v20.16B, v28.16B, #6
        saddl v5.4S, v1.4H, v0.4H
        ext v2.16B, v20.16B, v28.16B, #2
        saddl2 v1.4S, v1.8H, v0.8H
        ext v3.16B, v20.16B, v28.16B, #8
        saddl v6.4S, v2.4H, v3.4H
        ext v28.16B, v20.16B, v28.16B, #10
        saddl2 v2.4S, v2.8H, v3.8H
        saddl v0.4S, v20.4H, v28.4H
        saddl2 v4.4S, v20.8H, v28.8H

        shl v3.4S, v5.4S, #4
        shl v5.4S, v5.4S, #2
        shl v7.4S, v6.4S, #2
        add v5.4S, v5.4S, v3.4S
        add v6.4S, v6.4S, v7.4S

        shl v3.4S, v1.4S, #4
        shl v1.4S, v1.4S, #2
        shl v7.4S, v2.4S, #2
        add v1.4S, v1.4S, v3.4S
        add v2.4S, v2.4S, v7.4S

        add v5.4S, v5.4S, v0.4S
        sub v5.4S, v5.4S, v6.4S

        add v1.4S, v1.4S, v4.4S
        sub v1.4S, v1.4S, v2.4S

        rshrn v5.4H, v5.4S, #10
        rshrn2 v5.8H, v1.4S, #10

        sqxtun v20.8B, v5.8H
        ext v1.16B, v21.16B, v29.16B, #4
        ext v0.16B, v21.16B, v29.16B, #6
        saddl v5.4S, v1.4H, v0.4H
        ext v2.16B, v21.16B, v29.16B, #2
        saddl2 v1.4S, v1.8H, v0.8H
        ext v3.16B, v21.16B, v29.16B, #8
        saddl v6.4S, v2.4H, v3.4H
        ext v29.16B, v21.16B, v29.16B, #10
        saddl2 v2.4S, v2.8H, v3.8H
        saddl v0.4S, v21.4H, v29.4H
        saddl2 v4.4S, v21.8H, v29.8H

        shl v3.4S, v5.4S, #4
        shl v5.4S, v5.4S, #2
        shl v7.4S, v6.4S, #2
        add v5.4S, v5.4S, v3.4S
        add v6.4S, v6.4S, v7.4S

        shl v3.4S, v1.4S, #4
        shl v1.4S, v1.4S, #2
        shl v7.4S, v2.4S, #2
        add v1.4S, v1.4S, v3.4S
        add v2.4S, v2.4S, v7.4S

        add v5.4S, v5.4S, v0.4S
        sub v5.4S, v5.4S, v6.4S

        add v1.4S, v1.4S, v4.4S
        sub v1.4S, v1.4S, v2.4S

        rshrn v5.4H, v5.4S, #10
        rshrn2 v5.8H, v1.4S, #10

        sqxtun v21.8B, v5.8H

        ext v1.16B, v22.16B, v30.16B, #4
        ext v0.16B, v22.16B, v30.16B, #6
        saddl v5.4S, v1.4H, v0.4H
        ext v2.16B, v22.16B, v30.16B, #2
        saddl2 v1.4S, v1.8H, v0.8H
        ext v3.16B, v22.16B, v30.16B, #8
        saddl v6.4S, v2.4H, v3.4H
        ext v30.16B, v22.16B, v30.16B, #10
        saddl2 v2.4S, v2.8H, v3.8H
        saddl v0.4S, v22.4H, v30.4H
        saddl2 v4.4S, v22.8H, v30.8H

        shl v3.4S, v5.4S, #4
        shl v5.4S, v5.4S, #2
        shl v7.4S, v6.4S, #2
        add v5.4S, v5.4S, v3.4S
        add v6.4S, v6.4S, v7.4S

        shl v3.4S, v1.4S, #4
        shl v1.4S, v1.4S, #2
        shl v7.4S, v2.4S, #2
        add v1.4S, v1.4S, v3.4S
        add v2.4S, v2.4S, v7.4S

        add v5.4S, v5.4S, v0.4S
        sub v5.4S, v5.4S, v6.4S

        add v1.4S, v1.4S, v4.4S
        sub v1.4S, v1.4S, v2.4S

        rshrn v5.4H, v5.4S, #10
        rshrn2 v5.8H, v1.4S, #10

        sqxtun v22.8B, v5.8H
        ext v1.16B, v23.16B, v31.16B, #4
        ext v0.16B, v23.16B, v31.16B, #6
        saddl v5.4S, v1.4H, v0.4H
        ext v2.16B, v23.16B, v31.16B, #2
        saddl2 v1.4S, v1.8H, v0.8H
        ext v3.16B, v23.16B, v31.16B, #8
        saddl v6.4S, v2.4H, v3.4H
        ext v31.16B, v23.16B, v31.16B, #10
        saddl2 v2.4S, v2.8H, v3.8H
        saddl v0.4S, v23.4H, v31.4H
        saddl2 v4.4S, v23.8H, v31.8H

        shl v3.4S, v5.4S, #4
        shl v5.4S, v5.4S, #2
        shl v7.4S, v6.4S, #2
        add v5.4S, v5.4S, v3.4S
        add v6.4S, v6.4S, v7.4S

        shl v3.4S, v1.4S, #4
        shl v1.4S, v1.4S, #2
        shl v7.4S, v2.4S, #2
        add v1.4S, v1.4S, v3.4S
        add v2.4S, v2.4S, v7.4S

        add v5.4S, v5.4S, v0.4S
        sub v5.4S, v5.4S, v6.4S

        add v1.4S, v1.4S, v4.4S
        sub v1.4S, v1.4S, v2.4S

        rshrn v5.4H, v5.4S, #10
        rshrn2 v5.8H, v1.4S, #10

        sqxtun v23.8B, v5.8H

        trn1 v0.8B, v16.8B, v17.8B
        trn2 v1.8B, v16.8B, v17.8B
        trn1 v17.8B, v18.8B, v19.8B
        trn2 v19.8B, v18.8B, v19.8B
        trn1 v16.8B, v20.8B, v21.8B
        trn2 v21.8B, v20.8B, v21.8B
        trn1 v18.8B, v22.8B, v23.8B
        trn2 v23.8B, v22.8B, v23.8B

        trn1 v20.4H, v16.4H, v18.4H
        trn2 v18.4H, v16.4H, v18.4H
        trn1 v22.4H, v21.4H, v23.4H
        trn2 v23.4H, v21.4H, v23.4H
        trn1 v21.4H, v1.4H, v19.4H
        trn2 v1.4H, v1.4H, v19.4H
        trn1 v19.4H, v0.4H, v17.4H
        trn2 v0.4H, v0.4H, v17.4H

        trn1 v16.2S, v19.2S, v20.2S
        trn2 v20.2S, v19.2S, v20.2S

        trn1 v17.2S, v21.2S, v22.2S
        trn2 v21.2S, v21.2S, v22.2S

        trn2 v22.2S, v0.2S, v18.2S
        trn1 v18.2S, v0.2S, v18.2S

        trn1 v19.2S, v1.2S, v23.2S
        trn2 v23.2S, v1.2S, v23.2S

        ret
 # //.size put_h264_qpel8_hv_lowpass_neon_top, . - put_h264_qpel8_hv_lowpass_neon_top
 # //.endfunc


        .text
        .align 2
 # //.type put_h264_qpel8_hv_lowpass_neon, %function
 # //.func put_h264_qpel8_hv_lowpass_neon
put_h264_qpel8_hv_lowpass_neon:
        mov x10, x30
        bl put_h264_qpel8_hv_lowpass_neon_top

        st1 {v16.8B}, [x0], x2
        st1 {v17.8B}, [x0], x2
        st1 {v18.8B}, [x0], x2
        st1 {v19.8B}, [x0], x2
        st1 {v20.8B}, [x0], x2
        st1 {v21.8B}, [x0], x2
        st1 {v22.8B}, [x0], x2
        st1 {v23.8B}, [x0], x2

        ret x10
 # //.size put_h264_qpel8_hv_lowpass_neon, . - put_h264_qpel8_hv_lowpass_neon
 # //.endfunc
        .text
        .align 2
 # //.type avg_h264_qpel8_hv_lowpass_neon, %function
 # //.func avg_h264_qpel8_hv_lowpass_neon
avg_h264_qpel8_hv_lowpass_neon:
        mov x10, x30
        bl put_h264_qpel8_hv_lowpass_neon_top
        ld1 {v0.8B}, [x0], x2
        urhadd v16.8B, v16.8B, v0.8B
        ld1 {v1.8B}, [x0], x2
        urhadd v17.8B, v17.8B, v1.8B
        ld1 {v2.8B}, [x0], x2
        urhadd v18.8B, v18.8B, v2.8B
        ld1 {v3.8B}, [x0], x2
        urhadd v19.8B, v19.8B, v3.8B
        ld1 {v4.8B}, [x0], x2
        urhadd v20.8B, v20.8B, v4.8B
        ld1 {v5.8B}, [x0], x2
        urhadd v21.8B, v21.8B, v5.8B
        ld1 {v6.8B}, [x0], x2
        urhadd v22.8B, v22.8B, v6.8B
        ld1 {v7.8B}, [x0], x2
        urhadd v23.8B, v23.8B, v7.8B
        sub x0, x0, x2, lsl #3

        st1 {v16.8B}, [x0], x2
        st1 {v17.8B}, [x0], x2
        st1 {v18.8B}, [x0], x2
        st1 {v19.8B}, [x0], x2
        st1 {v20.8B}, [x0], x2
        st1 {v21.8B}, [x0], x2
        st1 {v22.8B}, [x0], x2
        st1 {v23.8B}, [x0], x2

        ret x10
 # //.size avg_h264_qpel8_hv_lowpass_neon, . - avg_h264_qpel8_hv_lowpass_neon
 # //.endfunc


        .text
        .align 2
 # //.type put_h264_qpel8_hv_lowpass_l2_neon, %function
 # //.func put_h264_qpel8_hv_lowpass_l2_neon
put_h264_qpel8_hv_lowpass_l2_neon:
        mov x10, x30
        bl put_h264_qpel8_hv_lowpass_neon_top

        ld1 {v0.8B, v1.8B}, [x2], #16
        ld1 {v2.8B, v3.8B}, [x2], #16
        urhadd v0.8B, v0.8B, v16.8B
        urhadd v1.8B, v1.8B, v17.8B
        ld1 {v4.8B, v5.8B}, [x2], #16
        urhadd v2.8B, v2.8B, v18.8B
        urhadd v3.8B, v3.8B, v19.8B
        ld1 {v6.8B, v7.8B}, [x2], #16
        urhadd v4.8B, v4.8B, v20.8B
        urhadd v5.8B, v5.8B, v21.8B
        urhadd v6.8B, v6.8B, v22.8B
        urhadd v7.8B, v7.8B, v23.8B
        st1 {v0.8B}, [x0], x3
        st1 {v1.8B}, [x0], x3
        st1 {v2.8B}, [x0], x3
        st1 {v3.8B}, [x0], x3
        st1 {v4.8B}, [x0], x3
        st1 {v5.8B}, [x0], x3
        st1 {v6.8B}, [x0], x3
        st1 {v7.8B}, [x0], x3

        ret x10
 # //.size put_h264_qpel8_hv_lowpass_l2_neon, . - put_h264_qpel8_hv_lowpass_l2_neon
 # //.endfunc
        .text
        .align 2
 # //.type avg_h264_qpel8_hv_lowpass_l2_neon, %function
 # //.func avg_h264_qpel8_hv_lowpass_l2_neon
avg_h264_qpel8_hv_lowpass_l2_neon:
        mov x10, x30
        bl put_h264_qpel8_hv_lowpass_neon_top

        ld1 {v0.8B, v1.8B}, [x2], #16
        ld1 {v2.8B, v3.8B}, [x2], #16
        urhadd v0.8B, v0.8B, v16.8B
        urhadd v1.8B, v1.8B, v17.8B
        ld1 {v4.8B, v5.8B}, [x2], #16
        urhadd v2.8B, v2.8B, v18.8B
        urhadd v3.8B, v3.8B, v19.8B
        ld1 {v6.8B, v7.8B}, [x2], #16
        urhadd v4.8B, v4.8B, v20.8B
        urhadd v5.8B, v5.8B, v21.8B
        urhadd v6.8B, v6.8B, v22.8B
        urhadd v7.8B, v7.8B, v23.8B
        ld1 {v16.8B}, [x0], x3
        urhadd v0.8B, v0.8B, v16.8B
        ld1 {v17.8B}, [x0], x3
        urhadd v1.8B, v1.8B, v17.8B
        ld1 {v18.8B}, [x0], x3
        urhadd v2.8B, v2.8B, v18.8B
        ld1 {v19.8B}, [x0], x3
        urhadd v3.8B, v3.8B, v19.8B
        ld1 {v20.8B}, [x0], x3
        urhadd v4.8B, v4.8B, v20.8B
        ld1 {v21.8B}, [x0], x3
        urhadd v5.8B, v5.8B, v21.8B
        ld1 {v22.8B}, [x0], x3
        urhadd v6.8B, v6.8B, v22.8B
        ld1 {v23.8B}, [x0], x3
        urhadd v7.8B, v7.8B, v23.8B
        sub x0, x0, x3, lsl #3
        st1 {v0.8B}, [x0], x3
        st1 {v1.8B}, [x0], x3
        st1 {v2.8B}, [x0], x3
        st1 {v3.8B}, [x0], x3
        st1 {v4.8B}, [x0], x3
        st1 {v5.8B}, [x0], x3
        st1 {v6.8B}, [x0], x3
        st1 {v7.8B}, [x0], x3

        ret x10
 # //.size avg_h264_qpel8_hv_lowpass_l2_neon, . - avg_h264_qpel8_hv_lowpass_l2_neon
 # //.endfunc


        .text
        .align 2
 # //.type put_h264_qpel16_hv_lowpass_neon, %function
 # //.func put_h264_qpel16_hv_lowpass_neon
put_h264_qpel16_hv_lowpass_neon:
        mov x13, x30
        bl put_h264_qpel8_hv_lowpass_neon
        sub x1, x1, x3, lsl #2
        bl put_h264_qpel8_hv_lowpass_neon
        sub x1, x1, x3, lsl #4
        sub x1, x1, x3, lsl #2
        add x1, x1, #8
        sub x0, x0, x2, lsl #4
        add x0, x0, #8
        bl put_h264_qpel8_hv_lowpass_neon
        sub x1, x1, x3, lsl #2
        mov x30, x13
        b put_h264_qpel8_hv_lowpass_neon
 # //.size put_h264_qpel16_hv_lowpass_neon, . - put_h264_qpel16_hv_lowpass_neon
 # //.endfunc

        .text
        .align 2
 # //.type put_h264_qpel16_hv_lowpass_l2_neon, %function
 # //.func put_h264_qpel16_hv_lowpass_l2_neon
put_h264_qpel16_hv_lowpass_l2_neon:
        mov x13, x30
        sub x2, x4, #256
        bl put_h264_qpel8_hv_lowpass_l2_neon
        sub x1, x1, x3, lsl #2
        bl put_h264_qpel8_hv_lowpass_l2_neon
        sub x1, x1, x3, lsl #4
        sub x1, x1, x3, lsl #2
        add x1, x1, #8
        sub x0, x0, x3, lsl #4
        add x0, x0, #8
        bl put_h264_qpel8_hv_lowpass_l2_neon
        sub x1, x1, x3, lsl #2
        mov x30, x13
        b put_h264_qpel8_hv_lowpass_l2_neon
 # //.size put_h264_qpel16_hv_lowpass_l2_neon, . - put_h264_qpel16_hv_lowpass_l2_neon
 # //.endfunc
        .text
        .align 2
 # //.type avg_h264_qpel16_hv_lowpass_neon, %function
 # //.func avg_h264_qpel16_hv_lowpass_neon
avg_h264_qpel16_hv_lowpass_neon:
        mov x13, x30
        bl avg_h264_qpel8_hv_lowpass_neon
        sub x1, x1, x3, lsl #2
        bl avg_h264_qpel8_hv_lowpass_neon
        sub x1, x1, x3, lsl #4
        sub x1, x1, x3, lsl #2
        add x1, x1, #8
        sub x0, x0, x2, lsl #4
        add x0, x0, #8
        bl avg_h264_qpel8_hv_lowpass_neon
        sub x1, x1, x3, lsl #2
        mov x30, x13
        b avg_h264_qpel8_hv_lowpass_neon
 # //.size avg_h264_qpel16_hv_lowpass_neon, . - avg_h264_qpel16_hv_lowpass_neon
 # //.endfunc

        .text
        .align 2
 # //.type avg_h264_qpel16_hv_lowpass_l2_neon, %function
 # //.func avg_h264_qpel16_hv_lowpass_l2_neon
avg_h264_qpel16_hv_lowpass_l2_neon:
        mov x13, x30
        sub x2, x4, #256
        bl avg_h264_qpel8_hv_lowpass_l2_neon
        sub x1, x1, x3, lsl #2
        bl avg_h264_qpel8_hv_lowpass_l2_neon
        sub x1, x1, x3, lsl #4
        sub x1, x1, x3, lsl #2
        add x1, x1, #8
        sub x0, x0, x3, lsl #4
        add x0, x0, #8
        bl avg_h264_qpel8_hv_lowpass_l2_neon
        sub x1, x1, x3, lsl #2
        mov x30, x13
        b avg_h264_qpel8_hv_lowpass_l2_neon
 # //.size avg_h264_qpel16_hv_lowpass_l2_neon, . - avg_h264_qpel16_hv_lowpass_l2_neon
 # //.endfunc


        .text
        .align 2
        .globl _ff_put_h264_qpel8_mc10_neon
 # //.type _ff_put_h264_qpel8_mc10_neon, %function
 # //.func _ff_put_h264_qpel8_mc10_neon
_ff_put_h264_qpel8_mc10_neon:
        movz w3, #20, lsl #16
        movk w3, #5
        mov v6.S[0], w3
        mov x3, x1
        sub x1, x1, #2
        mov x12, #8
        b put_h264_qpel8_h_lowpass_l2_neon
 # //.size ff_put_h264_qpel8_mc10_neon, . - ff_put_h264_qpel8_mc10_neon
 # //.endfunc

        .text
        .align 2
        .globl _ff_put_h264_qpel8_mc20_neon
 # //.type _ff_put_h264_qpel8_mc20_neon, %function
 # //.func _ff_put_h264_qpel8_mc20_neon
_ff_put_h264_qpel8_mc20_neon:
        movz w3, #20, lsl #16
        movk w3, #5
        mov v6.S[0], w3
        sub x1, x1, #2
        mov x3, x2
        mov x12, #8
        b put_h264_qpel8_h_lowpass_neon
 # //.size ff_put_h264_qpel8_mc20_neon, . - ff_put_h264_qpel8_mc20_neon
 # //.endfunc

        .text
        .align 2
        .globl _ff_put_h264_qpel8_mc30_neon
 # //.type _ff_put_h264_qpel8_mc30_neon, %function
 # //.func _ff_put_h264_qpel8_mc30_neon
_ff_put_h264_qpel8_mc30_neon:
        movz w3, #20, lsl #16
        movk w3, #5
        mov v6.S[0], w3
        add x3, x1, #1
        sub x1, x1, #2
        mov x12, #8
        b put_h264_qpel8_h_lowpass_l2_neon
 # //.size ff_put_h264_qpel8_mc30_neon, . - ff_put_h264_qpel8_mc30_neon
 # //.endfunc

        .text
        .align 2
        .globl _ff_put_h264_qpel8_mc01_neon
 # //.type _ff_put_h264_qpel8_mc01_neon, %function
 # //.func _ff_put_h264_qpel8_mc01_neon
_ff_put_h264_qpel8_mc01_neon:
        mov x14, x30
        mov x12, x1
put_h264_qpel8_mc01:
        movz w3, #20, lsl #16
        movk w3, #5
        mov v6.S[0], w3
        mov x3, x2
        sub x1, x1, x2, lsl #1
        bl put_h264_qpel8_v_lowpass_l2_neon
        ret x14
 # //.size ff_put_h264_qpel8_mc01_neon, . - ff_put_h264_qpel8_mc01_neon
 # //.endfunc

        .text
        .align 2
        .globl _ff_put_h264_qpel8_mc11_neon
 # //.type _ff_put_h264_qpel8_mc11_neon, %function
 # //.func _ff_put_h264_qpel8_mc11_neon
_ff_put_h264_qpel8_mc11_neon:
        mov x14, x30
        mov x8, x0
        mov x9, x1
put_h264_qpel8_mc11:
        movz w3, #20, lsl #16
        movk w3, #5
        mov v6.S[0], w3
        mov x11, sp
        sub sp, sp, #64
        mov x0, sp
        sub x1, x1, #2
        mov x3, #8
        mov x12, #8
        bl put_h264_qpel8_h_lowpass_neon
        mov x0, x8
        mov x3, x2
        mov x12, sp
        sub x1, x9, x2, lsl #1
        mov x2, #8
        bl put_h264_qpel8_v_lowpass_l2_neon
        mov sp, x11
        ret x14
 # //.size ff_put_h264_qpel8_mc11_neon, . - ff_put_h264_qpel8_mc11_neon
 # //.endfunc

        .text
        .align 2
        .globl _ff_put_h264_qpel8_mc21_neon
 # //.type _ff_put_h264_qpel8_mc21_neon, %function
 # //.func _ff_put_h264_qpel8_mc21_neon
_ff_put_h264_qpel8_mc21_neon:
        mov x14, x30
        mov x8, x0
        mov x9, x1
put_h264_qpel8_mc21:
        movz w3, #20, lsl #16
        movk w3, #5
        mov v6.S[0], w3
        mov x11, sp
        sub sp, sp, #(8*8+16*12)
        sub x1, x1, #2
        mov x3, #8
        mov x0, sp
        mov x12, #8
        bl put_h264_qpel8_h_lowpass_neon
        mov x4, x0
        mov x0, x8
        sub x1, x9, x2, lsl #1
        sub x1, x1, #2
        mov x3, x2
        sub x2, x4, #64
        bl put_h264_qpel8_hv_lowpass_l2_neon
        mov sp, x11
        ret x14
 # //.size ff_put_h264_qpel8_mc21_neon, . - ff_put_h264_qpel8_mc21_neon
 # //.endfunc

        .text
        .align 2
        .globl _ff_put_h264_qpel8_mc31_neon
 # //.type _ff_put_h264_qpel8_mc31_neon, %function
 # //.func _ff_put_h264_qpel8_mc31_neon
_ff_put_h264_qpel8_mc31_neon:
        add x1, x1, #1
        mov x14, x30
        mov x8, x0
        mov x9, x1
        sub x1, x1, #1
        b put_h264_qpel8_mc11
 # //.size ff_put_h264_qpel8_mc31_neon, . - ff_put_h264_qpel8_mc31_neon
 # //.endfunc

        .text
        .align 2
        .globl _ff_put_h264_qpel8_mc02_neon
 # //.type _ff_put_h264_qpel8_mc02_neon, %function
 # //.func _ff_put_h264_qpel8_mc02_neon
_ff_put_h264_qpel8_mc02_neon:
        mov x14, x30
        movz w3, #20, lsl #16
        movk w3, #5
        mov v6.S[0], w3
        sub x1, x1, x2, lsl #1
        mov x3, x2
        bl put_h264_qpel8_v_lowpass_neon
        ret x14
 # //.size ff_put_h264_qpel8_mc02_neon, . - ff_put_h264_qpel8_mc02_neon
 # //.endfunc

        .text
        .align 2
        .globl _ff_put_h264_qpel8_mc12_neon
 # //.type _ff_put_h264_qpel8_mc12_neon, %function
 # //.func _ff_put_h264_qpel8_mc12_neon
_ff_put_h264_qpel8_mc12_neon:
        mov x14, x30
        mov x8, x0
        mov x9, x1
put_h264_qpel8_mc12:
        movz w3, #20, lsl #16
        movk w3, #5
        mov v6.S[0], w3
        mov x11, sp
        sub sp, sp, #(8*8+16*12)
        sub x1, x1, x2, lsl #1
        mov x3, x2
        mov x2, #8
        mov x0, sp
        bl put_h264_qpel8_v_lowpass_neon
        mov x4, x0
        mov x0, x8
        sub x1, x9, x3, lsl #1
        sub x1, x1, #2
        sub x2, x4, #64
        bl put_h264_qpel8_hv_lowpass_l2_neon
        mov sp, x11
        ret x14
 # //.size ff_put_h264_qpel8_mc12_neon, . - ff_put_h264_qpel8_mc12_neon
 # //.endfunc

        .text
        .align 2
        .globl _ff_put_h264_qpel8_mc22_neon
 # //.type _ff_put_h264_qpel8_mc22_neon, %function
 # //.func _ff_put_h264_qpel8_mc22_neon
_ff_put_h264_qpel8_mc22_neon:
        mov x14, x30
        mov x11, sp
        sub x1, x1, x2, lsl #1
        sub x1, x1, #2
        mov x3, x2
        bl put_h264_qpel8_hv_lowpass_neon
        mov sp, x11
        ret x14
 # //.size ff_put_h264_qpel8_mc22_neon, . - ff_put_h264_qpel8_mc22_neon
 # //.endfunc

        .text
        .align 2
        .globl _ff_put_h264_qpel8_mc32_neon
 # //.type _ff_put_h264_qpel8_mc32_neon, %function
 # //.func _ff_put_h264_qpel8_mc32_neon
_ff_put_h264_qpel8_mc32_neon:
        mov x14, x30
        mov x8, x0
        mov x9, x1
        add x1, x1, #1
        b put_h264_qpel8_mc12
 # //.size ff_put_h264_qpel8_mc32_neon, . - ff_put_h264_qpel8_mc32_neon
 # //.endfunc

        .text
        .align 2
        .globl _ff_put_h264_qpel8_mc03_neon
 # //.type _ff_put_h264_qpel8_mc03_neon, %function
 # //.func _ff_put_h264_qpel8_mc03_neon
_ff_put_h264_qpel8_mc03_neon:
        mov x14, x30
        add x12, x1, x2
        b put_h264_qpel8_mc01
 # //.size ff_put_h264_qpel8_mc03_neon, . - ff_put_h264_qpel8_mc03_neon
 # //.endfunc

        .text
        .align 2
        .globl _ff_put_h264_qpel8_mc13_neon
 # //.type _ff_put_h264_qpel8_mc13_neon, %function
 # //.func _ff_put_h264_qpel8_mc13_neon
_ff_put_h264_qpel8_mc13_neon:
        mov x14, x30
        mov x8, x0
        mov x9, x1
        add x1, x1, x2
        b put_h264_qpel8_mc11
 # //.size ff_put_h264_qpel8_mc13_neon, . - ff_put_h264_qpel8_mc13_neon
 # //.endfunc

        .text
        .align 2
        .globl _ff_put_h264_qpel8_mc23_neon
 # //.type _ff_put_h264_qpel8_mc23_neon, %function
 # //.func _ff_put_h264_qpel8_mc23_neon
_ff_put_h264_qpel8_mc23_neon:
        mov x14, x30
        mov x8, x0
        mov x9, x1
        add x1, x1, x2
        b put_h264_qpel8_mc21
 # //.size ff_put_h264_qpel8_mc23_neon, . - ff_put_h264_qpel8_mc23_neon
 # //.endfunc

        .text
        .align 2
        .globl _ff_put_h264_qpel8_mc33_neon
 # //.type _ff_put_h264_qpel8_mc33_neon, %function
 # //.func _ff_put_h264_qpel8_mc33_neon
_ff_put_h264_qpel8_mc33_neon:
        add x1, x1, #1
        mov x14, x30
        mov x8, x0
        mov x9, x1
        add x1, x1, x2
        sub x1, x1, #1
        b put_h264_qpel8_mc11
 # //.size ff_put_h264_qpel8_mc33_neon, . - ff_put_h264_qpel8_mc33_neon
 # //.endfunc
        .text
        .align 2
        .globl _ff_avg_h264_qpel8_mc10_neon
 # //.type _ff_avg_h264_qpel8_mc10_neon, %function
 # //.func _ff_avg_h264_qpel8_mc10_neon
_ff_avg_h264_qpel8_mc10_neon:
        movz w3, #20, lsl #16
        movk w3, #5
        mov v6.S[0], w3
        mov x3, x1
        sub x1, x1, #2
        mov x12, #8
        b avg_h264_qpel8_h_lowpass_l2_neon
 # //.size ff_avg_h264_qpel8_mc10_neon, . - ff_avg_h264_qpel8_mc10_neon
 # //.endfunc

        .text
        .align 2
        .globl _ff_avg_h264_qpel8_mc20_neon
 # //.type _ff_avg_h264_qpel8_mc20_neon, %function
 # //.func _ff_avg_h264_qpel8_mc20_neon
_ff_avg_h264_qpel8_mc20_neon:
        movz w3, #20, lsl #16
        movk w3, #5
        mov v6.S[0], w3
        sub x1, x1, #2
        mov x3, x2
        mov x12, #8
        b avg_h264_qpel8_h_lowpass_neon
 # //.size ff_avg_h264_qpel8_mc20_neon, . - ff_avg_h264_qpel8_mc20_neon
 # //.endfunc

        .text
        .align 2
        .globl _ff_avg_h264_qpel8_mc30_neon
 # //.type _ff_avg_h264_qpel8_mc30_neon, %function
 # //.func _ff_avg_h264_qpel8_mc30_neon
_ff_avg_h264_qpel8_mc30_neon:
        movz w3, #20, lsl #16
        movk w3, #5
        mov v6.S[0], w3
        add x3, x1, #1
        sub x1, x1, #2
        mov x12, #8
        b avg_h264_qpel8_h_lowpass_l2_neon
 # //.size ff_avg_h264_qpel8_mc30_neon, . - ff_avg_h264_qpel8_mc30_neon
 # //.endfunc

        .text
        .align 2
        .globl _ff_avg_h264_qpel8_mc01_neon
 # //.type _ff_avg_h264_qpel8_mc01_neon, %function
 # //.func _ff_avg_h264_qpel8_mc01_neon
_ff_avg_h264_qpel8_mc01_neon:
        mov x14, x30
        mov x12, x1
avg_h264_qpel8_mc01:
        movz w3, #20, lsl #16
        movk w3, #5
        mov v6.S[0], w3
        mov x3, x2
        sub x1, x1, x2, lsl #1
        bl avg_h264_qpel8_v_lowpass_l2_neon
        ret x14
 # //.size ff_avg_h264_qpel8_mc01_neon, . - ff_avg_h264_qpel8_mc01_neon
 # //.endfunc

        .text
        .align 2
        .globl _ff_avg_h264_qpel8_mc11_neon
 # //.type _ff_avg_h264_qpel8_mc11_neon, %function
 # //.func _ff_avg_h264_qpel8_mc11_neon
_ff_avg_h264_qpel8_mc11_neon:
        mov x14, x30
        mov x8, x0
        mov x9, x1
avg_h264_qpel8_mc11:
        movz w3, #20, lsl #16
        movk w3, #5
        mov v6.S[0], w3
        mov x11, sp
        sub sp, sp, #64
        mov x0, sp
        sub x1, x1, #2
        mov x3, #8
        mov x12, #8
        bl put_h264_qpel8_h_lowpass_neon
        mov x0, x8
        mov x3, x2
        mov x12, sp
        sub x1, x9, x2, lsl #1
        mov x2, #8
        bl avg_h264_qpel8_v_lowpass_l2_neon
        mov sp, x11
        ret x14
 # //.size ff_avg_h264_qpel8_mc11_neon, . - ff_avg_h264_qpel8_mc11_neon
 # //.endfunc

        .text
        .align 2
        .globl _ff_avg_h264_qpel8_mc21_neon
 # //.type _ff_avg_h264_qpel8_mc21_neon, %function
 # //.func _ff_avg_h264_qpel8_mc21_neon
_ff_avg_h264_qpel8_mc21_neon:
        mov x14, x30
        mov x8, x0
        mov x9, x1
avg_h264_qpel8_mc21:
        movz w3, #20, lsl #16
        movk w3, #5
        mov v6.S[0], w3
        mov x11, sp
        sub sp, sp, #(8*8+16*12)
        sub x1, x1, #2
        mov x3, #8
        mov x0, sp
        mov x12, #8
        bl put_h264_qpel8_h_lowpass_neon
        mov x4, x0
        mov x0, x8
        sub x1, x9, x2, lsl #1
        sub x1, x1, #2
        mov x3, x2
        sub x2, x4, #64
        bl avg_h264_qpel8_hv_lowpass_l2_neon
        mov sp, x11
        ret x14
 # //.size ff_avg_h264_qpel8_mc21_neon, . - ff_avg_h264_qpel8_mc21_neon
 # //.endfunc

        .text
        .align 2
        .globl _ff_avg_h264_qpel8_mc31_neon
 # //.type _ff_avg_h264_qpel8_mc31_neon, %function
 # //.func _ff_avg_h264_qpel8_mc31_neon
_ff_avg_h264_qpel8_mc31_neon:
        add x1, x1, #1
        mov x14, x30
        mov x8, x0
        mov x9, x1
        sub x1, x1, #1
        b avg_h264_qpel8_mc11
 # //.size ff_avg_h264_qpel8_mc31_neon, . - ff_avg_h264_qpel8_mc31_neon
 # //.endfunc

        .text
        .align 2
        .globl _ff_avg_h264_qpel8_mc02_neon
 # //.type _ff_avg_h264_qpel8_mc02_neon, %function
 # //.func _ff_avg_h264_qpel8_mc02_neon
_ff_avg_h264_qpel8_mc02_neon:
        mov x14, x30
        movz w3, #20, lsl #16
        movk w3, #5
        mov v6.S[0], w3
        sub x1, x1, x2, lsl #1
        mov x3, x2
        bl avg_h264_qpel8_v_lowpass_neon
        ret x14
 # //.size ff_avg_h264_qpel8_mc02_neon, . - ff_avg_h264_qpel8_mc02_neon
 # //.endfunc

        .text
        .align 2
        .globl _ff_avg_h264_qpel8_mc12_neon
 # //.type _ff_avg_h264_qpel8_mc12_neon, %function
 # //.func _ff_avg_h264_qpel8_mc12_neon
_ff_avg_h264_qpel8_mc12_neon:
        mov x14, x30
        mov x8, x0
        mov x9, x1
avg_h264_qpel8_mc12:
        movz w3, #20, lsl #16
        movk w3, #5
        mov v6.S[0], w3
        mov x11, sp
        sub sp, sp, #(8*8+16*12)
        sub x1, x1, x2, lsl #1
        mov x3, x2
        mov x2, #8
        mov x0, sp
        bl put_h264_qpel8_v_lowpass_neon
        mov x4, x0
        mov x0, x8
        sub x1, x9, x3, lsl #1
        sub x1, x1, #2
        sub x2, x4, #64
        bl avg_h264_qpel8_hv_lowpass_l2_neon
        mov sp, x11
        ret x14
 # //.size ff_avg_h264_qpel8_mc12_neon, . - ff_avg_h264_qpel8_mc12_neon
 # //.endfunc

        .text
        .align 2
        .globl _ff_avg_h264_qpel8_mc22_neon
 # //.type _ff_avg_h264_qpel8_mc22_neon, %function
 # //.func _ff_avg_h264_qpel8_mc22_neon
_ff_avg_h264_qpel8_mc22_neon:
        mov x14, x30
        mov x11, sp
        sub x1, x1, x2, lsl #1
        sub x1, x1, #2
        mov x3, x2
        bl avg_h264_qpel8_hv_lowpass_neon
        mov sp, x11
        ret x14
 # //.size ff_avg_h264_qpel8_mc22_neon, . - ff_avg_h264_qpel8_mc22_neon
 # //.endfunc

        .text
        .align 2
        .globl _ff_avg_h264_qpel8_mc32_neon
 # //.type _ff_avg_h264_qpel8_mc32_neon, %function
 # //.func _ff_avg_h264_qpel8_mc32_neon
_ff_avg_h264_qpel8_mc32_neon:
        mov x14, x30
        mov x8, x0
        mov x9, x1
        add x1, x1, #1
        b avg_h264_qpel8_mc12
 # //.size ff_avg_h264_qpel8_mc32_neon, . - ff_avg_h264_qpel8_mc32_neon
 # //.endfunc

        .text
        .align 2
        .globl _ff_avg_h264_qpel8_mc03_neon
 # //.type _ff_avg_h264_qpel8_mc03_neon, %function
 # //.func _ff_avg_h264_qpel8_mc03_neon
_ff_avg_h264_qpel8_mc03_neon:
        mov x14, x30
        add x12, x1, x2
        b avg_h264_qpel8_mc01
 # //.size ff_avg_h264_qpel8_mc03_neon, . - ff_avg_h264_qpel8_mc03_neon
 # //.endfunc

        .text
        .align 2
        .globl _ff_avg_h264_qpel8_mc13_neon
 # //.type _ff_avg_h264_qpel8_mc13_neon, %function
 # //.func _ff_avg_h264_qpel8_mc13_neon
_ff_avg_h264_qpel8_mc13_neon:
        mov x14, x30
        mov x8, x0
        mov x9, x1
        add x1, x1, x2
        b avg_h264_qpel8_mc11
 # //.size ff_avg_h264_qpel8_mc13_neon, . - ff_avg_h264_qpel8_mc13_neon
 # //.endfunc

        .text
        .align 2
        .globl _ff_avg_h264_qpel8_mc23_neon
 # //.type _ff_avg_h264_qpel8_mc23_neon, %function
 # //.func _ff_avg_h264_qpel8_mc23_neon
_ff_avg_h264_qpel8_mc23_neon:
        mov x14, x30
        mov x8, x0
        mov x9, x1
        add x1, x1, x2
        b avg_h264_qpel8_mc21
 # //.size ff_avg_h264_qpel8_mc23_neon, . - ff_avg_h264_qpel8_mc23_neon
 # //.endfunc

        .text
        .align 2
        .globl _ff_avg_h264_qpel8_mc33_neon
 # //.type _ff_avg_h264_qpel8_mc33_neon, %function
 # //.func _ff_avg_h264_qpel8_mc33_neon
_ff_avg_h264_qpel8_mc33_neon:
        add x1, x1, #1
        mov x14, x30
        mov x8, x0
        mov x9, x1
        add x1, x1, x2
        sub x1, x1, #1
        b avg_h264_qpel8_mc11
 # //.size ff_avg_h264_qpel8_mc33_neon, . - ff_avg_h264_qpel8_mc33_neon
 # //.endfunc


        .text
        .align 2
        .globl _ff_put_h264_qpel16_mc10_neon
 # //.type _ff_put_h264_qpel16_mc10_neon, %function
 # //.func _ff_put_h264_qpel16_mc10_neon
_ff_put_h264_qpel16_mc10_neon:
        movz w3, #20, lsl #16
        movk w3, #5
        mov v6.S[0], w3
        mov x3, x1
        sub x1, x1, #2
        b put_h264_qpel16_h_lowpass_l2_neon
 # //.size ff_put_h264_qpel16_mc10_neon, . - ff_put_h264_qpel16_mc10_neon
 # //.endfunc

        .text
        .align 2
        .globl _ff_put_h264_qpel16_mc20_neon
 # //.type _ff_put_h264_qpel16_mc20_neon, %function
 # //.func _ff_put_h264_qpel16_mc20_neon
_ff_put_h264_qpel16_mc20_neon:
        movz w3, #20, lsl #16
        movk w3, #5
        mov v6.S[0], w3
        sub x1, x1, #2
        mov x3, x2
        b put_h264_qpel16_h_lowpass_neon
 # //.size ff_put_h264_qpel16_mc20_neon, . - ff_put_h264_qpel16_mc20_neon
 # //.endfunc

        .text
        .align 2
        .globl _ff_put_h264_qpel16_mc30_neon
 # //.type _ff_put_h264_qpel16_mc30_neon, %function
 # //.func _ff_put_h264_qpel16_mc30_neon
_ff_put_h264_qpel16_mc30_neon:
        movz w3, #20, lsl #16
        movk w3, #5
        mov v6.S[0], w3
        add x3, x1, #1
        sub x1, x1, #2
        b put_h264_qpel16_h_lowpass_l2_neon
 # //.size ff_put_h264_qpel16_mc30_neon, . - ff_put_h264_qpel16_mc30_neon
 # //.endfunc

        .text
        .align 2
        .globl _ff_put_h264_qpel16_mc01_neon
 # //.type _ff_put_h264_qpel16_mc01_neon, %function
 # //.func _ff_put_h264_qpel16_mc01_neon
_ff_put_h264_qpel16_mc01_neon:
        mov x14, x30
        mov x12, x1
put_h264_qpel16_mc01:
        movz w3, #20, lsl #16
        movk w3, #5
        mov v6.S[0], w3
        mov x3, x2
        sub x1, x1, x2, lsl #1
        bl put_h264_qpel16_v_lowpass_l2_neon
        ret x14
 # //.size ff_put_h264_qpel16_mc01_neon, . - ff_put_h264_qpel16_mc01_neon
 # //.endfunc

        .text
        .align 2
        .globl _ff_put_h264_qpel16_mc11_neon
 # //.type _ff_put_h264_qpel16_mc11_neon, %function
 # //.func _ff_put_h264_qpel16_mc11_neon
_ff_put_h264_qpel16_mc11_neon:
        mov x14, x30
        mov x8, x0
        mov x9, x1
put_h264_qpel16_mc11:
        movz w3, #20, lsl #16
        movk w3, #5
        mov v6.S[0], w3
        mov x11, sp
        sub sp, sp, #256
        mov x0, sp
        sub x1, x1, #2
        mov x3, #16
        bl put_h264_qpel16_h_lowpass_neon
        mov x0, x8
        mov x3, x2
        mov x12, sp
        sub x1, x9, x2, lsl #1
        mov x2, #16
        bl put_h264_qpel16_v_lowpass_l2_neon
        mov sp, x11
        ret x14
 # //.size ff_put_h264_qpel16_mc11_neon, . - ff_put_h264_qpel16_mc11_neon
 # //.endfunc

        .text
        .align 2
        .globl _ff_put_h264_qpel16_mc21_neon
 # //.type _ff_put_h264_qpel16_mc21_neon, %function
 # //.func _ff_put_h264_qpel16_mc21_neon
_ff_put_h264_qpel16_mc21_neon:
        mov x14, x30
        mov x8, x0
        mov x9, x1
put_h264_qpel16_mc21:
        movz w3, #20, lsl #16
        movk w3, #5
        mov v6.S[0], w3
        mov x11, sp
        sub sp, sp, #(16*16+16*12)
        sub x1, x1, #2
        mov x0, sp
        bl put_h264_qpel16_h_lowpass_neon_packed
        mov x4, x0
        mov x0, x8
        sub x1, x9, x2, lsl #1
        sub x1, x1, #2
        mov x3, x2
        bl put_h264_qpel16_hv_lowpass_l2_neon
        mov sp, x11
        ret x14
 # //.size ff_put_h264_qpel16_mc21_neon, . - ff_put_h264_qpel16_mc21_neon
 # //.endfunc

        .text
        .align 2
        .globl _ff_put_h264_qpel16_mc31_neon
 # //.type _ff_put_h264_qpel16_mc31_neon, %function
 # //.func _ff_put_h264_qpel16_mc31_neon
_ff_put_h264_qpel16_mc31_neon:
        add x1, x1, #1
        mov x14, x30
        mov x8, x0
        mov x9, x1
        sub x1, x1, #1
        b put_h264_qpel16_mc11
 # //.size ff_put_h264_qpel16_mc31_neon, . - ff_put_h264_qpel16_mc31_neon
 # //.endfunc

        .text
        .align 2
        .globl _ff_put_h264_qpel16_mc02_neon
 # //.type _ff_put_h264_qpel16_mc02_neon, %function
 # //.func _ff_put_h264_qpel16_mc02_neon
_ff_put_h264_qpel16_mc02_neon:
        mov x14, x30
        movz w3, #20, lsl #16
        movk w3, #5
        mov v6.S[0], w3
        sub x1, x1, x2, lsl #1
        mov x3, x2
        bl put_h264_qpel16_v_lowpass_neon
        ret x14
 # //.size ff_put_h264_qpel16_mc02_neon, . - ff_put_h264_qpel16_mc02_neon
 # //.endfunc

        .text
        .align 2
        .globl _ff_put_h264_qpel16_mc12_neon
 # //.type _ff_put_h264_qpel16_mc12_neon, %function
 # //.func _ff_put_h264_qpel16_mc12_neon
_ff_put_h264_qpel16_mc12_neon:
        mov x14, x30
        mov x8, x0
        mov x9, x1
put_h264_qpel16_mc12:
        movz w3, #20, lsl #16
        movk w3, #5
        mov v6.S[0], w3
        mov x11, sp
        sub sp, sp, #(16*16+16*12)
        sub x1, x1, x2, lsl #1
        mov x0, sp
        mov x3, x2
        bl put_h264_qpel16_v_lowpass_neon_packed
        mov x4, x0
        mov x0, x8
        sub x1, x9, x3, lsl #1
        sub x1, x1, #2
        mov x2, x3
        bl put_h264_qpel16_hv_lowpass_l2_neon
        mov sp, x11
        ret x14
 # //.size ff_put_h264_qpel16_mc12_neon, . - ff_put_h264_qpel16_mc12_neon
 # //.endfunc

        .text
        .align 2
        .globl _ff_put_h264_qpel16_mc22_neon
 # //.type _ff_put_h264_qpel16_mc22_neon, %function
 # //.func _ff_put_h264_qpel16_mc22_neon
_ff_put_h264_qpel16_mc22_neon:
        mov x14, x30
        movz w3, #20, lsl #16
        movk w3, #5
        mov v6.S[0], w3
        mov x11, sp
        sub x1, x1, x2, lsl #1
        sub x1, x1, #2
        mov x3, x2
        bl put_h264_qpel16_hv_lowpass_neon
        mov sp, x11
        ret x14
 # //.size ff_put_h264_qpel16_mc22_neon, . - ff_put_h264_qpel16_mc22_neon
 # //.endfunc

        .text
        .align 2
        .globl _ff_put_h264_qpel16_mc32_neon
 # //.type _ff_put_h264_qpel16_mc32_neon, %function
 # //.func _ff_put_h264_qpel16_mc32_neon
_ff_put_h264_qpel16_mc32_neon:
        mov x14, x30
        mov x8, x0
        mov x9, x1
        add x1, x1, #1
        b put_h264_qpel16_mc12
 # //.size ff_put_h264_qpel16_mc32_neon, . - ff_put_h264_qpel16_mc32_neon
 # //.endfunc

        .text
        .align 2
        .globl _ff_put_h264_qpel16_mc03_neon
 # //.type _ff_put_h264_qpel16_mc03_neon, %function
 # //.func _ff_put_h264_qpel16_mc03_neon
_ff_put_h264_qpel16_mc03_neon:
        mov x14, x30
        add x12, x1, x2
        b put_h264_qpel16_mc01
 # //.size ff_put_h264_qpel16_mc03_neon, . - ff_put_h264_qpel16_mc03_neon
 # //.endfunc

        .text
        .align 2
        .globl _ff_put_h264_qpel16_mc13_neon
 # //.type _ff_put_h264_qpel16_mc13_neon, %function
 # //.func _ff_put_h264_qpel16_mc13_neon
_ff_put_h264_qpel16_mc13_neon:
        mov x14, x30
        mov x8, x0
        mov x9, x1
        add x1, x1, x2
        b put_h264_qpel16_mc11
 # //.size ff_put_h264_qpel16_mc13_neon, . - ff_put_h264_qpel16_mc13_neon
 # //.endfunc

        .text
        .align 2
        .globl _ff_put_h264_qpel16_mc23_neon
 # //.type _ff_put_h264_qpel16_mc23_neon, %function
 # //.func _ff_put_h264_qpel16_mc23_neon
_ff_put_h264_qpel16_mc23_neon:
        mov x14, x30
        mov x8, x0
        mov x9, x1
        add x1, x1, x2
        b put_h264_qpel16_mc21
 # //.size ff_put_h264_qpel16_mc23_neon, . - ff_put_h264_qpel16_mc23_neon
 # //.endfunc

        .text
        .align 2
        .globl _ff_put_h264_qpel16_mc33_neon
 # //.type _ff_put_h264_qpel16_mc33_neon, %function
 # //.func _ff_put_h264_qpel16_mc33_neon
_ff_put_h264_qpel16_mc33_neon:
        add x1, x1, #1
        mov x14, x30
        mov x8, x0
        mov x9, x1
        add x1, x1, x2
        sub x1, x1, #1
        b put_h264_qpel16_mc11
 # //.size ff_put_h264_qpel16_mc33_neon, . - ff_put_h264_qpel16_mc33_neon
 # //.endfunc
        .text
        .align 2
        .globl _ff_avg_h264_qpel16_mc10_neon
 # //.type _ff_avg_h264_qpel16_mc10_neon, %function
 # //.func _ff_avg_h264_qpel16_mc10_neon
_ff_avg_h264_qpel16_mc10_neon:
        movz w3, #20, lsl #16
        movk w3, #5
        mov v6.S[0], w3
        mov x3, x1
        sub x1, x1, #2
        b avg_h264_qpel16_h_lowpass_l2_neon
 # //.size ff_avg_h264_qpel16_mc10_neon, . - ff_avg_h264_qpel16_mc10_neon
 # //.endfunc

        .text
        .align 2
        .globl _ff_avg_h264_qpel16_mc20_neon
 # //.type _ff_avg_h264_qpel16_mc20_neon, %function
 # //.func _ff_avg_h264_qpel16_mc20_neon
_ff_avg_h264_qpel16_mc20_neon:
        movz w3, #20, lsl #16
        movk w3, #5
        mov v6.S[0], w3
        sub x1, x1, #2
        mov x3, x2
        b avg_h264_qpel16_h_lowpass_neon
 # //.size ff_avg_h264_qpel16_mc20_neon, . - ff_avg_h264_qpel16_mc20_neon
 # //.endfunc

        .text
        .align 2
        .globl _ff_avg_h264_qpel16_mc30_neon
 # //.type _ff_avg_h264_qpel16_mc30_neon, %function
 # //.func _ff_avg_h264_qpel16_mc30_neon
_ff_avg_h264_qpel16_mc30_neon:
        movz w3, #20, lsl #16
        movk w3, #5
        mov v6.S[0], w3
        add x3, x1, #1
        sub x1, x1, #2
        b avg_h264_qpel16_h_lowpass_l2_neon
 # //.size ff_avg_h264_qpel16_mc30_neon, . - ff_avg_h264_qpel16_mc30_neon
 # //.endfunc

        .text
        .align 2
        .globl _ff_avg_h264_qpel16_mc01_neon
 # //.type _ff_avg_h264_qpel16_mc01_neon, %function
 # //.func _ff_avg_h264_qpel16_mc01_neon
_ff_avg_h264_qpel16_mc01_neon:
        mov x14, x30
        mov x12, x1
avg_h264_qpel16_mc01:
        movz w3, #20, lsl #16
        movk w3, #5
        mov v6.S[0], w3
        mov x3, x2
        sub x1, x1, x2, lsl #1
        bl avg_h264_qpel16_v_lowpass_l2_neon
        ret x14
 # //.size ff_avg_h264_qpel16_mc01_neon, . - ff_avg_h264_qpel16_mc01_neon
 # //.endfunc

        .text
        .align 2
        .globl _ff_avg_h264_qpel16_mc11_neon
 # //.type _ff_avg_h264_qpel16_mc11_neon, %function
 # //.func _ff_avg_h264_qpel16_mc11_neon
_ff_avg_h264_qpel16_mc11_neon:
        mov x14, x30
        mov x8, x0
        mov x9, x1
avg_h264_qpel16_mc11:
        movz w3, #20, lsl #16
        movk w3, #5
        mov v6.S[0], w3
        mov x11, sp
        sub sp, sp, #256
        mov x0, sp
        sub x1, x1, #2
        mov x3, #16
        bl put_h264_qpel16_h_lowpass_neon
        mov x0, x8
        mov x3, x2
        mov x12, sp
        sub x1, x9, x2, lsl #1
        mov x2, #16
        bl avg_h264_qpel16_v_lowpass_l2_neon
        mov sp, x11
        ret x14
 # //.size ff_avg_h264_qpel16_mc11_neon, . - ff_avg_h264_qpel16_mc11_neon
 # //.endfunc

        .text
        .align 2
        .globl _ff_avg_h264_qpel16_mc21_neon
 # //.type _ff_avg_h264_qpel16_mc21_neon, %function
 # //.func _ff_avg_h264_qpel16_mc21_neon
_ff_avg_h264_qpel16_mc21_neon:
        mov x14, x30
        mov x8, x0
        mov x9, x1
avg_h264_qpel16_mc21:
        movz w3, #20, lsl #16
        movk w3, #5
        mov v6.S[0], w3
        mov x11, sp
        sub sp, sp, #(16*16+16*12)
        sub x1, x1, #2
        mov x0, sp
        bl put_h264_qpel16_h_lowpass_neon_packed
        mov x4, x0
        mov x0, x8
        sub x1, x9, x2, lsl #1
        sub x1, x1, #2
        mov x3, x2
        bl avg_h264_qpel16_hv_lowpass_l2_neon
        mov sp, x11
        ret x14
 # //.size ff_avg_h264_qpel16_mc21_neon, . - ff_avg_h264_qpel16_mc21_neon
 # //.endfunc

        .text
        .align 2
        .globl _ff_avg_h264_qpel16_mc31_neon
 # //.type _ff_avg_h264_qpel16_mc31_neon, %function
 # //.func _ff_avg_h264_qpel16_mc31_neon
_ff_avg_h264_qpel16_mc31_neon:
        add x1, x1, #1
        mov x14, x30
        mov x8, x0
        mov x9, x1
        sub x1, x1, #1
        b avg_h264_qpel16_mc11
 # //.size ff_avg_h264_qpel16_mc31_neon, . - ff_avg_h264_qpel16_mc31_neon
 # //.endfunc

        .text
        .align 2
        .globl _ff_avg_h264_qpel16_mc02_neon
 # //.type _ff_avg_h264_qpel16_mc02_neon, %function
 # //.func _ff_avg_h264_qpel16_mc02_neon
_ff_avg_h264_qpel16_mc02_neon:
        mov x14, x30
        movz w3, #20, lsl #16
        movk w3, #5
        mov v6.S[0], w3
        sub x1, x1, x2, lsl #1
        mov x3, x2
        bl avg_h264_qpel16_v_lowpass_neon
        ret x14
 # //.size ff_avg_h264_qpel16_mc02_neon, . - ff_avg_h264_qpel16_mc02_neon
 # //.endfunc

        .text
        .align 2
        .globl _ff_avg_h264_qpel16_mc12_neon
 # //.type _ff_avg_h264_qpel16_mc12_neon, %function
 # //.func _ff_avg_h264_qpel16_mc12_neon
_ff_avg_h264_qpel16_mc12_neon:
        mov x14, x30
        mov x8, x0
        mov x9, x1
avg_h264_qpel16_mc12:
        movz w3, #20, lsl #16
        movk w3, #5
        mov v6.S[0], w3
        mov x11, sp
        sub sp, sp, #(16*16+16*12)
        sub x1, x1, x2, lsl #1
        mov x0, sp
        mov x3, x2
        bl put_h264_qpel16_v_lowpass_neon_packed
        mov x4, x0
        mov x0, x8
        sub x1, x9, x3, lsl #1
        sub x1, x1, #2
        mov x2, x3
        bl avg_h264_qpel16_hv_lowpass_l2_neon
        mov sp, x11
        ret x14
 # //.size ff_avg_h264_qpel16_mc12_neon, . - ff_avg_h264_qpel16_mc12_neon
 # //.endfunc

        .text
        .align 2
        .globl _ff_avg_h264_qpel16_mc22_neon
 # //.type _ff_avg_h264_qpel16_mc22_neon, %function
 # //.func _ff_avg_h264_qpel16_mc22_neon
_ff_avg_h264_qpel16_mc22_neon:
        mov x14, x30
        movz w3, #20, lsl #16
        movk w3, #5
        mov v6.S[0], w3
        mov x11, sp
        sub x1, x1, x2, lsl #1
        sub x1, x1, #2
        mov x3, x2
        bl avg_h264_qpel16_hv_lowpass_neon
        mov sp, x11
        ret x14
 # //.size ff_avg_h264_qpel16_mc22_neon, . - ff_avg_h264_qpel16_mc22_neon
 # //.endfunc

        .text
        .align 2
        .globl _ff_avg_h264_qpel16_mc32_neon
 # //.type _ff_avg_h264_qpel16_mc32_neon, %function
 # //.func _ff_avg_h264_qpel16_mc32_neon
_ff_avg_h264_qpel16_mc32_neon:
        mov x14, x30
        mov x8, x0
        mov x9, x1
        add x1, x1, #1
        b avg_h264_qpel16_mc12
 # //.size ff_avg_h264_qpel16_mc32_neon, . - ff_avg_h264_qpel16_mc32_neon
 # //.endfunc

        .text
        .align 2
        .globl _ff_avg_h264_qpel16_mc03_neon
 # //.type _ff_avg_h264_qpel16_mc03_neon, %function
 # //.func _ff_avg_h264_qpel16_mc03_neon
_ff_avg_h264_qpel16_mc03_neon:
        mov x14, x30
        add x12, x1, x2
        b avg_h264_qpel16_mc01
 # //.size ff_avg_h264_qpel16_mc03_neon, . - ff_avg_h264_qpel16_mc03_neon
 # //.endfunc

        .text
        .align 2
        .globl _ff_avg_h264_qpel16_mc13_neon
 # //.type _ff_avg_h264_qpel16_mc13_neon, %function
 # //.func _ff_avg_h264_qpel16_mc13_neon
_ff_avg_h264_qpel16_mc13_neon:
        mov x14, x30
        mov x8, x0
        mov x9, x1
        add x1, x1, x2
        b avg_h264_qpel16_mc11
 # //.size ff_avg_h264_qpel16_mc13_neon, . - ff_avg_h264_qpel16_mc13_neon
 # //.endfunc

        .text
        .align 2
        .globl _ff_avg_h264_qpel16_mc23_neon
 # //.type _ff_avg_h264_qpel16_mc23_neon, %function
 # //.func _ff_avg_h264_qpel16_mc23_neon
_ff_avg_h264_qpel16_mc23_neon:
        mov x14, x30
        mov x8, x0
        mov x9, x1
        add x1, x1, x2
        b avg_h264_qpel16_mc21
 # //.size ff_avg_h264_qpel16_mc23_neon, . - ff_avg_h264_qpel16_mc23_neon
 # //.endfunc

        .text
        .align 2
        .globl _ff_avg_h264_qpel16_mc33_neon
 # //.type _ff_avg_h264_qpel16_mc33_neon, %function
 # //.func _ff_avg_h264_qpel16_mc33_neon
_ff_avg_h264_qpel16_mc33_neon:
        add x1, x1, #1
        mov x14, x30
        mov x8, x0
        mov x9, x1
        add x1, x1, x2
        sub x1, x1, #1
        b avg_h264_qpel16_mc11
 # //.size ff_avg_h264_qpel16_mc33_neon, . - ff_avg_h264_qpel16_mc33_neon
 # //.endfunc
.text
.align 2

#endif
