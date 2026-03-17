#ifdef __arm__
        @.arch armv7-a
        @.fpu neon
        .syntax unified
        .thumb
        .set Lpic_idx, 0
        .set Lpic_gp, 0
        .text
        .align 2
        .globl _ff_prefetch_arm

        .thumb_func _ff_prefetch_arm
_ff_prefetch_arm:
1:
        subs r2, r2, #1
        pld [r0]
        add r0, r0, r1
        bne 1b
        bx lr

        @.endfunc
.text
.align 2
.thumb_func _ff_prefetch_arm
#endif
