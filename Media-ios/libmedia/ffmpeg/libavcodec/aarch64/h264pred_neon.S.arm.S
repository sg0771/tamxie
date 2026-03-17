#ifdef __arm64__
        .text
        .align 2
        .globl _ff_pred16x16_128_dc_neon
 # //.type _ff_pred16x16_128_dc_neon, %function
 # //.func _ff_pred16x16_128_dc_neon
_ff_pred16x16_128_dc_neon:
        movi v0.16b, #128
        b L_pred16x16_dc_end
 # //.size ff_pred16x16_128_dc_neon, . - ff_pred16x16_128_dc_neon
 # //.endfunc

        .text
        .align 2
        .globl _ff_pred16x16_top_dc_neon
 # //.type _ff_pred16x16_top_dc_neon, %function
 # //.func _ff_pred16x16_top_dc_neon
_ff_pred16x16_top_dc_neon:
        sub x2, x0, x1
        ld1 {v0.16b}, [x2]
        uaddlv h0, v0.16b
        rshrn v0.8b, v0.8h, #4
        dup v0.16b, v0.b[0]
        b L_pred16x16_dc_end
 # //.size ff_pred16x16_top_dc_neon, . - ff_pred16x16_top_dc_neon
 # //.endfunc

        .text
        .align 2
        .globl _ff_pred16x16_left_dc_neon
 # //.type _ff_pred16x16_left_dc_neon, %function
 # //.func _ff_pred16x16_left_dc_neon
_ff_pred16x16_left_dc_neon:
        sub x2, x0, #1
        ld1 {v0.b}[0], [x2], x1
        ld1 {v0.b}[1], [x2], x1
        ld1 {v0.b}[2], [x2], x1
        ld1 {v0.b}[3], [x2], x1
        ld1 {v0.b}[4], [x2], x1
        ld1 {v0.b}[5], [x2], x1
        ld1 {v0.b}[6], [x2], x1
        ld1 {v0.b}[7], [x2], x1
        ld1 {v0.b}[8], [x2], x1
        ld1 {v0.b}[9], [x2], x1
        ld1 {v0.b}[10], [x2], x1
        ld1 {v0.b}[11], [x2], x1
        ld1 {v0.b}[12], [x2], x1
        ld1 {v0.b}[13], [x2], x1
        ld1 {v0.b}[14], [x2], x1
        ld1 {v0.b}[15], [x2], x1
        uaddlv h0, v0.16b
        rshrn v0.8b, v0.8h, #4
        dup v0.16b, v0.b[0]
        b L_pred16x16_dc_end
 # //.size ff_pred16x16_left_dc_neon, . - ff_pred16x16_left_dc_neon
 # //.endfunc

        .text
        .align 2
        .globl _ff_pred16x16_dc_neon
 # //.type _ff_pred16x16_dc_neon, %function
 # //.func _ff_pred16x16_dc_neon
_ff_pred16x16_dc_neon:
        sub x2, x0, x1
        sub x3, x0, #1
        ld1 {v0.16b}, [x2]
        ld1 {v1.b}[0], [x3], x1
        ld1 {v1.b}[1], [x3], x1
        ld1 {v1.b}[2], [x3], x1
        ld1 {v1.b}[3], [x3], x1
        ld1 {v1.b}[4], [x3], x1
        ld1 {v1.b}[5], [x3], x1
        ld1 {v1.b}[6], [x3], x1
        ld1 {v1.b}[7], [x3], x1
        ld1 {v1.b}[8], [x3], x1
        ld1 {v1.b}[9], [x3], x1
        ld1 {v1.b}[10], [x3], x1
        ld1 {v1.b}[11], [x3], x1
        ld1 {v1.b}[12], [x3], x1
        ld1 {v1.b}[13], [x3], x1
        ld1 {v1.b}[14], [x3], x1
        ld1 {v1.b}[15], [x3], x1
        uaddlv h0, v0.16b
        uaddlv h1, v1.16b
        add v0.4h, v0.4h, v1.4h
        rshrn v0.8b, v0.8h, #5
        dup v0.16b, v0.b[0]
L_pred16x16_dc_end:
        mov w3, #8
6: st1 {v0.16b}, [x0], x1
        st1 {v0.16b}, [x0], x1
        subs w3, w3, #1
        b.ne 6b
        ret
 # //.size ff_pred16x16_dc_neon, . - ff_pred16x16_dc_neon
 # //.endfunc

        .text
        .align 2
        .globl _ff_pred16x16_hor_neon
 # //.type _ff_pred16x16_hor_neon, %function
 # //.func _ff_pred16x16_hor_neon
_ff_pred16x16_hor_neon:
        sub x2, x0, #1
        mov w3, #16
1: ld1r {v0.16b}, [x2], x1
        st1 {v0.16b}, [x0], x1
        subs w3, w3, #1
        b.ne 1b
        ret
 # //.size ff_pred16x16_hor_neon, . - ff_pred16x16_hor_neon
 # //.endfunc

        .text
        .align 2
        .globl _ff_pred16x16_vert_neon
 # //.type _ff_pred16x16_vert_neon, %function
 # //.func _ff_pred16x16_vert_neon
_ff_pred16x16_vert_neon:
        sub x2, x0, x1
        add x1, x1, x1
        ld1 {v0.16b}, [x2], x1
        mov w3, #8
1: st1 {v0.16b}, [x0], x1
        st1 {v0.16b}, [x2], x1
        subs w3, w3, #1
        b.ne 1b
        ret
 # //.size ff_pred16x16_vert_neon, . - ff_pred16x16_vert_neon
 # //.endfunc

        .text
        .align 2
        .globl _ff_pred16x16_plane_neon
 # //.type _ff_pred16x16_plane_neon, %function
 # //.func _ff_pred16x16_plane_neon
_ff_pred16x16_plane_neon:
        sub x3, x0, x1

        adrp x4, p16weight@PAGE
        add x4, x4, p16weight@PAGEOFF


        add x2, x3, #8
        sub x3, x3, #1
        ld1 {v0.8b}, [x3]
        ld1 {v2.8b}, [x2], x1
        ld1 {v1.b}[0], [x3], x1
        ld1 {v1.b}[1], [x3], x1
        ld1 {v1.b}[2], [x3], x1
        ld1 {v1.b}[3], [x3], x1
        ld1 {v1.b}[4], [x3], x1
        ld1 {v1.b}[5], [x3], x1
        ld1 {v1.b}[6], [x3], x1
        ld1 {v1.b}[7], [x3], x1
        add x3, x3, x1
        ld1 {v3.b}[0], [x3], x1
        ld1 {v3.b}[1], [x3], x1
        ld1 {v3.b}[2], [x3], x1
        ld1 {v3.b}[3], [x3], x1
        ld1 {v3.b}[4], [x3], x1
        ld1 {v3.b}[5], [x3], x1
        ld1 {v3.b}[6], [x3], x1
        ld1 {v3.b}[7], [x3], x1
        rev64 v0.8b, v0.8b
        rev64 v1.8b, v1.8b
        uaddl v7.8h, v2.8b, v3.8b
        usubl v2.8h, v2.8b, v0.8b
        usubl v3.8h, v3.8b, v1.8b
        ld1 {v0.8h}, [x4]
        mul v2.8h, v2.8h, v0.8h
        mul v3.8h, v3.8h, v0.8h
        addp v2.8h, v2.8h, v3.8h
        addp v2.8h, v2.8h, v2.8h
        addp v2.4h, v2.4h, v2.4h
        sshll v3.4s, v2.4h, #2
        saddw v2.4s, v3.4s, v2.4h
        rshrn v4.4h, v2.4s, #6
        trn2 v5.4h, v4.4h, v4.4h
        add v2.4h, v4.4h, v5.4h
        shl v3.4h, v2.4h, #3
        ext v7.16b, v7.16b, v7.16b, #14
        sub v3.4h, v3.4h, v2.4h
        add v7.4h, v7.4h, v0.4h
        shl v2.4h, v7.4h, #4
        sub v2.4h, v2.4h, v3.4h
        shl v3.4h, v4.4h, #4
        ext v0.16b, v0.16b, v0.16b, #14
        sub v6.4h, v5.4h, v3.4h
        mov v0.h[0], wzr
        mul v0.8h, v0.8h, v4.h[0]
        dup v1.8h, v2.h[0]
        dup v2.8h, v4.h[0]
        dup v3.8h, v6.h[0]
        shl v2.8h, v2.8h, #3
        add v1.8h, v1.8h, v0.8h
        add v3.8h, v3.8h, v2.8h
        mov w3, #16
1:
        sqshrun v0.8b, v1.8h, #5
        add v1.8h, v1.8h, v2.8h
        sqshrun2 v0.16b, v1.8h, #5
        add v1.8h, v1.8h, v3.8h
        st1 {v0.16b}, [x0], x1
        subs w3, w3, #1
        b.ne 1b
        ret
 # //.size ff_pred16x16_plane_neon, . - ff_pred16x16_plane_neon
 # //.endfunc


        .const_data

        .align 4
p16weight:
        .short 1,2,3,4,5,6,7,8
 # //.size p16weight, . - p16weight

        .const_data

        .align 4
p8weight:
        .short 1,2,3,4,1,2,3,4
 # //.size p8weight, . - p8weight

        .text
        .align 2
        .globl _ff_pred8x8_hor_neon
 # //.type _ff_pred8x8_hor_neon, %function
 # //.func _ff_pred8x8_hor_neon
_ff_pred8x8_hor_neon:
        sub x2, x0, #1
        mov w3, #8
1: ld1r {v0.8b}, [x2], x1
        st1 {v0.8b}, [x0], x1
        subs w3, w3, #1
        b.ne 1b
        ret
 # //.size ff_pred8x8_hor_neon, . - ff_pred8x8_hor_neon
 # //.endfunc

        .text
        .align 2
        .globl _ff_pred8x8_vert_neon
 # //.type _ff_pred8x8_vert_neon, %function
 # //.func _ff_pred8x8_vert_neon
_ff_pred8x8_vert_neon:
        sub x2, x0, x1
        lsl x1, x1, #1
        ld1 {v0.8b}, [x2], x1
        mov w3, #4
1: st1 {v0.8b}, [x0], x1
        st1 {v0.8b}, [x2], x1
        subs w3, w3, #1
        b.ne 1b
        ret
 # //.size ff_pred8x8_vert_neon, . - ff_pred8x8_vert_neon
 # //.endfunc

        .text
        .align 2
        .globl _ff_pred8x8_plane_neon
 # //.type _ff_pred8x8_plane_neon, %function
 # //.func _ff_pred8x8_plane_neon
_ff_pred8x8_plane_neon:
        sub x3, x0, x1

        adrp x4, p8weight@PAGE
        add x4, x4, p8weight@PAGEOFF
        adrp x5, p16weight@PAGE
        add x5, x5, p16weight@PAGEOFF


        add x2, x3, #4
        sub x3, x3, #1
        ld1 {v0.s}[0], [x3]
        ld1 {v2.s}[0], [x2], x1
        ld1 {v0.b}[4], [x3], x1
        ld1 {v0.b}[5], [x3], x1
        ld1 {v0.b}[6], [x3], x1
        ld1 {v0.b}[7], [x3], x1
        add x3, x3, x1
        ld1 {v3.b}[0], [x3], x1
        ld1 {v3.b}[1], [x3], x1
        ld1 {v3.b}[2], [x3], x1
        ld1 {v3.b}[3], [x3], x1
        uaddl v7.8h, v2.8b, v3.8b
        rev32 v0.8b, v0.8b
        trn1 v2.2s, v2.2s, v3.2s
        usubl v2.8h, v2.8b, v0.8b
        ld1 {v6.8h}, [x4]
        mul v2.8h, v2.8h, v6.8h
        ld1 {v0.8h}, [x5]
        saddlp v2.4s, v2.8h
        addp v2.4s, v2.4s, v2.4s
        shl v3.4s, v2.4s, #4
        add v2.4s, v3.4s, v2.4s
        rshrn v5.4h, v2.4s, #5
        addp v2.4h, v5.4h, v5.4h
        shl v3.4h, v2.4h, #1
        add v3.4h, v3.4h, v2.4h
        rev64 v7.4h, v7.4h
        add v7.4h, v7.4h, v0.4h
        shl v2.4h, v7.4h, #4
        sub v2.4h, v2.4h, v3.4h
        ext v0.16b, v0.16b, v0.16b, #14
        mov v0.h[0], wzr
        mul v0.8h, v0.8h, v5.h[0]
        dup v1.8h, v2.h[0]
        dup v2.8h, v5.h[1]
        add v1.8h, v1.8h, v0.8h
        mov w3, #8
1:
        sqshrun v0.8b, v1.8h, #5
        add v1.8h, v1.8h, v2.8h
        st1 {v0.8b}, [x0], x1
        subs w3, w3, #1
        b.ne 1b
        ret
 # //.size ff_pred8x8_plane_neon, . - ff_pred8x8_plane_neon
 # //.endfunc

        .text
        .align 2
        .globl _ff_pred8x8_128_dc_neon
 # //.type _ff_pred8x8_128_dc_neon, %function
 # //.func _ff_pred8x8_128_dc_neon
_ff_pred8x8_128_dc_neon:
        movi v0.8b, #128
        movi v1.8b, #128
        b L_pred8x8_dc_end
 # //.size ff_pred8x8_128_dc_neon, . - ff_pred8x8_128_dc_neon
 # //.endfunc

        .text
        .align 2
        .globl _ff_pred8x8_top_dc_neon
 # //.type _ff_pred8x8_top_dc_neon, %function
 # //.func _ff_pred8x8_top_dc_neon
_ff_pred8x8_top_dc_neon:
        sub x2, x0, x1
        ld1 {v0.8b}, [x2]
        uaddlp v0.4h, v0.8b
        addp v0.4h, v0.4h, v0.4h
        zip1 v0.8h, v0.8h, v0.8h
        rshrn v2.8b, v0.8h, #2
        zip1 v0.8b, v2.8b, v2.8b
        zip1 v1.8b, v2.8b, v2.8b
        b L_pred8x8_dc_end
 # //.size ff_pred8x8_top_dc_neon, . - ff_pred8x8_top_dc_neon
 # //.endfunc

        .text
        .align 2
        .globl _ff_pred8x8_left_dc_neon
 # //.type _ff_pred8x8_left_dc_neon, %function
 # //.func _ff_pred8x8_left_dc_neon
_ff_pred8x8_left_dc_neon:
        sub x2, x0, #1
        ld1 {v0.b}[0], [x2], x1
        ld1 {v0.b}[1], [x2], x1
        ld1 {v0.b}[2], [x2], x1
        ld1 {v0.b}[3], [x2], x1
        ld1 {v0.b}[4], [x2], x1
        ld1 {v0.b}[5], [x2], x1
        ld1 {v0.b}[6], [x2], x1
        ld1 {v0.b}[7], [x2], x1
        uaddlp v0.4h, v0.8b
        addp v0.4h, v0.4h, v0.4h
        rshrn v2.8b, v0.8h, #2
        dup v1.8b, v2.b[1]
        dup v0.8b, v2.b[0]
        b L_pred8x8_dc_end
 # //.size ff_pred8x8_left_dc_neon, . - ff_pred8x8_left_dc_neon
 # //.endfunc

        .text
        .align 2
        .globl _ff_pred8x8_dc_neon
 # //.type _ff_pred8x8_dc_neon, %function
 # //.func _ff_pred8x8_dc_neon
_ff_pred8x8_dc_neon:
        sub x2, x0, x1
        sub x3, x0, #1
        ld1 {v0.8b}, [x2]
        ld1 {v1.b}[0], [x3], x1
        ld1 {v1.b}[1], [x3], x1
        ld1 {v1.b}[2], [x3], x1
        ld1 {v1.b}[3], [x3], x1
        ld1 {v1.b}[4], [x3], x1
        ld1 {v1.b}[5], [x3], x1
        ld1 {v1.b}[6], [x3], x1
        ld1 {v1.b}[7], [x3], x1
        uaddlp v0.4h, v0.8b
        uaddlp v1.4h, v1.8b
        trn1 v2.2s, v0.2s, v1.2s
        trn2 v3.2s, v0.2s, v1.2s
        addp v4.4h, v2.4h, v3.4h
        addp v5.4h, v4.4h, v4.4h
        rshrn v6.8b, v5.8h, #3
        rshrn v7.8b, v4.8h, #2
        dup v0.8b, v6.b[0]
        dup v2.8b, v7.b[2]
        dup v1.8b, v7.b[3]
        dup v3.8b, v6.b[1]
        zip1 v0.2s, v0.2s, v2.2s
        zip1 v1.2s, v1.2s, v3.2s
L_pred8x8_dc_end:
        mov w3, #4
        add x2, x0, x1, lsl #2
6: st1 {v0.8b}, [x0], x1
        st1 {v1.8b}, [x2], x1
        subs w3, w3, #1
        b.ne 6b
        ret
 # //.size ff_pred8x8_dc_neon, . - ff_pred8x8_dc_neon
 # //.endfunc

        .text
        .align 2
        .globl _ff_pred8x8_l0t_dc_neon
 # //.type _ff_pred8x8_l0t_dc_neon, %function
 # //.func _ff_pred8x8_l0t_dc_neon
_ff_pred8x8_l0t_dc_neon:
        sub x2, x0, x1
        sub x3, x0, #1
        ld1 {v0.8b}, [x2]
        ld1 {v1.b}[0], [x3], x1
        ld1 {v1.b}[1], [x3], x1
        ld1 {v1.b}[2], [x3], x1
        ld1 {v1.b}[3], [x3], x1
        zip1 v0.4s, v0.4s, v1.4s
        uaddlp v0.8h, v0.16b
        addp v0.8h, v0.8h, v0.8h
        addp v1.4h, v0.4h, v0.4h
        rshrn v2.8b, v0.8h, #2
        rshrn v3.8b, v1.8h, #3
        dup v4.8b, v3.b[0]
        dup v6.8b, v2.b[2]
        dup v5.8b, v2.b[0]
        zip1 v0.2s, v4.2s, v6.2s
        zip1 v1.2s, v5.2s, v6.2s
        b L_pred8x8_dc_end
 # //.size ff_pred8x8_l0t_dc_neon, . - ff_pred8x8_l0t_dc_neon
 # //.endfunc

        .text
        .align 2
        .globl _ff_pred8x8_l00_dc_neon
 # //.type _ff_pred8x8_l00_dc_neon, %function
 # //.func _ff_pred8x8_l00_dc_neon
_ff_pred8x8_l00_dc_neon:
        sub x2, x0, #1
        ld1 {v0.b}[0], [x2], x1
        ld1 {v0.b}[1], [x2], x1
        ld1 {v0.b}[2], [x2], x1
        ld1 {v0.b}[3], [x2], x1
        uaddlp v0.4h, v0.8b
        addp v0.4h, v0.4h, v0.4h
        rshrn v0.8b, v0.8h, #2
        movi v1.8b, #128
        dup v0.8b, v0.b[0]
        b L_pred8x8_dc_end
 # //.size ff_pred8x8_l00_dc_neon, . - ff_pred8x8_l00_dc_neon
 # //.endfunc

        .text
        .align 2
        .globl _ff_pred8x8_0lt_dc_neon
 # //.type _ff_pred8x8_0lt_dc_neon, %function
 # //.func _ff_pred8x8_0lt_dc_neon
_ff_pred8x8_0lt_dc_neon:
        add x3, x0, x1, lsl #2
        sub x2, x0, x1
        sub x3, x3, #1
        ld1 {v0.8b}, [x2]
        ld1 {v1.b}[4], [x3], x1
        ld1 {v1.b}[5], [x3], x1
        ld1 {v1.b}[6], [x3], x1
        ld1 {v1.b}[7], [x3], x1
        zip1 v0.4s, v0.4s, v1.4s
        uaddlp v0.8h, v0.16b
        addp v0.8h, v0.8h, v0.8h
        addp v1.4h, v0.4h, v0.4h
        rshrn v2.8b, v0.8h, #2
        rshrn v3.8b, v1.8h, #3
        dup v4.8b, v2.b[0]
        dup v5.8b, v2.b[3]
        dup v6.8b, v2.b[2]
        dup v7.8b, v3.b[1]
        zip1 v0.2s, v4.2s, v6.2s
        zip1 v1.2s, v5.2s, v7.2s
        b L_pred8x8_dc_end
 # //.size ff_pred8x8_0lt_dc_neon, . - ff_pred8x8_0lt_dc_neon
 # //.endfunc

        .text
        .align 2
        .globl _ff_pred8x8_0l0_dc_neon
 # //.type _ff_pred8x8_0l0_dc_neon, %function
 # //.func _ff_pred8x8_0l0_dc_neon
_ff_pred8x8_0l0_dc_neon:
        add x2, x0, x1, lsl #2
        sub x2, x2, #1
        ld1 {v1.b}[0], [x2], x1
        ld1 {v1.b}[1], [x2], x1
        ld1 {v1.b}[2], [x2], x1
        ld1 {v1.b}[3], [x2], x1
        uaddlp v2.4h, v1.8b
        addp v2.4h, v2.4h, v2.4h
        rshrn v1.8b, v2.8h, #2
        movi v0.8b, #128
        dup v1.8b, v1.b[0]
        b L_pred8x8_dc_end
 # //.size ff_pred8x8_0l0_dc_neon, . - ff_pred8x8_0l0_dc_neon
 # //.endfunc
.text
.align 2
#endif
