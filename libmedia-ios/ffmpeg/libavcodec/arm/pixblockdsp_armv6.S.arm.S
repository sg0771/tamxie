#ifdef __arm__
        @.arch armv7-a

        @.fpu neon        .syntax unified
        .thumb


        .set Lpic_idx, 0
        .set Lpic_gp, 0
        .text
        .align 2
        .globl _ff_get_pixels_armv6

        .thumb_func _ff_get_pixels_armv6
_ff_get_pixels_armv6:
        pld [r1, r2]
        push {r4-r8, lr}
        mov lr, #8
1:

        ldrd r4, r5, [r1]
        add r1, r1, r2
        subs lr, lr, #1
        uxtb16 r6, r4
        uxtb16 r4, r4, ror #8
        uxtb16 r12, r5
        uxtb16 r8, r5, ror #8
        pld [r1, r2]
        pkhbt r5, r6, r4, lsl #16
        pkhtb r6, r4, r6, asr #16
        pkhbt r7, r12, r8, lsl #16
        pkhtb r12, r8, r12, asr #16
        stm r0!, {r5,r6,r7,r12}
        bgt 1b

        pop {r4-r8, pc}

        @.endfunc

        .set Lpic_idx, 0
        .set Lpic_gp, 0
        .text
        .align 2
        .globl _ff_diff_pixels_armv6

        .thumb_func _ff_diff_pixels_armv6
_ff_diff_pixels_armv6:
        pld [r1, r3]
        pld [r2, r3]
        push {r4-r9, lr}
        mov lr, #8
1:

        ldrd r4, r5, [r1]
        add r1, r1, r3

        ldrd r6, r7, [r2]
        add r2, r2, r3
        uxtb16 r8, r4
        uxtb16 r4, r4, ror #8
        uxtb16 r9, r6
        uxtb16 r6, r6, ror #8
        pld [r1, r3]
        ssub16 r9, r8, r9
        ssub16 r6, r4, r6
        uxtb16 r8, r5
        uxtb16 r5, r5, ror #8
        pld [r2, r3]
        pkhbt r4, r9, r6, lsl #16
        pkhtb r6, r6, r9, asr #16
        uxtb16 r9, r7
        uxtb16 r7, r7, ror #8
        ssub16 r9, r8, r9
        ssub16 r5, r5, r7
        subs lr, lr, #1
        pkhbt r8, r9, r5, lsl #16
        pkhtb r9, r5, r9, asr #16
        stm r0!, {r4,r6,r8,r9}
        bgt 1b

        pop {r4-r9, pc}

        @.endfunc
.text
.align 2
.thumb_func _ff_get_pixels_armv6
.thumb_func _ff_diff_pixels_armv6

#endif
