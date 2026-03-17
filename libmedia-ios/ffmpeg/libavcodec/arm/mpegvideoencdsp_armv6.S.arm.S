#ifdef __arm__
        @.arch armv7-a

        @.fpu neon        .syntax unified
        .thumb
        .set Lpic_idx, 0
        .set Lpic_gp, 0
        .text
        .align 2
        .globl _ff_pix_norm1_armv6

        .thumb_func _ff_pix_norm1_armv6
_ff_pix_norm1_armv6:
        push {r4-r6, lr}
        mov r12, #16
        mov lr, #0
1:
        ldm r0, {r2-r5}
        uxtb16 r6, r2
        uxtb16 r2, r2, ror #8
        smlad lr, r6, r6, lr
        uxtb16 r6, r3
        smlad lr, r2, r2, lr
        uxtb16 r3, r3, ror #8
        smlad lr, r6, r6, lr
        uxtb16 r6, r4
        smlad lr, r3, r3, lr
        uxtb16 r4, r4, ror #8
        smlad lr, r6, r6, lr
        uxtb16 r6, r5
        smlad lr, r4, r4, lr
        uxtb16 r5, r5, ror #8
        smlad lr, r6, r6, lr
        subs r12, r12, #1
        add r0, r0, r1
        smlad lr, r5, r5, lr
        bgt 1b

        mov r0, lr
        pop {r4-r6, pc}

        @.endfunc

        .set Lpic_idx, 0
        .set Lpic_gp, 0
        .text
        .align 2
        .globl _ff_pix_sum_armv6

        .thumb_func _ff_pix_sum_armv6
_ff_pix_sum_armv6:
        push {r4-r7, lr}
        mov r12, #16
        mov r2, #0
        mov r3, #0
        mov lr, #0
        ldr r4, [r0]
1:
        subs r12, r12, #1
        ldr r5, [r0, #4]
        usada8 r2, r4, lr, r2
        ldr r6, [r0, #8]
        usada8 r3, r5, lr, r3
        ldr r7, [r0, #12]
        usada8 r2, r6, lr, r2
        beq 2f

        add r0, r0, r1
        ldr r4, [r0]
        usada8 r3, r7, lr, r3
        bgt 1b
2:
        usada8 r3, r7, lr, r3
        add r0, r2, r3
        pop {r4-r7, pc}

        @.endfunc
.text
.align 2
.thumb_func _ff_pix_sum_armv6
.thumb_func _ff_pix_norm1_armv6

#endif