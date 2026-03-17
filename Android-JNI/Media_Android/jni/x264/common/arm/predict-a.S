#include "asm.S"

.fpu neon

.section .rodata
.align 4

p16weight: .short 1,2,3,4,5,6,7,8

.text

.macro ldcol.8  rd,  rs,  rt,  n=8,  hi=0
.if \n == 8 || \hi == 0
    vld1.8          {\rd[0]}, [\rs], \rt
    vld1.8          {\rd[1]}, [\rs], \rt
    vld1.8          {\rd[2]}, [\rs], \rt
    vld1.8          {\rd[3]}, [\rs], \rt
.endif
.if \n == 8 || \hi == 1
    vld1.8          {\rd[4]}, [\rs], \rt
    vld1.8          {\rd[5]}, [\rs], \rt
    vld1.8          {\rd[6]}, [\rs], \rt
    vld1.8          {\rd[7]}, [\rs], \rt
.endif
.endm

.macro add16x8  dq,  dl,  dh,  rl,  rh
    vaddl.u8        \dq, \rl, \rh
    vadd.u16        \dl, \dl, \dh
    vpadd.u16       \dl, \dl, \dl
    vpadd.u16       \dl, \dl, \dl
.endm


// return a1 = (a1+2*b1+c1+2)>>2  a2 = (a2+2*b2+c2+2)>>2
.macro PRED4x4_LOWPASS a1 b1 c1 a2 b2 c2 pb_1
    uhadd8  \a1, \a1, \c1
    uhadd8  \a2, \a2, \c2
    uhadd8  \c1, \a1, \b1
    uhadd8  \c2, \a2, \b2
    eor     \a1, \a1, \b1
    eor     \a2, \a2, \b2
    and     \a1, \a1, \pb_1
    and     \a2, \a2, \pb_1
    uadd8   \a1, \a1, \c1
    uadd8   \a2, \a2, \c2
.endm

function x264_predict_4x4_ddl_neon
    sub         r0, #FDEC_STRIDE
    mov         ip, #FDEC_STRIDE
    vld1.64     {d0}, [r0], ip
    vdup.8      d3, d0[7]
    vext.8      d1, d0, d0, #1
    vext.8      d2, d0, d3, #2
    vhadd.u8    d0, d0, d2
    vrhadd.u8   d0, d0, d1
    vst1.32     {d0[0]}, [r0,:32], ip
    vext.8      d1, d0, d0, #1
    vext.8      d2, d0, d0, #2
    vst1.32     {d1[0]}, [r0,:32], ip
    vext.8      d3, d0, d0, #3
    vst1.32     {d2[0]}, [r0,:32], ip
    vst1.32     {d3[0]}, [r0,:32], ip
    bx          lr
.endfunc

function x264_predict_8x8_h_neon
    add         r1, r1, #7
    mov         ip, #FDEC_STRIDE
    vld1.64     {d16}, [r1]
    vdup.8      d0, d16[7]
    vdup.8      d1, d16[6]
    vst1.64     {d0}, [r0,:64], ip
    vdup.8      d2, d16[5]
    vst1.64     {d1}, [r0,:64], ip
    vdup.8      d3, d16[4]
    vst1.64     {d2}, [r0,:64], ip
    vdup.8      d4, d16[3]
    vst1.64     {d3}, [r0,:64], ip
    vdup.8      d5, d16[2]
    vst1.64     {d4}, [r0,:64], ip
    vdup.8      d6, d16[1]
    vst1.64     {d5}, [r0,:64], ip
    vdup.8      d7, d16[0]
    vst1.64     {d6}, [r0,:64], ip
    vst1.64     {d7}, [r0,:64], ip
    bx          lr
.endfunc


function x264_predict_8x8c_dc_top_neon
    sub         r2,  r0,  #FDEC_STRIDE
    mov         r1,  #FDEC_STRIDE
    vld1.8      {d0}, [r2,:64]
    vpaddl.u8   d0,  d0
    vpadd.u16   d0,  d0,  d0
    vrshrn.u16  d0,  q0,  #2
    vdup.8      d1,  d0[1]
    vdup.8      d0,  d0[0]
    vtrn.32     d0,  d1
    b           pred8x8_dc_end
    .endfunc

function x264_predict_8x8c_dc_left_neon
    mov         r1,  #FDEC_STRIDE
    sub         r2,  r0,  #1
    ldcol.8     d0,  r2,  r1
    vpaddl.u8   d0,  d0
    vpadd.u16   d0,  d0,  d0
    vrshrn.u16  d0,  q0,  #2
    vdup.8      d1,  d0[1]
    vdup.8      d0,  d0[0]
    b           pred8x8_dc_end
.endfunc

function x264_predict_8x8c_dc_neon
    sub         r2,  r0,  #FDEC_STRIDE
    mov         r1,  #FDEC_STRIDE
    vld1.8      {d0}, [r2,:64]
    sub         r2,  r0,  #1
    ldcol.8     d1,  r2,  r1
    vtrn.32     d0,  d1
    vpaddl.u8   q0,  q0
    vpadd.u16   d0,  d0,  d1
    vpadd.u16   d1,  d0,  d0
    vrshrn.u16  d2,  q0,  #3
    vrshrn.u16  d3,  q0,  #2
    vdup.8      d0,  d2[4]
    vdup.8      d1,  d3[3]
    vdup.8      d4,  d3[2]
    vdup.8      d5,  d2[5]
    vtrn.32     q0,  q2
pred8x8_dc_end:
    add         r2,  r0,  r1,  lsl #2
.rept 4
    vst1.8      {d0}, [r0,:64], r1
    vst1.8      {d1}, [r2,:64], r1
.endr
    bx          lr
.endfunc

function x264_predict_8x8c_h_neon
    sub         r1, r0, #1
    mov         ip, #FDEC_STRIDE
.rept 4
    vld1.8      {d0[]}, [r1], ip
    vld1.8      {d2[]}, [r1], ip
    vst1.64     {d0}, [r0,:64], ip
    vst1.64     {d2}, [r0,:64], ip
.endr
    bx          lr
.endfunc

function x264_predict_8x8c_v_neon
    sub         r0, r0, #FDEC_STRIDE
    mov         ip, #FDEC_STRIDE
    vld1.64     {d0}, [r0,:64], ip
.rept 8
    vst1.64     {d0}, [r0,:64], ip
.endr
    bx          lr
.endfunc

function x264_predict_16x16_dc_top_neon
    sub         r2,  r0,  #FDEC_STRIDE
    mov         r1,  #FDEC_STRIDE
    vld1.8      {q0}, [r2,:128]
    add16x8     q0,  d0,  d1,  d0,  d1
    vrshrn.u16  d0,  q0,  #4
    vdup.8      q0,  d0[0]
    b           pred16x16_dc_end
.endfunc

function x264_predict_16x16_dc_left_neon
    mov         r1,  #FDEC_STRIDE
    sub         r2,  r0,  #1
    ldcol.8     d0,  r2,  r1
    ldcol.8     d1,  r2,  r1
    add16x8     q0,  d0,  d1,  d0,  d1
    vrshrn.u16  d0,  q0,  #4
    vdup.8      q0,  d0[0]
    b           pred16x16_dc_end
.endfunc

function x264_predict_16x16_dc_neon
    sub         r3, r0, #FDEC_STRIDE
    sub         r0, r0, #1
    vld1.64     {d0-d1}, [r3,:128]
    ldrb        ip, [r0], #FDEC_STRIDE
    vaddl.u8    q0, d0, d1
    ldrb        r1, [r0], #FDEC_STRIDE
    vadd.u16    d0, d0, d1
    vpadd.u16   d0, d0, d0
    vpadd.u16   d0, d0, d0
.rept 4
    ldrb        r2, [r0], #FDEC_STRIDE
    add         ip, ip, r1
    ldrb        r3, [r0], #FDEC_STRIDE
    add         ip, ip, r2
    ldrb        r1, [r0], #FDEC_STRIDE
    add         ip, ip, r3
.endr
    ldrb        r2, [r0], #FDEC_STRIDE
    add         ip, ip, r1
    ldrb        r3, [r0], #FDEC_STRIDE
    add         ip, ip, r2

    sub         r0, r0, #FDEC_STRIDE*16
    add         ip, ip, r3
    vdup.16     d1, ip
    vadd.u16    d0, d0, d1
    mov         r1, #FDEC_STRIDE
    add         r0, r0, #1
    vrshr.u16   d0, d0, #5
    vdup.8      q0, d0[0]
pred16x16_dc_end:
.rept 16
    vst1.64     {d0-d1}, [r0,:128], r1
.endr
    bx          lr
.endfunc

function x264_predict_16x16_h_neon
    sub         r1, r0, #1
    mov         ip, #FDEC_STRIDE
.rept 8
    vld1.8      {d0[]}, [r1], ip
    vmov        d1, d0
    vld1.8      {d2[]}, [r1], ip
    vmov        d3, d2
    vst1.64     {d0-d1}, [r0,:128], ip
    vst1.64     {d2-d3}, [r0,:128], ip
.endr
    bx          lr
.endfunc

function x264_predict_16x16_v_neon
    sub         r0, r0, #FDEC_STRIDE
    mov         ip, #FDEC_STRIDE
    vld1.64     {d0-d1}, [r0,:128], ip
.rept 16
    vst1.64     {d0-d1}, [r0,:128], ip
.endr
    bx          lr
.endfunc
