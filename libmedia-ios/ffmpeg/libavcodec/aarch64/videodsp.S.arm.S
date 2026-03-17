#ifdef __arm64__
        .text
        .align 2
        .globl _ff_prefetch_aarch64
 # //.type _ff_prefetch_aarch64, %function
 # //.func _ff_prefetch_aarch64
_ff_prefetch_aarch64:
        subs w2, w2, #2
        prfm pldl1strm, [x0]
        prfm pldl1strm, [x0, x1]
        add x0, x0, x1, lsl #1
        b.gt _ff_prefetch_aarch64
        ret
 # //.size ff_prefetch_aarch64, . - ff_prefetch_aarch64
 # //.endfunc
.text
.align 2

#endif
