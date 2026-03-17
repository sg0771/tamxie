#ifdef __arm__
        @.arch armv7-a
        @.fpu neon
        .syntax unified
        .thumb
        .set Lpic_idx, 0
        .set Lpic_gp, 0
        .text
        .align 2
        .globl _ff_add_pixels_clamped_armv6

        .thumb_func _ff_add_pixels_clamped_armv6
_ff_add_pixels_clamped_armv6:
        push {r4-r8,lr}
        mov r3, #8
1:
        ldm r0!, {r4,r5,r12,lr}
        ldrd r6, r7, [r1]
        pkhbt r8, r4, r5, lsl #16
        pkhtb r5, r5, r4, asr #16
        pkhbt r4, r12, lr, lsl #16
        pkhtb lr, lr, r12, asr #16
        pld [r1, r2]
        uxtab16 r8, r8, r6
        uxtab16 r5, r5, r6, ror #8
        uxtab16 r4, r4, r7
        uxtab16 lr, lr, r7, ror #8
        usat16 r8, #8, r8
        usat16 r5, #8, r5
        usat16 r4, #8, r4
        usat16 lr, #8, lr
        orr r6, r8, r5, lsl #8
        orr r7, r4, lr, lsl #8
        subs r3, r3, #1

        strd r6, r7, [r1]
        add r1, r1, r2
        bgt 1b
        pop {r4-r8,pc}

        @.endfunc
.text
.align 2
.thumb_func _ff_add_pixels_clamped_armv6
#endif
