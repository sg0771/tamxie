#ifdef __arm__
        @.arch armv7-a
        @.fpu neon
        .syntax unified
        .thumb
        .set Lpic_idx, 0
        .set Lpic_gp, 0
        .text
        .align 2
        .globl _ff_vector_fmul_vfp

        .thumb_func _ff_vector_fmul_vfp
_ff_vector_fmul_vfp:
        vpush {d8-d15}
        fmrx r12, fpscr
        orr r12, r12, #(3 << 16)
        fmxr fpscr, r12

        vldmia r1!, {s0-s3}
        vldmia r2!, {s8-s11}
        vldmia r1!, {s4-s7}
        vldmia r2!, {s12-s15}
        vmul.f32 s8, s0, s8
1:
        subs r3, r3, #16
        vmul.f32 s12, s4, s12
        itttt ge
        vldmiage r1!, {s16-s19}
        vldmiage r2!, {s24-s27}
        vldmiage r1!, {s20-s23}
        vldmiage r2!, {s28-s31}
        it ge
        vmulge.f32 s24, s16, s24
        vstmia r0!, {s8-s11}
        vstmia r0!, {s12-s15}
        it ge
        vmulge.f32 s28, s20, s28
        itttt gt
        vldmiagt r1!, {s0-s3}
        vldmiagt r2!, {s8-s11}
        vldmiagt r1!, {s4-s7}
        vldmiagt r2!, {s12-s15}
        ittt ge
        vmulge.f32 s8, s0, s8
        vstmiage r0!, {s24-s27}
        vstmiage r0!, {s28-s31}
        bgt 1b

        bic r12, r12, #(7 << 16)
        fmxr fpscr, r12
        vpop {d8-d15}
        bx lr

        @.endfunc        .set Lpic_idx, 0
        .set Lpic_gp, 0
        .text
        .align 2
        .globl _ff_vector_fmul_window_vfp

        .thumb_func _ff_vector_fmul_window_vfp
_ff_vector_fmul_window_vfp:
DST0 .req a1
SRC0 .req a2
SRC1 .req a3
WIN0 .req a4
LEN .req v1
DST1 .req v2
WIN1 .req v3
OLDFPSCR .req ip

        push {v1-v3,lr}
        ldr LEN, [sp, #4*4+0]
        vpush {s16-s31}
        fmrx OLDFPSCR, FPSCR
        add DST1, DST0, LEN, lsl #3
        add SRC1, SRC1, LEN, lsl #2
        add WIN1, WIN0, LEN, lsl #3

        tst LEN, #7
        beq 4f 

         ldr lr, Literal_0
        fmxr FPSCR, lr

        tst LEN, #1
        beq 1f
        vldmdb WIN1!, {s0}
        vldmia SRC0!, {s8}
        vldmia WIN0!, {s16}
        vmul.f s24, s0, s8
        vldmdb SRC1!, {s20}
        vmul.f s8, s16, s8
        vmls.f s24, s16, s20
        vmla.f s8, s0, s20
        vstmia DST0!, {s24}
        vstmdb DST1!, {s8}
1:
        tst LEN, #2
        beq 2f
        vldmdb WIN1!, {s0}
        vldmdb WIN1!, {s1}
        vldmia SRC0!, {s8-s9}
        vldmia WIN0!, {s16-s17}
        vmul.f s24, s0, s8
        vmul.f s25, s1, s9
        vldmdb SRC1!, {s20}
        vldmdb SRC1!, {s21}
        vmul.f s8, s16, s8
        vmul.f s9, s17, s9
        vmls.f s24, s16, s20
        vmls.f s25, s17, s21
        vmla.f s8, s0, s20
        vmla.f s9, s1, s21
        vstmia DST0!, {s24-s25}
        vstmdb DST1!, {s8}
        vstmdb DST1!, {s9}
2:
        tst LEN, #4
        beq 3f
        vldmdb WIN1!, {s0}
        vldmdb WIN1!, {s1}
        vldmdb WIN1!, {s2}
        vldmdb WIN1!, {s3}
        vldmia SRC0!, {s8-s11}
        vldmia WIN0!, {s16-s19}
        vmul.f s24, s0, s8
        vmul.f s25, s1, s9
        vmul.f s26, s2, s10
        vmul.f s27, s3, s11
        vldmdb SRC1!, {s20}
        vldmdb SRC1!, {s21}
        vldmdb SRC1!, {s22}
        vldmdb SRC1!, {s23}
        vmul.f s8, s16, s8
        vmul.f s9, s17, s9
        vmul.f s10, s18, s10
        vmul.f s11, s19, s11
        vmls.f s24, s16, s20
        vmls.f s25, s17, s21
        vmls.f s26, s18, s22
        vmls.f s27, s19, s23
        vmla.f s8, s0, s20
        vmla.f s9, s1, s21
        vmla.f s10, s2, s22
        vmla.f s11, s3, s23
        vstmia DST0!, {s24-s27}
        vstmdb DST1!, {s8}
        vstmdb DST1!, {s9}
        vstmdb DST1!, {s10}
        vstmdb DST1!, {s11}
3:
        bics LEN, LEN, #7
        beq 7f
4:
         ldr lr, Literal_1
        fmxr FPSCR, lr

        vldmdb WIN1!, {s0}
        vldmdb WIN1!, {s1}
        vldmdb WIN1!, {s2}
        vldmdb WIN1!, {s3}
        vldmia SRC0!, {s8-s11}
        vldmia WIN0!, {s16-s19}
        vmul.f s24, s0, s8 
        vldmdb SRC1!, {s20}
        vldmdb SRC1!, {s21}
        vldmdb SRC1!, {s22}
        vldmdb SRC1!, {s23}
        vmul.f s8, s16, s8 
        vmls.f s24, s16, s20 
            vldmdb WIN1!, {s4}
            vldmdb WIN1!, {s5}
            vldmdb WIN1!, {s6}
            vldmdb WIN1!, {s7}
            vldmia SRC0!, {s12-s13}
        vmla.f s8, s0, s20 
            vldmia SRC0!, {s14-s15}
        subs LEN, LEN, #8
        beq 6f
5: vldmia WIN0!, {s20-s23}
            vmul.f s28, s4, s12 
        vstmia DST0!, {s24-s25}
            vldmdb SRC1!, {s16}
            vldmdb SRC1!, {s17}
            vldmdb SRC1!, {s18}
            vldmdb SRC1!, {s19}
            vmul.f s12, s20, s12 
        vstmia DST0!, {s26-s27}
        vstmdb DST1!, {s8}
        vstmdb DST1!, {s9}
        vstmdb DST1!, {s10}
        vstmdb DST1!, {s11}
            vmls.f s28, s20, s16 
                vldmdb WIN1!, {s0}
                vldmdb WIN1!, {s1}
                vldmdb WIN1!, {s2}
                vldmdb WIN1!, {s3}
                vldmia SRC0!, {s8-s9}
            vmla.f s12, s4, s16 
                vldmia SRC0!, {s10-s11}
        subs LEN, LEN, #8
                vldmia WIN0!, {s16-s19}
                vmul.f s24, s0, s8 
            vstmia DST0!, {s28-s29}
                vldmdb SRC1!, {s20}
                vldmdb SRC1!, {s21}
                vldmdb SRC1!, {s22}
                vldmdb SRC1!, {s23}
                vmul.f s8, s16, s8 
            vstmia DST0!, {s30-s31}
            vstmdb DST1!, {s12}
            vstmdb DST1!, {s13}
            vstmdb DST1!, {s14}
            vstmdb DST1!, {s15}
                vmls.f s24, s16, s20 
                    vldmdb WIN1!, {s4}
                    vldmdb WIN1!, {s5}
                    vldmdb WIN1!, {s6}
                    vldmdb WIN1!, {s7}
                    vldmia SRC0!, {s12-s13}
                vmla.f s8, s0, s20 
                    vldmia SRC0!, {s14-s15}
        bne 5b
6: vldmia WIN0!, {s20-s23}
                    vmul.f s28, s4, s12 
                vstmia DST0!, {s24-s25}
                    vldmdb SRC1!, {s16}
                    vldmdb SRC1!, {s17}
                    vldmdb SRC1!, {s18}
                    vldmdb SRC1!, {s19}
                    vmul.f s12, s20, s12 
                vstmia DST0!, {s26-s27}
                vstmdb DST1!, {s8}
                vstmdb DST1!, {s9}
                vstmdb DST1!, {s10}
                vstmdb DST1!, {s11}
                    vmls.f s28, s20, s16 
                    vmla.f s12, s4, s16 
                    vstmia DST0!, {s28-s31}
                    vstmdb DST1!, {s12}
                    vstmdb DST1!, {s13}
                    vstmdb DST1!, {s14}
                    vstmdb DST1!, {s15}
7:
        fmxr FPSCR, OLDFPSCR
        vpop {s16-s31}
        pop {v1-v3,pc}

.unreq dst0
.unreq DST0
.unreq src0
.unreq SRC0
.unreq src1
.unreq SRC1
.unreq win0
.unreq WIN0
.unreq len
.unreq LEN
.unreq oldfpscr
.unreq OLDFPSCR
.unreq dst1
.unreq DST1
.unreq win1
.unreq WIN1

        @.endfunc        .set Lpic_idx, 0
        .set Lpic_gp, 0
        .text
        .align 2
        .globl _ff_vector_fmul_reverse_vfp

        .thumb_func _ff_vector_fmul_reverse_vfp
_ff_vector_fmul_reverse_vfp:
        vpush {d8-d15}
        add r2, r2, r3, lsl #2
        vldmdb r2!, {s0-s3}
        vldmia r1!, {s8-s11}
        vldmdb r2!, {s4-s7}
        vldmia r1!, {s12-s15}
        vmul.f32 s8, s3, s8
        vmul.f32 s9, s2, s9
        vmul.f32 s10, s1, s10
        vmul.f32 s11, s0, s11
1:
        subs r3, r3, #16
        it ge
        vldmdbge r2!, {s16-s19}
        vmul.f32 s12, s7, s12
        it ge
        vldmiage r1!, {s24-s27}
        vmul.f32 s13, s6, s13
        it ge
        vldmdbge r2!, {s20-s23}
        vmul.f32 s14, s5, s14
        it ge
        vldmiage r1!, {s28-s31}
        vmul.f32 s15, s4, s15
        it ge
        vmulge.f32 s24, s19, s24
        it gt
        vldmdbgt r2!, {s0-s3}
        it ge
        vmulge.f32 s25, s18, s25
        vstmia r0!, {s8-s13}
        it ge
        vmulge.f32 s26, s17, s26
        it gt
        vldmiagt r1!, {s8-s11}
        itt ge
        vmulge.f32 s27, s16, s27
        vmulge.f32 s28, s23, s28
        it gt
        vldmdbgt r2!, {s4-s7}
        it ge
        vmulge.f32 s29, s22, s29
        vstmia r0!, {s14-s15}
        ittt ge
        vmulge.f32 s30, s21, s30
        vmulge.f32 s31, s20, s31
        vmulge.f32 s8, s3, s8
        it gt
        vldmiagt r1!, {s12-s15}
        itttt ge
        vmulge.f32 s9, s2, s9
        vmulge.f32 s10, s1, s10
        vstmiage r0!, {s24-s27}
        vmulge.f32 s11, s0, s11
        it ge
        vstmiage r0!, {s28-s31}
        bgt 1b

        vpop {d8-d15}
        bx lr

        @.endfunc


        .set Lpic_idx, 0
        .set Lpic_gp, 0
        .text
        .align 2
        .globl _ff_butterflies_float_vfp

        .thumb_func _ff_butterflies_float_vfp
_ff_butterflies_float_vfp:
BASE1 .req a1
BASE2 .req a2
LEN .req a3
OLDFPSCR .req a4

        vpush {s16-s31}
        fmrx OLDFPSCR, FPSCR

        tst LEN, #7
        beq 4f 

         ldr ip, Literal_0
        fmxr FPSCR, ip

        tst LEN, #1
        beq 1f
        vldmia BASE1!, {s0}
        vldmia BASE2!, {s8}
        vadd.f s16, s0, s8
        vsub.f s24, s0, s8
        vstr s16, [BASE1, #0-4*1]
        vstr s24, [BASE2, #0-4*1]
1:
        tst LEN, #2
        beq 2f
        vldmia BASE1!, {s0-s1}
        vldmia BASE2!, {s8-s9}
        vadd.f s16, s0, s8
        vadd.f s17, s1, s9
        vsub.f s24, s0, s8
        vsub.f s25, s1, s9
        vstr d8, [BASE1, #0-8*1] 
        vstr d12, [BASE2, #0-8*1] 
2:
        tst LEN, #4
        beq 3f
        vldmia BASE1!, {s0-s1}
        vldmia BASE2!, {s8-s9}
        vldmia BASE1!, {s2-s3}
        vldmia BASE2!, {s10-s11}
        vadd.f s16, s0, s8
        vadd.f s17, s1, s9
        vsub.f s24, s0, s8
        vsub.f s25, s1, s9
        vadd.f s18, s2, s10
        vadd.f s19, s3, s11
        vsub.f s26, s2, s10
        vsub.f s27, s3, s11
        vstr d8, [BASE1, #0-16*1] 
        vstr d12, [BASE2, #0-16*1] 
        vstr d9, [BASE1, #8-16*1] 
        vstr d13, [BASE2, #8-16*1] 
3:
        bics LEN, LEN, #7
        beq 7f
4:
         ldr ip, Literal_1
        fmxr FPSCR, ip

        vldmia BASE1!, {s0-s1}
        vldmia BASE2!, {s8-s9}
        vldmia BASE1!, {s2-s3}
        vldmia BASE2!, {s10-s11}
        vadd.f s16, s0, s8
            vldmia BASE1!, {s4-s5}
            vldmia BASE2!, {s12-s13}
            vldmia BASE1!, {s6-s7}
            vldmia BASE2!, {s14-s15}
        vsub.f s24, s0, s8
            vadd.f s20, s4, s12
        subs LEN, LEN, #8
        beq 6f
5: vldmia BASE1!, {s0-s3}
                vldmia BASE2!, {s8-s11}
            vsub.f s28, s4, s12
        vstr d8, [BASE1, #0-16*3] 
        vstr d9, [BASE1, #8-16*3] 
        vstr d12, [BASE2, #0-16*3] 
        vstr d13, [BASE2, #8-16*3] 
                vadd.f s16, s0, s8
                    vldmia BASE1!, {s4-s7}
                    vldmia BASE2!, {s12-s15}
                vsub.f s24, s0, s8
            vstr d10, [BASE1, #0-16*3] 
            vstr d11, [BASE1, #8-16*3] 
            vstr d14, [BASE2, #0-16*3] 
            vstr d15, [BASE2, #8-16*3] 
                    vadd.f s20, s4, s12
        subs LEN, LEN, #8
        bne 5b
6: vsub.f s28, s4, s12
                vstr d8, [BASE1, #0-16*2] 
                vstr d9, [BASE1, #8-16*2] 
                vstr d12, [BASE2, #0-16*2] 
                vstr d13, [BASE2, #8-16*2] 
                    vstr d10, [BASE1, #0-16*1] 
                    vstr d11, [BASE1, #8-16*1] 
                    vstr d14, [BASE2, #0-16*1] 
                    vstr d15, [BASE2, #8-16*1] 
7:
        fmxr FPSCR, OLDFPSCR
        vpop {s16-s31}
        bx lr

.unreq base1
.unreq BASE1
.unreq base2
.unreq BASE2
.unreq len
.unreq LEN
.unreq oldfpscr
.unreq OLDFPSCR

        @.endfunc
.text
.align 2
Literal_1:
 .word 0x03030000 
Literal_0:
 .word 0x03000000 
.thumb_func _ff_vector_fmul_vfp
.thumb_func _ff_butterflies_float_vfp
.thumb_func _ff_vector_fmul_reverse_vfp
.thumb_func _ff_vector_fmul_window_vfp
#endif