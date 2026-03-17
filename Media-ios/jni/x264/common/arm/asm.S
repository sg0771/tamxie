#define HAVE_MALLOC_H 1
#define HAVE_ARMV6 1
#define HAVE_ARMV6T2 1
#define ARCH_ARM 1
#define SYS_LINUX 0
#define HAVE_POSIXTHREAD 1
#define HAVE_THREAD 1
#define HAVE_LOG2F 1
#define HAVE_VECTOREXT 1
#define fseek fseeko
#define ftell ftello
#define HAVE_GPL 1
#define HAVE_INTERLACED 1
#define HAVE_ALTIVEC 0
#define HAVE_ALTIVEC_H 0
#define HAVE_MMX 0
#define HAVE_NEON 1
#define HAVE_BEOSTHREAD 0
#define HAVE_WIN32THREAD 0
#define HAVE_VISUALIZE 0
#define HAVE_SWSCALE 0
#define HAVE_LAVF 0
#define HAVE_FFMS 0
#define HAVE_GPAC 0
#define HAVE_GF_MALLOC 0
#define HAVE_AVS 0
#define X264_VERSION ""
#define X264_POINTVER "0.116.x"

#ifdef PREFIX
#   define EXTERN_ASM _
#else
#   define EXTERN_ASM
#endif

#ifdef __ELF__
#   define ELF
#else
#   define ELF @
#endif

        .macro require8, val=1
ELF     .eabi_attribute 24, \val
        .endm

        .macro preserve8, val=1
ELF     .eabi_attribute 25, \val
        .endm

        .macro function name
        .global EXTERN_ASM\name
EXTERN_ASM\name:
ELF     .hidden \name
ELF     .type   \name, %function
        .func   \name
\name:
        .endm

        .macro movrel rd, val
#if HAVE_ARMV6T2 && !defined(PIC)
        movw            \rd, #:lower16:\val
        movt            \rd, #:upper16:\val
#else
        ldr             \rd, =\val
#endif
        .endm

.macro movconst rd, val
#if HAVE_ARMV6T2
    movw        \rd, #:lower16:\val
.if \val >> 16
    movt        \rd, #:upper16:\val
.endif
#else
    ldr         \rd, =\val
#endif
.endm

#define FENC_STRIDE 16
#define FDEC_STRIDE 32

.macro HORIZ_ADD dest, a, b
.ifnb \b
    vadd.u16    \a, \a, \b
.endif
    vpaddl.u16  \a, \a
    vpaddl.u32  \dest, \a
.endm

.macro SUMSUB_AB sum, diff, a, b
    vadd.s16    \sum,  \a, \b
    vsub.s16    \diff, \a, \b
.endm

.macro SUMSUB_ABCD s1, d1, s2, d2, a, b, c, d
    SUMSUB_AB   \s1, \d1, \a, \b
    SUMSUB_AB   \s2, \d2, \c, \d
.endm

.macro ABS2 a b
    vabs.s16 \a, \a
    vabs.s16 \b, \b
.endm

// dist = distance in elements (0 for vertical pass, 1/2 for horizontal passes)
// op = sumsub/amax (sum and diff / maximum of absolutes)
// d1/2 = destination registers
// s1/2 = source registers
.macro HADAMARD dist, op, d1, d2, s1, s2
.if \dist == 1
    vtrn.16     \s1, \s2
.else
    vtrn.32     \s1, \s2
.endif
.ifc \op, sumsub
    SUMSUB_AB   \d1, \d2, \s1, \s2
.else
    vabs.s16    \s1, \s1
    vabs.s16    \s2, \s2
    vmax.s16    \d1, \s1, \s2
.endif
.endm

.macro TRANSPOSE8x8 r0 r1 r2 r3 r4 r5 r6 r7
    vtrn.32         \r0, \r4
    vtrn.32         \r1, \r5
    vtrn.32         \r2, \r6
    vtrn.32         \r3, \r7
    vtrn.16         \r0, \r2
    vtrn.16         \r1, \r3
    vtrn.16         \r4, \r6
    vtrn.16         \r5, \r7
    vtrn.8          \r0, \r1
    vtrn.8          \r2, \r3
    vtrn.8          \r4, \r5
    vtrn.8          \r6, \r7
.endm

.macro TRANSPOSE4x4 r0 r1 r2 r3
    vtrn.16         \r0, \r2
    vtrn.16         \r1, \r3
    vtrn.8          \r0, \r1
    vtrn.8          \r2, \r3
.endm

.macro TRANSPOSE4x4_16  d0 d1 d2 d3
    vtrn.32     \d0, \d2
    vtrn.32     \d1, \d3
    vtrn.16     \d0, \d1
    vtrn.16     \d2, \d3
.endm
