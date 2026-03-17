#include "asm.S"

.fpu neon
.section .rodata
.align 4

.rept 16 .byte 0xff
.endr
mask_ff:
.rept 16 .byte 0
.endr

mask_ac4:
.short 0, -1, -1, -1,  0, -1, -1, -1
mask_ac8:
.short 0, -1, -1, -1, -1, -1, -1, -1

.text

.macro SAD_START_4 align:vararg
    vld1.32     {d1[]}, [r2 \align], r3
    vld1.32     {d0[]}, [r0,:32], r1
    vabdl.u8    q8,  d0,  d1
.endm

.macro SAD_4 align:vararg
    vld1.32     {d1[]}, [r2 \align], r3
    vld1.32     {d0[]}, [r0,:32], r1
    vabal.u8    q8,  d0,  d1
.endm

.macro SAD_START_8 align:vararg
    vld1.64     {d1}, [r2 \align], r3
    vld1.64     {d0}, [r0,:64], r1
    vabdl.u8    q8,  d0,  d1
.endm

.macro SAD_8 align:vararg
    vld1.64     {d1}, [r2 \align], r3
    vld1.64     {d0}, [r0,:64], r1
    vabal.u8    q8,  d0,  d1
.endm

.macro SAD_START_16 align:vararg
    vld1.64     {d2-d3}, [r2 \align], r3
    vld1.64     {d0-d1}, [r0,:128], r1
    vabdl.u8    q8,  d0,  d2
    vld1.64     {d6-d7}, [r2 \align], r3
    vabdl.u8    q9,  d1,  d3
    vld1.64     {d4-d5}, [r0,:128], r1
.endm

.macro SAD_16 align:vararg
    vabal.u8    q8,  d4,  d6
    vld1.64     {d2-d3}, [r2 \align], r3
    vabal.u8    q9,  d5,  d7
    vld1.64     {d0-d1}, [r0,:128], r1
    vabal.u8    q8,  d0,  d2
    vld1.64     {d6-d7}, [r2 \align], r3
    vabal.u8    q9,  d1,  d3
    vld1.64     {d4-d5}, [r0,:128], r1
.endm

.macro SAD_FUNC w, h, name, align:vararg
function x264_pixel_sad\name\()_\w\()x\h\()_neon
    SAD_START_\w \align

.if \w == 16
.rept \h / 2 - 1
    SAD_\w \align
.endr
.else
.rept \h - 1
    SAD_\w \align
.endr
.endif

.if \w > 8
    vabal.u8    q8,  d4,  d6
    vabal.u8    q9,  d5,  d7
    vadd.u16    q8,  q8,  q9
.endif
.if \w > 4
    vadd.u16    d16, d16, d17
.endif
    vpadd.u16   d0,  d16, d16
    vpaddl.u16  d0,  d0
    vmov.u32    r0,  d0[0]
    bx          lr
.endfunc
.endm

SAD_FUNC  4,  4
SAD_FUNC  4,  8
SAD_FUNC  8,  4
SAD_FUNC  8,  8
SAD_FUNC  8,  16
SAD_FUNC  16, 8
SAD_FUNC  16, 16

SAD_FUNC  4,  4,  _aligned, ,:32
SAD_FUNC  4,  8,  _aligned, ,:32
SAD_FUNC  8,  4,  _aligned, ,:64
SAD_FUNC  8,  8,  _aligned, ,:64
SAD_FUNC  8,  16, _aligned, ,:64
SAD_FUNC  16, 8,  _aligned, ,:128
SAD_FUNC  16, 16, _aligned, ,:128

// If dual issue is possible, use additional accumulators to avoid
// stalls from vadal's latency. This only matters for aligned.
.macro SAD_DUAL_START_8
    SAD_START_8 ,:64
    vld1.64     {d3}, [r2,:64], r3
    vld1.64     {d2}, [r0,:64], r1
    vabdl.u8    q9,  d2,  d3
.endm

.macro SAD_DUAL_8 align:vararg
    vld1.64     {d1}, [r2,:64], r3
    vld1.64     {d0}, [r0,:64], r1
    vabal.u8    q8,  d0,  d1
    vld1.64     {d3}, [r2,:64], r3
    vld1.64     {d2}, [r0,:64], r1
    vabal.u8    q9,  d2,  d3
.endm

.macro SAD_DUAL_START_16
    SAD_START_16 ,:128
    vabdl.u8    q10, d4,  d6
    vld1.64     {d2-d3}, [r2,:128], r3
    vabdl.u8    q11, d5,  d7
    vld1.64     {d0-d1}, [r0,:128], r1
.endm

.macro SAD_DUAL_16
    vabal.u8    q8,  d0,  d2
    vld1.64     {d6-d7}, [r2,:128], r3
    vabal.u8    q9,  d1,  d3
    vld1.64     {d4-d5}, [r0,:128], r1
    vabal.u8    q10, d4,  d6
    vld1.64     {d2-d3}, [r2,:128], r3
    vabal.u8    q11, d5,  d7
    vld1.64     {d0-d1}, [r0,:128], r1
.endm

.macro SAD_DUAL_END_16
    vabal.u8    q8,  d0,  d2
    vld1.64     {d6-d7}, [r2,:128], r3
    vabal.u8    q9,  d1,  d3
    vld1.64     {d4-d5}, [r0,:128], r1
    vabal.u8    q10, d4,  d6
    vabal.u8    q11, d5,  d7
.endm

.macro SAD_FUNC_DUAL w, h
function x264_pixel_sad_aligned_\w\()x\h\()_neon_dual
    SAD_DUAL_START_\w
.rept \h / 2 - \w / 8
    SAD_DUAL_\w
.endr

.if \w > 8
    SAD_DUAL_END_16
    vadd.u16    q8,  q8,  q9
    vadd.u16    q9,  q10, q11
.endif
.if \w > 4
    vadd.u16    q8,  q8,  q9
    vadd.u16    d16, d16, d17
.endif
    vpadd.u16   d0,  d16, d16
    vpaddl.u16  d0,  d0
    vmov.u32    r0,  d0[0]
    bx          lr
.endfunc
.endm

SAD_FUNC_DUAL  8,  4
SAD_FUNC_DUAL  8,  8
SAD_FUNC_DUAL  8,  16
SAD_FUNC_DUAL  16, 8
SAD_FUNC_DUAL  16, 16


.macro SAD_X_START_4 x
    vld1.32     {d0[]}, [r0,:32], lr
    vld1.32     {d1[]}, [r1], r6
    vabdl.u8    q8,  d1,  d0
    vld1.32     {d2[]}, [r2], r6
    vabdl.u8    q9,  d2,  d0
    vld1.32     {d3[]}, [r3], r6
    vabdl.u8    q10, d3,  d0
.if \x == 4
    vld1.32     {d4[]}, [r12], r6
    vabdl.u8    q11, d4,  d0
.endif
.endm

.macro SAD_X_4 x
    vld1.32     {d0[]}, [r0,:32], lr
    vld1.32     {d1[]}, [r1], r6
    vabal.u8    q8,  d1,  d0
    vld1.32     {d2[]}, [r2], r6
    vabal.u8    q9,  d2,  d0
    vld1.32     {d3[]}, [r3], r6
    vabal.u8    q10, d3,  d0
.if \x == 4
    vld1.32     {d4[]}, [r12], r6
    vabal.u8    q11, d4,  d0
.endif
.endm

.macro SAD_X_START_8 x
    vld1.64     {d0}, [r0,:64], lr
    vld1.64     {d1}, [r1], r6
    vabdl.u8    q8,  d1,  d0
    vld1.64     {d2}, [r2], r6
    vabdl.u8    q9,  d2,  d0
    vld1.64     {d3}, [r3], r6
    vabdl.u8    q10, d3,  d0
.if \x == 4
    vld1.64     {d4}, [r12], r6
    vabdl.u8    q11, d4,  d0
.endif
.endm

.macro SAD_X_8 x
    vld1.64     {d0}, [r0,:64], lr
    vld1.64     {d1}, [r1], r6
    vabal.u8    q8,  d1,  d0
    vld1.64     {d2}, [r2], r6
    vabal.u8    q9,  d2,  d0
    vld1.64     {d3}, [r3], r6
    vabal.u8    q10, d3,  d0
.if \x == 4
    vld1.64     {d4}, [r12], r6
    vabal.u8    q11, d4,  d0
.endif
.endm

.macro SAD_X_START_16 x
    vld1.64     {d0-d1}, [r0,:128], lr
    vld1.64     {d2-d3}, [r1], r6
    vabdl.u8    q8,  d2,  d0
    vabdl.u8    q12, d3,  d1
    vld1.64     {d4-d5}, [r2], r6
    vabdl.u8    q9,  d4,  d0
    vabdl.u8    q13, d5,  d1
    vld1.64     {d6-d7}, [r3], r6
    vabdl.u8    q10, d6,  d0
    vabdl.u8    q14, d7,  d1
.if \x == 4
    vld1.64     {d2-d3}, [r12], r6
    vabdl.u8    q11, d2,  d0
    vabdl.u8    q15, d3,  d1
.endif
.endm

.macro SAD_X_16 x
    vld1.64     {d0-d1}, [r0,:128], lr
    vld1.64     {d2-d3}, [r1], r6
    vabal.u8    q8,  d2,  d0
    vabal.u8    q12, d3,  d1
    vld1.64     {d4-d5}, [r2], r6
    vabal.u8    q9,  d4,  d0
    vabal.u8    q13, d5,  d1
    vld1.64     {d6-d7}, [r3], r6
    vabal.u8    q10, d6,  d0
    vabal.u8    q14, d7,  d1
.if \x == 4
    vld1.64     {d2-d3}, [r12], r6
    vabal.u8    q11, d2,  d0
    vabal.u8    q15, d3,  d1
.endif
.endm

.macro SAD_X_FUNC x, w, h
function x264_pixel_sad_x\x\()_\w\()x\h\()_neon
    push        {r6-r7,lr}
.if \x == 3
    ldrd        r6,  [sp, #12]
.else
    ldrd        r6,  [sp, #16]
    ldr         r12, [sp, #12]
.endif
    mov         lr,  #FENC_STRIDE

    SAD_X_START_\w \x
.rept \h - 1
    SAD_X_\w \x
.endr

// add up the sads
.if \w > 8
    vadd.u16    q8,  q8,  q12
    vadd.u16    q9,  q9,  q13
    vadd.u16    q10, q10, q14
.if \x == 4
    vadd.u16    q11, q11, q15
.endif
.endif
.if \w > 4
    vadd.u16    d16, d16, d17
    vadd.u16    d18, d18, d19
    vadd.u16    d20, d20, d21
.if \x == 4
    vadd.u16    d22, d22, d23
.endif
.endif
    vpadd.u16   d0,  d16, d18
    vpadd.u16   d1,  d20, d22
    vpaddl.u16  q0,  q0

.if \x == 3
    vst1.32     {d0},    [r7]!
    vst1.32     {d1[0]}, [r7,:32]
.else
    vst1.32     {d0-d1}, [r7]
.endif
    pop         {r6-r7,pc}
.endfunc
.endm

SAD_X_FUNC  3, 4,  4
SAD_X_FUNC  3, 4,  8
SAD_X_FUNC  3, 8,  4
SAD_X_FUNC  3, 8,  8
SAD_X_FUNC  3, 8,  16
SAD_X_FUNC  3, 16, 8
SAD_X_FUNC  3, 16, 16

SAD_X_FUNC  4, 4,  4
SAD_X_FUNC  4, 4,  8
SAD_X_FUNC  4, 8,  4
SAD_X_FUNC  4, 8,  8
SAD_X_FUNC  4, 8,  16
SAD_X_FUNC  4, 16, 8
SAD_X_FUNC  4, 16, 16


.macro SSD_START_4
    vld1.32     {d16[]}, [r0,:32], r1
    vld1.32     {d17[]}, [r2,:32], r3
    vsubl.u8    q2, d16, d17
    vld1.32     {d16[]}, [r0,:32], r1
    vmull.s16   q0, d4, d4
    vld1.32     {d17[]}, [r2,:32], r3
.endm

.macro SSD_4
    vsubl.u8    q2, d16, d17
    vld1.32     {d16[]}, [r0,:32], r1
    vmlal.s16   q0, d4, d4
    vld1.32     {d17[]}, [r2,:32], r3
.endm

.macro SSD_END_4
    vsubl.u8    q2, d16, d17
    vmlal.s16   q0, d4, d4
.endm

.macro SSD_START_8
    vld1.64     {d16}, [r0,:64], r1
    vld1.64     {d17}, [r2,:64], r3
    vsubl.u8    q2, d16, d17
    vld1.64     {d16}, [r0,:64], r1
    vmull.s16   q0, d4, d4
    vmlal.s16   q0, d5, d5
    vld1.64     {d17}, [r2,:64], r3
.endm

.macro SSD_8
    vsubl.u8    q2, d16, d17
    vld1.64     {d16}, [r0,:64], r1
    vmlal.s16   q0, d4, d4
    vmlal.s16   q0, d5, d5
    vld1.64     {d17}, [r2,:64], r3
.endm

.macro SSD_END_8
    vsubl.u8    q2, d16, d17
    vmlal.s16   q0, d4, d4
    vmlal.s16   q0, d5, d5
.endm

.macro SSD_START_16
    vld1.64     {d16-d17}, [r0,:128], r1
    vld1.64     {d18-d19}, [r2,:128], r3
    vsubl.u8    q2, d16, d18
    vsubl.u8    q3, d17, d19
    vld1.64     {d16-d17}, [r0,:128], r1
    vmull.s16   q0, d4, d4
    vmlal.s16   q0, d5, d5
    vld1.64     {d18-d19}, [r2,:128], r3
    vmlal.s16   q0, d6, d6
    vmlal.s16   q0, d7, d7
.endm

.macro SSD_16
    vsubl.u8    q2, d16, d18
    vsubl.u8    q3, d17, d19
    vld1.64     {d16-d17}, [r0,:128], r1
    vmlal.s16   q0, d4, d4
    vmlal.s16   q0, d5, d5
    vld1.64     {d18-d19}, [r2,:128], r3
    vmlal.s16   q0, d6, d6
    vmlal.s16   q0, d7, d7
.endm

.macro SSD_END_16
    vsubl.u8    q2, d16, d18
    vsubl.u8    q3, d17, d19
    vmlal.s16   q0, d4, d4
    vmlal.s16   q0, d5, d5
    vmlal.s16   q0, d6, d6
    vmlal.s16   q0, d7, d7
.endm

.macro SSD_FUNC w h
function x264_pixel_ssd_\w\()x\h\()_neon
    SSD_START_\w
.rept \h-2
    SSD_\w
.endr
    SSD_END_\w
    vadd.s32    d0, d0, d1
    vpadd.s32   d0, d0, d0
    vmov.32     r0, d0[0]
    bx          lr
.endfunc
.endm

SSD_FUNC   4, 4
SSD_FUNC   4, 8
SSD_FUNC   8, 4
SSD_FUNC   8, 8
SSD_FUNC   8, 16
SSD_FUNC  16, 8
SSD_FUNC  16, 16


.macro VAR_SQR_SUM qsqr_sum qsqr_last qsqr dsrc vpadal=vpadal.u16
    vmull.u8        \qsqr, \dsrc, \dsrc
    vaddw.u8        q0, q0, \dsrc
    \vpadal         \qsqr_sum, \qsqr_last
.endm

function x264_pixel_var_8x8_neon
    vld1.64         {d16}, [r0,:64], r1
    vmull.u8        q1,  d16, d16
    vmovl.u8        q0,  d16
    vld1.64         {d18}, [r0,:64], r1
    vmull.u8        q2,  d18, d18
    vaddw.u8        q0,  q0,  d18

    vld1.64         {d20}, [r0,:64], r1
    VAR_SQR_SUM     q1,  q1,   q3,  d20, vpaddl.u16
    vld1.64         {d22}, [r0,:64], r1
    VAR_SQR_SUM     q2,  q2,   q8,  d22, vpaddl.u16

    vld1.64         {d24}, [r0,:64], r1
    VAR_SQR_SUM     q1,  q3,   q9,  d24
    vld1.64         {d26}, [r0,:64], r1
    VAR_SQR_SUM     q2,  q8,   q10, d26
    vld1.64         {d24}, [r0,:64], r1
    VAR_SQR_SUM     q1,  q9,   q14, d24
    vld1.64         {d26}, [r0,:64], r1
    VAR_SQR_SUM     q2,  q10,  q15, d26
    b               x264_var_end
.endfunc

function x264_pixel_var_16x16_neon
    vld1.64         {d16-d17}, [r0,:128], r1
    vmull.u8        q12, d16, d16
    vmovl.u8        q0,  d16
    vmull.u8        q13, d17, d17
    vaddw.u8        q0,  q0,  d17

    vld1.64         {d18-d19}, [r0,:128], r1
    VAR_SQR_SUM     q1,  q12,  q14, d18, vpaddl.u16
    VAR_SQR_SUM     q2,  q13,  q15, d19, vpaddl.u16

    mov             ip,  #7
var16_loop:
    subs            ip,  ip,  #1
    vld1.64         {d16-d17}, [r0,:128], r1
    VAR_SQR_SUM     q1,  q14,  q12, d16
    VAR_SQR_SUM     q2,  q15,  q13, d17

    vld1.64         {d18-d19}, [r0,:128], r1
    VAR_SQR_SUM     q1,  q12,  q14, d18
    VAR_SQR_SUM     q2,  q13,  q15, d19
    bgt             var16_loop
.endfunc

function x264_var_end
    vpaddl.u16      q8,  q14
    vpaddl.u16      q9,  q15
    vadd.u32        q1,  q1,  q8
    vadd.u16        d0,  d0,  d1
    vadd.u32        q1,  q1,  q9
    vadd.u32        q1,  q1,  q2
    vpaddl.u16      d0,  d0
    vadd.u32        d2,  d2,  d3
    vpadd.u32       d0,  d0,  d2

    vmov            r0,  r1,  d0
    bx              lr
.endfunc

.macro DIFF_SUM diff da db lastdiff
    vld1.64         {\da}, [r0,:64], r1
    vld1.64         {\db}, [r2,:64], r3
.ifnb \lastdiff
    vadd.s16        q0,  q0,  \lastdiff
.endif
    vsubl.u8        \diff, \da, \db
.endm

.macro SQR_ACC acc d0 d1 vmlal=vmlal.s16
    \vmlal          \acc, \d0, \d0
    vmlal.s16       \acc, \d1, \d1
.endm

function x264_pixel_var2_8x8_neon
    DIFF_SUM        q0,  d0,  d1
    DIFF_SUM        q8,  d16, d17
    SQR_ACC         q1,  d0,  d1,  vmull.s16
    DIFF_SUM        q9,  d18, d19, q8
    SQR_ACC         q2,  d16, d17, vmull.s16
.rept 2
    DIFF_SUM        q8,  d16, d17, q9
    SQR_ACC         q1,  d18, d19
    DIFF_SUM        q9,  d18, d19, q8
    SQR_ACC         q2,  d16, d17
.endr
    DIFF_SUM        q8,  d16, d17, q9
    SQR_ACC         q1,  d18, d19
    vadd.s16        q0,  q0,  q8
    SQR_ACC         q2,  d16, d17

    ldr             ip,  [sp]
    vadd.s16        d0,  d0,  d1
    vadd.s32        q1,  q1,  q2
    vpaddl.s16      d0,  d0
    vadd.s32        d1,  d2,  d3
    vpadd.s32       d0,  d0,  d1

    vmov.32         r0,  r1,  d0
    vst1.32         {d0[1]}, [ip,:32]
    mul             r3,  r0,  r0
    sub             r0,  r1,  r3,  lsr #6
    bx              lr
.endfunc


.macro LOAD_DIFF_8x4 q0 q1 q2 q3
    vld1.32     {d1}, [r2], r3
    vld1.32     {d0}, [r0,:64], r1
    vsubl.u8    \q0, d0,  d1
    vld1.32     {d3}, [r2], r3
    vld1.32     {d2}, [r0,:64], r1
    vsubl.u8    \q1, d2,  d3
    vld1.32     {d5}, [r2], r3
    vld1.32     {d4}, [r0,:64], r1
    vsubl.u8    \q2, d4,  d5
    vld1.32     {d7}, [r2], r3
    vld1.32     {d6}, [r0,:64], r1
    vsubl.u8    \q3, d6,  d7
.endm

function x264_pixel_satd_4x4_neon
    vld1.32     {d1[]},  [r2], r3
    vld1.32     {d0[]},  [r0,:32], r1
    vld1.32     {d3[]},  [r2], r3
    vld1.32     {d2[]},  [r0,:32], r1
    vld1.32     {d1[1]}, [r2], r3
    vld1.32     {d0[1]}, [r0,:32], r1
    vld1.32     {d3[1]}, [r2], r3
    vld1.32     {d2[1]}, [r0,:32], r1
    vsubl.u8    q0,  d0,  d1
    vsubl.u8    q1,  d2,  d3

    SUMSUB_AB   q2, q3, q0, q1
    SUMSUB_ABCD d0, d2, d1, d3, d4, d5, d6, d7
    HADAMARD    1, sumsub, q2, q3, q0, q1
    HADAMARD    2, amax,   q0,,    q2, q3

    HORIZ_ADD   d0,  d0,  d1
    vmov.32     r0,  d0[0]
    bx          lr
.endfunc

function x264_pixel_satd_4x8_neon
    vld1.32     {d1[]},  [r2], r3
    vld1.32     {d0[]},  [r0,:32], r1
    vld1.32     {d3[]},  [r2], r3
    vld1.32     {d2[]},  [r0,:32], r1
    vld1.32     {d5[]},  [r2], r3
    vld1.32     {d4[]},  [r0,:32], r1
    vld1.32     {d7[]},  [r2], r3
    vld1.32     {d6[]},  [r0,:32], r1

    vld1.32     {d1[1]}, [r2], r3
    vld1.32     {d0[1]}, [r0,:32], r1
    vsubl.u8    q0,  d0,  d1
    vld1.32     {d3[1]}, [r2], r3
    vld1.32     {d2[1]}, [r0,:32], r1
    vsubl.u8    q1,  d2,  d3
    vld1.32     {d5[1]}, [r2], r3
    vld1.32     {d4[1]}, [r0,:32], r1
    vsubl.u8    q2,  d4,  d5
    vld1.32     {d7[1]}, [r2], r3
    SUMSUB_AB   q8,  q9,  q0,  q1
    vld1.32     {d6[1]}, [r0,:32], r1
    vsubl.u8    q3,  d6,  d7
    SUMSUB_AB   q10, q11, q2,  q3
    b           x264_satd_4x8_8x4_end_neon
.endfunc

function x264_pixel_satd_8x4_neon
    vld1.64     {d1}, [r2], r3
    vld1.64     {d0}, [r0,:64], r1
    vsubl.u8    q0,  d0,  d1
    vld1.64     {d3}, [r2], r3
    vld1.64     {d2}, [r0,:64], r1
    vsubl.u8    q1,  d2,  d3
    vld1.64     {d5}, [r2], r3
    vld1.64     {d4}, [r0,:64], r1
    vsubl.u8    q2,  d4,  d5
    vld1.64     {d7}, [r2], r3
    SUMSUB_AB   q8,  q9,  q0,  q1
    vld1.64     {d6}, [r0,:64], r1
    vsubl.u8    q3,  d6,  d7
    SUMSUB_AB   q10, q11, q2,  q3
.endfunc

function x264_satd_4x8_8x4_end_neon
    vadd.s16    q0,  q8,  q10
    vadd.s16    q1,  q9,  q11
    vsub.s16    q2,  q8,  q10
    vsub.s16    q3,  q9,  q11

    vtrn.16     q0,  q1
    vadd.s16    q8,  q0,  q1
    vtrn.16     q2,  q3
    vsub.s16    q9,  q0,  q1
    vadd.s16    q10, q2,  q3
    vsub.s16    q11, q2,  q3
    vtrn.32     q8,  q10
    vabs.s16    q8,  q8
    vtrn.32     q9,  q11
    vabs.s16    q10, q10
    vabs.s16    q9,  q9
    vabs.s16    q11, q11
    vmax.u16    q0,  q8,  q10
    vmax.u16    q1,  q9,  q11

    vadd.u16    q0,  q0,  q1
    HORIZ_ADD   d0,  d0,  d1
    vmov.32     r0,  d0[0]
    bx          lr
.endfunc

function x264_pixel_satd_8x8_neon
    mov         ip,  lr

    bl x264_satd_8x8_neon
    vadd.u16    q0,  q12, q13
    vadd.u16    q1,  q14, q15

    vadd.u16    q0,  q0,  q1
    HORIZ_ADD   d0,  d0,  d1
    mov         lr,  ip
    vmov.32     r0,  d0[0]
    bx          lr
.endfunc

function x264_pixel_satd_8x16_neon
    vpush       {d8-d11}
    mov         ip,  lr

    bl x264_satd_8x8_neon
    vadd.u16    q4,  q12, q13
    vadd.u16    q5,  q14, q15

    bl x264_satd_8x8_neon
    vadd.u16    q4,  q4,  q12
    vadd.u16    q5,  q5,  q13
    vadd.u16    q4,  q4,  q14
    vadd.u16    q5,  q5,  q15

    vadd.u16    q0,  q4,  q5
    HORIZ_ADD   d0,  d0,  d1
    vpop        {d8-d11}
    mov         lr,  ip
    vmov.32     r0,  d0[0]
    bx          lr
.endfunc

function x264_satd_8x8_neon
    LOAD_DIFF_8x4 q8,  q9,  q10, q11
    vld1.64     {d7}, [r2], r3
    SUMSUB_AB   q0,  q1,  q8,  q9
    vld1.64     {d6}, [r0,:64], r1
    vsubl.u8    q12, d6,  d7
    vld1.64     {d17}, [r2], r3
    SUMSUB_AB   q2,  q3,  q10, q11
    vld1.64     {d16}, [r0,:64], r1
    vsubl.u8    q13, d16, d17
    vld1.64     {d19}, [r2], r3
    SUMSUB_AB   q8,  q10, q0,  q2
    vld1.64     {d18}, [r0,:64], r1
    vsubl.u8    q14, d18, d19
    vld1.64     {d1}, [r2], r3
    SUMSUB_AB   q9,  q11, q1,  q3
    vld1.64     {d0}, [r0,:64], r1
    vsubl.u8    q15, d0,  d1
.endfunc

// one vertical hadamard pass and two horizontal
function x264_satd_8x4v_8x8h_neon
    SUMSUB_ABCD q0, q1, q2, q3, q12, q13, q14, q15
    vtrn.16     q8,  q9
    SUMSUB_AB   q12, q14, q0,  q2
    vtrn.16     q10, q11
    SUMSUB_AB   q13, q15, q1,  q3
    SUMSUB_AB   q0,  q1,  q8,  q9
    vtrn.16     q12, q13
    SUMSUB_AB   q2,  q3,  q10, q11
    vtrn.16     q14, q15
    SUMSUB_AB   q8,  q9,  q12, q13
    vtrn.32     q0,  q2
    SUMSUB_AB   q10, q11, q14, q15

    vtrn.32     q1,  q3
    ABS2        q0,  q2
    vtrn.32     q8,  q10
    ABS2        q1,  q3
    vtrn.32     q9,  q11
    ABS2        q8,  q10
    ABS2        q9,  q11
    vmax.s16    q12, q0,  q2
    vmax.s16    q13, q1,  q3
    vmax.s16    q14, q8,  q10
    vmax.s16    q15, q9,  q11
    bx          lr
.endfunc

function x264_pixel_satd_16x8_neon
    vpush       {d8-d11}
    mov         ip, lr

    bl          x264_satd_16x4_neon
    vadd.u16    q4,  q12, q13
    vadd.u16    q5,  q14, q15

    bl          x264_satd_16x4_neon
    vadd.u16    q4,  q4,  q12
    vadd.u16    q5,  q5,  q13
    vadd.u16    q4,  q4,  q14
    vadd.u16    q5,  q5,  q15

    vadd.u16    q0,  q4,  q5
    HORIZ_ADD   d0,  d0,  d1
    vpop        {d8-d11}
    mov         lr,  ip
    vmov.32     r0,  d0[0]
    bx          lr
.endfunc

function x264_pixel_satd_16x16_neon
    vpush       {d8-d11}
    mov         ip, lr

    bl          x264_satd_16x4_neon
    vadd.u16    q4,  q12, q13
    vadd.u16    q5,  q14, q15

    bl          x264_satd_16x4_neon
    vadd.u16    q4,  q4,  q12
    vadd.u16    q5,  q5,  q13
    vadd.u16    q4,  q4,  q14
    vadd.u16    q5,  q5,  q15

    bl          x264_satd_16x4_neon
    vadd.u16    q4,  q4,  q12
    vadd.u16    q5,  q5,  q13
    vadd.u16    q4,  q4,  q14
    vadd.u16    q5,  q5,  q15

    bl          x264_satd_16x4_neon
    vadd.u16    q4,  q4,  q12
    vadd.u16    q5,  q5,  q13
    vadd.u16    q4,  q4,  q14
    vadd.u16    q5,  q5,  q15

    vadd.u16    q0,  q4,  q5
    HORIZ_ADD   d0,  d0,  d1
    vpop        {d8-d11}
    mov         lr,  ip
    vmov.32     r0,  d0[0]
    bx          lr
.endfunc

function x264_satd_16x4_neon
    vld1.64     {d2-d3}, [r2], r3
    vld1.64     {d0-d1}, [r0,:128], r1
    vsubl.u8    q8,  d0,  d2
    vld1.64     {d6-d7}, [r2], r3
    vsubl.u8    q12, d1,  d3
    vld1.64     {d4-d5}, [r0,:128], r1
    vsubl.u8    q9,  d4,  d6
    vld1.64     {d2-d3}, [r2], r3
    vsubl.u8    q13, d5,  d7
    vld1.64     {d0-d1}, [r0,:128], r1
    vsubl.u8    q10, d0,  d2
    vld1.64     {d6-d7}, [r2], r3
    vsubl.u8    q14, d1,  d3
    vadd.s16    q0,  q8,  q9
    vld1.64     {d4-d5}, [r0,:128], r1
    vsub.s16    q1,  q8,  q9
    vsubl.u8    q11, d4,  d6
    vsubl.u8    q15, d5,  d7
    SUMSUB_AB   q2,  q3,  q10, q11
    SUMSUB_ABCD q8,  q10, q9,  q11, q0,  q2,  q1,  q3
    b           x264_satd_8x4v_8x8h_neon
.endfunc


function x264_pixel_sa8d_8x8_neon
    mov             ip,  lr
    bl              x264_sa8d_8x8_neon
    vadd.u16        q0,  q8,  q9
    HORIZ_ADD       d0,  d0,  d1
    mov             lr,  ip
    vmov.32         r0,  d0[0]
    add             r0,  r0,  #1
    lsr             r0,  r0,  #1
    bx              lr
.endfunc

function x264_pixel_sa8d_16x16_neon
    vpush           {d8-d11}
    mov             ip,  lr
    bl              x264_sa8d_8x8_neon
    vpaddl.u16      q4,  q8
    vpaddl.u16      q5,  q9
    bl              x264_sa8d_8x8_neon
    vpadal.u16      q4,  q8
    vpadal.u16      q5,  q9
    sub             r0,  r0,  r1,  lsl #4
    sub             r2,  r2,  r3,  lsl #4
    add             r0,  r0,  #8
    add             r2,  r2,  #8
    bl              x264_sa8d_8x8_neon
    vpadal.u16      q4,  q8
    vpadal.u16      q5,  q9
    bl              x264_sa8d_8x8_neon
    vpaddl.u16      q8,  q8
    vpaddl.u16      q9,  q9
    vadd.u32        q0,  q4,  q8
    vadd.u32        q1,  q5,  q9
    vadd.u32        q0,  q0,  q1
    vadd.u32        d0,  d0,  d1
    vpadd.u32       d0,  d0,  d0
    vpop            {d8-d11}
    mov             lr,  ip
    vmov.32         r0,  d0[0]
    add             r0,  r0,  #1
    lsr             r0,  r0,  #1
    bx              lr
.endfunc

.macro HADAMARD4_V r1, r2, r3, r4, t1, t2, t3, t4
    SUMSUB_ABCD \t1, \t2, \t3, \t4, \r1, \r2, \r3, \r4
    SUMSUB_ABCD \r1, \r3, \r2, \r4, \t1, \t3, \t2, \t4
.endm

function x264_sa8d_8x8_neon
    LOAD_DIFF_8x4   q8,  q9,  q10, q11
    vld1.64         {d7}, [r2], r3
    SUMSUB_AB       q0,  q1,  q8,  q9
    vld1.64         {d6}, [r0,:64], r1
    vsubl.u8        q12, d6,  d7
    vld1.64         {d17}, [r2], r3
    SUMSUB_AB       q2,  q3,  q10, q11
    vld1.64         {d16}, [r0,:64], r1
    vsubl.u8        q13, d16, d17
    vld1.64         {d19}, [r2], r3
    SUMSUB_AB       q8,  q10, q0,  q2
    vld1.64         {d18}, [r0,:64], r1
    vsubl.u8        q14, d18, d19
    vld1.64         {d1}, [r2], r3
    SUMSUB_AB       q9,  q11, q1,  q3
    vld1.64         {d0}, [r0,:64], r1
    vsubl.u8        q15, d0,  d1

    HADAMARD4_V     q12, q13, q14, q15,  q0,  q1,  q2,  q3
    SUMSUB_ABCD     q0,  q8,  q1,  q9,   q8,  q12, q9,  q13
    SUMSUB_AB       q2,  q10, q10, q14
    vtrn.16         q8,  q9
    SUMSUB_AB       q3,  q11, q11, q15
    vtrn.16         q0,  q1
    SUMSUB_AB       q12, q13, q8,  q9
    vtrn.16         q10, q11
    SUMSUB_AB       q8,  q9,  q0,  q1
    vtrn.16         q2,  q3
    SUMSUB_AB       q14, q15, q10, q11
    vadd.i16        q10, q2,  q3
    vtrn.32         q12, q14
    vsub.i16        q11, q2,  q3
    vtrn.32         q13, q15
    SUMSUB_AB       q0,  q2,  q12, q14
    vtrn.32         q8,  q10
    SUMSUB_AB       q1,  q3,  q13, q15
    vtrn.32         q9,  q11
    SUMSUB_AB       q12, q14, q8,  q10
    SUMSUB_AB       q13, q15, q9,  q11

    vswp            d1,  d24
    ABS2            q0,  q12
    vswp            d3,  d26
    ABS2            q1,  q13
    vswp            d5,  d28
    ABS2            q2,  q14
    vswp            d7,  d30
    ABS2            q3,  q15
    vmax.s16        q8,  q0,  q12
    vmax.s16        q9,  q1,  q13
    vmax.s16        q10, q2,  q14
    vmax.s16        q11, q3,  q15
    vadd.i16        q8,  q8,  q9
    vadd.i16        q9,  q10, q11
    bx              lr
.endfunc


