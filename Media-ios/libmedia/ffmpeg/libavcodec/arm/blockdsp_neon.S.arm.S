#ifdef __arm__
        @.arch armv7-a
        @.fpu neon
        .syntax unified
        .thumb
        .set Lpic_idx, 0
        .set Lpic_gp, 0
        .text
        .align 2
        .globl _ff_clear_block_neon

        .thumb_func _ff_clear_block_neon
_ff_clear_block_neon:
        vmov.i16 q0, #0

        vst1.16 {q0}, [r0,:128]!

        vst1.16 {q0}, [r0,:128]!

        vst1.16 {q0}, [r0,:128]!

        vst1.16 {q0}, [r0,:128]!

        vst1.16 {q0}, [r0,:128]!

        vst1.16 {q0}, [r0,:128]!

        vst1.16 {q0}, [r0,:128]!

        vst1.16 {q0}, [r0,:128]!
        bx lr

        @.endfunc

        .set Lpic_idx, 0
        .set Lpic_gp, 0
        .text
        .align 2
        .globl _ff_clear_blocks_neon

        .thumb_func _ff_clear_blocks_neon
_ff_clear_blocks_neon:
        vmov.i16 q0, #0

        vst1.16 {q0}, [r0,:128]!

        vst1.16 {q0}, [r0,:128]!

        vst1.16 {q0}, [r0,:128]!

        vst1.16 {q0}, [r0,:128]!

        vst1.16 {q0}, [r0,:128]!

        vst1.16 {q0}, [r0,:128]!

        vst1.16 {q0}, [r0,:128]!

        vst1.16 {q0}, [r0,:128]!

        vst1.16 {q0}, [r0,:128]!

        vst1.16 {q0}, [r0,:128]!

        vst1.16 {q0}, [r0,:128]!

        vst1.16 {q0}, [r0,:128]!

        vst1.16 {q0}, [r0,:128]!

        vst1.16 {q0}, [r0,:128]!

        vst1.16 {q0}, [r0,:128]!

        vst1.16 {q0}, [r0,:128]!

        vst1.16 {q0}, [r0,:128]!

        vst1.16 {q0}, [r0,:128]!

        vst1.16 {q0}, [r0,:128]!

        vst1.16 {q0}, [r0,:128]!

        vst1.16 {q0}, [r0,:128]!

        vst1.16 {q0}, [r0,:128]!

        vst1.16 {q0}, [r0,:128]!

        vst1.16 {q0}, [r0,:128]!

        vst1.16 {q0}, [r0,:128]!

        vst1.16 {q0}, [r0,:128]!

        vst1.16 {q0}, [r0,:128]!

        vst1.16 {q0}, [r0,:128]!

        vst1.16 {q0}, [r0,:128]!

        vst1.16 {q0}, [r0,:128]!

        vst1.16 {q0}, [r0,:128]!

        vst1.16 {q0}, [r0,:128]!

        vst1.16 {q0}, [r0,:128]!

        vst1.16 {q0}, [r0,:128]!

        vst1.16 {q0}, [r0,:128]!

        vst1.16 {q0}, [r0,:128]!

        vst1.16 {q0}, [r0,:128]!

        vst1.16 {q0}, [r0,:128]!

        vst1.16 {q0}, [r0,:128]!

        vst1.16 {q0}, [r0,:128]!

        vst1.16 {q0}, [r0,:128]!

        vst1.16 {q0}, [r0,:128]!

        vst1.16 {q0}, [r0,:128]!

        vst1.16 {q0}, [r0,:128]!

        vst1.16 {q0}, [r0,:128]!

        vst1.16 {q0}, [r0,:128]!

        vst1.16 {q0}, [r0,:128]!

        vst1.16 {q0}, [r0,:128]!
        bx lr

        @.endfunc
.text
.align 2
.thumb_func _ff_clear_blocks_neon
.thumb_func _ff_clear_block_neon

#endif
