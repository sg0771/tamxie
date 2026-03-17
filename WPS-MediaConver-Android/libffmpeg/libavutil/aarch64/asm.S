/*
 * Copyright (c) 2008 Mans Rullgard <mans@mansr.com>
 *
 * This file is part of FFmpeg.
 */

#include "config.h"

#ifdef __ELF__
#   define ELF
#else
#   define ELF #
#endif


#define FUNC #

.macro  function name, export=0, align=2
    .macro endfunc
ELF     .size   \name, . - \name
FUNC    .endfunc
        .purgem endfunc
    .endm
        .text
        .align          \align
    .if \export
        .global EXTERN_ASM\name
ELF     .type   EXTERN_ASM\name, %function
FUNC    .func   EXTERN_ASM\name
EXTERN_ASM\name:
    .else
ELF     .type   \name, %function
FUNC    .func   \name
\name:
    .endif
.endm

.macro  const   name, align=3, relocate=0
    .macro endconst
ELF     .size   \name, . - \name
        .purgem endconst
    .endm
/* 关键修正点 1：确保 #if 结构完整 */
#if HAVE_SECTION_DATA_REL_RO
    .if \relocate
        .section        .data.rel.ro
    .else
        .section        .rodata
    .endif
#elif defined(_WIN32)
        .section        .rdata
#elif !defined(__MACH__)
        .section        .rodata
#else
        .const_data
#endif
/* 强制 8 字节对齐 */
    .balign 8
    /* 显式声明符号类型为对象，有助于链接器处理重定位 */
ELF .type \name, %object
\name:
.endm

.macro  movrel rd, val, offset=0
#if CONFIG_PIC && defined(__APPLE__)
    .if \offset < 0
        adrp            \rd, \val@PAGE
        add             \rd, \rd, \val@PAGEOFF
        sub             \rd, \rd, -(\offset)
    .else
        adrp            \rd, \val+(\offset)@PAGE
        add             \rd, \rd, \val+(\offset)@PAGEOFF
    .endif
#elif CONFIG_PIC && defined(_WIN32)
    .if \offset < 0
        adrp            \rd, \val
        add             \rd, \rd, :lo12:\val
        sub             \rd, \rd, -(\offset)
    .else
        adrp            \rd, \val+(\offset)
        add             \rd, \rd, :lo12:\val+(\offset)
    .endif
#elif CONFIG_PIC && defined(__ELF__)
    /* 关键修正点 3：重构 GOT 加载逻辑 */
    adrp            \rd, :got:\val
    /* 括号内严禁加 \offset，确保基地址对齐 */
    ldr             \rd, [\rd, :got_lo12:\val]
    .if \offset != 0
        /* 独立加偏移量，使用 AArch64 标准立即数语法 # */
        add             \rd, \rd, #\offset
    .endif
#elif CONFIG_PIC
    adrp            \rd, \val+(\offset)
    add             \rd, \rd, :lo12:\val+(\offset)
#else
    ldr             \rd, =\val+\offset
#endif
.endm

#define GLUE(a, b) a ## b
#define JOIN(a, b) GLUE(a, b)
#define X(s) JOIN(EXTERN_ASM, s)