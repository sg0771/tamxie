#ifdef __arm__

        @.arch armv7-a
        @.fpu neon        .syntax unified
        .thumb

        .set Lpic_idx, 0
        .set Lpic_gp, 0
        .text
        .align 5
        .globl _ff_put_pixels16_arm

        .thumb_func _ff_put_pixels16_arm
_ff_put_pixels16_arm:
        
        
        pld [r1]
        push {r4-r11, lr}
        ands r5, r1, #3
        bic r1, r1, #3
        beq 1f
        subs r5, r5, #1
        beq 2f
        subs r5, r5, #1
        beq 3f
        b 4f
1:
        ldm r1, {r4-r7}
        add r1, r1, r2
        stm r0, {r4-r7}
        pld [r1]
        subs r3, r3, #1
        add r0, r0, r2
        bne 1b
        pop {r4-r11, pc}
        .align 5
2:
        ldm r1, {r4-r8}
        add r1, r1, r2
        mov r9, r4, lsr #(1 * 8)
        mov r10, r5, lsr #(1 * 8)
        mov r11, r6, lsr #(1 * 8)
        mov r12, r7, lsr #(1 * 8)
        orr r9, r9, r5, lsl #(32 - 1 * 8)
        orr r10, r10, r6, lsl #(32 - 1 * 8)
        orr r11, r11, r7, lsl #(32 - 1 * 8)
        orr r12, r12, r8, lsl #(32 - 1 * 8)
        pld [r1]
        subs r3, r3, #1
        stm r0, {r9-r12}
        add r0, r0, r2
        bne 2b
        pop {r4-r11, pc}
        .align 5
3:
        ldm r1, {r4-r8}
        add r1, r1, r2
        mov r9, r4, lsr #(2 * 8)
        mov r10, r5, lsr #(2 * 8)
        mov r11, r6, lsr #(2 * 8)
        mov r12, r7, lsr #(2 * 8)
        orr r9, r9, r5, lsl #(32 - 2 * 8)
        orr r10, r10, r6, lsl #(32 - 2 * 8)
        orr r11, r11, r7, lsl #(32 - 2 * 8)
        orr r12, r12, r8, lsl #(32 - 2 * 8)
        pld [r1]
        subs r3, r3, #1
        stm r0, {r9-r12}
        add r0, r0, r2
        bne 3b
        pop {r4-r11, pc}
        .align 5
4:
        ldm r1, {r4-r8}
        add r1, r1, r2
        mov r9, r4, lsr #(3 * 8)
        mov r10, r5, lsr #(3 * 8)
        mov r11, r6, lsr #(3 * 8)
        mov r12, r7, lsr #(3 * 8)
        orr r9, r9, r5, lsl #(32 - 3 * 8)
        orr r10, r10, r6, lsl #(32 - 3 * 8)
        orr r11, r11, r7, lsl #(32 - 3 * 8)
        orr r12, r12, r8, lsl #(32 - 3 * 8)
        pld [r1]
        subs r3, r3, #1
        stm r0, {r9-r12}
        add r0, r0, r2
        bne 4b
        pop {r4-r11,pc}

        @.endfunc


        .set Lpic_idx, 0
        .set Lpic_gp, 0
        .text
        .align 5
        .globl _ff_put_pixels8_arm

        .thumb_func _ff_put_pixels8_arm
_ff_put_pixels8_arm:
        
        
        pld [r1]
        push {r4-r5,lr}
        ands r5, r1, #3
        bic r1, r1, #3
        beq 1f
        subs r5, r5, #1
        beq 2f
        subs r5, r5, #1
        beq 3f
        b 4f
1:
        ldm r1, {r4-r5}
        add r1, r1, r2
        subs r3, r3, #1
        pld [r1]
        stm r0, {r4-r5}
        add r0, r0, r2
        bne 1b
        pop {r4-r5,pc}
        .align 5
2:
        ldm r1, {r4-r5, r12}
        add r1, r1, r2
        mov r4, r4, lsr #(1 * 8)
        orr r4, r4, r5, lsl #(32 - 1 * 8)
        mov r5, r5, lsr #(1 * 8)
        orr r5, r5, r12, lsl #(32 - 1 * 8)
        pld [r1]
        subs r3, r3, #1
        stm r0, {r4-r5}
        add r0, r0, r2
        bne 2b
        pop {r4-r5,pc}
        .align 5
3:
        ldm r1, {r4-r5, r12}
        add r1, r1, r2
        mov r4, r4, lsr #(2 * 8)
        orr r4, r4, r5, lsl #(32 - 2 * 8)
        mov r5, r5, lsr #(2 * 8)
        orr r5, r5, r12, lsl #(32 - 2 * 8)
        pld [r1]
        subs r3, r3, #1
        stm r0, {r4-r5}
        add r0, r0, r2
        bne 3b
        pop {r4-r5,pc}
        .align 5
4:
        ldm r1, {r4-r5, r12}
        add r1, r1, r2
        mov r4, r4, lsr #(3 * 8)
        orr r4, r4, r5, lsl #(32 - 3 * 8)
        mov r5, r5, lsr #(3 * 8)
        orr r5, r5, r12, lsl #(32 - 3 * 8)
        pld [r1]
        subs r3, r3, #1
        stm r0, {r4-r5}
        add r0, r0, r2
        bne 4b
        pop {r4-r5,pc}

        @.endfunc


        .set Lpic_idx, 0
        .set Lpic_gp, 0
        .text
        .align 5
        .globl _ff_put_pixels8_x2_arm

        .thumb_func _ff_put_pixels8_x2_arm
_ff_put_pixels8_x2_arm:
        
        
        pld [r1]
        push {r4-r10,lr}
         ldr r12, Literal_0
        ands r5, r1, #3
        bic r1, r1, #3
        beq 1f
        subs r5, r5, #1
        beq 2f
        subs r5, r5, #1
        beq 3f
        b 4f
1:
        ldm r1, {r4-r5, r10}
        add r1, r1, r2
        mov r6, r4, lsr #(1 * 8)
        mov r7, r5, lsr #(1 * 8)
        orr r6, r6, r5, lsl #(32 - (1 * 8))
        orr r7, r7, r10, lsl #(32 - (1 * 8))
        pld [r1]
        
        
        
        eor r8, r4, r6
        eor r9, r5, r7
        orr r4, r4, r6
        orr r5, r5, r7
        and r8, r8, r12
        and r9, r9, r12
        sub r8, r4, r8, lsr #1
        sub r9, r5, r9, lsr #1
        subs r3, r3, #1
        stm r0, {r8-r9}
        add r0, r0, r2
        bne 1b
        pop {r4-r10,pc}
        .align 5
2:
        ldm r1, {r4-r5, r10}
        add r1, r1, r2
        mov r6, r4, lsr #(1 * 8)
        mov r7, r5, lsr #(1 * 8)
        orr r6, r6, r5, lsl #(32 - (1 * 8))
        orr r7, r7, r10, lsl #(32 - (1 * 8))
        mov r8, r4, lsr #(2 * 8)
        mov r9, r5, lsr #(2 * 8)
        orr r8, r8, r5, lsl #(32 - (2 * 8))
        orr r9, r9, r10, lsl #(32 - (2 * 8))
        pld [r1]
        
        
        
        eor r4, r6, r8
        eor r5, r7, r9
        orr r6, r6, r8
        orr r7, r7, r9
        and r4, r4, r12
        and r5, r5, r12
        sub r4, r6, r4, lsr #1
        sub r5, r7, r5, lsr #1
        subs r3, r3, #1
        stm r0, {r4-r5}
        add r0, r0, r2
        bne 2b
        pop {r4-r10,pc}
        .align 5
3:
        ldm r1, {r4-r5, r10}
        add r1, r1, r2
        mov r6, r4, lsr #(2 * 8)
        mov r7, r5, lsr #(2 * 8)
        orr r6, r6, r5, lsl #(32 - (2 * 8))
        orr r7, r7, r10, lsl #(32 - (2 * 8))
        mov r8, r4, lsr #(3 * 8)
        mov r9, r5, lsr #(3 * 8)
        orr r8, r8, r5, lsl #(32 - (3 * 8))
        orr r9, r9, r10, lsl #(32 - (3 * 8))
        pld [r1]
        
        
        
        eor r4, r6, r8
        eor r5, r7, r9
        orr r6, r6, r8
        orr r7, r7, r9
        and r4, r4, r12
        and r5, r5, r12
        sub r4, r6, r4, lsr #1
        sub r5, r7, r5, lsr #1
        subs r3, r3, #1
        stm r0, {r4-r5}
        add r0, r0, r2
        bne 3b
        pop {r4-r10,pc}
        .align 5
4:
        ldm r1, {r4-r5, r10}
        add r1, r1, r2
        mov r6, r4, lsr #(3 * 8)
        mov r7, r5, lsr #(3 * 8)
        orr r6, r6, r5, lsl #(32 - (3 * 8))
        orr r7, r7, r10, lsl #(32 - (3 * 8))
        pld [r1]
        
        
        
        eor r8, r6, r5
        eor r9, r7, r10
        orr r6, r6, r5
        orr r7, r7, r10
        and r8, r8, r12
        and r9, r9, r12
        sub r8, r6, r8, lsr #1
        sub r9, r7, r9, lsr #1
        subs r3, r3, #1
        stm r0, {r8-r9}
        add r0, r0, r2
        bne 4b
        pop {r4-r10,pc}

        @.endfunc

        .set Lpic_idx, 0
        .set Lpic_gp, 0
        .text
        .align 5
        .globl _ff_put_no_rnd_pixels8_x2_arm

        .thumb_func _ff_put_no_rnd_pixels8_x2_arm
_ff_put_no_rnd_pixels8_x2_arm:
        
        
        pld [r1]
        push {r4-r10,lr}
         ldr r12, Literal_0
        ands r5, r1, #3
        bic r1, r1, #3
        beq 1f
        subs r5, r5, #1
        beq 2f
        subs r5, r5, #1
        beq 3f
        b 4f
1:
        ldm r1, {r4-r5, r10}
        add r1, r1, r2
        mov r6, r4, lsr #(1 * 8)
        mov r7, r5, lsr #(1 * 8)
        orr r6, r6, r5, lsl #(32 - (1 * 8))
        orr r7, r7, r10, lsl #(32 - (1 * 8))
        pld [r1]
        
        
        
        eor r8, r4, r6
        eor r9, r5, r7
        and r4, r4, r6
        and r5, r5, r7
        and r8, r8, r12
        and r9, r9, r12
        add r8, r4, r8, lsr #1
        add r9, r5, r9, lsr #1
        subs r3, r3, #1
        stm r0, {r8-r9}
        add r0, r0, r2
        bne 1b
        pop {r4-r10,pc}
        .align 5
2:
        ldm r1, {r4-r5, r10}
        add r1, r1, r2
        mov r6, r4, lsr #(1 * 8)
        mov r7, r5, lsr #(1 * 8)
        orr r6, r6, r5, lsl #(32 - (1 * 8))
        orr r7, r7, r10, lsl #(32 - (1 * 8))
        mov r8, r4, lsr #(2 * 8)
        mov r9, r5, lsr #(2 * 8)
        orr r8, r8, r5, lsl #(32 - (2 * 8))
        orr r9, r9, r10, lsl #(32 - (2 * 8))
        pld [r1]
        
        
        
        eor r4, r6, r8
        eor r5, r7, r9
        and r6, r6, r8
        and r7, r7, r9
        and r4, r4, r12
        and r5, r5, r12
        add r4, r6, r4, lsr #1
        add r5, r7, r5, lsr #1
        subs r3, r3, #1
        stm r0, {r4-r5}
        add r0, r0, r2
        bne 2b
        pop {r4-r10,pc}
        .align 5
3:
        ldm r1, {r4-r5, r10}
        add r1, r1, r2
        mov r6, r4, lsr #(2 * 8)
        mov r7, r5, lsr #(2 * 8)
        orr r6, r6, r5, lsl #(32 - (2 * 8))
        orr r7, r7, r10, lsl #(32 - (2 * 8))
        mov r8, r4, lsr #(3 * 8)
        mov r9, r5, lsr #(3 * 8)
        orr r8, r8, r5, lsl #(32 - (3 * 8))
        orr r9, r9, r10, lsl #(32 - (3 * 8))
        pld [r1]
        
        
        
        eor r4, r6, r8
        eor r5, r7, r9
        and r6, r6, r8
        and r7, r7, r9
        and r4, r4, r12
        and r5, r5, r12
        add r4, r6, r4, lsr #1
        add r5, r7, r5, lsr #1
        subs r3, r3, #1
        stm r0, {r4-r5}
        add r0, r0, r2
        bne 3b
        pop {r4-r10,pc}
        .align 5
4:
        ldm r1, {r4-r5, r10}
        add r1, r1, r2
        mov r6, r4, lsr #(3 * 8)
        mov r7, r5, lsr #(3 * 8)
        orr r6, r6, r5, lsl #(32 - (3 * 8))
        orr r7, r7, r10, lsl #(32 - (3 * 8))
        pld [r1]
        
        
        
        eor r8, r6, r5
        eor r9, r7, r10
        and r6, r6, r5
        and r7, r7, r10
        and r8, r8, r12
        and r9, r9, r12
        add r8, r6, r8, lsr #1
        add r9, r7, r9, lsr #1
        subs r3, r3, #1
        stm r0, {r8-r9}
        add r0, r0, r2
        bne 4b
        pop {r4-r10,pc}

        @.endfunc        .set Lpic_idx, 0
        .set Lpic_gp, 0
        .text
        .align 5
        .globl _ff_put_pixels8_y2_arm

        .thumb_func _ff_put_pixels8_y2_arm
_ff_put_pixels8_y2_arm:
        
        
        pld [r1]
        push {r4-r11,lr}
        mov r3, r3, lsr #1
         ldr r12, Literal_0
        ands r5, r1, #3
        bic r1, r1, #3
        beq 1f
        subs r5, r5, #1
        beq 2f
        subs r5, r5, #1
        beq 3f
        b 4f
1:
        ldm r1, {r4-r5}
        add r1, r1, r2
6: ldm r1, {r6-r7}
        add r1, r1, r2
        pld [r1]
        
        
        
        eor r8, r4, r6
        eor r9, r5, r7
        orr r4, r4, r6
        orr r5, r5, r7
        and r8, r8, r12
        and r9, r9, r12
        sub r8, r4, r8, lsr #1
        sub r9, r5, r9, lsr #1
        ldm r1, {r4-r5}
        add r1, r1, r2
        stm r0, {r8-r9}
        add r0, r0, r2
        pld [r1]
        
        
        
        eor r8, r6, r4
        eor r9, r7, r5
        orr r6, r6, r4
        orr r7, r7, r5
        and r8, r8, r12
        and r9, r9, r12
        sub r8, r6, r8, lsr #1
        sub r9, r7, r9, lsr #1
        subs r3, r3, #1
        stm r0, {r8-r9}
        add r0, r0, r2
        bne 6b
        pop {r4-r11,pc}
        .align 5
2:
        ldm r1, {r4-r6}
        add r1, r1, r2
        pld [r1]
        mov r4, r4, lsr #(1 * 8)
        orr r4, r4, r5, lsl #(32 - 1 * 8)
        mov r5, r5, lsr #(1 * 8)
        orr r5, r5, r6, lsl #(32 - 1 * 8)
6: ldm r1, {r7-r9}
        add r1, r1, r2
        pld [r1]
        mov r7, r7, lsr #(1 * 8)
        orr r7, r7, r8, lsl #(32 - 1 * 8)
        mov r8, r8, lsr #(1 * 8)
        orr r8, r8, r9, lsl #(32 - 1 * 8)
        
        
        
        eor r10, r4, r7
        eor r11, r5, r8
        orr r4, r4, r7
        orr r5, r5, r8
        and r10, r10, r12
        and r11, r11, r12
        sub r10, r4, r10, lsr #1
        sub r11, r5, r11, lsr #1
        stm r0, {r10-r11}
        add r0, r0, r2
        ldm r1, {r4-r6}
        add r1, r1, r2
        pld [r1]
        mov r4, r4, lsr #(1 * 8)
        orr r4, r4, r5, lsl #(32 - 1 * 8)
        mov r5, r5, lsr #(1 * 8)
        orr r5, r5, r6, lsl #(32 - 1 * 8)
        subs r3, r3, #1
        
        
        
        eor r10, r7, r4
        eor r11, r8, r5
        orr r7, r7, r4
        orr r8, r8, r5
        and r10, r10, r12
        and r11, r11, r12
        sub r10, r7, r10, lsr #1
        sub r11, r8, r11, lsr #1
        stm r0, {r10-r11}
        add r0, r0, r2
        bne 6b
        pop {r4-r11,pc}
        .align 5
3:
        ldm r1, {r4-r6}
        add r1, r1, r2
        pld [r1]
        mov r4, r4, lsr #(2 * 8)
        orr r4, r4, r5, lsl #(32 - 2 * 8)
        mov r5, r5, lsr #(2 * 8)
        orr r5, r5, r6, lsl #(32 - 2 * 8)
6: ldm r1, {r7-r9}
        add r1, r1, r2
        pld [r1]
        mov r7, r7, lsr #(2 * 8)
        orr r7, r7, r8, lsl #(32 - 2 * 8)
        mov r8, r8, lsr #(2 * 8)
        orr r8, r8, r9, lsl #(32 - 2 * 8)
        
        
        
        eor r10, r4, r7
        eor r11, r5, r8
        orr r4, r4, r7
        orr r5, r5, r8
        and r10, r10, r12
        and r11, r11, r12
        sub r10, r4, r10, lsr #1
        sub r11, r5, r11, lsr #1
        stm r0, {r10-r11}
        add r0, r0, r2
        ldm r1, {r4-r6}
        add r1, r1, r2
        pld [r1]
        mov r4, r4, lsr #(2 * 8)
        orr r4, r4, r5, lsl #(32 - 2 * 8)
        mov r5, r5, lsr #(2 * 8)
        orr r5, r5, r6, lsl #(32 - 2 * 8)
        subs r3, r3, #1
        
        
        
        eor r10, r7, r4
        eor r11, r8, r5
        orr r7, r7, r4
        orr r8, r8, r5
        and r10, r10, r12
        and r11, r11, r12
        sub r10, r7, r10, lsr #1
        sub r11, r8, r11, lsr #1
        stm r0, {r10-r11}
        add r0, r0, r2
        bne 6b
        pop {r4-r11,pc}
        .align 5
4:
        ldm r1, {r4-r6}
        add r1, r1, r2
        pld [r1]
        mov r4, r4, lsr #(3 * 8)
        orr r4, r4, r5, lsl #(32 - 3 * 8)
        mov r5, r5, lsr #(3 * 8)
        orr r5, r5, r6, lsl #(32 - 3 * 8)
6: ldm r1, {r7-r9}
        add r1, r1, r2
        pld [r1]
        mov r7, r7, lsr #(3 * 8)
        orr r7, r7, r8, lsl #(32 - 3 * 8)
        mov r8, r8, lsr #(3 * 8)
        orr r8, r8, r9, lsl #(32 - 3 * 8)
        
        
        
        eor r10, r4, r7
        eor r11, r5, r8
        orr r4, r4, r7
        orr r5, r5, r8
        and r10, r10, r12
        and r11, r11, r12
        sub r10, r4, r10, lsr #1
        sub r11, r5, r11, lsr #1
        stm r0, {r10-r11}
        add r0, r0, r2
        ldm r1, {r4-r6}
        add r1, r1, r2
        pld [r1]
        mov r4, r4, lsr #(3 * 8)
        orr r4, r4, r5, lsl #(32 - 3 * 8)
        mov r5, r5, lsr #(3 * 8)
        orr r5, r5, r6, lsl #(32 - 3 * 8)
        subs r3, r3, #1
        
        
        
        eor r10, r7, r4
        eor r11, r8, r5
        orr r7, r7, r4
        orr r8, r8, r5
        and r10, r10, r12
        and r11, r11, r12
        sub r10, r7, r10, lsr #1
        sub r11, r8, r11, lsr #1
        stm r0, {r10-r11}
        add r0, r0, r2
        bne 6b
        pop {r4-r11,pc}

        @.endfunc

        .set Lpic_idx, 0
        .set Lpic_gp, 0
        .text
        .align 5
        .globl _ff_put_no_rnd_pixels8_y2_arm

        .thumb_func _ff_put_no_rnd_pixels8_y2_arm
_ff_put_no_rnd_pixels8_y2_arm:
        
        
        pld [r1]
        push {r4-r11,lr}
        mov r3, r3, lsr #1
         ldr r12, Literal_0
        ands r5, r1, #3
        bic r1, r1, #3
        beq 1f
        subs r5, r5, #1
        beq 2f
        subs r5, r5, #1
        beq 3f
        b 4f
1:
        ldm r1, {r4-r5}
        add r1, r1, r2
6: ldm r1, {r6-r7}
        add r1, r1, r2
        pld [r1]
        
        
        
        eor r8, r4, r6
        eor r9, r5, r7
        and r4, r4, r6
        and r5, r5, r7
        and r8, r8, r12
        and r9, r9, r12
        add r8, r4, r8, lsr #1
        add r9, r5, r9, lsr #1
        ldm r1, {r4-r5}
        add r1, r1, r2
        stm r0, {r8-r9}
        add r0, r0, r2
        pld [r1]
        
        
        
        eor r8, r6, r4
        eor r9, r7, r5
        and r6, r6, r4
        and r7, r7, r5
        and r8, r8, r12
        and r9, r9, r12
        add r8, r6, r8, lsr #1
        add r9, r7, r9, lsr #1
        subs r3, r3, #1
        stm r0, {r8-r9}
        add r0, r0, r2
        bne 6b
        pop {r4-r11,pc}
        .align 5
2:
        ldm r1, {r4-r6}
        add r1, r1, r2
        pld [r1]
        mov r4, r4, lsr #(1 * 8)
        orr r4, r4, r5, lsl #(32 - 1 * 8)
        mov r5, r5, lsr #(1 * 8)
        orr r5, r5, r6, lsl #(32 - 1 * 8)
6: ldm r1, {r7-r9}
        add r1, r1, r2
        pld [r1]
        mov r7, r7, lsr #(1 * 8)
        orr r7, r7, r8, lsl #(32 - 1 * 8)
        mov r8, r8, lsr #(1 * 8)
        orr r8, r8, r9, lsl #(32 - 1 * 8)
        
        
        
        eor r10, r4, r7
        eor r11, r5, r8
        and r4, r4, r7
        and r5, r5, r8
        and r10, r10, r12
        and r11, r11, r12
        add r10, r4, r10, lsr #1
        add r11, r5, r11, lsr #1
        stm r0, {r10-r11}
        add r0, r0, r2
        ldm r1, {r4-r6}
        add r1, r1, r2
        pld [r1]
        mov r4, r4, lsr #(1 * 8)
        orr r4, r4, r5, lsl #(32 - 1 * 8)
        mov r5, r5, lsr #(1 * 8)
        orr r5, r5, r6, lsl #(32 - 1 * 8)
        subs r3, r3, #1
        
        
        
        eor r10, r7, r4
        eor r11, r8, r5
        and r7, r7, r4
        and r8, r8, r5
        and r10, r10, r12
        and r11, r11, r12
        add r10, r7, r10, lsr #1
        add r11, r8, r11, lsr #1
        stm r0, {r10-r11}
        add r0, r0, r2
        bne 6b
        pop {r4-r11,pc}
        .align 5
3:
        ldm r1, {r4-r6}
        add r1, r1, r2
        pld [r1]
        mov r4, r4, lsr #(2 * 8)
        orr r4, r4, r5, lsl #(32 - 2 * 8)
        mov r5, r5, lsr #(2 * 8)
        orr r5, r5, r6, lsl #(32 - 2 * 8)
6: ldm r1, {r7-r9}
        add r1, r1, r2
        pld [r1]
        mov r7, r7, lsr #(2 * 8)
        orr r7, r7, r8, lsl #(32 - 2 * 8)
        mov r8, r8, lsr #(2 * 8)
        orr r8, r8, r9, lsl #(32 - 2 * 8)
        
        
        
        eor r10, r4, r7
        eor r11, r5, r8
        and r4, r4, r7
        and r5, r5, r8
        and r10, r10, r12
        and r11, r11, r12
        add r10, r4, r10, lsr #1
        add r11, r5, r11, lsr #1
        stm r0, {r10-r11}
        add r0, r0, r2
        ldm r1, {r4-r6}
        add r1, r1, r2
        pld [r1]
        mov r4, r4, lsr #(2 * 8)
        orr r4, r4, r5, lsl #(32 - 2 * 8)
        mov r5, r5, lsr #(2 * 8)
        orr r5, r5, r6, lsl #(32 - 2 * 8)
        subs r3, r3, #1
        
        
        
        eor r10, r7, r4
        eor r11, r8, r5
        and r7, r7, r4
        and r8, r8, r5
        and r10, r10, r12
        and r11, r11, r12
        add r10, r7, r10, lsr #1
        add r11, r8, r11, lsr #1
        stm r0, {r10-r11}
        add r0, r0, r2
        bne 6b
        pop {r4-r11,pc}
        .align 5
4:
        ldm r1, {r4-r6}
        add r1, r1, r2
        pld [r1]
        mov r4, r4, lsr #(3 * 8)
        orr r4, r4, r5, lsl #(32 - 3 * 8)
        mov r5, r5, lsr #(3 * 8)
        orr r5, r5, r6, lsl #(32 - 3 * 8)
6: ldm r1, {r7-r9}
        add r1, r1, r2
        pld [r1]
        mov r7, r7, lsr #(3 * 8)
        orr r7, r7, r8, lsl #(32 - 3 * 8)
        mov r8, r8, lsr #(3 * 8)
        orr r8, r8, r9, lsl #(32 - 3 * 8)
        
        
        
        eor r10, r4, r7
        eor r11, r5, r8
        and r4, r4, r7
        and r5, r5, r8
        and r10, r10, r12
        and r11, r11, r12
        add r10, r4, r10, lsr #1
        add r11, r5, r11, lsr #1
        stm r0, {r10-r11}
        add r0, r0, r2
        ldm r1, {r4-r6}
        add r1, r1, r2
        pld [r1]
        mov r4, r4, lsr #(3 * 8)
        orr r4, r4, r5, lsl #(32 - 3 * 8)
        mov r5, r5, lsr #(3 * 8)
        orr r5, r5, r6, lsl #(32 - 3 * 8)
        subs r3, r3, #1
        
        
        
        eor r10, r7, r4
        eor r11, r8, r5
        and r7, r7, r4
        and r8, r8, r5
        and r10, r10, r12
        and r11, r11, r12
        add r10, r7, r10, lsr #1
        add r11, r8, r11, lsr #1
        stm r0, {r10-r11}
        add r0, r0, r2
        bne 6b
        pop {r4-r11,pc}

        @.endfunc

        @.ltorg
.align 2
Literal_0:
 .word 0xfefefefe
        .set Lpic_idx, 0
        .set Lpic_gp, 0
        .text
        .align 5
        .globl _ff_put_pixels8_xy2_arm

        .thumb_func _ff_put_pixels8_xy2_arm
_ff_put_pixels8_xy2_arm:
        
        
        pld [r1]
        push {r4-r11,lr} 
        ands r5, r1, #3
        bic r1, r1, #3
        beq 1f
        subs r5, r5, #1
        beq 2f
        subs r5, r5, #1
        beq 3f
        b 4f
1:        
        
        ldm r1, {r6-r8}
        add r1, r1, r2
        pld [r1]
        mov r4, r6, lsr #(1 * 8)
        mov r5, r7, lsr #(1 * 8)
        orr r4, r4, r7, lsl #(32 - (1 * 8))
        orr r5, r5, r8, lsl #(32 - (1 * 8))
         ldr r14, Literal_1
        tst r3, #1
        and r8, r4, r14
        and r9, r5, r14
        and r10, r6, r14
        and r11, r7, r14
        it eq
        andeq r14, r14, r14, lsl #1
        add r8, r8, r10
        add r9, r9, r11
         ldr r12, Literal_2
        itt eq
        addeq r8, r8, r14
        addeq r9, r9, r14
        and r4, r12, r4, lsr #2
        and r5, r12, r5, lsr #2
        and r6, r12, r6, lsr #2
        and r7, r12, r7, lsr #2
        add r10, r4, r6
        add r11, r5, r7
        subs r3, r3, #1
6: push {r8-r11}
        
        
        ldm r1, {r6-r8}
        add r1, r1, r2
        pld [r1]
        mov r4, r6, lsr #(1 * 8)
        mov r5, r7, lsr #(1 * 8)
        orr r4, r4, r7, lsl #(32 - (1 * 8))
        orr r5, r5, r8, lsl #(32 - (1 * 8))
         ldr r14, Literal_1
        tst r3, #1
        and r8, r4, r14
        and r9, r5, r14
        and r10, r6, r14
        and r11, r7, r14
        it eq
        andeq r14, r14, r14, lsl #1
        add r8, r8, r10
        add r9, r9, r11
         ldr r12, Literal_2
        itt eq
        addeq r8, r8, r14
        addeq r9, r9, r14
        and r4, r12, r4, lsr #2
        and r5, r12, r5, lsr #2
        and r6, r12, r6, lsr #2
        and r7, r12, r7, lsr #2
        add r10, r4, r6
        add r11, r5, r7
        subs r3, r3, #1
        pop {r4-r7}
        add r4, r4, r8
        add r5, r5, r9
         ldr r14, Literal_3
        add r6, r6, r10
        add r7, r7, r11
        and r4, r14, r4, lsr #2
        and r5, r14, r5, lsr #2
        add r4, r4, r6
        add r5, r5, r7
        stm r0, {r4-r5}
        add r0, r0, r2
        bge 6b
        pop {r4-r11,pc}
        .align 5
2:        
        
        ldm r1, {r8-r10}
        add r1, r1, r2
        pld [r1]
        mov r4, r8, lsr #(1 * 8)
        mov r5, r9, lsr #(1 * 8)
        orr r4, r4, r9, lsl #(32 - (1 * 8))
        orr r5, r5, r10, lsl #(32 - (1 * 8))
        mov r6, r8, lsr #(2 * 8)
        mov r7, r9, lsr #(2 * 8)
        orr r6, r6, r9, lsl #(32 - (2 * 8))
        orr r7, r7, r10, lsl #(32 - (2 * 8))
         ldr r14, Literal_1
        tst r3, #1
        and r8, r4, r14
        and r9, r5, r14
        and r10, r6, r14
        and r11, r7, r14
        it eq
        andeq r14, r14, r14, lsl #1
        add r8, r8, r10
        add r9, r9, r11
         ldr r12, Literal_2
        itt eq
        addeq r8, r8, r14
        addeq r9, r9, r14
        and r4, r12, r4, lsr #2
        and r5, r12, r5, lsr #2
        and r6, r12, r6, lsr #2
        and r7, r12, r7, lsr #2
        add r10, r4, r6
        add r11, r5, r7
        subs r3, r3, #1
6: push {r8-r11}
        
        
        ldm r1, {r8-r10}
        add r1, r1, r2
        pld [r1]
        mov r4, r8, lsr #(1 * 8)
        mov r5, r9, lsr #(1 * 8)
        orr r4, r4, r9, lsl #(32 - (1 * 8))
        orr r5, r5, r10, lsl #(32 - (1 * 8))
        mov r6, r8, lsr #(2 * 8)
        mov r7, r9, lsr #(2 * 8)
        orr r6, r6, r9, lsl #(32 - (2 * 8))
        orr r7, r7, r10, lsl #(32 - (2 * 8))
         ldr r14, Literal_1
        tst r3, #1
        and r8, r4, r14
        and r9, r5, r14
        and r10, r6, r14
        and r11, r7, r14
        it eq
        andeq r14, r14, r14, lsl #1
        add r8, r8, r10
        add r9, r9, r11
         ldr r12, Literal_2
        itt eq
        addeq r8, r8, r14
        addeq r9, r9, r14
        and r4, r12, r4, lsr #2
        and r5, r12, r5, lsr #2
        and r6, r12, r6, lsr #2
        and r7, r12, r7, lsr #2
        add r10, r4, r6
        add r11, r5, r7
        subs r3, r3, #1
        pop {r4-r7}
        add r4, r4, r8
        add r5, r5, r9
         ldr r14, Literal_3
        add r6, r6, r10
        add r7, r7, r11
        and r4, r14, r4, lsr #2
        and r5, r14, r5, lsr #2
        add r4, r4, r6
        add r5, r5, r7
        stm r0, {r4-r5}
        add r0, r0, r2
        bge 6b
        pop {r4-r11,pc}
        .align 5
3:        
        
        ldm r1, {r8-r10}
        add r1, r1, r2
        pld [r1]
        mov r4, r8, lsr #(2 * 8)
        mov r5, r9, lsr #(2 * 8)
        orr r4, r4, r9, lsl #(32 - (2 * 8))
        orr r5, r5, r10, lsl #(32 - (2 * 8))
        mov r6, r8, lsr #(3 * 8)
        mov r7, r9, lsr #(3 * 8)
        orr r6, r6, r9, lsl #(32 - (3 * 8))
        orr r7, r7, r10, lsl #(32 - (3 * 8))
         ldr r14, Literal_1
        tst r3, #1
        and r8, r4, r14
        and r9, r5, r14
        and r10, r6, r14
        and r11, r7, r14
        it eq
        andeq r14, r14, r14, lsl #1
        add r8, r8, r10
        add r9, r9, r11
         ldr r12, Literal_2
        itt eq
        addeq r8, r8, r14
        addeq r9, r9, r14
        and r4, r12, r4, lsr #2
        and r5, r12, r5, lsr #2
        and r6, r12, r6, lsr #2
        and r7, r12, r7, lsr #2
        add r10, r4, r6
        add r11, r5, r7
        subs r3, r3, #1
6: push {r8-r11}
        
        
        ldm r1, {r8-r10}
        add r1, r1, r2
        pld [r1]
        mov r4, r8, lsr #(2 * 8)
        mov r5, r9, lsr #(2 * 8)
        orr r4, r4, r9, lsl #(32 - (2 * 8))
        orr r5, r5, r10, lsl #(32 - (2 * 8))
        mov r6, r8, lsr #(3 * 8)
        mov r7, r9, lsr #(3 * 8)
        orr r6, r6, r9, lsl #(32 - (3 * 8))
        orr r7, r7, r10, lsl #(32 - (3 * 8))
         ldr r14, Literal_1
        tst r3, #1
        and r8, r4, r14
        and r9, r5, r14
        and r10, r6, r14
        and r11, r7, r14
        it eq
        andeq r14, r14, r14, lsl #1
        add r8, r8, r10
        add r9, r9, r11
         ldr r12, Literal_2
        itt eq
        addeq r8, r8, r14
        addeq r9, r9, r14
        and r4, r12, r4, lsr #2
        and r5, r12, r5, lsr #2
        and r6, r12, r6, lsr #2
        and r7, r12, r7, lsr #2
        add r10, r4, r6
        add r11, r5, r7
        subs r3, r3, #1
        pop {r4-r7}
        add r4, r4, r8
        add r5, r5, r9
         ldr r14, Literal_3
        add r6, r6, r10
        add r7, r7, r11
        and r4, r14, r4, lsr #2
        and r5, r14, r5, lsr #2
        add r4, r4, r6
        add r5, r5, r7
        stm r0, {r4-r5}
        add r0, r0, r2
        bge 6b
        pop {r4-r11,pc}
        .align 5
4:        
        
        ldm r1, {r5-r7}
        add r1, r1, r2
        pld [r1]
        mov r4, r5, lsr #(3 * 8)
        mov r5, r6, lsr #(3 * 8)
        orr r4, r4, r6, lsl #(32 - (3 * 8))
        orr r5, r5, r7, lsl #(32 - (3 * 8))
         ldr r14, Literal_1
        tst r3, #1
        and r8, r4, r14
        and r9, r5, r14
        and r10, r6, r14
        and r11, r7, r14
        it eq
        andeq r14, r14, r14, lsl #1
        add r8, r8, r10
        add r9, r9, r11
         ldr r12, Literal_2
        itt eq
        addeq r8, r8, r14
        addeq r9, r9, r14
        and r4, r12, r4, lsr #2
        and r5, r12, r5, lsr #2
        and r6, r12, r6, lsr #2
        and r7, r12, r7, lsr #2
        add r10, r4, r6
        add r11, r5, r7
        subs r3, r3, #1
6: push {r8-r11}
        
        
        ldm r1, {r5-r7}
        add r1, r1, r2
        pld [r1]
        mov r4, r5, lsr #(3 * 8)
        mov r5, r6, lsr #(3 * 8)
        orr r4, r4, r6, lsl #(32 - (3 * 8))
        orr r5, r5, r7, lsl #(32 - (3 * 8))
         ldr r14, Literal_1
        tst r3, #1
        and r8, r4, r14
        and r9, r5, r14
        and r10, r6, r14
        and r11, r7, r14
        it eq
        andeq r14, r14, r14, lsl #1
        add r8, r8, r10
        add r9, r9, r11
         ldr r12, Literal_2
        itt eq
        addeq r8, r8, r14
        addeq r9, r9, r14
        and r4, r12, r4, lsr #2
        and r5, r12, r5, lsr #2
        and r6, r12, r6, lsr #2
        and r7, r12, r7, lsr #2
        add r10, r4, r6
        add r11, r5, r7
        subs r3, r3, #1
        pop {r4-r7}
        add r4, r4, r8
        add r5, r5, r9
         ldr r14, Literal_3
        add r6, r6, r10
        add r7, r7, r11
        and r4, r14, r4, lsr #2
        and r5, r14, r5, lsr #2
        add r4, r4, r6
        add r5, r5, r7
        stm r0, {r4-r5}
        add r0, r0, r2
        bge 6b
        pop {r4-r11,pc}

        @.endfunc

        .set Lpic_idx, 0
        .set Lpic_gp, 0
        .text
        .align 5
        .globl _ff_put_no_rnd_pixels8_xy2_arm

        .thumb_func _ff_put_no_rnd_pixels8_xy2_arm
_ff_put_no_rnd_pixels8_xy2_arm:
        
        
        pld [r1]
        push {r4-r11,lr}
        ands r5, r1, #3
        bic r1, r1, #3
        beq 1f
        subs r5, r5, #1
        beq 2f
        subs r5, r5, #1
        beq 3f
        b 4f
1:        
        
        ldm r1, {r6-r8}
        add r1, r1, r2
        pld [r1]
        mov r4, r6, lsr #(1 * 8)
        mov r5, r7, lsr #(1 * 8)
        orr r4, r4, r7, lsl #(32 - (1 * 8))
        orr r5, r5, r8, lsl #(32 - (1 * 8))
         ldr r14, Literal_1
        tst r3, #1
        and r8, r4, r14
        and r9, r5, r14
        and r10, r6, r14
        and r11, r7, r14
        it eq
        andeq r14, r14, r14, lsr #1
        add r8, r8, r10
        add r9, r9, r11
         ldr r12, Literal_2
        itt eq
        addeq r8, r8, r14
        addeq r9, r9, r14
        and r4, r12, r4, lsr #2
        and r5, r12, r5, lsr #2
        and r6, r12, r6, lsr #2
        and r7, r12, r7, lsr #2
        add r10, r4, r6
        add r11, r5, r7
        subs r3, r3, #1
6: push {r8-r11}
        
        
        ldm r1, {r6-r8}
        add r1, r1, r2
        pld [r1]
        mov r4, r6, lsr #(1 * 8)
        mov r5, r7, lsr #(1 * 8)
        orr r4, r4, r7, lsl #(32 - (1 * 8))
        orr r5, r5, r8, lsl #(32 - (1 * 8))
         ldr r14, Literal_1
        tst r3, #1
        and r8, r4, r14
        and r9, r5, r14
        and r10, r6, r14
        and r11, r7, r14
        it eq
        andeq r14, r14, r14, lsr #1
        add r8, r8, r10
        add r9, r9, r11
         ldr r12, Literal_2
        itt eq
        addeq r8, r8, r14
        addeq r9, r9, r14
        and r4, r12, r4, lsr #2
        and r5, r12, r5, lsr #2
        and r6, r12, r6, lsr #2
        and r7, r12, r7, lsr #2
        add r10, r4, r6
        add r11, r5, r7
        subs r3, r3, #1
        pop {r4-r7}
        add r4, r4, r8
        add r5, r5, r9
         ldr r14, Literal_3
        add r6, r6, r10
        add r7, r7, r11
        and r4, r14, r4, lsr #2
        and r5, r14, r5, lsr #2
        add r4, r4, r6
        add r5, r5, r7
        stm r0, {r4-r5}
        add r0, r0, r2
        bge 6b
        pop {r4-r11,pc}
        .align 5
2:        
        
        ldm r1, {r8-r10}
        add r1, r1, r2
        pld [r1]
        mov r4, r8, lsr #(1 * 8)
        mov r5, r9, lsr #(1 * 8)
        orr r4, r4, r9, lsl #(32 - (1 * 8))
        orr r5, r5, r10, lsl #(32 - (1 * 8))
        mov r6, r8, lsr #(2 * 8)
        mov r7, r9, lsr #(2 * 8)
        orr r6, r6, r9, lsl #(32 - (2 * 8))
        orr r7, r7, r10, lsl #(32 - (2 * 8))
         ldr r14, Literal_1
        tst r3, #1
        and r8, r4, r14
        and r9, r5, r14
        and r10, r6, r14
        and r11, r7, r14
        it eq
        andeq r14, r14, r14, lsr #1
        add r8, r8, r10
        add r9, r9, r11
         ldr r12, Literal_2
        itt eq
        addeq r8, r8, r14
        addeq r9, r9, r14
        and r4, r12, r4, lsr #2
        and r5, r12, r5, lsr #2
        and r6, r12, r6, lsr #2
        and r7, r12, r7, lsr #2
        add r10, r4, r6
        add r11, r5, r7
        subs r3, r3, #1
6: push {r8-r11}
        
        
        ldm r1, {r8-r10}
        add r1, r1, r2
        pld [r1]
        mov r4, r8, lsr #(1 * 8)
        mov r5, r9, lsr #(1 * 8)
        orr r4, r4, r9, lsl #(32 - (1 * 8))
        orr r5, r5, r10, lsl #(32 - (1 * 8))
        mov r6, r8, lsr #(2 * 8)
        mov r7, r9, lsr #(2 * 8)
        orr r6, r6, r9, lsl #(32 - (2 * 8))
        orr r7, r7, r10, lsl #(32 - (2 * 8))
         ldr r14, Literal_1
        tst r3, #1
        and r8, r4, r14
        and r9, r5, r14
        and r10, r6, r14
        and r11, r7, r14
        it eq
        andeq r14, r14, r14, lsr #1
        add r8, r8, r10
        add r9, r9, r11
         ldr r12, Literal_2
        itt eq
        addeq r8, r8, r14
        addeq r9, r9, r14
        and r4, r12, r4, lsr #2
        and r5, r12, r5, lsr #2
        and r6, r12, r6, lsr #2
        and r7, r12, r7, lsr #2
        add r10, r4, r6
        add r11, r5, r7
        subs r3, r3, #1
        pop {r4-r7}
        add r4, r4, r8
        add r5, r5, r9
         ldr r14, Literal_3
        add r6, r6, r10
        add r7, r7, r11
        and r4, r14, r4, lsr #2
        and r5, r14, r5, lsr #2
        add r4, r4, r6
        add r5, r5, r7
        stm r0, {r4-r5}
        add r0, r0, r2
        bge 6b
        pop {r4-r11,pc}
        .align 5
3:        
        
        ldm r1, {r8-r10}
        add r1, r1, r2
        pld [r1]
        mov r4, r8, lsr #(2 * 8)
        mov r5, r9, lsr #(2 * 8)
        orr r4, r4, r9, lsl #(32 - (2 * 8))
        orr r5, r5, r10, lsl #(32 - (2 * 8))
        mov r6, r8, lsr #(3 * 8)
        mov r7, r9, lsr #(3 * 8)
        orr r6, r6, r9, lsl #(32 - (3 * 8))
        orr r7, r7, r10, lsl #(32 - (3 * 8))
         ldr r14, Literal_1
        tst r3, #1
        and r8, r4, r14
        and r9, r5, r14
        and r10, r6, r14
        and r11, r7, r14
        it eq
        andeq r14, r14, r14, lsr #1
        add r8, r8, r10
        add r9, r9, r11
         ldr r12, Literal_2
        itt eq
        addeq r8, r8, r14
        addeq r9, r9, r14
        and r4, r12, r4, lsr #2
        and r5, r12, r5, lsr #2
        and r6, r12, r6, lsr #2
        and r7, r12, r7, lsr #2
        add r10, r4, r6
        add r11, r5, r7
        subs r3, r3, #1
6: push {r8-r11}
        
        
        ldm r1, {r8-r10}
        add r1, r1, r2
        pld [r1]
        mov r4, r8, lsr #(2 * 8)
        mov r5, r9, lsr #(2 * 8)
        orr r4, r4, r9, lsl #(32 - (2 * 8))
        orr r5, r5, r10, lsl #(32 - (2 * 8))
        mov r6, r8, lsr #(3 * 8)
        mov r7, r9, lsr #(3 * 8)
        orr r6, r6, r9, lsl #(32 - (3 * 8))
        orr r7, r7, r10, lsl #(32 - (3 * 8))
         ldr r14, Literal_1
        tst r3, #1
        and r8, r4, r14
        and r9, r5, r14
        and r10, r6, r14
        and r11, r7, r14
        it eq
        andeq r14, r14, r14, lsr #1
        add r8, r8, r10
        add r9, r9, r11
         ldr r12, Literal_2
        itt eq
        addeq r8, r8, r14
        addeq r9, r9, r14
        and r4, r12, r4, lsr #2
        and r5, r12, r5, lsr #2
        and r6, r12, r6, lsr #2
        and r7, r12, r7, lsr #2
        add r10, r4, r6
        add r11, r5, r7
        subs r3, r3, #1
        pop {r4-r7}
        add r4, r4, r8
        add r5, r5, r9
         ldr r14, Literal_3
        add r6, r6, r10
        add r7, r7, r11
        and r4, r14, r4, lsr #2
        and r5, r14, r5, lsr #2
        add r4, r4, r6
        add r5, r5, r7
        stm r0, {r4-r5}
        add r0, r0, r2
        bge 6b
        pop {r4-r11,pc}
        .align 5
4:        
        
        ldm r1, {r5-r7}
        add r1, r1, r2
        pld [r1]
        mov r4, r5, lsr #(3 * 8)
        mov r5, r6, lsr #(3 * 8)
        orr r4, r4, r6, lsl #(32 - (3 * 8))
        orr r5, r5, r7, lsl #(32 - (3 * 8))
         ldr r14, Literal_1
        tst r3, #1
        and r8, r4, r14
        and r9, r5, r14
        and r10, r6, r14
        and r11, r7, r14
        it eq
        andeq r14, r14, r14, lsr #1
        add r8, r8, r10
        add r9, r9, r11
         ldr r12, Literal_2
        itt eq
        addeq r8, r8, r14
        addeq r9, r9, r14
        and r4, r12, r4, lsr #2
        and r5, r12, r5, lsr #2
        and r6, r12, r6, lsr #2
        and r7, r12, r7, lsr #2
        add r10, r4, r6
        add r11, r5, r7
        subs r3, r3, #1
6: push {r8-r11}
        
        
        ldm r1, {r5-r7}
        add r1, r1, r2
        pld [r1]
        mov r4, r5, lsr #(3 * 8)
        mov r5, r6, lsr #(3 * 8)
        orr r4, r4, r6, lsl #(32 - (3 * 8))
        orr r5, r5, r7, lsl #(32 - (3 * 8))
         ldr r14, Literal_1
        tst r3, #1
        and r8, r4, r14
        and r9, r5, r14
        and r10, r6, r14
        and r11, r7, r14
        it eq
        andeq r14, r14, r14, lsr #1
        add r8, r8, r10
        add r9, r9, r11
         ldr r12, Literal_2
        itt eq
        addeq r8, r8, r14
        addeq r9, r9, r14
        and r4, r12, r4, lsr #2
        and r5, r12, r5, lsr #2
        and r6, r12, r6, lsr #2
        and r7, r12, r7, lsr #2
        add r10, r4, r6
        add r11, r5, r7
        subs r3, r3, #1
        pop {r4-r7}
        add r4, r4, r8
        add r5, r5, r9
         ldr r14, Literal_3
        add r6, r6, r10
        add r7, r7, r11
        and r4, r14, r4, lsr #2
        and r5, r14, r5, lsr #2
        add r4, r4, r6
        add r5, r5, r7
        stm r0, {r4-r5}
        add r0, r0, r2
        bge 6b
        pop {r4-r11,pc}

        @.endfunc
.text
.align 2
Literal_1:
 .word 0x03030303
Literal_2:
 .word 0xfcfcfcfc >> 2
Literal_3:
 .word 0x0f0f0f0f
.thumb_func _ff_put_pixels8_x2_arm
.thumb_func _ff_put_pixels8_y2_arm
.thumb_func _ff_put_pixels8_arm
.thumb_func _ff_put_no_rnd_pixels8_y2_arm
.thumb_func _ff_put_no_rnd_pixels8_xy2_arm
.thumb_func _ff_put_no_rnd_pixels8_x2_arm
.thumb_func _ff_put_pixels16_arm
.thumb_func _ff_put_pixels8_xy2_arm
#endif
