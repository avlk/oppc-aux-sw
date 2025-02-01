	.cpu cortex-m0plus
	.arch armv6s-m
	.fpu softvfp
	.eabi_attribute 20, 1	@ Tag_ABI_FP_denormal
	.eabi_attribute 21, 1	@ Tag_ABI_FP_exceptions
	.eabi_attribute 23, 3	@ Tag_ABI_FP_number_model
	.eabi_attribute 24, 1	@ Tag_ABI_align8_needed
	.eabi_attribute 25, 1	@ Tag_ABI_align8_preserved
	.eabi_attribute 26, 1	@ Tag_ABI_enum_size
	.eabi_attribute 30, 1	@ Tag_ABI_optimization_goals
	.eabi_attribute 34, 0	@ Tag_CPU_unaligned_access
	.eabi_attribute 18, 4	@ Tag_ABI_PCS_wchar_t
	.file	"cic.cpp"
@ GNU C++17 (GCC) version 14.2.0 (arm-none-eabi)
@	compiled by GNU C version 9.3-win32 20200320, GMP version 6.1.2, MPFR version 3.1.4, MPC version 1.0.3, isl version isl-0.18-GMP

@ GGC heuristics: --param ggc-min-expand=100 --param ggc-min-heapsize=131072
@ options passed: -mcpu=cortex-m0plus -mthumb -march=armv6-m -g -O1 -std=gnu++17 -fno-exceptions -fno-rtti -ffunction-sections -fdata-sections
	.text
.Ltext0:
	.cfi_sections	.debug_frame
	.file 1 "lib/filter/src/cic.cpp"
	.section	.text._Z8cic_initP12cic_filter_tmm,"ax",%progbits
	.align	1
	.global	_Z8cic_initP12cic_filter_tmm
	.syntax unified
	.code	16
	.thumb_func
	.type	_Z8cic_initP12cic_filter_tmm, %function
_Z8cic_initP12cic_filter_tmm:
.LVL0:
.LFB10:
	.loc 1 9 42 view -0
	.cfi_startproc
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	.loc 1 9 42 is_stmt 0 view .LVU1
	push	{r4, r5, r6, lr}	@
	.cfi_def_cfa_offset 16
	.cfi_offset 4, -16
	.cfi_offset 5, -12
	.cfi_offset 6, -8
	.cfi_offset 14, -4
	movs	r6, r0	@ filter, tmp139
	movs	r4, r1	@ order, tmp140
	movs	r5, r2	@ downsample, tmp141
	.loc 1 10 5 is_stmt 1 view .LVU2
@ lib/filter/src/cic.cpp:10:     memset(filter, 0, sizeof(cic_filter_t));
	.loc 1 10 11 is_stmt 0 view .LVU3
	movs	r2, #152	@ tmp148,
.LVL1:
	.loc 1 10 11 view .LVU4
	lsls	r2, r2, #2	@, tmp148,
	movs	r1, #0	@,
.LVL2:
	.loc 1 10 11 view .LVU5
	bl	memset		@
.LVL3:
	.loc 1 11 5 is_stmt 1 view .LVU6
@ lib/filter/src/cic.cpp:11:     filter->order = order;
	.loc 1 11 19 is_stmt 0 view .LVU7
	str	r4, [r6]	@ order, filter_11(D)->order
	.loc 1 12 5 is_stmt 1 view .LVU8
@ lib/filter/src/cic.cpp:12:     filter->decimation_factor = downsample; 
	.loc 1 12 31 is_stmt 0 view .LVU9
	str	r5, [r6, #4]	@ downsample, filter_11(D)->decimation_factor
	.loc 1 14 5 is_stmt 1 view .LVU10
.LVL4:
	.loc 1 15 5 view .LVU11
	.loc 1 15 17 view .LVU12
	subs	r3, r4, #1	@ order, order,
.LVL5:
	.loc 1 15 17 is_stmt 0 view .LVU13
	cmp	r4, #0	@ order,
	beq	.L4		@,
@ lib/filter/src/cic.cpp:14:     uint32_t n = 1;
	.loc 1 14 14 view .LVU14
	movs	r4, #1	@ n,
.LVL6:
.L3:
	.loc 1 16 9 is_stmt 1 view .LVU15
@ lib/filter/src/cic.cpp:16:         n *= downsample;
	.loc 1 16 11 is_stmt 0 view .LVU16
	muls	r4, r5	@ n, downsample
.LVL7:
	.loc 1 15 17 is_stmt 1 view .LVU17
	.loc 1 15 17 is_stmt 0 view .LVU18
	subs	r3, r3, #1	@ order, order
	bcs	.L3		@,
.LVL8:
.L2:
	.loc 1 17 5 is_stmt 1 view .LVU19
	.loc 1 18 5 view .LVU20
@ lib/filter/src/cic.cpp:18:     filter->attenuate_shift = 32 - __builtin_clz(n) - 1;
	.loc 1 18 49 is_stmt 0 view .LVU21
	movs	r0, r4	@, n
	bl	__clzsi2		@
.LVL9:
@ lib/filter/src/cic.cpp:18:     filter->attenuate_shift = 32 - __builtin_clz(n) - 1;
	.loc 1 18 53 discriminator 1 view .LVU22
	movs	r5, #31	@ tmp131,
.LVL10:
	.loc 1 18 53 discriminator 1 view .LVU23
	subs	r5, r5, r0	@ _3, tmp131, tmp142
@ lib/filter/src/cic.cpp:18:     filter->attenuate_shift = 32 - __builtin_clz(n) - 1;
	.loc 1 18 29 discriminator 1 view .LVU24
	movs	r3, #150	@ tmp132,
	lsls	r3, r3, #2	@ tmp132, tmp132,
	str	r5, [r6, r3]	@ _3, filter_11(D)->attenuate_shift
	.loc 1 19 5 is_stmt 1 view .LVU25
@ lib/filter/src/cic.cpp:19:     filter->gain = (float)actual_gain / (1UL << filter->attenuate_shift);
	.loc 1 19 20 is_stmt 0 view .LVU26
	movs	r0, r4	@, n
	bl	__aeabi_ui2f		@
.LVL11:
	adds	r4, r0, #0	@ _4, tmp143
.LVL12:
@ lib/filter/src/cic.cpp:19:     filter->gain = (float)actual_gain / (1UL << filter->attenuate_shift);
	.loc 1 19 46 view .LVU27
	movs	r0, #1	@ tmp136,
	lsls	r0, r0, r5	@ _5, tmp136, _3
@ lib/filter/src/cic.cpp:19:     filter->gain = (float)actual_gain / (1UL << filter->attenuate_shift);
	.loc 1 19 39 view .LVU28
	bl	__aeabi_ui2f		@
.LVL13:
	adds	r1, r0, #0	@ _6, tmp144
	adds	r0, r4, #0	@, _4
	bl	__aeabi_fdiv		@
.LVL14:
@ lib/filter/src/cic.cpp:19:     filter->gain = (float)actual_gain / (1UL << filter->attenuate_shift);
	.loc 1 19 18 view .LVU29
	movs	r3, #151	@ tmp133,
	lsls	r3, r3, #2	@ tmp133, tmp133,
	str	r0, [r6, r3]	@ tmp145, filter_11(D)->gain
@ lib/filter/src/cic.cpp:20: }
	.loc 1 20 1 view .LVU30
	@ sp needed	@
.LVL15:
	.loc 1 20 1 view .LVU31
	pop	{r4, r5, r6, pc}
.LVL16:
.L4:
@ lib/filter/src/cic.cpp:14:     uint32_t n = 1;
	.loc 1 14 14 view .LVU32
	movs	r4, #1	@ n,
	b	.L2		@
	.cfi_endproc
.LFE10:
	.size	_Z8cic_initP12cic_filter_tmm, .-_Z8cic_initP12cic_filter_tmm
	.section	.text._Z9cic_writeP12cic_filter_tPKtjj,"ax",%progbits
	.align	1
	.global	_Z9cic_writeP12cic_filter_tPKtjj
	.syntax unified
	.code	16
	.thumb_func
	.type	_Z9cic_writeP12cic_filter_tPKtjj, %function
_Z9cic_writeP12cic_filter_tPKtjj:
.LVL17:
.LFB11:
	.loc 1 23 44 is_stmt 1 view -0
	.cfi_startproc
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 0, uses_anonymous_args = 0
	.loc 1 23 44 is_stmt 0 view .LVU34
	push	{r4, r5, r6, r7, lr}	@
	.cfi_def_cfa_offset 20
	.cfi_offset 4, -20
	.cfi_offset 5, -16
	.cfi_offset 6, -12
	.cfi_offset 7, -8
	.cfi_offset 14, -4
	mov	lr, fp	@,
	mov	r7, r10	@,
	mov	r6, r9	@,
	mov	r5, r8	@,
	push	{r5, r6, r7, lr}	@
	.cfi_def_cfa_offset 36
	.cfi_offset 8, -36
	.cfi_offset 9, -32
	.cfi_offset 10, -28
	.cfi_offset 11, -24
	sub	sp, sp, #12	@,,
	.cfi_def_cfa_offset 48
	movs	r7, r0	@ filter, tmp173
	mov	ip, r1	@ data, tmp174
	subs	r0, r2, #0	@ length, tmp175,
.LVL18:
	.loc 1 23 44 view .LVU35
	mov	fp, r3	@ step, tmp176
	.loc 1 24 5 is_stmt 1 view .LVU36
	.loc 1 25 5 view .LVU37
	.loc 1 26 5 view .LVU38
.LVL19:
	.loc 1 27 5 view .LVU39
@ lib/filter/src/cic.cpp:27:     uint32_t data_counter = filter->data_counter;
	.loc 1 27 14 is_stmt 0 view .LVU40
	ldr	r1, [r7, #8]	@ data_counter, filter_30(D)->data_counter
.LVL20:
	.loc 1 28 5 is_stmt 1 view .LVU41
@ lib/filter/src/cic.cpp:28:     uint32_t order = filter->order;
	.loc 1 28 14 is_stmt 0 view .LVU42
	ldr	r3, [r7]	@ order, filter_30(D)->order
.LVL21:
	.loc 1 28 14 view .LVU43
	mov	r9, r3	@ order, order
.LVL22:
	.loc 1 29 5 is_stmt 1 view .LVU44
@ lib/filter/src/cic.cpp:29:     uint32_t decimation = filter->decimation_factor;
	.loc 1 29 14 is_stmt 0 view .LVU45
	ldr	r3, [r7, #4]	@ decimation, filter_30(D)->decimation_factor
.LVL23:
	.loc 1 29 14 view .LVU46
	mov	r10, r3	@ decimation, decimation
.LVL24:
	.loc 1 31 5 is_stmt 1 view .LVU47
.LBB2:
	.loc 1 31 19 discriminator 1 view .LVU48
	beq	.L16		@,
	mov	r3, r9	@ order, order
.LVL25:
	.loc 1 31 19 is_stmt 0 discriminator 1 view .LVU49
	subs	r2, r3, #1	@ _62, order,
.LVL26:
	.loc 1 31 19 discriminator 1 view .LVU50
	str	r2, [sp]	@ _62, %sfp
	movs	r3, r7	@ _12, filter
	adds	r3, r3, #20	@ _12,
	lsls	r2, r2, #2	@ _24, _62,
	adds	r3, r3, r2	@ state, _12, _24
	str	r3, [sp, #4]	@ state, %sfp
.LBE2:
@ lib/filter/src/cic.cpp:26:     uint32_t err_cnt = 0;
	.loc 1 26 14 view .LVU51
	movs	r3, #0	@ err_cnt,
	mov	r8, r3	@ err_cnt, err_cnt
.LBB6:
@ lib/filter/src/cic.cpp:31:     for (n = 0; n < length; n += step) {
	.loc 1 31 12 view .LVU52
	movs	r6, #0	@ n,
	mov	r3, r10	@ decimation, decimation
	movs	r5, r3	@ decimation, decimation
	movs	r3, r0	@ length, length
	mov	r0, ip	@ data, data
.LVL27:
	.loc 1 31 12 view .LVU53
	mov	ip, r3	@ length, length
.LVL28:
	.loc 1 31 12 view .LVU54
	b	.L14		@
.LVL29:
.L13:
.LBB3:
	.loc 1 61 13 is_stmt 1 view .LVU55
@ lib/filter/src/cic.cpp:61:             err_cnt++;
	.loc 1 61 20 is_stmt 0 view .LVU56
	movs	r3, #1	@ tmp229,
	mov	r10, r3	@ tmp229, tmp229
	add	r8, r8, r10	@ err_cnt, tmp229
.LVL30:
@ lib/filter/src/cic.cpp:47:             data_counter = 0;
	.loc 1 47 26 view .LVU57
	movs	r1, #0	@ data_counter,
.L11:
.LVL31:
	.loc 1 47 26 view .LVU58
.LBE3:
	.loc 1 31 5 is_stmt 1 discriminator 2 view .LVU59
@ lib/filter/src/cic.cpp:31:     for (n = 0; n < length; n += step) {
	.loc 1 31 31 is_stmt 0 discriminator 2 view .LVU60
	add	r6, r6, fp	@ n, step
.LVL32:
	.loc 1 31 19 is_stmt 1 discriminator 1 view .LVU61
	cmp	ip, r6	@ length, n
	bls	.L23		@,
.LVL33:
.L14:
.LBB4:
	.loc 1 32 9 view .LVU62
	.loc 1 35 9 view .LVU63
@ lib/filter/src/cic.cpp:35:         stage_in = data[n];
	.loc 1 35 26 is_stmt 0 view .LVU64
	lsls	r3, r6, #1	@ _65, n,
@ lib/filter/src/cic.cpp:35:         stage_in = data[n];
	.loc 1 35 18 view .LVU65
	ldrh	r3, [r0, r3]	@ stage_out, MEM[(const uint16_t *)data_40(D) + _65 * 1]
.LVL34:
	.loc 1 36 9 is_stmt 1 view .LVU66
	.loc 1 37 9 view .LVU67
	.loc 1 37 19 view .LVU68
	.loc 1 37 19 is_stmt 0 view .LVU69
	mov	r2, r9	@ order, order
	cmp	r2, #0	@ order,
	beq	.L9		@,
.LVL35:
	.loc 1 37 19 view .LVU70
	movs	r4, r7	@ ivtmp.41, filter
	adds	r4, r4, #16	@ ivtmp.41,
.LVL36:
	.loc 1 37 19 view .LVU71
	ldr	r2, [sp]	@ ord, %sfp
.LVL37:
	.loc 1 37 19 view .LVU72
	mov	r10, r5	@ decimation, decimation
.LVL38:
.L10:
	.loc 1 38 13 is_stmt 1 view .LVU73
@ lib/filter/src/cic.cpp:38:             stage_out = stage_in + *state;
	.loc 1 38 23 is_stmt 0 view .LVU74
	ldr	r5, [r4]	@ MEM[(int32_t *)_58], MEM[(int32_t *)_58]
	adds	r3, r3, r5	@ stage_out, stage_out, MEM[(int32_t *)_58]
.LVL39:
	.loc 1 39 13 is_stmt 1 view .LVU75
@ lib/filter/src/cic.cpp:39:             *state++ = stage_out;
	.loc 1 39 22 is_stmt 0 view .LVU76
	stmia	r4!, {r3}	@ MEM[(int32_t *)_58], stage_out
.LVL40:
	.loc 1 40 13 is_stmt 1 view .LVU77
	.loc 1 37 19 view .LVU78
	.loc 1 37 19 is_stmt 0 view .LVU79
	subs	r2, r2, #1	@ ord, ord
	bcs	.L10		@,
.LVL41:
@ lib/filter/src/cic.cpp:44:         if (++data_counter < decimation)
	.loc 1 44 28 view .LVU80
	mov	r2, r10	@ decimation, decimation
.LVL42:
	.loc 1 44 28 view .LVU81
	movs	r5, r2	@ decimation, decimation
	.loc 1 44 9 is_stmt 1 view .LVU82
@ lib/filter/src/cic.cpp:44:         if (++data_counter < decimation)
	.loc 1 44 28 is_stmt 0 view .LVU83
	adds	r1, r1, #1	@ data_counter,
.LVL43:
@ lib/filter/src/cic.cpp:44:         if (++data_counter < decimation)
	.loc 1 44 9 view .LVU84
	cmp	r2, r1	@ decimation, data_counter
	bhi	.L11		@,
.LVL44:
@ lib/filter/src/cic.cpp:51:         while (ord--) {
	.loc 1 51 19 view .LVU85
	ldr	r5, [sp]	@ ord, %sfp
	ldr	r4, [sp, #4]	@ state, %sfp
.LVL45:
.L12:
	.loc 1 52 13 is_stmt 1 view .LVU86
	movs	r1, r3	@ stage_in, stage_out
@ lib/filter/src/cic.cpp:52:             stage_out = stage_in - *state;
	.loc 1 52 23 is_stmt 0 view .LVU87
	ldr	r2, [r4]	@ *state_80, *state_80
	subs	r3, r3, r2	@ stage_out, stage_out, *state_80
.LVL46:
	.loc 1 53 13 is_stmt 1 view .LVU88
@ lib/filter/src/cic.cpp:53:             *state++ = stage_in;
	.loc 1 53 22 is_stmt 0 view .LVU89
	str	r1, [r4]	@ stage_in, *state_63
	adds	r4, r4, #4	@ state,
.LVL47:
	.loc 1 54 13 is_stmt 1 view .LVU90
	.loc 1 51 19 view .LVU91
	.loc 1 51 19 is_stmt 0 view .LVU92
	subs	r5, r5, #1	@ ord, ord
	bcs	.L12		@,
.LVL48:
	.loc 1 51 19 view .LVU93
	mov	r2, r10	@ decimation, decimation
	movs	r5, r2	@ decimation, decimation
.LVL49:
.L15:
	.loc 1 58 9 is_stmt 1 view .LVU94
@ lib/filter/src/cic.cpp:58:         if (filter->out_cnt < F_CIC_OUTPUT_LEN) {
	.loc 1 58 21 is_stmt 0 view .LVU95
	ldr	r2, [r7, #12]	@ _6, filter_30(D)->out_cnt
@ lib/filter/src/cic.cpp:58:         if (filter->out_cnt < F_CIC_OUTPUT_LEN) {
	.loc 1 58 9 view .LVU96
	cmp	r2, #127	@ _6,
	bhi	.L13		@,
	.loc 1 59 13 is_stmt 1 view .LVU97
@ lib/filter/src/cic.cpp:59:             filter->out_buf[filter->out_cnt++] = stage_in;
	.loc 1 59 44 is_stmt 0 view .LVU98
	adds	r4, r2, #1	@ _7, _6,
	str	r4, [r7, #12]	@ _7, filter_30(D)->out_cnt
@ lib/filter/src/cic.cpp:59:             filter->out_buf[filter->out_cnt++] = stage_in;
	.loc 1 59 48 view .LVU99
	adds	r2, r2, #20	@ tmp158,
	lsls	r2, r2, #2	@ tmp159, tmp158,
	str	r3, [r2, r7]	@ stage_out, filter_30(D)->out_buf[_6]
@ lib/filter/src/cic.cpp:47:             data_counter = 0;
	.loc 1 47 26 view .LVU100
	movs	r1, #0	@ data_counter,
	b	.L11		@
.LVL50:
.L23:
	.loc 1 47 26 view .LVU101
	mov	r0, ip	@ length, length
.LVL51:
.L8:
	.loc 1 47 26 view .LVU102
.LBE4:
.LBE6:
	.loc 1 64 5 is_stmt 1 view .LVU103
@ lib/filter/src/cic.cpp:64:     filter->data_counter = data_counter;
	.loc 1 64 26 is_stmt 0 view .LVU104
	str	r1, [r7, #8]	@ data_counter, filter_30(D)->data_counter
	.loc 1 65 5 is_stmt 1 view .LVU105
@ lib/filter/src/cic.cpp:65:     filter->overflow_cnt += err_cnt;
	.loc 1 65 26 is_stmt 0 view .LVU106
	movs	r2, #148	@ tmp160,
	lsls	r2, r2, #2	@ tmp160, tmp160,
	ldr	r3, [r7, r2]	@ filter_30(D)->overflow_cnt, filter_30(D)->overflow_cnt
	add	r3, r3, r8	@ _9, err_cnt
	str	r3, [r7, r2]	@ _9, filter_30(D)->overflow_cnt
	.loc 1 66 5 is_stmt 1 view .LVU107
@ lib/filter/src/cic.cpp:66:     filter->samples_out_cnt += length/step - err_cnt;
	.loc 1 66 29 is_stmt 0 view .LVU108
	movs	r4, #149	@ tmp164,
	lsls	r4, r4, #2	@ tmp164, tmp164,
@ lib/filter/src/cic.cpp:66:     filter->samples_out_cnt += length/step - err_cnt;
	.loc 1 66 38 view .LVU109
	mov	r1, fp	@, step
.LVL52:
	.loc 1 66 38 view .LVU110
	bl	__aeabi_uidiv		@
.LVL53:
@ lib/filter/src/cic.cpp:66:     filter->samples_out_cnt += length/step - err_cnt;
	.loc 1 66 29 view .LVU111
	ldr	r3, [r7, r4]	@ filter_30(D)->samples_out_cnt, filter_30(D)->samples_out_cnt
	adds	r3, r0, r3	@ _71, tmp177, filter_30(D)->samples_out_cnt
	mov	r2, r8	@ err_cnt, err_cnt
	subs	r3, r3, r2	@ _13, _71, err_cnt
	str	r3, [r7, r4]	@ _13, filter_30(D)->samples_out_cnt
@ lib/filter/src/cic.cpp:67: }
	.loc 1 67 1 view .LVU112
	add	sp, sp, #12	@,,
	@ sp needed	@
.LVL54:
.LVL55:
.LVL56:
	.loc 1 67 1 view .LVU113
	pop	{r4, r5, r6, r7}
	mov	fp, r7
	mov	r10, r6
	mov	r9, r5
	mov	r8, r4
	pop	{r4, r5, r6, r7, pc}
.LVL57:
.L16:
@ lib/filter/src/cic.cpp:26:     uint32_t err_cnt = 0;
	.loc 1 26 14 view .LVU114
	mov	r8, r2	@ err_cnt, length
	b	.L8		@
.LVL58:
.L9:
.LBB7:
.LBB5:
	.loc 1 44 9 is_stmt 1 view .LVU115
@ lib/filter/src/cic.cpp:44:         if (++data_counter < decimation)
	.loc 1 44 28 is_stmt 0 view .LVU116
	adds	r1, r1, #1	@ data_counter,
.LVL59:
@ lib/filter/src/cic.cpp:44:         if (++data_counter < decimation)
	.loc 1 44 9 view .LVU117
	cmp	r1, r5	@ data_counter, decimation
	bcc	.L11		@,
	b	.L15		@
.LBE5:
.LBE7:
	.cfi_endproc
.LFE11:
	.size	_Z9cic_writeP12cic_filter_tPKtjj, .-_Z9cic_writeP12cic_filter_tPKtjj
	.section	.text._Z8cic_readP12cic_filter_tPtj,"ax",%progbits
	.align	1
	.global	_Z8cic_readP12cic_filter_tPtj
	.syntax unified
	.code	16
	.thumb_func
	.type	_Z8cic_readP12cic_filter_tPtj, %function
_Z8cic_readP12cic_filter_tPtj:
.LVL60:
.LFB12:
	.loc 1 69 73 is_stmt 1 view -0
	.cfi_startproc
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	.loc 1 69 73 is_stmt 0 view .LVU119
	push	{r3, r4, r5, r6, r7, lr}	@
	.cfi_def_cfa_offset 24
	.cfi_offset 3, -24
	.cfi_offset 4, -20
	.cfi_offset 5, -16
	.cfi_offset 6, -12
	.cfi_offset 7, -8
	.cfi_offset 14, -4
	mov	lr, r9	@,
	mov	r7, r8	@,
	push	{r7, lr}	@
	.cfi_def_cfa_offset 32
	.cfi_offset 8, -32
	.cfi_offset 9, -28
	movs	r5, r0	@ filter, tmp153
	.loc 1 70 5 is_stmt 1 view .LVU120
@ lib/filter/src/cic.cpp:70:     size_t output_size = filter->out_cnt;
	.loc 1 70 12 is_stmt 0 view .LVU121
	ldr	r6, [r0, #12]	@ <retval>, filter_18(D)->out_cnt
.LVL61:
	.loc 1 71 5 is_stmt 1 view .LVU122
	.loc 1 72 9 view .LVU123
	.loc 1 73 5 view .LVU124
	cmp	r6, #0	@ <retval>,
	beq	.L24		@,
	movs	r2, r0	@ ivtmp.53, filter
	adds	r2, r2, #80	@ ivtmp.53,
	lsls	r3, r6, #2	@ _38, <retval>,
	mov	r9, r3	@ _38, _38
	mov	r8, r2	@ _41, ivtmp.53
	add	r8, r8, r9	@ _41, _38
.LBB8:
.LBB9:
@ lib/filter/src/cic.cpp:76:         int32_t val = filter->out_buf[n] >> filter->attenuate_shift;
	.loc 1 76 53 is_stmt 0 view .LVU125
	movs	r3, #150	@ tmp140,
	lsls	r3, r3, #2	@ tmp140, tmp140,
	mov	ip, r3	@ tmp140, tmp140
@ lib/filter/src/cic.cpp:79:         if (val > UINT16_MAX) 
	.loc 1 79 9 view .LVU126
	movs	r7, #128	@ tmp143,
	lsls	r7, r7, #9	@ tmp143, tmp143,
	b	.L28		@
.LVL62:
.L26:
@ lib/filter/src/cic.cpp:77:         if (val < 0)
	.loc 1 77 9 view .LVU127
	mvns	r4, r3	@ tmp152, val_24
	asrs	r4, r4, #31	@ tmp151, tmp152,
	ands	r3, r4	@ val_24, tmp151
@ lib/filter/src/cic.cpp:81:         out[n] = val;
	.loc 1 81 16 view .LVU128
	strh	r3, [r1]	@ val_24, MEM[(uint16_t *)_37]
.LVL63:
	.loc 1 81 16 view .LVU129
.LBE9:
	.loc 1 75 5 is_stmt 1 discriminator 2 view .LVU130
	.loc 1 75 26 discriminator 1 view .LVU131
	adds	r2, r2, #4	@ ivtmp.53,
	adds	r1, r1, #2	@ ivtmp.55,
	cmp	r2, r8	@ ivtmp.53, _41
	beq	.L31		@,
.LVL64:
.L28:
.LBB10:
	.loc 1 76 9 view .LVU132
	.loc 1 77 9 view .LVU133
	.loc 1 81 9 view .LVU134
@ lib/filter/src/cic.cpp:76:         int32_t val = filter->out_buf[n] >> filter->attenuate_shift;
	.loc 1 76 17 is_stmt 0 view .LVU135
	ldr	r3, [r2]	@ MEM[(long int *)_36], MEM[(long int *)_36]
	mov	r0, ip	@ tmp140, tmp140
	ldr	r0, [r5, r0]	@ filter_18(D)->attenuate_shift, filter_18(D)->attenuate_shift
	asrs	r3, r3, r0	@ val_24, MEM[(long int *)_36], filter_18(D)->attenuate_shift
@ lib/filter/src/cic.cpp:79:         if (val > UINT16_MAX) 
	.loc 1 79 9 view .LVU136
	cmp	r3, r7	@ val_24, tmp143
	blt	.L26		@,
	ldr	r3, .L32	@ val_24,
	b	.L26		@
.LVL65:
.L31:
	.loc 1 79 9 view .LVU137
.LBE10:
.LBE8:
	.loc 1 83 5 is_stmt 1 view .LVU138
@ lib/filter/src/cic.cpp:83:     if (output_size == filter->out_cnt)
	.loc 1 83 32 is_stmt 0 view .LVU139
	ldr	r4, [r5, #12]	@ _7, filter_18(D)->out_cnt
@ lib/filter/src/cic.cpp:83:     if (output_size == filter->out_cnt)
	.loc 1 83 5 view .LVU140
	cmp	r4, r6	@ _7, <retval>
	bne	.L29		@,
	.loc 1 84 9 is_stmt 1 view .LVU141
@ lib/filter/src/cic.cpp:84:         filter->out_cnt = 0;
	.loc 1 84 25 is_stmt 0 view .LVU142
	movs	r3, #0	@ tmp145,
	str	r3, [r5, #12]	@ tmp145, filter_18(D)->out_cnt
.LVL66:
.L24:
@ lib/filter/src/cic.cpp:91: }
	.loc 1 91 1 view .LVU143
	movs	r0, r6	@, <retval>
	@ sp needed	@
.LVL67:
.LVL68:
	.loc 1 91 1 view .LVU144
	pop	{r6, r7}
	mov	r9, r7
	mov	r8, r6
	pop	{r3, r4, r5, r6, r7, pc}
.LVL69:
.L29:
	.loc 1 86 9 is_stmt 1 view .LVU145
@ lib/filter/src/cic.cpp:86:         memmove(filter->out_buf, filter->out_buf + output_size, sizeof(filter->out_buf[0]) * (filter->out_cnt - output_size));
	.loc 1 86 25 is_stmt 0 view .LVU146
	movs	r0, r5	@ _8, filter
	adds	r0, r0, #80	@ _8,
@ lib/filter/src/cic.cpp:86:         memmove(filter->out_buf, filter->out_buf + output_size, sizeof(filter->out_buf[0]) * (filter->out_cnt - output_size));
	.loc 1 86 111 view .LVU147
	subs	r4, r4, r6	@ _11, _7, <retval>
@ lib/filter/src/cic.cpp:86:         memmove(filter->out_buf, filter->out_buf + output_size, sizeof(filter->out_buf[0]) * (filter->out_cnt - output_size));
	.loc 1 86 92 view .LVU148
	lsls	r2, r4, #2	@ _12, _11,
@ lib/filter/src/cic.cpp:86:         memmove(filter->out_buf, filter->out_buf + output_size, sizeof(filter->out_buf[0]) * (filter->out_cnt - output_size));
	.loc 1 86 16 view .LVU149
	mov	r3, r9	@ _38, _38
	adds	r1, r0, r3	@ _10, _8, _38
	bl	memmove		@
.LVL70:
	.loc 1 87 9 is_stmt 1 view .LVU150
@ lib/filter/src/cic.cpp:87:         filter->out_cnt -= output_size;
	.loc 1 87 25 is_stmt 0 view .LVU151
	str	r4, [r5, #12]	@ _11, filter_18(D)->out_cnt
	b	.L24		@
.L33:
	.align	2
.L32:
	.word	65535
	.cfi_endproc
.LFE12:
	.size	_Z8cic_readP12cic_filter_tPtj, .-_Z8cic_readP12cic_filter_tPtj
	.text
.Letext0:
	.file 2 "C:/Users/andre/.platformio/packages/toolchain-rp2040-earlephilhower/arm-none-eabi/include/machine/_default_types.h"
	.file 3 "C:/Users/andre/.platformio/packages/toolchain-rp2040-earlephilhower/arm-none-eabi/include/sys/_stdint.h"
	.file 4 "C:/Users/andre/.platformio/packages/toolchain-rp2040-earlephilhower/lib/gcc/arm-none-eabi/14.2.0/include/stddef.h"
	.file 5 "C:/Users/andre/.platformio/packages/toolchain-rp2040-earlephilhower/arm-none-eabi/include/stdlib.h"
	.file 6 "C:/Users/andre/.platformio/packages/toolchain-rp2040-earlephilhower/arm-none-eabi/include/c++/14.2.0/cstdlib"
	.file 7 "C:/Users/andre/.platformio/packages/toolchain-rp2040-earlephilhower/arm-none-eabi/include/c++/14.2.0/bits/std_abs.h"
	.file 8 "C:/Users/andre/.platformio/packages/toolchain-rp2040-earlephilhower/arm-none-eabi/include/c++/14.2.0/stdlib.h"
	.file 9 "lib/filter/src/cic.h"
	.file 10 "C:/Users/andre/.platformio/packages/toolchain-rp2040-earlephilhower/arm-none-eabi/include/string.h"
	.file 11 "C:/Users/andre/.platformio/packages/toolchain-rp2040-earlephilhower/arm-none-eabi/include/c++/14.2.0/arm-none-eabi/thumb/bits/c++config.h"
	.section	.debug_info,"",%progbits
.Ldebug_info0:
	.4byte	0xb32
	.2byte	0x5
	.byte	0x1
	.byte	0x4
	.4byte	.Ldebug_abbrev0
	.uleb128 0x19
	.4byte	.LASF96
	.byte	0x21
	.4byte	.LASF97
	.4byte	.LASF98
	.4byte	.LLRL25
	.4byte	0
	.4byte	.Ldebug_line0
	.uleb128 0x4
	.byte	0x8
	.byte	0x7
	.4byte	.LASF0
	.uleb128 0x4
	.byte	0x4
	.byte	0x7
	.4byte	.LASF1
	.uleb128 0x4
	.byte	0x1
	.byte	0x6
	.4byte	.LASF2
	.uleb128 0x4
	.byte	0x1
	.byte	0x8
	.4byte	.LASF3
	.uleb128 0x4
	.byte	0x2
	.byte	0x5
	.4byte	.LASF4
	.uleb128 0x5
	.4byte	.LASF6
	.byte	0x2
	.byte	0x39
	.byte	0x19
	.4byte	0x55
	.uleb128 0x4
	.byte	0x2
	.byte	0x7
	.4byte	.LASF5
	.uleb128 0x5
	.4byte	.LASF7
	.byte	0x2
	.byte	0x4d
	.byte	0x18
	.4byte	0x68
	.uleb128 0x4
	.byte	0x4
	.byte	0x5
	.4byte	.LASF8
	.uleb128 0x5
	.4byte	.LASF9
	.byte	0x2
	.byte	0x4f
	.byte	0x19
	.4byte	0x7b
	.uleb128 0x4
	.byte	0x4
	.byte	0x7
	.4byte	.LASF10
	.uleb128 0x4
	.byte	0x8
	.byte	0x5
	.4byte	.LASF11
	.uleb128 0x1a
	.byte	0x4
	.byte	0x5
	.ascii	"int\000"
	.uleb128 0x5
	.4byte	.LASF12
	.byte	0x3
	.byte	0x24
	.byte	0x14
	.4byte	0x49
	.uleb128 0x11
	.4byte	0x90
	.uleb128 0x5
	.4byte	.LASF13
	.byte	0x3
	.byte	0x2c
	.byte	0x13
	.4byte	0x5c
	.uleb128 0x5
	.4byte	.LASF14
	.byte	0x3
	.byte	0x30
	.byte	0x14
	.4byte	0x6f
	.uleb128 0x5
	.4byte	.LASF15
	.byte	0x4
	.byte	0xd6
	.byte	0x17
	.4byte	0x2d
	.uleb128 0x4
	.byte	0x8
	.byte	0x4
	.4byte	.LASF16
	.uleb128 0x1b
	.byte	0x4
	.uleb128 0x6
	.4byte	0xd3
	.uleb128 0x1c
	.uleb128 0x6
	.4byte	0xd9
	.uleb128 0x4
	.byte	0x1
	.byte	0x8
	.4byte	.LASF17
	.uleb128 0x11
	.4byte	0xd9
	.uleb128 0x6
	.4byte	0xe0
	.uleb128 0x12
	.byte	0x8
	.byte	0x24
	.4byte	.LASF20
	.4byte	0x10f
	.uleb128 0x7
	.4byte	.LASF18
	.byte	0x5
	.byte	0x25
	.byte	0x7
	.4byte	0x89
	.byte	0
	.uleb128 0x13
	.ascii	"rem\000"
	.byte	0x26
	.byte	0x7
	.4byte	0x89
	.byte	0x4
	.byte	0
	.uleb128 0x5
	.4byte	.LASF19
	.byte	0x5
	.byte	0x27
	.byte	0x3
	.4byte	0xea
	.uleb128 0x12
	.byte	0x8
	.byte	0x2a
	.4byte	.LASF21
	.4byte	0x140
	.uleb128 0x7
	.4byte	.LASF18
	.byte	0x5
	.byte	0x2b
	.byte	0x8
	.4byte	0x68
	.byte	0
	.uleb128 0x13
	.ascii	"rem\000"
	.byte	0x2c
	.byte	0x8
	.4byte	0x68
	.byte	0x4
	.byte	0
	.uleb128 0x5
	.4byte	.LASF22
	.byte	0x5
	.byte	0x2d
	.byte	0x3
	.4byte	0x11b
	.uleb128 0x12
	.byte	0x10
	.byte	0x31
	.4byte	.LASF23
	.4byte	0x171
	.uleb128 0x7
	.4byte	.LASF18
	.byte	0x5
	.byte	0x32
	.byte	0x11
	.4byte	0x82
	.byte	0
	.uleb128 0x13
	.ascii	"rem\000"
	.byte	0x33
	.byte	0x11
	.4byte	0x82
	.byte	0x8
	.byte	0
	.uleb128 0x5
	.4byte	.LASF24
	.byte	0x5
	.byte	0x34
	.byte	0x3
	.4byte	0x14c
	.uleb128 0x5
	.4byte	.LASF25
	.byte	0x5
	.byte	0x39
	.byte	0xf
	.4byte	0x189
	.uleb128 0x6
	.4byte	0x18e
	.uleb128 0x1d
	.4byte	0x89
	.4byte	0x1a2
	.uleb128 0x1
	.4byte	0x1a2
	.uleb128 0x1
	.4byte	0x1a2
	.byte	0
	.uleb128 0x6
	.4byte	0x1a7
	.uleb128 0x1e
	.uleb128 0x1f
	.ascii	"std\000"
	.byte	0xb
	.2byte	0x134
	.byte	0xb
	.4byte	0x338
	.uleb128 0x2
	.byte	0x6
	.byte	0x83
	.byte	0xb
	.4byte	0x10f
	.uleb128 0x2
	.byte	0x6
	.byte	0x84
	.byte	0xb
	.4byte	0x140
	.uleb128 0x2
	.byte	0x6
	.byte	0x8a
	.byte	0xb
	.4byte	0x338
	.uleb128 0x2
	.byte	0x6
	.byte	0x90
	.byte	0xb
	.4byte	0x34e
	.uleb128 0x2
	.byte	0x6
	.byte	0x91
	.byte	0xb
	.4byte	0x36b
	.uleb128 0x2
	.byte	0x6
	.byte	0x92
	.byte	0xb
	.4byte	0x381
	.uleb128 0x2
	.byte	0x6
	.byte	0x93
	.byte	0xb
	.4byte	0x397
	.uleb128 0x2
	.byte	0x6
	.byte	0x95
	.byte	0xb
	.4byte	0x3c1
	.uleb128 0x2
	.byte	0x6
	.byte	0x98
	.byte	0xb
	.4byte	0x3dc
	.uleb128 0x2
	.byte	0x6
	.byte	0x9a
	.byte	0xb
	.4byte	0x3f2
	.uleb128 0x2
	.byte	0x6
	.byte	0x9d
	.byte	0xb
	.4byte	0x40d
	.uleb128 0x2
	.byte	0x6
	.byte	0x9e
	.byte	0xb
	.4byte	0x428
	.uleb128 0x2
	.byte	0x6
	.byte	0x9f
	.byte	0xb
	.4byte	0x459
	.uleb128 0x2
	.byte	0x6
	.byte	0xa1
	.byte	0xb
	.4byte	0x479
	.uleb128 0x2
	.byte	0x6
	.byte	0xa7
	.byte	0xb
	.4byte	0x498
	.uleb128 0x2
	.byte	0x6
	.byte	0xa9
	.byte	0xb
	.4byte	0x4a4
	.uleb128 0x2
	.byte	0x6
	.byte	0xaa
	.byte	0xb
	.4byte	0x4b4
	.uleb128 0x2
	.byte	0x6
	.byte	0xab
	.byte	0xb
	.4byte	0x4d4
	.uleb128 0x2
	.byte	0x6
	.byte	0xac
	.byte	0xb
	.4byte	0x4f4
	.uleb128 0x2
	.byte	0x6
	.byte	0xad
	.byte	0xb
	.4byte	0x514
	.uleb128 0x2
	.byte	0x6
	.byte	0xaf
	.byte	0xb
	.4byte	0x52a
	.uleb128 0x2
	.byte	0x6
	.byte	0xb0
	.byte	0xb
	.4byte	0x54f
	.uleb128 0x2
	.byte	0x6
	.byte	0xf4
	.byte	0x16
	.4byte	0x171
	.uleb128 0x2
	.byte	0x6
	.byte	0xf9
	.byte	0x16
	.4byte	0x5af
	.uleb128 0x2
	.byte	0x6
	.byte	0xfa
	.byte	0x16
	.4byte	0x5c9
	.uleb128 0x2
	.byte	0x6
	.byte	0xfc
	.byte	0x16
	.4byte	0x5e4
	.uleb128 0x2
	.byte	0x6
	.byte	0xfd
	.byte	0x16
	.4byte	0x63a
	.uleb128 0x2
	.byte	0x6
	.byte	0xfe
	.byte	0x16
	.4byte	0x5fa
	.uleb128 0x2
	.byte	0x6
	.byte	0xff
	.byte	0x16
	.4byte	0x61a
	.uleb128 0x20
	.byte	0x6
	.2byte	0x100
	.byte	0x16
	.4byte	0x65c
	.uleb128 0xb
	.ascii	"abs\000"
	.byte	0x4f
	.4byte	.LASF26
	.4byte	0xc5
	.4byte	0x2be
	.uleb128 0x1
	.4byte	0xc5
	.byte	0
	.uleb128 0xb
	.ascii	"abs\000"
	.byte	0x4b
	.4byte	.LASF27
	.4byte	0x655
	.4byte	0x2d6
	.uleb128 0x1
	.4byte	0x655
	.byte	0
	.uleb128 0xb
	.ascii	"abs\000"
	.byte	0x47
	.4byte	.LASF28
	.4byte	0x364
	.4byte	0x2ee
	.uleb128 0x1
	.4byte	0x364
	.byte	0
	.uleb128 0xb
	.ascii	"abs\000"
	.byte	0x3d
	.4byte	.LASF29
	.4byte	0x82
	.4byte	0x306
	.uleb128 0x1
	.4byte	0x82
	.byte	0
	.uleb128 0xb
	.ascii	"abs\000"
	.byte	0x38
	.4byte	.LASF30
	.4byte	0x68
	.4byte	0x31e
	.uleb128 0x1
	.4byte	0x68
	.byte	0
	.uleb128 0x14
	.ascii	"div\000"
	.byte	0xb5
	.4byte	.LASF52
	.4byte	0x140
	.uleb128 0x1
	.4byte	0x68
	.uleb128 0x1
	.4byte	0x68
	.byte	0
	.byte	0
	.uleb128 0x3
	.4byte	.LASF31
	.byte	0x5
	.byte	0x4c
	.byte	0x5
	.4byte	0x89
	.4byte	0x34e
	.uleb128 0x1
	.4byte	0xce
	.byte	0
	.uleb128 0x3
	.4byte	.LASF32
	.byte	0x5
	.byte	0x4d
	.byte	0x8
	.4byte	0x364
	.4byte	0x364
	.uleb128 0x1
	.4byte	0xe5
	.byte	0
	.uleb128 0x4
	.byte	0x8
	.byte	0x4
	.4byte	.LASF33
	.uleb128 0x3
	.4byte	.LASF34
	.byte	0x5
	.byte	0x51
	.byte	0x5
	.4byte	0x89
	.4byte	0x381
	.uleb128 0x1
	.4byte	0xe5
	.byte	0
	.uleb128 0x3
	.4byte	.LASF35
	.byte	0x5
	.byte	0x53
	.byte	0x6
	.4byte	0x68
	.4byte	0x397
	.uleb128 0x1
	.4byte	0xe5
	.byte	0
	.uleb128 0x3
	.4byte	.LASF36
	.byte	0x5
	.byte	0x55
	.byte	0x8
	.4byte	0xcc
	.4byte	0x3c1
	.uleb128 0x1
	.4byte	0x1a2
	.uleb128 0x1
	.4byte	0x1a2
	.uleb128 0x1
	.4byte	0xb9
	.uleb128 0x1
	.4byte	0xb9
	.uleb128 0x1
	.4byte	0x17d
	.byte	0
	.uleb128 0x21
	.ascii	"div\000"
	.byte	0x5
	.byte	0x5c
	.byte	0x7
	.4byte	0x10f
	.4byte	0x3dc
	.uleb128 0x1
	.4byte	0x89
	.uleb128 0x1
	.4byte	0x89
	.byte	0
	.uleb128 0x3
	.4byte	.LASF37
	.byte	0x5
	.byte	0x5f
	.byte	0x9
	.4byte	0xd4
	.4byte	0x3f2
	.uleb128 0x1
	.4byte	0xe5
	.byte	0
	.uleb128 0x3
	.4byte	.LASF38
	.byte	0x5
	.byte	0x6b
	.byte	0x8
	.4byte	0x140
	.4byte	0x40d
	.uleb128 0x1
	.4byte	0x68
	.uleb128 0x1
	.4byte	0x68
	.byte	0
	.uleb128 0x3
	.4byte	.LASF39
	.byte	0x5
	.byte	0x6d
	.byte	0x5
	.4byte	0x89
	.4byte	0x428
	.uleb128 0x1
	.4byte	0xe5
	.uleb128 0x1
	.4byte	0xb9
	.byte	0
	.uleb128 0x3
	.4byte	.LASF40
	.byte	0x5
	.byte	0x73
	.byte	0x8
	.4byte	0xb9
	.4byte	0x448
	.uleb128 0x1
	.4byte	0x448
	.uleb128 0x1
	.4byte	0xe5
	.uleb128 0x1
	.4byte	0xb9
	.byte	0
	.uleb128 0x6
	.4byte	0x44d
	.uleb128 0x4
	.byte	0x4
	.byte	0x7
	.4byte	.LASF41
	.uleb128 0x11
	.4byte	0x44d
	.uleb128 0x3
	.4byte	.LASF42
	.byte	0x5
	.byte	0x6f
	.byte	0x5
	.4byte	0x89
	.4byte	0x479
	.uleb128 0x1
	.4byte	0x448
	.uleb128 0x1
	.4byte	0xe5
	.uleb128 0x1
	.4byte	0xb9
	.byte	0
	.uleb128 0x15
	.4byte	.LASF43
	.byte	0x8f
	.4byte	0x498
	.uleb128 0x1
	.4byte	0xcc
	.uleb128 0x1
	.4byte	0xb9
	.uleb128 0x1
	.4byte	0xb9
	.uleb128 0x1
	.4byte	0x17d
	.byte	0
	.uleb128 0x22
	.4byte	.LASF99
	.byte	0x5
	.byte	0x90
	.byte	0x5
	.4byte	0x89
	.uleb128 0x15
	.4byte	.LASF44
	.byte	0x9f
	.4byte	0x4b4
	.uleb128 0x1
	.4byte	0x2d
	.byte	0
	.uleb128 0x3
	.4byte	.LASF45
	.byte	0x5
	.byte	0xa0
	.byte	0x8
	.4byte	0x364
	.4byte	0x4cf
	.uleb128 0x1
	.4byte	0xe5
	.uleb128 0x1
	.4byte	0x4cf
	.byte	0
	.uleb128 0x6
	.4byte	0xd4
	.uleb128 0x3
	.4byte	.LASF46
	.byte	0x5
	.byte	0xab
	.byte	0x6
	.4byte	0x68
	.4byte	0x4f4
	.uleb128 0x1
	.4byte	0xe5
	.uleb128 0x1
	.4byte	0x4cf
	.uleb128 0x1
	.4byte	0x89
	.byte	0
	.uleb128 0x3
	.4byte	.LASF47
	.byte	0x5
	.byte	0xad
	.byte	0xf
	.4byte	0x7b
	.4byte	0x514
	.uleb128 0x1
	.4byte	0xe5
	.uleb128 0x1
	.4byte	0x4cf
	.uleb128 0x1
	.4byte	0x89
	.byte	0
	.uleb128 0x3
	.4byte	.LASF48
	.byte	0x5
	.byte	0xbf
	.byte	0x5
	.4byte	0x89
	.4byte	0x52a
	.uleb128 0x1
	.4byte	0xe5
	.byte	0
	.uleb128 0x3
	.4byte	.LASF49
	.byte	0x5
	.byte	0x75
	.byte	0x8
	.4byte	0xb9
	.4byte	0x54a
	.uleb128 0x1
	.4byte	0xd4
	.uleb128 0x1
	.4byte	0x54a
	.uleb128 0x1
	.4byte	0xb9
	.byte	0
	.uleb128 0x6
	.4byte	0x454
	.uleb128 0x3
	.4byte	.LASF50
	.byte	0x5
	.byte	0x71
	.byte	0x5
	.4byte	0x89
	.4byte	0x56a
	.uleb128 0x1
	.4byte	0xd4
	.uleb128 0x1
	.4byte	0x44d
	.byte	0
	.uleb128 0x23
	.4byte	.LASF51
	.byte	0xb
	.2byte	0x159
	.byte	0xb
	.4byte	0x5c9
	.uleb128 0x2
	.byte	0x6
	.byte	0xcc
	.byte	0xb
	.4byte	0x171
	.uleb128 0x2
	.byte	0x6
	.byte	0xdc
	.byte	0xb
	.4byte	0x5c9
	.uleb128 0x2
	.byte	0x6
	.byte	0xe7
	.byte	0xb
	.4byte	0x5e4
	.uleb128 0x2
	.byte	0x6
	.byte	0xe8
	.byte	0xb
	.4byte	0x5fa
	.uleb128 0x2
	.byte	0x6
	.byte	0xe9
	.byte	0xb
	.4byte	0x61a
	.uleb128 0x2
	.byte	0x6
	.byte	0xeb
	.byte	0xb
	.4byte	0x63a
	.uleb128 0x2
	.byte	0x6
	.byte	0xec
	.byte	0xb
	.4byte	0x65c
	.uleb128 0x14
	.ascii	"div\000"
	.byte	0xd9
	.4byte	.LASF53
	.4byte	0x171
	.uleb128 0x1
	.4byte	0x82
	.uleb128 0x1
	.4byte	0x82
	.byte	0
	.byte	0
	.uleb128 0xc
	.4byte	.LASF54
	.2byte	0x10c
	.byte	0x9
	.4byte	0x171
	.4byte	0x5e4
	.uleb128 0x1
	.4byte	0x82
	.uleb128 0x1
	.4byte	0x82
	.byte	0
	.uleb128 0xc
	.4byte	.LASF55
	.2byte	0x107
	.byte	0xb
	.4byte	0x82
	.4byte	0x5fa
	.uleb128 0x1
	.4byte	0xe5
	.byte	0
	.uleb128 0xc
	.4byte	.LASF56
	.2byte	0x10d
	.byte	0xb
	.4byte	0x82
	.4byte	0x61a
	.uleb128 0x1
	.4byte	0xe5
	.uleb128 0x1
	.4byte	0x4cf
	.uleb128 0x1
	.4byte	0x89
	.byte	0
	.uleb128 0xc
	.4byte	.LASF57
	.2byte	0x111
	.byte	0x14
	.4byte	0x26
	.4byte	0x63a
	.uleb128 0x1
	.4byte	0xe5
	.uleb128 0x1
	.4byte	0x4cf
	.uleb128 0x1
	.4byte	0x89
	.byte	0
	.uleb128 0x3
	.4byte	.LASF58
	.byte	0x5
	.byte	0xa3
	.byte	0x7
	.4byte	0x655
	.4byte	0x655
	.uleb128 0x1
	.4byte	0xe5
	.uleb128 0x1
	.4byte	0x4cf
	.byte	0
	.uleb128 0x4
	.byte	0x4
	.byte	0x4
	.4byte	.LASF59
	.uleb128 0xc
	.4byte	.LASF60
	.2byte	0x144
	.byte	0x14
	.4byte	0xc5
	.4byte	0x677
	.uleb128 0x1
	.4byte	0xe5
	.uleb128 0x1
	.4byte	0x4cf
	.byte	0
	.uleb128 0x2
	.byte	0x8
	.byte	0x27
	.byte	0xc
	.4byte	0x338
	.uleb128 0x2
	.byte	0x8
	.byte	0x36
	.byte	0xc
	.4byte	0x10f
	.uleb128 0x2
	.byte	0x8
	.byte	0x37
	.byte	0xc
	.4byte	0x140
	.uleb128 0x2
	.byte	0x8
	.byte	0x39
	.byte	0xc
	.4byte	0x2a6
	.uleb128 0x2
	.byte	0x8
	.byte	0x39
	.byte	0xc
	.4byte	0x2be
	.uleb128 0x2
	.byte	0x8
	.byte	0x39
	.byte	0xc
	.4byte	0x2d6
	.uleb128 0x2
	.byte	0x8
	.byte	0x39
	.byte	0xc
	.4byte	0x2ee
	.uleb128 0x2
	.byte	0x8
	.byte	0x39
	.byte	0xc
	.4byte	0x306
	.uleb128 0x2
	.byte	0x8
	.byte	0x3a
	.byte	0xc
	.4byte	0x34e
	.uleb128 0x2
	.byte	0x8
	.byte	0x3b
	.byte	0xc
	.4byte	0x36b
	.uleb128 0x2
	.byte	0x8
	.byte	0x3c
	.byte	0xc
	.4byte	0x381
	.uleb128 0x2
	.byte	0x8
	.byte	0x3d
	.byte	0xc
	.4byte	0x397
	.uleb128 0x2
	.byte	0x8
	.byte	0x3f
	.byte	0xc
	.4byte	0x5af
	.uleb128 0x2
	.byte	0x8
	.byte	0x3f
	.byte	0xc
	.4byte	0x31e
	.uleb128 0x2
	.byte	0x8
	.byte	0x3f
	.byte	0xc
	.4byte	0x3c1
	.uleb128 0x2
	.byte	0x8
	.byte	0x41
	.byte	0xc
	.4byte	0x3dc
	.uleb128 0x2
	.byte	0x8
	.byte	0x43
	.byte	0xc
	.4byte	0x3f2
	.uleb128 0x2
	.byte	0x8
	.byte	0x46
	.byte	0xc
	.4byte	0x40d
	.uleb128 0x2
	.byte	0x8
	.byte	0x47
	.byte	0xc
	.4byte	0x428
	.uleb128 0x2
	.byte	0x8
	.byte	0x48
	.byte	0xc
	.4byte	0x459
	.uleb128 0x2
	.byte	0x8
	.byte	0x4a
	.byte	0xc
	.4byte	0x479
	.uleb128 0x2
	.byte	0x8
	.byte	0x4b
	.byte	0xc
	.4byte	0x498
	.uleb128 0x2
	.byte	0x8
	.byte	0x4d
	.byte	0xc
	.4byte	0x4a4
	.uleb128 0x2
	.byte	0x8
	.byte	0x4e
	.byte	0xc
	.4byte	0x4b4
	.uleb128 0x2
	.byte	0x8
	.byte	0x4f
	.byte	0xc
	.4byte	0x4d4
	.uleb128 0x2
	.byte	0x8
	.byte	0x50
	.byte	0xc
	.4byte	0x4f4
	.uleb128 0x2
	.byte	0x8
	.byte	0x51
	.byte	0xc
	.4byte	0x514
	.uleb128 0x2
	.byte	0x8
	.byte	0x53
	.byte	0xc
	.4byte	0x52a
	.uleb128 0x2
	.byte	0x8
	.byte	0x54
	.byte	0xc
	.4byte	0x54f
	.uleb128 0x24
	.2byte	0x260
	.byte	0x9
	.byte	0xa
	.byte	0x10
	.4byte	.LASF100
	.4byte	0x7ec
	.uleb128 0x7
	.4byte	.LASF61
	.byte	0x9
	.byte	0xb
	.byte	0x11
	.4byte	0xad
	.byte	0
	.uleb128 0x7
	.4byte	.LASF62
	.byte	0x9
	.byte	0xc
	.byte	0x11
	.4byte	0xad
	.byte	0x4
	.uleb128 0x7
	.4byte	.LASF63
	.byte	0x9
	.byte	0xd
	.byte	0x11
	.4byte	0xad
	.byte	0x8
	.uleb128 0x7
	.4byte	.LASF64
	.byte	0x9
	.byte	0xe
	.byte	0x11
	.4byte	0xad
	.byte	0xc
	.uleb128 0x7
	.4byte	.LASF65
	.byte	0x9
	.byte	0xf
	.byte	0x11
	.4byte	0x7ec
	.byte	0x10
	.uleb128 0x7
	.4byte	.LASF66
	.byte	0x9
	.byte	0x10
	.byte	0x11
	.4byte	0x7fc
	.byte	0x50
	.uleb128 0xf
	.4byte	.LASF67
	.byte	0x11
	.4byte	0xad
	.2byte	0x250
	.uleb128 0xf
	.4byte	.LASF68
	.byte	0x12
	.4byte	0xad
	.2byte	0x254
	.uleb128 0xf
	.4byte	.LASF69
	.byte	0x13
	.4byte	0xad
	.2byte	0x258
	.uleb128 0xf
	.4byte	.LASF70
	.byte	0x14
	.4byte	0x655
	.2byte	0x25c
	.byte	0
	.uleb128 0x16
	.4byte	0xa1
	.4byte	0x7fc
	.uleb128 0x17
	.4byte	0x2d
	.byte	0xf
	.byte	0
	.uleb128 0x16
	.4byte	0xa1
	.4byte	0x80c
	.uleb128 0x17
	.4byte	0x2d
	.byte	0x7f
	.byte	0
	.uleb128 0x5
	.4byte	.LASF71
	.byte	0x9
	.byte	0x15
	.byte	0x3
	.4byte	0x75f
	.uleb128 0x3
	.4byte	.LASF72
	.byte	0xa
	.byte	0x20
	.byte	0x9
	.4byte	0xcc
	.4byte	0x838
	.uleb128 0x1
	.4byte	0xcc
	.uleb128 0x1
	.4byte	0x1a2
	.uleb128 0x1
	.4byte	0xb9
	.byte	0
	.uleb128 0x3
	.4byte	.LASF73
	.byte	0xa
	.byte	0x21
	.byte	0x9
	.4byte	0xcc
	.4byte	0x858
	.uleb128 0x1
	.4byte	0xcc
	.uleb128 0x1
	.4byte	0x89
	.uleb128 0x1
	.4byte	0xb9
	.byte	0
	.uleb128 0x25
	.4byte	.LASF101
	.byte	0x1
	.byte	0x45
	.byte	0x8
	.4byte	.LASF102
	.4byte	0xb9
	.4byte	.LFB12
	.4byte	.LFE12-.LFB12
	.uleb128 0x1
	.byte	0x9c
	.4byte	0x919
	.uleb128 0x8
	.4byte	.LASF74
	.byte	0x45
	.byte	0x1f
	.4byte	0x919
	.4byte	.LLST19
	.4byte	.LVUS19
	.uleb128 0x26
	.ascii	"out\000"
	.byte	0x1
	.byte	0x45
	.byte	0x31
	.4byte	0x91e
	.4byte	.LLST20
	.4byte	.LVUS20
	.uleb128 0x8
	.4byte	.LASF75
	.byte	0x45
	.byte	0x3d
	.4byte	0xb9
	.4byte	.LLST21
	.4byte	.LVUS21
	.uleb128 0x9
	.4byte	.LASF76
	.byte	0x46
	.byte	0xc
	.4byte	0xb9
	.4byte	.LLST22
	.4byte	.LVUS22
	.uleb128 0x27
	.4byte	.LBB8
	.4byte	.LBE8-.LBB8
	.4byte	0x8f4
	.uleb128 0x28
	.ascii	"n\000"
	.byte	0x1
	.byte	0x4b
	.byte	0x11
	.4byte	0xb9
	.uleb128 0x29
	.4byte	.LLRL23
	.uleb128 0x10
	.ascii	"val\000"
	.byte	0x4c
	.byte	0x11
	.4byte	0xa1
	.4byte	.LLST24
	.4byte	.LVUS24
	.byte	0
	.byte	0
	.uleb128 0x2a
	.4byte	.LVL70
	.4byte	0x818
	.uleb128 0xa
	.uleb128 0x1
	.byte	0x50
	.uleb128 0x3
	.byte	0x75
	.sleb128 80
	.uleb128 0xa
	.uleb128 0x1
	.byte	0x51
	.uleb128 0x7
	.byte	0x75
	.sleb128 0
	.byte	0x79
	.sleb128 0
	.byte	0x22
	.byte	0x23
	.uleb128 0x50
	.uleb128 0xa
	.uleb128 0x1
	.byte	0x52
	.uleb128 0x4
	.byte	0x74
	.sleb128 0
	.byte	0x32
	.byte	0x24
	.byte	0
	.byte	0
	.uleb128 0x6
	.4byte	0x80c
	.uleb128 0x6
	.4byte	0x90
	.uleb128 0x18
	.4byte	.LASF85
	.byte	0x16
	.4byte	.LASF87
	.4byte	.LFB11
	.4byte	.LFE11-.LFB11
	.uleb128 0x1
	.byte	0x9c
	.4byte	0xa44
	.uleb128 0x8
	.4byte	.LASF74
	.byte	0x16
	.byte	0x1e
	.4byte	0x919
	.4byte	.LLST5
	.4byte	.LVUS5
	.uleb128 0x8
	.4byte	.LASF77
	.byte	0x16
	.byte	0x36
	.4byte	0xa44
	.4byte	.LLST6
	.4byte	.LVUS6
	.uleb128 0x8
	.4byte	.LASF78
	.byte	0x17
	.byte	0x17
	.4byte	0xb9
	.4byte	.LLST7
	.4byte	.LVUS7
	.uleb128 0x8
	.4byte	.LASF79
	.byte	0x17
	.byte	0x26
	.4byte	0xb9
	.4byte	.LLST8
	.4byte	.LVUS8
	.uleb128 0x10
	.ascii	"n\000"
	.byte	0x18
	.byte	0xc
	.4byte	0xb9
	.4byte	.LLST9
	.4byte	.LVUS9
	.uleb128 0x10
	.ascii	"ord\000"
	.byte	0x18
	.byte	0xf
	.4byte	0xb9
	.4byte	.LLST10
	.4byte	.LVUS10
	.uleb128 0x9
	.4byte	.LASF80
	.byte	0x19
	.byte	0xd
	.4byte	0xa1
	.4byte	.LLST11
	.4byte	.LVUS11
	.uleb128 0x9
	.4byte	.LASF81
	.byte	0x19
	.byte	0x17
	.4byte	0xa1
	.4byte	.LLST12
	.4byte	.LVUS12
	.uleb128 0x9
	.4byte	.LASF82
	.byte	0x1a
	.byte	0xe
	.4byte	0xad
	.4byte	.LLST13
	.4byte	.LVUS13
	.uleb128 0x9
	.4byte	.LASF63
	.byte	0x1b
	.byte	0xe
	.4byte	0xad
	.4byte	.LLST14
	.4byte	.LVUS14
	.uleb128 0x9
	.4byte	.LASF61
	.byte	0x1c
	.byte	0xe
	.4byte	0xad
	.4byte	.LLST15
	.4byte	.LVUS15
	.uleb128 0x9
	.4byte	.LASF83
	.byte	0x1d
	.byte	0xe
	.4byte	0xad
	.4byte	.LLST16
	.4byte	.LVUS16
	.uleb128 0x2b
	.4byte	.LLRL17
	.4byte	0xa3a
	.uleb128 0x9
	.4byte	.LASF84
	.byte	0x20
	.byte	0x12
	.4byte	0xa49
	.4byte	.LLST18
	.4byte	.LVUS18
	.byte	0
	.uleb128 0xd
	.4byte	.LVL53
	.4byte	0xb08
	.byte	0
	.uleb128 0x6
	.4byte	0x9c
	.uleb128 0x6
	.4byte	0xa1
	.uleb128 0x18
	.4byte	.LASF86
	.byte	0x7
	.4byte	.LASF88
	.4byte	.LFB10
	.4byte	.LFE10-.LFB10
	.uleb128 0x1
	.byte	0x9c
	.4byte	0xb08
	.uleb128 0x8
	.4byte	.LASF74
	.byte	0x7
	.byte	0x1d
	.4byte	0x919
	.4byte	.LLST0
	.4byte	.LVUS0
	.uleb128 0x8
	.4byte	.LASF61
	.byte	0x8
	.byte	0x18
	.4byte	0xad
	.4byte	.LLST1
	.4byte	.LVUS1
	.uleb128 0x8
	.4byte	.LASF89
	.byte	0x9
	.byte	0x17
	.4byte	0xad
	.4byte	.LLST2
	.4byte	.LVUS2
	.uleb128 0x10
	.ascii	"n\000"
	.byte	0xe
	.byte	0xe
	.4byte	0xad
	.4byte	.LLST3
	.4byte	.LVUS3
	.uleb128 0x9
	.4byte	.LASF90
	.byte	0x11
	.byte	0xe
	.4byte	0xad
	.4byte	.LLST4
	.4byte	.LVUS4
	.uleb128 0x2c
	.4byte	.LVL3
	.4byte	0xb11
	.4byte	0xae3
	.uleb128 0xa
	.uleb128 0x1
	.byte	0x50
	.uleb128 0x2
	.byte	0x76
	.sleb128 0
	.uleb128 0xa
	.uleb128 0x1
	.byte	0x51
	.uleb128 0x1
	.byte	0x30
	.uleb128 0xa
	.uleb128 0x1
	.byte	0x52
	.uleb128 0x3
	.byte	0xa
	.2byte	0x260
	.byte	0
	.uleb128 0xd
	.4byte	.LVL9
	.4byte	0xb1a
	.uleb128 0xd
	.4byte	.LVL11
	.4byte	0xb23
	.uleb128 0xd
	.4byte	.LVL13
	.4byte	0xb23
	.uleb128 0xd
	.4byte	.LVL14
	.4byte	0xb2c
	.byte	0
	.uleb128 0xe
	.4byte	.LASF91
	.4byte	.LASF91
	.uleb128 0xe
	.4byte	.LASF73
	.4byte	.LASF92
	.uleb128 0xe
	.4byte	.LASF93
	.4byte	.LASF93
	.uleb128 0xe
	.4byte	.LASF94
	.4byte	.LASF94
	.uleb128 0xe
	.4byte	.LASF95
	.4byte	.LASF95
	.byte	0
	.section	.debug_abbrev,"",%progbits
.Ldebug_abbrev0:
	.uleb128 0x1
	.uleb128 0x5
	.byte	0
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x2
	.uleb128 0x8
	.byte	0
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x18
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x3
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3f
	.uleb128 0x19
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x3c
	.uleb128 0x19
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x4
	.uleb128 0x24
	.byte	0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3e
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0xe
	.byte	0
	.byte	0
	.uleb128 0x5
	.uleb128 0x16
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x6
	.uleb128 0xf
	.byte	0
	.uleb128 0xb
	.uleb128 0x21
	.sleb128 4
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x7
	.uleb128 0xd
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x38
	.uleb128 0xb
	.byte	0
	.byte	0
	.uleb128 0x8
	.uleb128 0x5
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0x21
	.sleb128 1
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x17
	.uleb128 0x2137
	.uleb128 0x17
	.byte	0
	.byte	0
	.uleb128 0x9
	.uleb128 0x34
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0x21
	.sleb128 1
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x17
	.uleb128 0x2137
	.uleb128 0x17
	.byte	0
	.byte	0
	.uleb128 0xa
	.uleb128 0x49
	.byte	0
	.uleb128 0x2
	.uleb128 0x18
	.uleb128 0x7e
	.uleb128 0x18
	.byte	0
	.byte	0
	.uleb128 0xb
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3f
	.uleb128 0x19
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0x21
	.sleb128 7
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0x21
	.sleb128 3
	.uleb128 0x6e
	.uleb128 0xe
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x3c
	.uleb128 0x19
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0xc
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3f
	.uleb128 0x19
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0x21
	.sleb128 5
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x3c
	.uleb128 0x19
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0xd
	.uleb128 0x48
	.byte	0
	.uleb128 0x7d
	.uleb128 0x1
	.uleb128 0x7f
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0xe
	.uleb128 0x2e
	.byte	0
	.uleb128 0x3f
	.uleb128 0x19
	.uleb128 0x3c
	.uleb128 0x19
	.uleb128 0x6e
	.uleb128 0xe
	.uleb128 0x3
	.uleb128 0xe
	.byte	0
	.byte	0
	.uleb128 0xf
	.uleb128 0xd
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0x21
	.sleb128 9
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0x21
	.sleb128 17
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x38
	.uleb128 0x5
	.byte	0
	.byte	0
	.uleb128 0x10
	.uleb128 0x34
	.byte	0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0x21
	.sleb128 1
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x17
	.uleb128 0x2137
	.uleb128 0x17
	.byte	0
	.byte	0
	.uleb128 0x11
	.uleb128 0x26
	.byte	0
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x12
	.uleb128 0x13
	.byte	0x1
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3a
	.uleb128 0x21
	.sleb128 5
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0x21
	.sleb128 1
	.uleb128 0x6e
	.uleb128 0xe
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x13
	.uleb128 0xd
	.byte	0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0x21
	.sleb128 5
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x38
	.uleb128 0xb
	.byte	0
	.byte	0
	.uleb128 0x14
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3f
	.uleb128 0x19
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0x21
	.sleb128 6
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0x21
	.sleb128 3
	.uleb128 0x6e
	.uleb128 0xe
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x3c
	.uleb128 0x19
	.byte	0
	.byte	0
	.uleb128 0x15
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3f
	.uleb128 0x19
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0x21
	.sleb128 5
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0x21
	.sleb128 6
	.uleb128 0x3c
	.uleb128 0x19
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x16
	.uleb128 0x1
	.byte	0x1
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x17
	.uleb128 0x21
	.byte	0
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2f
	.uleb128 0xb
	.byte	0
	.byte	0
	.uleb128 0x18
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3f
	.uleb128 0x19
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0x21
	.sleb128 1
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0x21
	.sleb128 6
	.uleb128 0x6e
	.uleb128 0xe
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x6
	.uleb128 0x40
	.uleb128 0x18
	.uleb128 0x7a
	.uleb128 0x19
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x19
	.uleb128 0x11
	.byte	0x1
	.uleb128 0x25
	.uleb128 0xe
	.uleb128 0x13
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x1b
	.uleb128 0xe
	.uleb128 0x55
	.uleb128 0x17
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x10
	.uleb128 0x17
	.byte	0
	.byte	0
	.uleb128 0x1a
	.uleb128 0x24
	.byte	0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3e
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0x8
	.byte	0
	.byte	0
	.uleb128 0x1b
	.uleb128 0xf
	.byte	0
	.uleb128 0xb
	.uleb128 0xb
	.byte	0
	.byte	0
	.uleb128 0x1c
	.uleb128 0x15
	.byte	0
	.byte	0
	.byte	0
	.uleb128 0x1d
	.uleb128 0x15
	.byte	0x1
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x1e
	.uleb128 0x26
	.byte	0
	.byte	0
	.byte	0
	.uleb128 0x1f
	.uleb128 0x39
	.byte	0x1
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x20
	.uleb128 0x8
	.byte	0
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x18
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x21
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3f
	.uleb128 0x19
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x3c
	.uleb128 0x19
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x22
	.uleb128 0x2e
	.byte	0
	.uleb128 0x3f
	.uleb128 0x19
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x3c
	.uleb128 0x19
	.byte	0
	.byte	0
	.uleb128 0x23
	.uleb128 0x39
	.byte	0x1
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x24
	.uleb128 0x13
	.byte	0x1
	.uleb128 0xb
	.uleb128 0x5
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x6e
	.uleb128 0xe
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x25
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3f
	.uleb128 0x19
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x6e
	.uleb128 0xe
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x6
	.uleb128 0x40
	.uleb128 0x18
	.uleb128 0x7a
	.uleb128 0x19
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x26
	.uleb128 0x5
	.byte	0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x17
	.uleb128 0x2137
	.uleb128 0x17
	.byte	0
	.byte	0
	.uleb128 0x27
	.uleb128 0xb
	.byte	0x1
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x6
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x28
	.uleb128 0x34
	.byte	0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x29
	.uleb128 0xb
	.byte	0x1
	.uleb128 0x55
	.uleb128 0x17
	.byte	0
	.byte	0
	.uleb128 0x2a
	.uleb128 0x48
	.byte	0x1
	.uleb128 0x7d
	.uleb128 0x1
	.uleb128 0x7f
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x2b
	.uleb128 0xb
	.byte	0x1
	.uleb128 0x55
	.uleb128 0x17
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x2c
	.uleb128 0x48
	.byte	0x1
	.uleb128 0x7d
	.uleb128 0x1
	.uleb128 0x7f
	.uleb128 0x13
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.byte	0
	.section	.debug_loclists,"",%progbits
	.4byte	.Ldebug_loc3-.Ldebug_loc2
.Ldebug_loc2:
	.2byte	0x5
	.byte	0x4
	.byte	0
	.4byte	0
.Ldebug_loc0:
.LVUS19:
	.uleb128 0
	.uleb128 .LVU127
	.uleb128 .LVU127
	.uleb128 .LVU144
	.uleb128 .LVU144
	.uleb128 .LVU145
	.uleb128 .LVU145
	.uleb128 0
.LLST19:
	.byte	0x6
	.4byte	.LVL60
	.byte	0x4
	.uleb128 .LVL60-.LVL60
	.uleb128 .LVL62-.LVL60
	.uleb128 0x1
	.byte	0x50
	.byte	0x4
	.uleb128 .LVL62-.LVL60
	.uleb128 .LVL67-.LVL60
	.uleb128 0x1
	.byte	0x55
	.byte	0x4
	.uleb128 .LVL67-.LVL60
	.uleb128 .LVL69-.LVL60
	.uleb128 0xa
	.byte	0xa3
	.uleb128 0x3
	.byte	0xa5
	.uleb128 0
	.uleb128 0x26
	.byte	0xa8
	.uleb128 0x2d
	.byte	0xa8
	.uleb128 0
	.byte	0x9f
	.byte	0x4
	.uleb128 .LVL69-.LVL60
	.uleb128 .LFE12-.LVL60
	.uleb128 0x1
	.byte	0x55
	.byte	0
.LVUS20:
	.uleb128 0
	.uleb128 .LVU127
	.uleb128 .LVU127
	.uleb128 0
.LLST20:
	.byte	0x6
	.4byte	.LVL60
	.byte	0x4
	.uleb128 .LVL60-.LVL60
	.uleb128 .LVL62-.LVL60
	.uleb128 0x1
	.byte	0x51
	.byte	0x4
	.uleb128 .LVL62-.LVL60
	.uleb128 .LFE12-.LVL60
	.uleb128 0xa
	.byte	0xa3
	.uleb128 0x3
	.byte	0xa5
	.uleb128 0x1
	.uleb128 0x26
	.byte	0xa8
	.uleb128 0x2d
	.byte	0xa8
	.uleb128 0
	.byte	0x9f
	.byte	0
.LVUS21:
	.uleb128 0
	.uleb128 .LVU124
	.uleb128 .LVU124
	.uleb128 .LVU144
	.uleb128 .LVU144
	.uleb128 .LVU145
	.uleb128 .LVU145
	.uleb128 0
.LLST21:
	.byte	0x6
	.4byte	.LVL60
	.byte	0x4
	.uleb128 .LVL60-.LVL60
	.uleb128 .LVL61-.LVL60
	.uleb128 0x1
	.byte	0x52
	.byte	0x4
	.uleb128 .LVL61-.LVL60
	.uleb128 .LVL68-.LVL60
	.uleb128 0x1
	.byte	0x56
	.byte	0x4
	.uleb128 .LVL68-.LVL60
	.uleb128 .LVL69-.LVL60
	.uleb128 0x1
	.byte	0x50
	.byte	0x4
	.uleb128 .LVL69-.LVL60
	.uleb128 .LFE12-.LVL60
	.uleb128 0x1
	.byte	0x56
	.byte	0
.LVUS22:
	.uleb128 .LVU122
	.uleb128 .LVU144
	.uleb128 .LVU144
	.uleb128 .LVU145
	.uleb128 .LVU145
	.uleb128 0
.LLST22:
	.byte	0x6
	.4byte	.LVL61
	.byte	0x4
	.uleb128 .LVL61-.LVL61
	.uleb128 .LVL68-.LVL61
	.uleb128 0x1
	.byte	0x56
	.byte	0x4
	.uleb128 .LVL68-.LVL61
	.uleb128 .LVL69-.LVL61
	.uleb128 0x1
	.byte	0x50
	.byte	0x4
	.uleb128 .LVL69-.LVL61
	.uleb128 .LFE12-.LVL61
	.uleb128 0x1
	.byte	0x56
	.byte	0
.LVUS24:
	.uleb128 .LVU127
	.uleb128 .LVU129
	.uleb128 .LVU133
	.uleb128 .LVU134
	.uleb128 .LVU134
	.uleb128 .LVU137
.LLST24:
	.byte	0x6
	.4byte	.LVL62
	.byte	0x4
	.uleb128 .LVL62-.LVL62
	.uleb128 .LVL63-.LVL62
	.uleb128 0x1f
	.byte	0x72
	.sleb128 0
	.byte	0x6
	.byte	0x75
	.sleb128 600
	.byte	0x6
	.byte	0x26
	.byte	0x12
	.byte	0xa
	.2byte	0xffff
	.byte	0x16
	.byte	0x14
	.byte	0x2d
	.byte	0x28
	.2byte	0x1
	.byte	0x16
	.byte	0x13
	.byte	0x12
	.byte	0x30
	.byte	0x16
	.byte	0x14
	.byte	0x2b
	.byte	0x28
	.2byte	0x1
	.byte	0x16
	.byte	0x13
	.byte	0x9f
	.byte	0x4
	.uleb128 .LVL64-.LVL62
	.uleb128 .LVL64-.LVL62
	.uleb128 0x9
	.byte	0x72
	.sleb128 0
	.byte	0x6
	.byte	0x75
	.sleb128 600
	.byte	0x6
	.byte	0x26
	.byte	0x9f
	.byte	0x4
	.uleb128 .LVL64-.LVL62
	.uleb128 .LVL65-.LVL62
	.uleb128 0x1f
	.byte	0x72
	.sleb128 0
	.byte	0x6
	.byte	0x75
	.sleb128 600
	.byte	0x6
	.byte	0x26
	.byte	0x12
	.byte	0xa
	.2byte	0xffff
	.byte	0x16
	.byte	0x14
	.byte	0x2d
	.byte	0x28
	.2byte	0x1
	.byte	0x16
	.byte	0x13
	.byte	0x12
	.byte	0x30
	.byte	0x16
	.byte	0x14
	.byte	0x2b
	.byte	0x28
	.2byte	0x1
	.byte	0x16
	.byte	0x13
	.byte	0x9f
	.byte	0
.LVUS5:
	.uleb128 0
	.uleb128 .LVU35
	.uleb128 .LVU35
	.uleb128 .LVU113
	.uleb128 .LVU113
	.uleb128 .LVU114
	.uleb128 .LVU114
	.uleb128 0
.LLST5:
	.byte	0x6
	.4byte	.LVL17
	.byte	0x4
	.uleb128 .LVL17-.LVL17
	.uleb128 .LVL18-.LVL17
	.uleb128 0x1
	.byte	0x50
	.byte	0x4
	.uleb128 .LVL18-.LVL17
	.uleb128 .LVL54-.LVL17
	.uleb128 0x1
	.byte	0x57
	.byte	0x4
	.uleb128 .LVL54-.LVL17
	.uleb128 .LVL57-.LVL17
	.uleb128 0xa
	.byte	0xa3
	.uleb128 0x3
	.byte	0xa5
	.uleb128 0
	.uleb128 0x26
	.byte	0xa8
	.uleb128 0x2d
	.byte	0xa8
	.uleb128 0
	.byte	0x9f
	.byte	0x4
	.uleb128 .LVL57-.LVL17
	.uleb128 .LFE11-.LVL17
	.uleb128 0x1
	.byte	0x57
	.byte	0
.LVUS6:
	.uleb128 0
	.uleb128 .LVU41
	.uleb128 .LVU41
	.uleb128 .LVU54
	.uleb128 .LVU54
	.uleb128 .LVU102
	.uleb128 .LVU102
	.uleb128 .LVU114
	.uleb128 .LVU114
	.uleb128 .LVU115
	.uleb128 .LVU115
	.uleb128 0
.LLST6:
	.byte	0x6
	.4byte	.LVL17
	.byte	0x4
	.uleb128 .LVL17-.LVL17
	.uleb128 .LVL20-.LVL17
	.uleb128 0x1
	.byte	0x51
	.byte	0x4
	.uleb128 .LVL20-.LVL17
	.uleb128 .LVL28-.LVL17
	.uleb128 0x1
	.byte	0x5c
	.byte	0x4
	.uleb128 .LVL28-.LVL17
	.uleb128 .LVL51-.LVL17
	.uleb128 0x1
	.byte	0x50
	.byte	0x4
	.uleb128 .LVL51-.LVL17
	.uleb128 .LVL57-.LVL17
	.uleb128 0xa
	.byte	0xa3
	.uleb128 0x3
	.byte	0xa5
	.uleb128 0x1
	.uleb128 0x26
	.byte	0xa8
	.uleb128 0x2d
	.byte	0xa8
	.uleb128 0
	.byte	0x9f
	.byte	0x4
	.uleb128 .LVL57-.LVL17
	.uleb128 .LVL58-.LVL17
	.uleb128 0x1
	.byte	0x5c
	.byte	0x4
	.uleb128 .LVL58-.LVL17
	.uleb128 .LFE11-.LVL17
	.uleb128 0x1
	.byte	0x50
	.byte	0
.LVUS7:
	.uleb128 0
	.uleb128 .LVU50
	.uleb128 .LVU50
	.uleb128 .LVU53
	.uleb128 .LVU53
	.uleb128 .LVU55
	.uleb128 .LVU55
	.uleb128 .LVU102
	.uleb128 .LVU102
	.uleb128 .LVU111
	.uleb128 .LVU111
	.uleb128 .LVU114
	.uleb128 .LVU114
	.uleb128 .LVU115
	.uleb128 .LVU115
	.uleb128 0
.LLST7:
	.byte	0x6
	.4byte	.LVL17
	.byte	0x4
	.uleb128 .LVL17-.LVL17
	.uleb128 .LVL26-.LVL17
	.uleb128 0x1
	.byte	0x52
	.byte	0x4
	.uleb128 .LVL26-.LVL17
	.uleb128 .LVL27-.LVL17
	.uleb128 0x1
	.byte	0x50
	.byte	0x4
	.uleb128 .LVL27-.LVL17
	.uleb128 .LVL29-.LVL17
	.uleb128 0x1
	.byte	0x53
	.byte	0x4
	.uleb128 .LVL29-.LVL17
	.uleb128 .LVL51-.LVL17
	.uleb128 0x1
	.byte	0x5c
	.byte	0x4
	.uleb128 .LVL51-.LVL17
	.uleb128 .LVL53-1-.LVL17
	.uleb128 0x1
	.byte	0x50
	.byte	0x4
	.uleb128 .LVL53-1-.LVL17
	.uleb128 .LVL57-.LVL17
	.uleb128 0xa
	.byte	0xa3
	.uleb128 0x3
	.byte	0xa5
	.uleb128 0x2
	.uleb128 0x26
	.byte	0xa8
	.uleb128 0x2d
	.byte	0xa8
	.uleb128 0
	.byte	0x9f
	.byte	0x4
	.uleb128 .LVL57-.LVL17
	.uleb128 .LVL58-.LVL17
	.uleb128 0x1
	.byte	0x50
	.byte	0x4
	.uleb128 .LVL58-.LVL17
	.uleb128 .LFE11-.LVL17
	.uleb128 0x1
	.byte	0x5c
	.byte	0
.LVUS8:
	.uleb128 0
	.uleb128 .LVU43
	.uleb128 .LVU43
	.uleb128 .LVU113
	.uleb128 .LVU113
	.uleb128 .LVU114
	.uleb128 .LVU114
	.uleb128 0
.LLST8:
	.byte	0x6
	.4byte	.LVL17
	.byte	0x4
	.uleb128 .LVL17-.LVL17
	.uleb128 .LVL21-.LVL17
	.uleb128 0x1
	.byte	0x53
	.byte	0x4
	.uleb128 .LVL21-.LVL17
	.uleb128 .LVL56-.LVL17
	.uleb128 0x1
	.byte	0x5b
	.byte	0x4
	.uleb128 .LVL56-.LVL17
	.uleb128 .LVL57-.LVL17
	.uleb128 0xa
	.byte	0xa3
	.uleb128 0x3
	.byte	0xa5
	.uleb128 0x3
	.uleb128 0x26
	.byte	0xa8
	.uleb128 0x2d
	.byte	0xa8
	.uleb128 0
	.byte	0x9f
	.byte	0x4
	.uleb128 .LVL57-.LVL17
	.uleb128 .LFE11-.LVL17
	.uleb128 0x1
	.byte	0x5b
	.byte	0
.LVUS9:
	.uleb128 .LVU48
	.uleb128 .LVU55
	.uleb128 .LVU55
	.uleb128 .LVU102
	.uleb128 .LVU114
	.uleb128 .LVU115
	.uleb128 .LVU115
	.uleb128 0
.LLST9:
	.byte	0x6
	.4byte	.LVL24
	.byte	0x4
	.uleb128 .LVL24-.LVL24
	.uleb128 .LVL29-.LVL24
	.uleb128 0x2
	.byte	0x30
	.byte	0x9f
	.byte	0x4
	.uleb128 .LVL29-.LVL24
	.uleb128 .LVL51-.LVL24
	.uleb128 0x1
	.byte	0x56
	.byte	0x4
	.uleb128 .LVL57-.LVL24
	.uleb128 .LVL58-.LVL24
	.uleb128 0x2
	.byte	0x30
	.byte	0x9f
	.byte	0x4
	.uleb128 .LVL58-.LVL24
	.uleb128 .LFE11-.LVL24
	.uleb128 0x1
	.byte	0x56
	.byte	0
.LVUS10:
	.uleb128 .LVU67
	.uleb128 .LVU69
	.uleb128 .LVU69
	.uleb128 .LVU73
	.uleb128 .LVU73
	.uleb128 .LVU79
	.uleb128 .LVU79
	.uleb128 .LVU80
	.uleb128 .LVU80
	.uleb128 .LVU81
	.uleb128 .LVU86
	.uleb128 .LVU92
	.uleb128 .LVU92
	.uleb128 .LVU93
	.uleb128 .LVU93
	.uleb128 .LVU94
	.uleb128 .LVU115
	.uleb128 0
.LLST10:
	.byte	0x6
	.4byte	.LVL34
	.byte	0x4
	.uleb128 .LVL34-.LVL34
	.uleb128 .LVL34-.LVL34
	.uleb128 0x1
	.byte	0x59
	.byte	0x4
	.uleb128 .LVL34-.LVL34
	.uleb128 .LVL38-.LVL34
	.uleb128 0x2
	.byte	0x7d
	.sleb128 0
	.byte	0x4
	.uleb128 .LVL38-.LVL34
	.uleb128 .LVL40-.LVL34
	.uleb128 0x1
	.byte	0x52
	.byte	0x4
	.uleb128 .LVL40-.LVL34
	.uleb128 .LVL41-.LVL34
	.uleb128 0x3
	.byte	0x72
	.sleb128 -1
	.byte	0x9f
	.byte	0x4
	.uleb128 .LVL41-.LVL34
	.uleb128 .LVL42-.LVL34
	.uleb128 0x1
	.byte	0x52
	.byte	0x4
	.uleb128 .LVL45-.LVL34
	.uleb128 .LVL47-.LVL34
	.uleb128 0x1
	.byte	0x55
	.byte	0x4
	.uleb128 .LVL47-.LVL34
	.uleb128 .LVL48-.LVL34
	.uleb128 0x3
	.byte	0x75
	.sleb128 -1
	.byte	0x9f
	.byte	0x4
	.uleb128 .LVL48-.LVL34
	.uleb128 .LVL49-.LVL34
	.uleb128 0x1
	.byte	0x55
	.byte	0x4
	.uleb128 .LVL58-.LVL34
	.uleb128 .LFE11-.LVL34
	.uleb128 0x2
	.byte	0x7d
	.sleb128 0
	.byte	0
.LVUS11:
	.uleb128 .LVU66
	.uleb128 .LVU75
	.uleb128 .LVU78
	.uleb128 .LVU88
	.uleb128 .LVU88
	.uleb128 .LVU91
	.uleb128 .LVU91
	.uleb128 .LVU94
	.uleb128 .LVU115
	.uleb128 0
.LLST11:
	.byte	0x6
	.4byte	.LVL34
	.byte	0x4
	.uleb128 .LVL34-.LVL34
	.uleb128 .LVL39-.LVL34
	.uleb128 0x1
	.byte	0x53
	.byte	0x4
	.uleb128 .LVL40-.LVL34
	.uleb128 .LVL46-.LVL34
	.uleb128 0x1
	.byte	0x53
	.byte	0x4
	.uleb128 .LVL46-.LVL34
	.uleb128 .LVL47-.LVL34
	.uleb128 0x1
	.byte	0x51
	.byte	0x4
	.uleb128 .LVL47-.LVL34
	.uleb128 .LVL49-.LVL34
	.uleb128 0x1
	.byte	0x53
	.byte	0x4
	.uleb128 .LVL58-.LVL34
	.uleb128 .LFE11-.LVL34
	.uleb128 0x1
	.byte	0x53
	.byte	0
.LVUS12:
	.uleb128 .LVU75
	.uleb128 .LVU94
.LLST12:
	.byte	0x8
	.4byte	.LVL39
	.uleb128 .LVL49-.LVL39
	.uleb128 0x1
	.byte	0x53
	.byte	0
.LVUS13:
	.uleb128 .LVU39
	.uleb128 .LVU55
	.uleb128 .LVU55
	.uleb128 .LVU102
	.uleb128 .LVU114
	.uleb128 .LVU115
	.uleb128 .LVU115
	.uleb128 0
.LLST13:
	.byte	0x6
	.4byte	.LVL19
	.byte	0x4
	.uleb128 .LVL19-.LVL19
	.uleb128 .LVL29-.LVL19
	.uleb128 0x2
	.byte	0x30
	.byte	0x9f
	.byte	0x4
	.uleb128 .LVL29-.LVL19
	.uleb128 .LVL51-.LVL19
	.uleb128 0x1
	.byte	0x58
	.byte	0x4
	.uleb128 .LVL57-.LVL19
	.uleb128 .LVL58-.LVL19
	.uleb128 0x2
	.byte	0x30
	.byte	0x9f
	.byte	0x4
	.uleb128 .LVL58-.LVL19
	.uleb128 .LFE11-.LVL19
	.uleb128 0x1
	.byte	0x58
	.byte	0
.LVUS14:
	.uleb128 .LVU41
	.uleb128 .LVU55
	.uleb128 .LVU58
	.uleb128 .LVU86
	.uleb128 .LVU101
	.uleb128 .LVU110
	.uleb128 .LVU114
	.uleb128 0
.LLST14:
	.byte	0x6
	.4byte	.LVL20
	.byte	0x4
	.uleb128 .LVL20-.LVL20
	.uleb128 .LVL29-.LVL20
	.uleb128 0x1
	.byte	0x51
	.byte	0x4
	.uleb128 .LVL31-.LVL20
	.uleb128 .LVL45-.LVL20
	.uleb128 0x1
	.byte	0x51
	.byte	0x4
	.uleb128 .LVL50-.LVL20
	.uleb128 .LVL52-.LVL20
	.uleb128 0x1
	.byte	0x51
	.byte	0x4
	.uleb128 .LVL57-.LVL20
	.uleb128 .LFE11-.LVL20
	.uleb128 0x1
	.byte	0x51
	.byte	0
.LVUS15:
	.uleb128 .LVU44
	.uleb128 .LVU46
	.uleb128 .LVU46
	.uleb128 .LVU70
	.uleb128 .LVU70
	.uleb128 .LVU72
	.uleb128 .LVU72
	.uleb128 .LVU113
	.uleb128 .LVU114
	.uleb128 .LVU115
	.uleb128 .LVU115
	.uleb128 0
.LLST15:
	.byte	0x6
	.4byte	.LVL22
	.byte	0x4
	.uleb128 .LVL22-.LVL22
	.uleb128 .LVL23-.LVL22
	.uleb128 0x1
	.byte	0x53
	.byte	0x4
	.uleb128 .LVL23-.LVL22
	.uleb128 .LVL35-.LVL22
	.uleb128 0x1
	.byte	0x59
	.byte	0x4
	.uleb128 .LVL35-.LVL22
	.uleb128 .LVL37-.LVL22
	.uleb128 0x1
	.byte	0x52
	.byte	0x4
	.uleb128 .LVL37-.LVL22
	.uleb128 .LVL55-.LVL22
	.uleb128 0x1
	.byte	0x59
	.byte	0x4
	.uleb128 .LVL57-.LVL22
	.uleb128 .LVL58-.LVL22
	.uleb128 0x1
	.byte	0x59
	.byte	0x4
	.uleb128 .LVL58-.LVL22
	.uleb128 .LFE11-.LVL22
	.uleb128 0x1
	.byte	0x52
	.byte	0
.LVUS16:
	.uleb128 .LVU47
	.uleb128 .LVU49
	.uleb128 .LVU49
	.uleb128 .LVU55
	.uleb128 .LVU55
	.uleb128 .LVU73
	.uleb128 .LVU73
	.uleb128 .LVU85
	.uleb128 .LVU85
	.uleb128 .LVU86
	.uleb128 .LVU86
	.uleb128 .LVU94
	.uleb128 .LVU94
	.uleb128 .LVU102
	.uleb128 .LVU114
	.uleb128 .LVU115
	.uleb128 .LVU115
	.uleb128 0
.LLST16:
	.byte	0x6
	.4byte	.LVL24
	.byte	0x4
	.uleb128 .LVL24-.LVL24
	.uleb128 .LVL25-.LVL24
	.uleb128 0x1
	.byte	0x53
	.byte	0x4
	.uleb128 .LVL25-.LVL24
	.uleb128 .LVL29-.LVL24
	.uleb128 0x1
	.byte	0x5a
	.byte	0x4
	.uleb128 .LVL29-.LVL24
	.uleb128 .LVL38-.LVL24
	.uleb128 0x1
	.byte	0x55
	.byte	0x4
	.uleb128 .LVL38-.LVL24
	.uleb128 .LVL44-.LVL24
	.uleb128 0x1
	.byte	0x5a
	.byte	0x4
	.uleb128 .LVL44-.LVL24
	.uleb128 .LVL45-.LVL24
	.uleb128 0x1
	.byte	0x52
	.byte	0x4
	.uleb128 .LVL45-.LVL24
	.uleb128 .LVL49-.LVL24
	.uleb128 0x1
	.byte	0x5a
	.byte	0x4
	.uleb128 .LVL49-.LVL24
	.uleb128 .LVL51-.LVL24
	.uleb128 0x1
	.byte	0x55
	.byte	0x4
	.uleb128 .LVL57-.LVL24
	.uleb128 .LVL58-.LVL24
	.uleb128 0x1
	.byte	0x53
	.byte	0x4
	.uleb128 .LVL58-.LVL24
	.uleb128 .LFE11-.LVL24
	.uleb128 0x1
	.byte	0x55
	.byte	0
.LVUS18:
	.uleb128 .LVU63
	.uleb128 .LVU71
	.uleb128 .LVU71
	.uleb128 .LVU76
	.uleb128 .LVU76
	.uleb128 .LVU77
	.uleb128 .LVU77
	.uleb128 .LVU90
	.uleb128 .LVU90
	.uleb128 .LVU91
	.uleb128 .LVU91
	.uleb128 .LVU94
	.uleb128 .LVU115
	.uleb128 0
.LLST18:
	.byte	0x6
	.4byte	.LVL33
	.byte	0x4
	.uleb128 .LVL33-.LVL33
	.uleb128 .LVL36-.LVL33
	.uleb128 0x3
	.byte	0x77
	.sleb128 16
	.byte	0x9f
	.byte	0x4
	.uleb128 .LVL36-.LVL33
	.uleb128 .LVL39-.LVL33
	.uleb128 0x1
	.byte	0x54
	.byte	0x4
	.uleb128 .LVL39-.LVL33
	.uleb128 .LVL40-.LVL33
	.uleb128 0x3
	.byte	0x74
	.sleb128 4
	.byte	0x9f
	.byte	0x4
	.uleb128 .LVL40-.LVL33
	.uleb128 .LVL47-.LVL33
	.uleb128 0x1
	.byte	0x54
	.byte	0x4
	.uleb128 .LVL47-.LVL33
	.uleb128 .LVL47-.LVL33
	.uleb128 0x3
	.byte	0x74
	.sleb128 -4
	.byte	0x9f
	.byte	0x4
	.uleb128 .LVL47-.LVL33
	.uleb128 .LVL49-.LVL33
	.uleb128 0x1
	.byte	0x54
	.byte	0x4
	.uleb128 .LVL58-.LVL33
	.uleb128 .LFE11-.LVL33
	.uleb128 0x3
	.byte	0x77
	.sleb128 16
	.byte	0x9f
	.byte	0
.LVUS0:
	.uleb128 0
	.uleb128 .LVU6
	.uleb128 .LVU6
	.uleb128 .LVU31
	.uleb128 .LVU31
	.uleb128 .LVU32
	.uleb128 .LVU32
	.uleb128 0
.LLST0:
	.byte	0x6
	.4byte	.LVL0
	.byte	0x4
	.uleb128 .LVL0-.LVL0
	.uleb128 .LVL3-1-.LVL0
	.uleb128 0x1
	.byte	0x50
	.byte	0x4
	.uleb128 .LVL3-1-.LVL0
	.uleb128 .LVL15-.LVL0
	.uleb128 0x1
	.byte	0x56
	.byte	0x4
	.uleb128 .LVL15-.LVL0
	.uleb128 .LVL16-.LVL0
	.uleb128 0xa
	.byte	0xa3
	.uleb128 0x3
	.byte	0xa5
	.uleb128 0
	.uleb128 0x26
	.byte	0xa8
	.uleb128 0x2d
	.byte	0xa8
	.uleb128 0
	.byte	0x9f
	.byte	0x4
	.uleb128 .LVL16-.LVL0
	.uleb128 .LFE10-.LVL0
	.uleb128 0x1
	.byte	0x56
	.byte	0
.LVUS1:
	.uleb128 0
	.uleb128 .LVU5
	.uleb128 .LVU5
	.uleb128 .LVU13
	.uleb128 .LVU13
	.uleb128 .LVU18
	.uleb128 .LVU18
	.uleb128 .LVU19
	.uleb128 .LVU19
	.uleb128 .LVU22
	.uleb128 .LVU32
	.uleb128 0
.LLST1:
	.byte	0x6
	.4byte	.LVL0
	.byte	0x4
	.uleb128 .LVL0-.LVL0
	.uleb128 .LVL2-.LVL0
	.uleb128 0x1
	.byte	0x51
	.byte	0x4
	.uleb128 .LVL2-.LVL0
	.uleb128 .LVL5-.LVL0
	.uleb128 0x1
	.byte	0x54
	.byte	0x4
	.uleb128 .LVL5-.LVL0
	.uleb128 .LVL7-.LVL0
	.uleb128 0x1
	.byte	0x53
	.byte	0x4
	.uleb128 .LVL7-.LVL0
	.uleb128 .LVL8-.LVL0
	.uleb128 0x3
	.byte	0x73
	.sleb128 -1
	.byte	0x9f
	.byte	0x4
	.uleb128 .LVL8-.LVL0
	.uleb128 .LVL9-1-.LVL0
	.uleb128 0x1
	.byte	0x53
	.byte	0x4
	.uleb128 .LVL16-.LVL0
	.uleb128 .LFE10-.LVL0
	.uleb128 0x1
	.byte	0x53
	.byte	0
.LVUS2:
	.uleb128 0
	.uleb128 .LVU4
	.uleb128 .LVU4
	.uleb128 .LVU23
	.uleb128 .LVU23
	.uleb128 .LVU32
	.uleb128 .LVU32
	.uleb128 0
.LLST2:
	.byte	0x6
	.4byte	.LVL0
	.byte	0x4
	.uleb128 .LVL0-.LVL0
	.uleb128 .LVL1-.LVL0
	.uleb128 0x1
	.byte	0x52
	.byte	0x4
	.uleb128 .LVL1-.LVL0
	.uleb128 .LVL10-.LVL0
	.uleb128 0x1
	.byte	0x55
	.byte	0x4
	.uleb128 .LVL10-.LVL0
	.uleb128 .LVL16-.LVL0
	.uleb128 0xa
	.byte	0xa3
	.uleb128 0x3
	.byte	0xa5
	.uleb128 0x2
	.uleb128 0x26
	.byte	0xa8
	.uleb128 0x2d
	.byte	0xa8
	.uleb128 0
	.byte	0x9f
	.byte	0x4
	.uleb128 .LVL16-.LVL0
	.uleb128 .LFE10-.LVL0
	.uleb128 0x1
	.byte	0x55
	.byte	0
.LVUS3:
	.uleb128 .LVU11
	.uleb128 .LVU15
	.uleb128 .LVU15
	.uleb128 .LVU19
	.uleb128 .LVU32
	.uleb128 0
.LLST3:
	.byte	0x6
	.4byte	.LVL4
	.byte	0x4
	.uleb128 .LVL4-.LVL4
	.uleb128 .LVL6-.LVL4
	.uleb128 0x2
	.byte	0x31
	.byte	0x9f
	.byte	0x4
	.uleb128 .LVL6-.LVL4
	.uleb128 .LVL8-.LVL4
	.uleb128 0x1
	.byte	0x54
	.byte	0x4
	.uleb128 .LVL16-.LVL4
	.uleb128 .LFE10-.LVL4
	.uleb128 0x2
	.byte	0x31
	.byte	0x9f
	.byte	0
.LVUS4:
	.uleb128 .LVU20
	.uleb128 .LVU27
.LLST4:
	.byte	0x8
	.4byte	.LVL8
	.uleb128 .LVL12-.LVL8
	.uleb128 0x1
	.byte	0x54
	.byte	0
.Ldebug_loc3:
	.section	.debug_aranges,"",%progbits
	.4byte	0x2c
	.2byte	0x2
	.4byte	.Ldebug_info0
	.byte	0x4
	.byte	0
	.2byte	0
	.2byte	0
	.4byte	.LFB10
	.4byte	.LFE10-.LFB10
	.4byte	.LFB11
	.4byte	.LFE11-.LFB11
	.4byte	.LFB12
	.4byte	.LFE12-.LFB12
	.4byte	0
	.4byte	0
	.section	.debug_rnglists,"",%progbits
.Ldebug_ranges0:
	.4byte	.Ldebug_ranges3-.Ldebug_ranges2
.Ldebug_ranges2:
	.2byte	0x5
	.byte	0x4
	.byte	0
	.4byte	0
.LLRL17:
	.byte	0x5
	.4byte	.LBB3
	.byte	0x4
	.uleb128 .LBB3-.LBB3
	.uleb128 .LBE3-.LBB3
	.byte	0x4
	.uleb128 .LBB4-.LBB3
	.uleb128 .LBE4-.LBB3
	.byte	0x4
	.uleb128 .LBB5-.LBB3
	.uleb128 .LBE5-.LBB3
	.byte	0
.LLRL23:
	.byte	0x5
	.4byte	.LBB9
	.byte	0x4
	.uleb128 .LBB9-.LBB9
	.uleb128 .LBE9-.LBB9
	.byte	0x4
	.uleb128 .LBB10-.LBB9
	.uleb128 .LBE10-.LBB9
	.byte	0
.LLRL25:
	.byte	0x7
	.4byte	.LFB10
	.uleb128 .LFE10-.LFB10
	.byte	0x7
	.4byte	.LFB11
	.uleb128 .LFE11-.LFB11
	.byte	0x7
	.4byte	.LFB12
	.uleb128 .LFE12-.LFB12
	.byte	0
.Ldebug_ranges3:
	.section	.debug_line,"",%progbits
.Ldebug_line0:
	.section	.debug_str,"MS",%progbits,1
.LASF55:
	.ascii	"atoll\000"
.LASF57:
	.ascii	"strtoull\000"
.LASF67:
	.ascii	"overflow_cnt\000"
.LASF18:
	.ascii	"quot\000"
.LASF15:
	.ascii	"size_t\000"
.LASF62:
	.ascii	"decimation_factor\000"
.LASF95:
	.ascii	"__aeabi_fdiv\000"
.LASF52:
	.ascii	"_ZSt3divll\000"
.LASF29:
	.ascii	"_ZSt3absx\000"
.LASF81:
	.ascii	"stage_out\000"
.LASF93:
	.ascii	"__clzsi2\000"
.LASF49:
	.ascii	"wcstombs\000"
.LASF23:
	.ascii	"7lldiv_t\000"
.LASF92:
	.ascii	"__builtin_memset\000"
.LASF0:
	.ascii	"long long unsigned int\000"
.LASF77:
	.ascii	"data\000"
.LASF61:
	.ascii	"order\000"
.LASF88:
	.ascii	"_Z8cic_initP12cic_filter_tmm\000"
.LASF31:
	.ascii	"atexit\000"
.LASF19:
	.ascii	"div_t\000"
.LASF11:
	.ascii	"long long int\000"
.LASF2:
	.ascii	"signed char\000"
.LASF43:
	.ascii	"qsort\000"
.LASF39:
	.ascii	"mblen\000"
.LASF102:
	.ascii	"_Z8cic_readP12cic_filter_tPtj\000"
.LASF45:
	.ascii	"strtod\000"
.LASF84:
	.ascii	"state\000"
.LASF58:
	.ascii	"strtof\000"
.LASF8:
	.ascii	"long int\000"
.LASF46:
	.ascii	"strtol\000"
.LASF94:
	.ascii	"__aeabi_ui2f\000"
.LASF71:
	.ascii	"cic_filter_t\000"
.LASF22:
	.ascii	"ldiv_t\000"
.LASF12:
	.ascii	"uint16_t\000"
.LASF33:
	.ascii	"double\000"
.LASF42:
	.ascii	"mbtowc\000"
.LASF9:
	.ascii	"__uint32_t\000"
.LASF35:
	.ascii	"atol\000"
.LASF63:
	.ascii	"data_counter\000"
.LASF68:
	.ascii	"samples_out_cnt\000"
.LASF69:
	.ascii	"attenuate_shift\000"
.LASF1:
	.ascii	"unsigned int\000"
.LASF10:
	.ascii	"long unsigned int\000"
.LASF44:
	.ascii	"srand\000"
.LASF99:
	.ascii	"rand\000"
.LASF76:
	.ascii	"output_size\000"
.LASF20:
	.ascii	"5div_t\000"
.LASF97:
	.ascii	"lib/filter/src/cic.cpp\000"
.LASF5:
	.ascii	"short unsigned int\000"
.LASF82:
	.ascii	"err_cnt\000"
.LASF36:
	.ascii	"bsearch\000"
.LASF100:
	.ascii	"12cic_filter_t\000"
.LASF54:
	.ascii	"lldiv\000"
.LASF41:
	.ascii	"wchar_t\000"
.LASF65:
	.ascii	"int_state\000"
.LASF96:
	.ascii	"GNU C++17 14.2.0 -mcpu=cortex-m0plus -mthumb -march"
	.ascii	"=armv6-m -g -O1 -std=gnu++17 -fno-exceptions -fno-r"
	.ascii	"tti -ffunction-sections -fdata-sections\000"
.LASF37:
	.ascii	"getenv\000"
.LASF16:
	.ascii	"long double\000"
.LASF38:
	.ascii	"ldiv\000"
.LASF89:
	.ascii	"downsample\000"
.LASF70:
	.ascii	"gain\000"
.LASF59:
	.ascii	"float\000"
.LASF40:
	.ascii	"mbstowcs\000"
.LASF32:
	.ascii	"atof\000"
.LASF34:
	.ascii	"atoi\000"
.LASF13:
	.ascii	"int32_t\000"
.LASF3:
	.ascii	"unsigned char\000"
.LASF21:
	.ascii	"6ldiv_t\000"
.LASF24:
	.ascii	"lldiv_t\000"
.LASF79:
	.ascii	"step\000"
.LASF87:
	.ascii	"_Z9cic_writeP12cic_filter_tPKtjj\000"
.LASF4:
	.ascii	"short int\000"
.LASF83:
	.ascii	"decimation\000"
.LASF60:
	.ascii	"strtold\000"
.LASF56:
	.ascii	"strtoll\000"
.LASF74:
	.ascii	"filter\000"
.LASF80:
	.ascii	"stage_in\000"
.LASF50:
	.ascii	"wctomb\000"
.LASF28:
	.ascii	"_ZSt3absd\000"
.LASF26:
	.ascii	"_ZSt3abse\000"
.LASF27:
	.ascii	"_ZSt3absf\000"
.LASF53:
	.ascii	"_ZN9__gnu_cxx3divExx\000"
.LASF30:
	.ascii	"_ZSt3absl\000"
.LASF14:
	.ascii	"uint32_t\000"
.LASF78:
	.ascii	"length\000"
.LASF17:
	.ascii	"char\000"
.LASF64:
	.ascii	"out_cnt\000"
.LASF6:
	.ascii	"__uint16_t\000"
.LASF7:
	.ascii	"__int32_t\000"
.LASF66:
	.ascii	"out_buf\000"
.LASF98:
	.ascii	"C:\\Projects\\oppc_sw\\oppc-aux-sw\000"
.LASF73:
	.ascii	"memset\000"
.LASF47:
	.ascii	"strtoul\000"
.LASF86:
	.ascii	"cic_init\000"
.LASF72:
	.ascii	"memmove\000"
.LASF101:
	.ascii	"cic_read\000"
.LASF48:
	.ascii	"system\000"
.LASF25:
	.ascii	"__compar_fn_t\000"
.LASF91:
	.ascii	"__aeabi_uidiv\000"
.LASF75:
	.ascii	"max_length\000"
.LASF90:
	.ascii	"actual_gain\000"
.LASF85:
	.ascii	"cic_write\000"
.LASF51:
	.ascii	"__gnu_cxx\000"
	.global	__aeabi_uidiv
	.global	__aeabi_fdiv
	.global	__aeabi_ui2f
	.global	__clzsi2
	.ident	"GCC: (GNU) 14.2.0"
