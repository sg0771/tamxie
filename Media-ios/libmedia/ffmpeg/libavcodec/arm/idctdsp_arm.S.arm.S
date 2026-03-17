#ifdef __arm__
        @.arch armv7-a

        @.fpu neon        .syntax unified
        .thumb

        .set Lpic_idx, 0
        .set Lpic_gp, 0
        .text
        .align 5
        .globl _ff_add_pixels_clamped_arm

        .thumb_func _ff_add_pixels_clamped_arm
_ff_add_pixels_clamped_arm:
        push {r4-r10}
        mov r10, #8
1:
        ldr r4, [r1]

        ldrsh r5, [r0]
        ldrsh r7, [r0, #2]
        and r6, r4, #0xFF
        and r8, r4, #0xFF00
        add r6, r6, r5
        add r8, r7, r8, lsr #8
        mvn r5, r5
        mvn r7, r7
        tst r6, #0x100
        it ne
        movne r6, r5, lsr #24
        tst r8, #0x100
        it ne
        movne r8, r7, lsr #24
        mov r9, r6
        ldrsh r5, [r0, #4]
        orr r9, r9, r8, lsl #8


        ldrsh r7, [r0, #6]
        and r6, r4, #0xFF0000
        and r8, r4, #0xFF000000
        add r6, r5, r6, lsr #16
        add r8, r7, r8, lsr #24
        mvn r5, r5
        mvn r7, r7
        tst r6, #0x100
        it ne
        movne r6, r5, lsr #24
        tst r8, #0x100
        it ne
        movne r8, r7, lsr #24
        orr r9, r9, r6, lsl #16
        ldr r4, [r1, #4]
        orr r9, r9, r8, lsl #24

        ldrsh r5, [r0, #8]
        str r9, [r1]

        ldrsh r7, [r0, #10]
        and r6, r4, #0xFF
        and r8, r4, #0xFF00
        add r6, r6, r5
        add r8, r7, r8, lsr #8
        mvn r5, r5
        mvn r7, r7
        tst r6, #0x100
        it ne
        movne r6, r5, lsr #24
        tst r8, #0x100
        it ne
        movne r8, r7, lsr #24
        mov r9, r6
        ldrsh r5, [r0, #12]
        orr r9, r9, r8, lsl #8


        ldrsh r7, [r0, #14]
        and r6, r4, #0xFF0000
        and r8, r4, #0xFF000000
        add r6, r5, r6, lsr #16
        add r8, r7, r8, lsr #24
        mvn r5, r5
        mvn r7, r7
        tst r6, #0x100
        it ne
        movne r6, r5, lsr #24
        tst r8, #0x100
        it ne
        movne r8, r7, lsr #24
        orr r9, r9, r6, lsl #16
        add r0, r0, #16
        orr r9, r9, r8, lsl #24
        subs r10, r10, #1

        str r9, [r1, #4]
        add r1, r1, r2
        bne 1b

        pop {r4-r10}
        bx lr

        @.endfunc
.text
.align 2
.thumb_func _ff_add_pixels_clamped_arm
#endif
