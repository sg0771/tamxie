#ifdef __arm64__
        .text
        .align 2
        .globl _ff_put_h264_qpel16_mc00_neon
 # //.type _ff_put_h264_qpel16_mc00_neon, %function
 # //.func _ff_put_h264_qpel16_mc00_neon
_ff_put_h264_qpel16_mc00_neon:
        mov w3, #16
 # //.size ff_put_h264_qpel16_mc00_neon, . - ff_put_h264_qpel16_mc00_neon
 # //.endfunc

        .text
        .align 2
        .globl _ff_put_pixels16_neon
 # //.type _ff_put_pixels16_neon, %function
 # //.func _ff_put_pixels16_neon
_ff_put_pixels16_neon:
1: ld1 {v0.16B}, [x1], x2
        ld1 {v1.16B}, [x1], x2
        ld1 {v2.16B}, [x1], x2
        ld1 {v3.16B}, [x1], x2
        subs w3, w3, #4
        st1 {v0.16B}, [x0], x2
        st1 {v1.16B}, [x0], x2
        st1 {v2.16B}, [x0], x2
        st1 {v3.16B}, [x0], x2
        b.ne 1b
        ret
 # //.size ff_put_pixels16_neon, . - ff_put_pixels16_neon
 # //.endfunc
        .text
        .align 2
        .globl _ff_put_pixels16_x2_neon
 # //.type _ff_put_pixels16_x2_neon, %function
 # //.func _ff_put_pixels16_x2_neon
_ff_put_pixels16_x2_neon:
1: ld1 {v0.16B, v1.16B}, [x1], x2
        ld1 {v2.16B, v3.16B}, [x1], x2
        subs w3, w3, #2
        ext v1.16B, v0.16B, v1.16B, #1
        urhadd v0.16B, v0.16B, v1.16B
        ext v3.16B, v2.16B, v3.16B, #1
        urhadd v2.16B, v2.16B, v3.16B
        st1 {v0.16B}, [x0], x2
        st1 {v2.16B}, [x0], x2
        b.ne 1b
        ret
 # //.size ff_put_pixels16_x2_neon, . - ff_put_pixels16_x2_neon
 # //.endfunc
        .text
        .align 2
        .globl _ff_put_pixels16_x2_no_rnd_neon
 # //.type _ff_put_pixels16_x2_no_rnd_neon, %function
 # //.func _ff_put_pixels16_x2_no_rnd_neon
_ff_put_pixels16_x2_no_rnd_neon:
1: ld1 {v0.16B, v1.16B}, [x1], x2
        ld1 {v2.16B, v3.16B}, [x1], x2
        subs w3, w3, #2
        ext v1.16B, v0.16B, v1.16B, #1
        uhadd v0.16B, v0.16B, v1.16B
        ext v3.16B, v2.16B, v3.16B, #1
        uhadd v2.16B, v2.16B, v3.16B
        st1 {v0.16B}, [x0], x2
        st1 {v2.16B}, [x0], x2
        b.ne 1b
        ret
 # //.size ff_put_pixels16_x2_no_rnd_neon, . - ff_put_pixels16_x2_no_rnd_neon
 # //.endfunc
        .text
        .align 2
        .globl _ff_put_pixels16_y2_neon
 # //.type _ff_put_pixels16_y2_neon, %function
 # //.func _ff_put_pixels16_y2_neon
_ff_put_pixels16_y2_neon:
        sub w3, w3, #2
        ld1 {v0.16B}, [x1], x2
        ld1 {v1.16B}, [x1], x2
1: subs w3, w3, #2
        urhadd v2.16B, v0.16B, v1.16B
        ld1 {v0.16B}, [x1], x2
        urhadd v3.16B, v0.16B, v1.16B
        ld1 {v1.16B}, [x1], x2
        st1 {v2.16B}, [x0], x2
        st1 {v3.16B}, [x0], x2
        b.ne 1b

        urhadd v2.16B, v0.16B, v1.16B
        ld1 {v0.16B}, [x1], x2
        urhadd v3.16B, v0.16B, v1.16B
        st1 {v2.16B}, [x0], x2
        st1 {v3.16B}, [x0], x2

        ret
 # //.size ff_put_pixels16_y2_neon, . - ff_put_pixels16_y2_neon
 # //.endfunc
        .text
        .align 2
        .globl _ff_put_pixels16_y2_no_rnd_neon
 # //.type _ff_put_pixels16_y2_no_rnd_neon, %function
 # //.func _ff_put_pixels16_y2_no_rnd_neon
_ff_put_pixels16_y2_no_rnd_neon:
        sub w3, w3, #2
        ld1 {v0.16B}, [x1], x2
        ld1 {v1.16B}, [x1], x2
1: subs w3, w3, #2
        uhadd v2.16B, v0.16B, v1.16B
        ld1 {v0.16B}, [x1], x2
        uhadd v3.16B, v0.16B, v1.16B
        ld1 {v1.16B}, [x1], x2
        st1 {v2.16B}, [x0], x2
        st1 {v3.16B}, [x0], x2
        b.ne 1b

        uhadd v2.16B, v0.16B, v1.16B
        ld1 {v0.16B}, [x1], x2
        uhadd v3.16B, v0.16B, v1.16B
        st1 {v2.16B}, [x0], x2
        st1 {v3.16B}, [x0], x2

        ret
 # //.size ff_put_pixels16_y2_no_rnd_neon, . - ff_put_pixels16_y2_no_rnd_neon
 # //.endfunc
        .text
        .align 2
        .globl _ff_put_pixels16_xy2_neon
 # //.type _ff_put_pixels16_xy2_neon, %function
 # //.func _ff_put_pixels16_xy2_neon
_ff_put_pixels16_xy2_neon:
        sub w3, w3, #2
        ld1 {v0.16B, v1.16B}, [x1], x2
        ld1 {v4.16B, v5.16B}, [x1], x2
        ext v1.16B, v0.16B, v1.16B, #1
        ext v5.16B, v4.16B, v5.16B, #1
        uaddl v16.8H, v0.8B, v1.8B
        uaddl2 v20.8H, v0.16B, v1.16B
        uaddl v18.8H, v4.8B, v5.8B
        uaddl2 v22.8H, v4.16B, v5.16B
1: subs w3, w3, #2
        ld1 {v0.16B, v1.16B}, [x1], x2
        add v24.8H, v16.8H, v18.8H
        ext v30.16B, v0.16B, v1.16B, #1
        add v1.8H, v20.8H, v22.8H
        rshrn v28.8B, v24.8H, #2
        rshrn2 v28.16B, v1.8H, #2
        uaddl v16.8H, v0.8B, v30.8B
        ld1 {v2.16B, v3.16B}, [x1], x2
        uaddl2 v20.8H, v0.16B, v30.16B
        st1 {v28.16B}, [x0], x2
        add v24.8H, v16.8H, v18.8H
        ext v3.16B, v2.16B, v3.16B, #1
        add v0.8H, v20.8H, v22.8H
        rshrn v30.8B, v24.8H, #2
        rshrn2 v30.16B, v0.8H, #2
        uaddl v18.8H, v2.8B, v3.8B
        uaddl2 v22.8H, v2.16B, v3.16B
        st1 {v30.16B}, [x0], x2
        b.gt 1b

        ld1 {v0.16B, v1.16B}, [x1], x2
        add v24.8H, v16.8H, v18.8H
        ext v30.16B, v0.16B, v1.16B, #1
        add v1.8H, v20.8H, v22.8H
        rshrn v28.8B, v24.8H, #2
        rshrn2 v28.16B, v1.8H, #2
        uaddl v16.8H, v0.8B, v30.8B
        uaddl2 v20.8H, v0.16B, v30.16B
        st1 {v28.16B}, [x0], x2
        add v24.8H, v16.8H, v18.8H
        add v0.8H, v20.8H, v22.8H
        rshrn v30.8B, v24.8H, #2
        rshrn2 v30.16B, v0.8H, #2
        st1 {v30.16B}, [x0], x2

        ret
 # //.size ff_put_pixels16_xy2_neon, . - ff_put_pixels16_xy2_neon
 # //.endfunc
        .text
        .align 2
        .globl _ff_put_pixels16_xy2_no_rnd_neon
 # //.type _ff_put_pixels16_xy2_no_rnd_neon, %function
 # //.func _ff_put_pixels16_xy2_no_rnd_neon
_ff_put_pixels16_xy2_no_rnd_neon:
        sub w3, w3, #2
        ld1 {v0.16B, v1.16B}, [x1], x2
        ld1 {v4.16B, v5.16B}, [x1], x2
        movi v26.8H, #1, lsl #0
        ext v1.16B, v0.16B, v1.16B, #1
        ext v5.16B, v4.16B, v5.16B, #1
        uaddl v16.8H, v0.8B, v1.8B
        uaddl2 v20.8H, v0.16B, v1.16B
        uaddl v18.8H, v4.8B, v5.8B
        uaddl2 v22.8H, v4.16B, v5.16B
1: subs w3, w3, #2
        ld1 {v0.16B, v1.16B}, [x1], x2
        add v24.8H, v16.8H, v18.8H
        add  v24.8H, v24.8H, v26.8H
        ext v30.16B, v0.16B, v1.16B, #1
        add v1.8H, v20.8H, v22.8H
        shrn v28.8B, v24.8H, #2
        add  v1.8H, v1.8H, v26.8H
        shrn2 v28.16B, v1.8H, #2
        uaddl v16.8H, v0.8B, v30.8B
        ld1 {v2.16B, v3.16B}, [x1], x2
        uaddl2 v20.8H, v0.16B, v30.16B
        st1 {v28.16B}, [x0], x2
        add v24.8H, v16.8H, v18.8H
        add  v24.8H, v24.8H, v26.8H
        ext v3.16B, v2.16B, v3.16B, #1
        add v0.8H, v20.8H, v22.8H
        shrn v30.8B, v24.8H, #2
        add  v0.8H, v0.8H, v26.8H
        shrn2 v30.16B, v0.8H, #2
        uaddl v18.8H, v2.8B, v3.8B
        uaddl2 v22.8H, v2.16B, v3.16B
        st1 {v30.16B}, [x0], x2
        b.gt 1b

        ld1 {v0.16B, v1.16B}, [x1], x2
        add v24.8H, v16.8H, v18.8H
        add  v24.8H, v24.8H, v26.8H
        ext v30.16B, v0.16B, v1.16B, #1
        add v1.8H, v20.8H, v22.8H
        shrn v28.8B, v24.8H, #2
        add  v1.8H, v1.8H, v26.8H
        shrn2 v28.16B, v1.8H, #2
        uaddl v16.8H, v0.8B, v30.8B
        uaddl2 v20.8H, v0.16B, v30.16B
        st1 {v28.16B}, [x0], x2
        add v24.8H, v16.8H, v18.8H
        add  v24.8H, v24.8H, v26.8H
        add v0.8H, v20.8H, v22.8H
        shrn v30.8B, v24.8H, #2
        add  v0.8H, v0.8H, v26.8H
        shrn2 v30.16B, v0.8H, #2
        st1 {v30.16B}, [x0], x2

        ret
 # //.size ff_put_pixels16_xy2_no_rnd_neon, . - ff_put_pixels16_xy2_no_rnd_neon
 # //.endfunc

        .text
        .align 2
        .globl _ff_avg_h264_qpel16_mc00_neon
 # //.type _ff_avg_h264_qpel16_mc00_neon, %function
 # //.func _ff_avg_h264_qpel16_mc00_neon
_ff_avg_h264_qpel16_mc00_neon:
        mov w3, #16
 # //.size ff_avg_h264_qpel16_mc00_neon, . - ff_avg_h264_qpel16_mc00_neon
 # //.endfunc

        .text
        .align 2
        .globl _ff_avg_pixels16_neon
 # //.type _ff_avg_pixels16_neon, %function
 # //.func _ff_avg_pixels16_neon
_ff_avg_pixels16_neon:
        mov x12, x0
1: ld1 {v0.16B}, [x1], x2
        ld1 {v1.16B}, [x1], x2
        ld1 {v2.16B}, [x1], x2
        ld1 {v3.16B}, [x1], x2
        ld1 {v4.16B}, [x12], x2
        urhadd v0.16B, v0.16B, v4.16B
        ld1 {v5.16B}, [x12], x2
        urhadd v1.16B, v1.16B, v5.16B
        ld1 {v6.16B}, [x12], x2
        urhadd v2.16B, v2.16B, v6.16B
        ld1 {v7.16B}, [x12], x2
        urhadd v3.16B, v3.16B, v7.16B
        subs w3, w3, #4
        st1 {v0.16B}, [x0], x2
        st1 {v1.16B}, [x0], x2
        st1 {v2.16B}, [x0], x2
        st1 {v3.16B}, [x0], x2
        b.ne 1b
        ret
 # //.size ff_avg_pixels16_neon, . - ff_avg_pixels16_neon
 # //.endfunc
        .text
        .align 2
        .globl _ff_avg_pixels16_x2_neon
 # //.type _ff_avg_pixels16_x2_neon, %function
 # //.func _ff_avg_pixels16_x2_neon
_ff_avg_pixels16_x2_neon:
1: ld1 {v0.16B, v1.16B}, [x1], x2
        ld1 {v2.16B, v3.16B}, [x1], x2
        subs w3, w3, #2
        ext v1.16B, v0.16B, v1.16B, #1
        urhadd v0.16B, v0.16B, v1.16B
        ext v3.16B, v2.16B, v3.16B, #1
        urhadd v2.16B, v2.16B, v3.16B
        ld1 {v1.16B}, [x0], x2
        ld1 {v3.16B}, [x0]
        urhadd v0.16B, v0.16B, v1.16B
        urhadd v2.16B, v2.16B, v3.16B
        sub x0, x0, x2
        st1 {v0.16B}, [x0], x2
        st1 {v2.16B}, [x0], x2
        b.ne 1b
        ret
 # //.size ff_avg_pixels16_x2_neon, . - ff_avg_pixels16_x2_neon
 # //.endfunc
        .text
        .align 2
        .globl _ff_avg_pixels16_x2_no_rnd_neon
 # //.type _ff_avg_pixels16_x2_no_rnd_neon, %function
 # //.func _ff_avg_pixels16_x2_no_rnd_neon
_ff_avg_pixels16_x2_no_rnd_neon:
1: ld1 {v0.16B, v1.16B}, [x1], x2
        ld1 {v2.16B, v3.16B}, [x1], x2
        subs w3, w3, #2
        ext v1.16B, v0.16B, v1.16B, #1
        uhadd v0.16B, v0.16B, v1.16B
        ext v3.16B, v2.16B, v3.16B, #1
        uhadd v2.16B, v2.16B, v3.16B
        ld1 {v1.16B}, [x0], x2
        ld1 {v3.16B}, [x0]
        urhadd v0.16B, v0.16B, v1.16B
        urhadd v2.16B, v2.16B, v3.16B
        sub x0, x0, x2
        st1 {v0.16B}, [x0], x2
        st1 {v2.16B}, [x0], x2
        b.ne 1b
        ret
 # //.size ff_avg_pixels16_x2_no_rnd_neon, . - ff_avg_pixels16_x2_no_rnd_neon
 # //.endfunc
        .text
        .align 2
        .globl _ff_avg_pixels16_y2_neon
 # //.type _ff_avg_pixels16_y2_neon, %function
 # //.func _ff_avg_pixels16_y2_neon
_ff_avg_pixels16_y2_neon:
        sub w3, w3, #2
        ld1 {v0.16B}, [x1], x2
        ld1 {v1.16B}, [x1], x2
1: subs w3, w3, #2
        urhadd v2.16B, v0.16B, v1.16B
        ld1 {v0.16B}, [x1], x2
        urhadd v3.16B, v0.16B, v1.16B
        ld1 {v1.16B}, [x1], x2
        ld1 {v4.16B}, [x0], x2
        ld1 {v5.16B}, [x0]
        urhadd v2.16B, v2.16B, v4.16B
        urhadd v3.16B, v3.16B, v5.16B
        sub x0, x0, x2
        st1 {v2.16B}, [x0], x2
        st1 {v3.16B}, [x0], x2
        b.ne 1b

        urhadd v2.16B, v0.16B, v1.16B
        ld1 {v0.16B}, [x1], x2
        urhadd v3.16B, v0.16B, v1.16B
        ld1 {v4.16B}, [x0], x2
        ld1 {v5.16B}, [x0]
        urhadd v2.16B, v2.16B, v4.16B
        urhadd v3.16B, v3.16B, v5.16B
        sub x0, x0, x2
        st1 {v2.16B}, [x0], x2
        st1 {v3.16B}, [x0], x2

        ret
 # //.size ff_avg_pixels16_y2_neon, . - ff_avg_pixels16_y2_neon
 # //.endfunc
        .text
        .align 2
        .globl _ff_avg_pixels16_y2_no_rnd_neon
 # //.type _ff_avg_pixels16_y2_no_rnd_neon, %function
 # //.func _ff_avg_pixels16_y2_no_rnd_neon
_ff_avg_pixels16_y2_no_rnd_neon:
        sub w3, w3, #2
        ld1 {v0.16B}, [x1], x2
        ld1 {v1.16B}, [x1], x2
1: subs w3, w3, #2
        uhadd v2.16B, v0.16B, v1.16B
        ld1 {v0.16B}, [x1], x2
        uhadd v3.16B, v0.16B, v1.16B
        ld1 {v1.16B}, [x1], x2
        ld1 {v4.16B}, [x0], x2
        ld1 {v5.16B}, [x0]
        urhadd v2.16B, v2.16B, v4.16B
        urhadd v3.16B, v3.16B, v5.16B
        sub x0, x0, x2
        st1 {v2.16B}, [x0], x2
        st1 {v3.16B}, [x0], x2
        b.ne 1b

        uhadd v2.16B, v0.16B, v1.16B
        ld1 {v0.16B}, [x1], x2
        uhadd v3.16B, v0.16B, v1.16B
        ld1 {v4.16B}, [x0], x2
        ld1 {v5.16B}, [x0]
        urhadd v2.16B, v2.16B, v4.16B
        urhadd v3.16B, v3.16B, v5.16B
        sub x0, x0, x2
        st1 {v2.16B}, [x0], x2
        st1 {v3.16B}, [x0], x2

        ret
 # //.size ff_avg_pixels16_y2_no_rnd_neon, . - ff_avg_pixels16_y2_no_rnd_neon
 # //.endfunc
        .text
        .align 2
        .globl _ff_avg_pixels16_xy2_neon
 # //.type _ff_avg_pixels16_xy2_neon, %function
 # //.func _ff_avg_pixels16_xy2_neon
_ff_avg_pixels16_xy2_neon:
        sub w3, w3, #2
        ld1 {v0.16B, v1.16B}, [x1], x2
        ld1 {v4.16B, v5.16B}, [x1], x2
        ext v1.16B, v0.16B, v1.16B, #1
        ext v5.16B, v4.16B, v5.16B, #1
        uaddl v16.8H, v0.8B, v1.8B
        uaddl2 v20.8H, v0.16B, v1.16B
        uaddl v18.8H, v4.8B, v5.8B
        uaddl2 v22.8H, v4.16B, v5.16B
1: subs w3, w3, #2
        ld1 {v0.16B, v1.16B}, [x1], x2
        add v24.8H, v16.8H, v18.8H
        ext v30.16B, v0.16B, v1.16B, #1
        add v1.8H, v20.8H, v22.8H
        rshrn v28.8B, v24.8H, #2
        rshrn2 v28.16B, v1.8H, #2
        ld1 {v16.16B}, [x0]
        urhadd v28.16B, v28.16B, v16.16B
        uaddl v16.8H, v0.8B, v30.8B
        ld1 {v2.16B, v3.16B}, [x1], x2
        uaddl2 v20.8H, v0.16B, v30.16B
        st1 {v28.16B}, [x0], x2
        add v24.8H, v16.8H, v18.8H
        ext v3.16B, v2.16B, v3.16B, #1
        add v0.8H, v20.8H, v22.8H
        rshrn v30.8B, v24.8H, #2
        rshrn2 v30.16B, v0.8H, #2
        ld1 {v18.16B}, [x0]
        urhadd v30.16B, v30.16B, v18.16B
        uaddl v18.8H, v2.8B, v3.8B
        uaddl2 v22.8H, v2.16B, v3.16B
        st1 {v30.16B}, [x0], x2
        b.gt 1b

        ld1 {v0.16B, v1.16B}, [x1], x2
        add v24.8H, v16.8H, v18.8H
        ext v30.16B, v0.16B, v1.16B, #1
        add v1.8H, v20.8H, v22.8H
        rshrn v28.8B, v24.8H, #2
        rshrn2 v28.16B, v1.8H, #2
        ld1 {v16.16B}, [x0]
        urhadd v28.16B, v28.16B, v16.16B
        uaddl v16.8H, v0.8B, v30.8B
        uaddl2 v20.8H, v0.16B, v30.16B
        st1 {v28.16B}, [x0], x2
        add v24.8H, v16.8H, v18.8H
        add v0.8H, v20.8H, v22.8H
        rshrn v30.8B, v24.8H, #2
        rshrn2 v30.16B, v0.8H, #2
        ld1 {v18.16B}, [x0]
        urhadd v30.16B, v30.16B, v18.16B
        st1 {v30.16B}, [x0], x2

        ret
 # //.size ff_avg_pixels16_xy2_neon, . - ff_avg_pixels16_xy2_neon
 # //.endfunc
        .text
        .align 2
        .globl _ff_avg_pixels16_xy2_no_rnd_neon
 # //.type _ff_avg_pixels16_xy2_no_rnd_neon, %function
 # //.func _ff_avg_pixels16_xy2_no_rnd_neon
_ff_avg_pixels16_xy2_no_rnd_neon:
        sub w3, w3, #2
        ld1 {v0.16B, v1.16B}, [x1], x2
        ld1 {v4.16B, v5.16B}, [x1], x2
        movi v26.8H, #1, lsl #0
        ext v1.16B, v0.16B, v1.16B, #1
        ext v5.16B, v4.16B, v5.16B, #1
        uaddl v16.8H, v0.8B, v1.8B
        uaddl2 v20.8H, v0.16B, v1.16B
        uaddl v18.8H, v4.8B, v5.8B
        uaddl2 v22.8H, v4.16B, v5.16B
1: subs w3, w3, #2
        ld1 {v0.16B, v1.16B}, [x1], x2
        add v24.8H, v16.8H, v18.8H
        add  v24.8H, v24.8H, v26.8H
        ext v30.16B, v0.16B, v1.16B, #1
        add v1.8H, v20.8H, v22.8H
        shrn v28.8B, v24.8H, #2
        add  v1.8H, v1.8H, v26.8H
        shrn2 v28.16B, v1.8H, #2
        ld1 {v16.16B}, [x0]
        urhadd v28.16B, v28.16B, v16.16B
        uaddl v16.8H, v0.8B, v30.8B
        ld1 {v2.16B, v3.16B}, [x1], x2
        uaddl2 v20.8H, v0.16B, v30.16B
        st1 {v28.16B}, [x0], x2
        add v24.8H, v16.8H, v18.8H
        add  v24.8H, v24.8H, v26.8H
        ext v3.16B, v2.16B, v3.16B, #1
        add v0.8H, v20.8H, v22.8H
        shrn v30.8B, v24.8H, #2
        add  v0.8H, v0.8H, v26.8H
        shrn2 v30.16B, v0.8H, #2
        ld1 {v18.16B}, [x0]
        urhadd v30.16B, v30.16B, v18.16B
        uaddl v18.8H, v2.8B, v3.8B
        uaddl2 v22.8H, v2.16B, v3.16B
        st1 {v30.16B}, [x0], x2
        b.gt 1b

        ld1 {v0.16B, v1.16B}, [x1], x2
        add v24.8H, v16.8H, v18.8H
        add  v24.8H, v24.8H, v26.8H
        ext v30.16B, v0.16B, v1.16B, #1
        add v1.8H, v20.8H, v22.8H
        shrn v28.8B, v24.8H, #2
        add  v1.8H, v1.8H, v26.8H
        shrn2 v28.16B, v1.8H, #2
        ld1 {v16.16B}, [x0]
        urhadd v28.16B, v28.16B, v16.16B
        uaddl v16.8H, v0.8B, v30.8B
        uaddl2 v20.8H, v0.16B, v30.16B
        st1 {v28.16B}, [x0], x2
        add v24.8H, v16.8H, v18.8H
        add  v24.8H, v24.8H, v26.8H
        add v0.8H, v20.8H, v22.8H
        shrn v30.8B, v24.8H, #2
        add  v0.8H, v0.8H, v26.8H
        shrn2 v30.16B, v0.8H, #2
        ld1 {v18.16B}, [x0]
        urhadd v30.16B, v30.16B, v18.16B
        st1 {v30.16B}, [x0], x2

        ret
 # //.size ff_avg_pixels16_xy2_no_rnd_neon, . - ff_avg_pixels16_xy2_no_rnd_neon
 # //.endfunc

        .text
        .align 2
        .globl _ff_put_h264_qpel8_mc00_neon
 # //.type _ff_put_h264_qpel8_mc00_neon, %function
 # //.func _ff_put_h264_qpel8_mc00_neon
_ff_put_h264_qpel8_mc00_neon:
        mov w3, #8
 # //.size ff_put_h264_qpel8_mc00_neon, . - ff_put_h264_qpel8_mc00_neon
 # //.endfunc

        .text
        .align 2
        .globl _ff_put_pixels8_neon
 # //.type _ff_put_pixels8_neon, %function
 # //.func _ff_put_pixels8_neon
_ff_put_pixels8_neon:
1: ld1 {v0.8B}, [x1], x2
        ld1 {v1.8B}, [x1], x2
        ld1 {v2.8B}, [x1], x2
        ld1 {v3.8B}, [x1], x2
        subs w3, w3, #4
        st1 {v0.8B}, [x0], x2
        st1 {v1.8B}, [x0], x2
        st1 {v2.8B}, [x0], x2
        st1 {v3.8B}, [x0], x2
        b.ne 1b
        ret
 # //.size ff_put_pixels8_neon, . - ff_put_pixels8_neon
 # //.endfunc
        .text
        .align 2
        .globl _ff_put_pixels8_x2_neon
 # //.type _ff_put_pixels8_x2_neon, %function
 # //.func _ff_put_pixels8_x2_neon
_ff_put_pixels8_x2_neon:
1: ld1 {v0.8B, v1.8B}, [x1], x2
        ext v1.8B, v0.8B, v1.8B, #1
        ld1 {v2.8B, v3.8B}, [x1], x2
        ext v3.8B, v2.8B, v3.8B, #1
        subs w3, w3, #2
        urhadd v0.8B, v0.8B, v1.8B
        urhadd v2.8B, v2.8B, v3.8B
        st1 {v0.8B}, [x0], x2
        st1 {v2.8B}, [x0], x2
        b.ne 1b
        ret
 # //.size ff_put_pixels8_x2_neon, . - ff_put_pixels8_x2_neon
 # //.endfunc
        .text
        .align 2
        .globl _ff_put_pixels8_x2_no_rnd_neon
 # //.type _ff_put_pixels8_x2_no_rnd_neon, %function
 # //.func _ff_put_pixels8_x2_no_rnd_neon
_ff_put_pixels8_x2_no_rnd_neon:
1: ld1 {v0.8B, v1.8B}, [x1], x2
        ext v1.8B, v0.8B, v1.8B, #1
        ld1 {v2.8B, v3.8B}, [x1], x2
        ext v3.8B, v2.8B, v3.8B, #1
        subs w3, w3, #2
        uhadd v0.8B, v0.8B, v1.8B
        uhadd v2.8B, v2.8B, v3.8B
        st1 {v0.8B}, [x0], x2
        st1 {v2.8B}, [x0], x2
        b.ne 1b
        ret
 # //.size ff_put_pixels8_x2_no_rnd_neon, . - ff_put_pixels8_x2_no_rnd_neon
 # //.endfunc
        .text
        .align 2
        .globl _ff_put_pixels8_y2_neon
 # //.type _ff_put_pixels8_y2_neon, %function
 # //.func _ff_put_pixels8_y2_neon
_ff_put_pixels8_y2_neon:
        sub w3, w3, #2
        ld1 {v0.8B}, [x1], x2
        ld1 {v1.8B}, [x1], x2
1: subs w3, w3, #2
        urhadd v4.8B, v0.8B, v1.8B
        ld1 {v0.8B}, [x1], x2
        urhadd v5.8B, v0.8B, v1.8B
        ld1 {v1.8B}, [x1], x2
        st1 {v4.8B}, [x0], x2
        st1 {v5.8B}, [x0], x2
        b.ne 1b

        urhadd v4.8B, v0.8B, v1.8B
        ld1 {v0.8B}, [x1], x2
        urhadd v5.8B, v0.8B, v1.8B
        st1 {v4.8B}, [x0], x2
        st1 {v5.8B}, [x0], x2

        ret
 # //.size ff_put_pixels8_y2_neon, . - ff_put_pixels8_y2_neon
 # //.endfunc
        .text
        .align 2
        .globl _ff_put_pixels8_y2_no_rnd_neon
 # //.type _ff_put_pixels8_y2_no_rnd_neon, %function
 # //.func _ff_put_pixels8_y2_no_rnd_neon
_ff_put_pixels8_y2_no_rnd_neon:
        sub w3, w3, #2
        ld1 {v0.8B}, [x1], x2
        ld1 {v1.8B}, [x1], x2
1: subs w3, w3, #2
        uhadd v4.8B, v0.8B, v1.8B
        ld1 {v0.8B}, [x1], x2
        uhadd v5.8B, v0.8B, v1.8B
        ld1 {v1.8B}, [x1], x2
        st1 {v4.8B}, [x0], x2
        st1 {v5.8B}, [x0], x2
        b.ne 1b

        uhadd v4.8B, v0.8B, v1.8B
        ld1 {v0.8B}, [x1], x2
        uhadd v5.8B, v0.8B, v1.8B
        st1 {v4.8B}, [x0], x2
        st1 {v5.8B}, [x0], x2

        ret
 # //.size ff_put_pixels8_y2_no_rnd_neon, . - ff_put_pixels8_y2_no_rnd_neon
 # //.endfunc
        .text
        .align 2
        .globl _ff_put_pixels8_xy2_neon
 # //.type _ff_put_pixels8_xy2_neon, %function
 # //.func _ff_put_pixels8_xy2_neon
_ff_put_pixels8_xy2_neon:
        sub w3, w3, #2
        ld1 {v0.16B}, [x1], x2
        ld1 {v1.16B}, [x1], x2
        ext v4.16B, v0.16B, v4.16B, #1
        ext v6.16B, v1.16B, v6.16B, #1
        uaddl v16.8H, v0.8B, v4.8B
        uaddl v17.8H, v1.8B, v6.8B
1: subs w3, w3, #2
        ld1 {v0.16B}, [x1], x2
        add v18.8H, v16.8H, v17.8H
        ext v4.16B, v0.16B, v4.16B, #1
        uaddl v16.8H, v0.8B, v4.8B
        rshrn v5.8B, v18.8H, #2
        ld1 {v1.16B}, [x1], x2
        add v18.8H, v16.8H, v17.8H
        st1 {v5.8B}, [x0], x2
        rshrn v7.8B, v18.8H, #2
        ext v6.16B, v1.16B, v6.16B, #1
        uaddl v17.8H, v1.8B, v6.8B
        st1 {v7.8B}, [x0], x2
        b.gt 1b

        ld1 {v0.16B}, [x1], x2
        add v18.8H, v16.8H, v17.8H
        ext v4.16B, v0.16B, v4.16B, #1
        uaddl v16.8H, v0.8B, v4.8B
        rshrn v5.8B, v18.8H, #2
        add v18.8H, v16.8H, v17.8H
        st1 {v5.8B}, [x0], x2
        rshrn v7.8B, v18.8H, #2
        st1 {v7.8B}, [x0], x2

        ret
 # //.size ff_put_pixels8_xy2_neon, . - ff_put_pixels8_xy2_neon
 # //.endfunc
        .text
        .align 2
        .globl _ff_put_pixels8_xy2_no_rnd_neon
 # //.type _ff_put_pixels8_xy2_no_rnd_neon, %function
 # //.func _ff_put_pixels8_xy2_no_rnd_neon
_ff_put_pixels8_xy2_no_rnd_neon:
        sub w3, w3, #2
        ld1 {v0.16B}, [x1], x2
        ld1 {v1.16B}, [x1], x2
        movi v19.8H, #1, lsl #0
        ext v4.16B, v0.16B, v4.16B, #1
        ext v6.16B, v1.16B, v6.16B, #1
        uaddl v16.8H, v0.8B, v4.8B
        uaddl v17.8H, v1.8B, v6.8B
1: subs w3, w3, #2
        ld1 {v0.16B}, [x1], x2
        add v18.8H, v16.8H, v17.8H
        ext v4.16B, v0.16B, v4.16B, #1
        add  v18.8H, v18.8H, v19.8H
        uaddl v16.8H, v0.8B, v4.8B
        shrn v5.8B, v18.8H, #2
        ld1 {v1.16B}, [x1], x2
        add v18.8H, v16.8H, v17.8H
        add  v18.8H, v18.8H, v19.8H
        st1 {v5.8B}, [x0], x2
        shrn v7.8B, v18.8H, #2
        ext v6.16B, v1.16B, v6.16B, #1
        uaddl v17.8H, v1.8B, v6.8B
        st1 {v7.8B}, [x0], x2
        b.gt 1b

        ld1 {v0.16B}, [x1], x2
        add v18.8H, v16.8H, v17.8H
        ext v4.16B, v0.16B, v4.16B, #1
        add  v18.8H, v18.8H, v19.8H
        uaddl v16.8H, v0.8B, v4.8B
        shrn v5.8B, v18.8H, #2
        add v18.8H, v16.8H, v17.8H
        add  v18.8H, v18.8H, v19.8H
        st1 {v5.8B}, [x0], x2
        shrn v7.8B, v18.8H, #2
        st1 {v7.8B}, [x0], x2

        ret
 # //.size ff_put_pixels8_xy2_no_rnd_neon, . - ff_put_pixels8_xy2_no_rnd_neon
 # //.endfunc

        .text
        .align 2
        .globl _ff_avg_h264_qpel8_mc00_neon
 # //.type _ff_avg_h264_qpel8_mc00_neon, %function
 # //.func _ff_avg_h264_qpel8_mc00_neon
_ff_avg_h264_qpel8_mc00_neon:
        mov w3, #8
 # //.size ff_avg_h264_qpel8_mc00_neon, . - ff_avg_h264_qpel8_mc00_neon
 # //.endfunc

        .text
        .align 2
        .globl _ff_avg_pixels8_neon
 # //.type _ff_avg_pixels8_neon, %function
 # //.func _ff_avg_pixels8_neon
_ff_avg_pixels8_neon:
1: ld1 {v0.8B}, [x1], x2
        ld1 {v1.8B}, [x1], x2
        ld1 {v2.8B}, [x1], x2
        ld1 {v3.8B}, [x1], x2
        ld1 {v4.8B}, [x0], x2
        urhadd v0.8B, v0.8B, v4.8B
        ld1 {v5.8B}, [x0], x2
        urhadd v1.8B, v1.8B, v5.8B
        ld1 {v6.8B}, [x0], x2
        urhadd v2.8B, v2.8B, v6.8B
        ld1 {v7.8B}, [x0], x2
        urhadd v3.8B, v3.8B, v7.8B
        sub x0, x0, x2, lsl #2
        subs w3, w3, #4
        st1 {v0.8B}, [x0], x2
        st1 {v1.8B}, [x0], x2
        st1 {v2.8B}, [x0], x2
        st1 {v3.8B}, [x0], x2
        b.ne 1b
        ret
 # //.size ff_avg_pixels8_neon, . - ff_avg_pixels8_neon
 # //.endfunc
        .text
        .align 2
        .globl _ff_avg_pixels8_x2_neon
 # //.type _ff_avg_pixels8_x2_neon, %function
 # //.func _ff_avg_pixels8_x2_neon
_ff_avg_pixels8_x2_neon:
1: ld1 {v0.8B, v1.8B}, [x1], x2
        ext v1.8B, v0.8B, v1.8B, #1
        ld1 {v2.8B, v3.8B}, [x1], x2
        ext v3.8B, v2.8B, v3.8B, #1
        subs w3, w3, #2
        urhadd v0.8B, v0.8B, v1.8B
        urhadd v2.8B, v2.8B, v3.8B
        ld1 {v4.8B}, [x0], x2
        ld1 {v5.8B}, [x0]
        urhadd v0.8B, v0.8B, v4.8B
        urhadd v2.8B, v2.8B, v5.8B
        sub x0, x0, x2
        st1 {v0.8B}, [x0], x2
        st1 {v2.8B}, [x0], x2
        b.ne 1b
        ret
 # //.size ff_avg_pixels8_x2_neon, . - ff_avg_pixels8_x2_neon
 # //.endfunc
        .text
        .align 2
        .globl _ff_avg_pixels8_y2_neon
 # //.type _ff_avg_pixels8_y2_neon, %function
 # //.func _ff_avg_pixels8_y2_neon
_ff_avg_pixels8_y2_neon:
        sub w3, w3, #2
        ld1 {v0.8B}, [x1], x2
        ld1 {v1.8B}, [x1], x2
1: subs w3, w3, #2
        urhadd v4.8B, v0.8B, v1.8B
        ld1 {v0.8B}, [x1], x2
        urhadd v5.8B, v0.8B, v1.8B
        ld1 {v1.8B}, [x1], x2
        ld1 {v2.8B}, [x0], x2
        ld1 {v3.8B}, [x0]
        urhadd v4.8B, v4.8B, v2.8B
        urhadd v5.8B, v5.8B, v3.8B
        sub x0, x0, x2
        st1 {v4.8B}, [x0], x2
        st1 {v5.8B}, [x0], x2
        b.ne 1b

        urhadd v4.8B, v0.8B, v1.8B
        ld1 {v0.8B}, [x1], x2
        urhadd v5.8B, v0.8B, v1.8B
        ld1 {v2.8B}, [x0], x2
        ld1 {v3.8B}, [x0]
        urhadd v4.8B, v4.8B, v2.8B
        urhadd v5.8B, v5.8B, v3.8B
        sub x0, x0, x2
        st1 {v4.8B}, [x0], x2
        st1 {v5.8B}, [x0], x2

        ret
 # //.size ff_avg_pixels8_y2_neon, . - ff_avg_pixels8_y2_neon
 # //.endfunc
        .text
        .align 2
        .globl _ff_avg_pixels8_xy2_neon
 # //.type _ff_avg_pixels8_xy2_neon, %function
 # //.func _ff_avg_pixels8_xy2_neon
_ff_avg_pixels8_xy2_neon:
        sub w3, w3, #2
        ld1 {v0.16B}, [x1], x2
        ld1 {v1.16B}, [x1], x2
        ext v4.16B, v0.16B, v4.16B, #1
        ext v6.16B, v1.16B, v6.16B, #1
        uaddl v16.8H, v0.8B, v4.8B
        uaddl v17.8H, v1.8B, v6.8B
1: subs w3, w3, #2
        ld1 {v0.16B}, [x1], x2
        add v18.8H, v16.8H, v17.8H
        ext v4.16B, v0.16B, v4.16B, #1
        uaddl v16.8H, v0.8B, v4.8B
        rshrn v5.8B, v18.8H, #2
        ld1 {v1.16B}, [x1], x2
        add v18.8H, v16.8H, v17.8H
        ld1 {v7.8B}, [x0]
        urhadd v5.8B, v5.8B, v7.8B
        st1 {v5.8B}, [x0], x2
        rshrn v7.8B, v18.8H, #2
        ld1 {v5.8B}, [x0]
        urhadd v7.8B, v7.8B, v5.8B
        ext v6.16B, v1.16B, v6.16B, #1
        uaddl v17.8H, v1.8B, v6.8B
        st1 {v7.8B}, [x0], x2
        b.gt 1b

        ld1 {v0.16B}, [x1], x2
        add v18.8H, v16.8H, v17.8H
        ext v4.16B, v0.16B, v4.16B, #1
        uaddl v16.8H, v0.8B, v4.8B
        rshrn v5.8B, v18.8H, #2
        add v18.8H, v16.8H, v17.8H
        ld1 {v7.8B}, [x0]
        urhadd v5.8B, v5.8B, v7.8B
        st1 {v5.8B}, [x0], x2
        rshrn v7.8B, v18.8H, #2
        ld1 {v5.8B}, [x0]
        urhadd v7.8B, v7.8B, v5.8B
        st1 {v7.8B}, [x0], x2

        ret
 # //.size ff_avg_pixels8_xy2_neon, . - ff_avg_pixels8_xy2_neon
 # //.endfunc
.text
.align 2
#endif
