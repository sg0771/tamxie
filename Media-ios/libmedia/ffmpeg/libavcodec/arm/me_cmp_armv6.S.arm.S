#ifdef __arm__

        @.arch armv7-a
        @.fpu neon
        .syntax unified
        .thumb
        .set Lpic_idx, 0
        .set Lpic_gp, 0
        .text
        .align 2
        .globl _ff_pix_abs16_armv6

        .thumb_func _ff_pix_abs16_armv6
_ff_pix_abs16_armv6:
        ldr r0, [sp]
        push {r4-r9, lr}
        mov r12, #0
        mov lr, #0
        ldm r1, {r4-r7}
        ldr r8, [r2]
1:
        ldr r9, [r2, #4]
        pld [r1, r3]
        usada8 r12, r4, r8, r12
        ldr r8, [r2, #8]
        pld [r2, r3]
        usada8 lr, r5, r9, lr
        ldr r9, [r2, #12]
        usada8 r12, r6, r8, r12
        subs r0, r0, #1
        usada8 lr, r7, r9, lr
        beq 2f
        add r1, r1, r3
        ldm r1, {r4-r7}
        add r2, r2, r3
        ldr r8, [r2]
        b 1b
2:
        add r0, r12, lr
        pop {r4-r9, pc}

        @.endfunc

        .set Lpic_idx, 0
        .set Lpic_gp, 0
        .text
        .align 2
        .globl _ff_pix_abs16_x2_armv6

        .thumb_func _ff_pix_abs16_x2_armv6
_ff_pix_abs16_x2_armv6:
        ldr r12, [sp]
        push {r4-r11, lr}
        mov r0, #0
        mov lr, #1
        orr lr, lr, lr, lsl #8
        orr lr, lr, lr, lsl #16
1:
        ldr r8, [r2]
        ldr r9, [r2, #4]
        lsr r10, r8, #8
        ldr r4, [r1]
        lsr r6, r9, #8
        orr r10, r10, r9, lsl #24
        ldr r5, [r2, #8]
        eor r11, r8, r10
        uhadd8 r7, r8, r10
        orr r6, r6, r5, lsl #24
        and r11, r11, lr
        uadd8 r7, r7, r11
        ldr r8, [r1, #4]
        usada8 r0, r4, r7, r0
        eor r7, r9, r6
        lsr r10, r5, #8
        and r7, r7, lr
        uhadd8 r4, r9, r6
        ldr r6, [r2, #12]
        uadd8 r4, r4, r7
        pld [r1, r3]
        orr r10, r10, r6, lsl #24
        usada8 r0, r8, r4, r0
        ldr r4, [r1, #8]
        eor r11, r5, r10
        ldrb r7, [r2, #16]
        and r11, r11, lr
        uhadd8 r8, r5, r10
        ldr r5, [r1, #12]
        uadd8 r8, r8, r11
        pld [r2, r3]
        lsr r10, r6, #8
        usada8 r0, r4, r8, r0
        orr r10, r10, r7, lsl #24
        subs r12, r12, #1
        eor r11, r6, r10
        add r1, r1, r3
        uhadd8 r9, r6, r10
        and r11, r11, lr
        uadd8 r9, r9, r11
        add r2, r2, r3
        usada8 r0, r5, r9, r0
        bgt 1b

        pop {r4-r11, pc}

        @.endfunc


        .set Lpic_idx, 0
        .set Lpic_gp, 0
        .text
        .align 2
        .globl _ff_pix_abs16_y2_armv6

        .thumb_func _ff_pix_abs16_y2_armv6
_ff_pix_abs16_y2_armv6:
        pld [r1]
        pld [r2]
        ldr r12, [sp]
        push {r4-r11, lr}
        mov r0, #0
        mov lr, #1
        orr lr, lr, lr, lsl #8
        orr lr, lr, lr, lsl #16
        ldr r4, [r2]
        ldr r5, [r2, #4]
        ldr r6, [r2, #8]
        ldr r7, [r2, #12]
        add r2, r2, r3
1:
        ldr r8, [r2]
        eor r9, r4, r8
        uhadd8 r4, r4, r8
        and r9, r9, lr
        ldr r10, [r1]
        uadd8 r4, r4, r9
        ldr r9, [r2, #4]
        usada8 r0, r4, r10, r0
        pld [r1, r3]
        eor r11, r5, r9
        uhadd8 r5, r5, r9
        and r11, r11, lr
        ldr r4, [r1, #4]
        uadd8 r5, r5, r11
        ldr r10, [r2, #8]
        usada8 r0, r5, r4, r0
        pld [r2, r3]
        eor r4, r6, r10
        uhadd8 r6, r6, r10
        and r4, r4, lr
        ldr r5, [r1, #8]
        uadd8 r6, r6, r4
        ldr r11, [r2, #12]
        usada8 r0, r6, r5, r0
        eor r5, r7, r11
        uhadd8 r7, r7, r11
        and r5, r5, lr
        ldr r4, [r1, #12]
        uadd8 r7, r7, r5
        add r1, r1, r3
        usada8 r0, r7, r4, r0
        add r2, r2, r3
        subs r12, r12, #2
        ldr r4, [r2]
        eor r5, r8, r4
        uhadd8 r8, r8, r4
        and r5, r5, lr
        ldr r6, [r1]
        uadd8 r8, r8, r5
        ldr r5, [r2, #4]
        usada8 r0, r8, r6, r0
        pld [r1, r3]
        eor r7, r9, r5
        uhadd8 r9, r9, r5
        and r7, r7, lr
        ldr r8, [r1, #4]
        uadd8 r9, r9, r7
        ldr r6, [r2, #8]
        usada8 r0, r9, r8, r0
        pld [r2, r3]
        eor r8, r10, r6
        uhadd8 r10, r10, r6
        and r8, r8, lr
        ldr r9, [r1, #8]
        uadd8 r10, r10, r8
        ldr r7, [r2, #12]
        usada8 r0, r10, r9, r0
        eor r9, r11, r7
        uhadd8 r11, r11, r7
        and r9, r9, lr
        ldr r8, [r1, #12]
        uadd8 r11, r11, r9
        add r1, r1, r3
        usada8 r0, r11, r8, r0
        add r2, r2, r3
        bgt 1b

        pop {r4-r11, pc}

        @.endfunc

        .set Lpic_idx, 0
        .set Lpic_gp, 0
        .text
        .align 2
        .globl _ff_pix_abs8_armv6

        .thumb_func _ff_pix_abs8_armv6
_ff_pix_abs8_armv6:
        pld [r2, r3]
        ldr r12, [sp]
        push {r4-r9, lr}
        mov r0, #0
        mov lr, #0

        ldrd r4, r5, [r1]
        add r1, r1, r3
1:
        subs r12, r12, #2
        ldr r7, [r2, #4]

        ldr r6, [r2]
        add r2, r2, r3

        ldrd r8, r9, [r1]
        add r1, r1, r3
        usada8 r0, r4, r6, r0
        pld [r2, r3]
        usada8 lr, r5, r7, lr
        ldr r7, [r2, #4]

        ldr r6, [r2]
        add r2, r2, r3
        beq 2f

        ldrd r4, r5, [r1]
        add r1, r1, r3
        usada8 r0, r8, r6, r0
        pld [r2, r3]
        usada8 lr, r9, r7, lr
        b 1b
2:
        usada8 r0, r8, r6, r0
        usada8 lr, r9, r7, lr
        add r0, r0, lr
        pop {r4-r9, pc}

        @.endfunc

        .set Lpic_idx, 0
        .set Lpic_gp, 0
        .text
        .align 2
        .globl _ff_sse16_armv6

        .thumb_func _ff_sse16_armv6
_ff_sse16_armv6:
        ldr r12, [sp]
        push {r4-r9, lr}
        mov r0, #0
1:
        ldrd r4, r5, [r1]
        ldr r8, [r2]
        uxtb16 lr, r4
        uxtb16 r4, r4, ror #8
        uxtb16 r9, r8
        uxtb16 r8, r8, ror #8
        ldr r7, [r2, #4]
        usub16 lr, lr, r9
        usub16 r4, r4, r8
        smlad r0, lr, lr, r0
        uxtb16 r6, r5
        uxtb16 lr, r5, ror #8
        uxtb16 r8, r7
        uxtb16 r9, r7, ror #8
        smlad r0, r4, r4, r0
        ldrd r4, r5, [r1, #8]
        usub16 r6, r6, r8
        usub16 r8, lr, r9
        ldr r7, [r2, #8]
        smlad r0, r6, r6, r0
        uxtb16 lr, r4
        uxtb16 r4, r4, ror #8
        uxtb16 r9, r7
        uxtb16 r7, r7, ror #8
        smlad r0, r8, r8, r0
        ldr r8, [r2, #12]
        usub16 lr, lr, r9
        usub16 r4, r4, r7
        smlad r0, lr, lr, r0
        uxtb16 r6, r5
        uxtb16 r5, r5, ror #8
        uxtb16 r9, r8
        uxtb16 r8, r8, ror #8
        smlad r0, r4, r4, r0
        usub16 r6, r6, r9
        usub16 r5, r5, r8
        smlad r0, r6, r6, r0
        add r1, r1, r3
        add r2, r2, r3
        subs r12, r12, #1
        smlad r0, r5, r5, r0
        bgt 1b

        pop {r4-r9, pc}

        @.endfunc
.text
.align 2
.thumb_func _ff_sse16_armv6
.thumb_func _ff_pix_abs16_x2_armv6
.thumb_func _ff_pix_abs16_y2_armv6
.thumb_func _ff_pix_abs16_armv6
.thumb_func _ff_pix_abs8_armv6

#endif
