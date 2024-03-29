/* ----------------------------------------------------------------------------
 *         ATMEL Microcontroller Software Support
 * ----------------------------------------------------------------------------
 * Copyright (c) 2006, Atmel Corporation

 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the disclaimer below.
 *
 * Atmel's name may not be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * DISCLAIMER: THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef __AT91SAM9G20_H__
#define __AT91SAM9G20_H__

/*
 * Peripheral identifiers/interrupts.
 */
#define AT91C_ID_AIC	0	/* Advanced Interrupt Controller (FIQ) */
#define AT91C_ID_SYSC	1	/* System Controller Interrupt */
#define AT91C_ID_PIOA	2	/* Parallel IO Controller A */
#define AT91C_ID_PIOB	3	/* Parallel IO Controller B */
#define AT91C_ID_PIOC	4	/* Parallel IO Controller C */
#define AT91C_ID_ADC	5	/* Analog to Digital Converter */
#define AT91C_ID_US0	6	/* USART 0 */
#define AT91C_ID_US1	7	/* USART 1 */
#define AT91C_ID_US2	8	/* USART 2 */
#define AT91C_ID_MCI	9	/* Multimedia Card Interface */
#define AT91C_ID_UDP	10	/* USB Device Port */
#define AT91C_ID_TWI	11	/* Two-Wire Interface */
#define AT91C_ID_SPI0	12	/* Serial Peripheral Interface 0 */
#define AT91C_ID_SPI1	13	/* Serial Peripheral Interface 1 */
#define AT91C_ID_SSC	14	/* Serial Synchronous Controller */
/* Reserved 15,16 */
#define AT91C_ID_TC0	17	/* Timer/Counter 0 */
#define AT91C_ID_TC1	18	/* Timer/Counter 0 */
#define AT91C_ID_TC2	19	/* Timer/Counter 0 */
#define AT91C_ID_UHP	20	/* USB Host port */
#define AT91C_ID_EMAC	21	/* Ethernet MAC */
#define AT91C_ID_ISI	22	/* Image Sensor Interface */
#define AT91C_ID_US3	23	/* USART 3 */
#define AT91C_ID_US4	24	/* USART 4 */
#define AT91C_ID_US5	25	/* USART 5 */
#define AT91C_ID_TC3	26	/* Timer/Counter 3 */
#define AT91C_ID_TC4	27	/* Timer/Counter 4 */
#define AT91C_ID_TC5	28	/* Timer/Counter 5 */
#define AT91C_ID_IRQ0	29	/* Advanced Interrupt Controller 0 */
#define AT91C_ID_IRQ1	30	/* Advanced Interrupt Controller 1 */
#define AT91C_ID_IRQ2	31	/* Advanced Interrupt Controller 2 */

/*
 * User Peripherals physical base addresses.
 */
#define AT91C_BASE_TC0		0xfffa0000
#define AT91C_BASE_TC1		0xfffa0040
#define AT91C_BASE_TC2		0xfffa0080
#define AT91C_BASE_UDP		0xfffa4000
#define AT91C_BASE_MCI		0xfffa8000
#define AT91C_BASE_TWI		0xfffac000
#define AT91C_BASE_USART0	0xfffb0000
#define AT91C_BASE_USART1	0xfffb4000
#define AT91C_BASE_USART2	0xfffb8000
#define AT91C_BASE_SSC		0xfffbc000
#define AT91C_BASE_ISI		0xfffc0000
#define AT91C_BASE_EMAC		0xfffc4000
#define AT91C_BASE_SPI0		0xfffc8000
#define AT91C_BASE_SPI1		0xfffcc000
#define AT91C_BASE_USART3	0xfffd0000
#define AT91C_BASE_USART4	0xfffd4000
#define AT91C_BASE_USART5	0xfffd8000
#define AT91C_BASE_TC3		0xfffdc000
#define AT91C_BASE_TC4		0xfffdc040
#define AT91C_BASE_TC5		0xfffdc080
#define AT91C_BASE_ADC		0xfffe0000

/*
 * System Peripherals physical base addresses.
 */
#define AT91C_BASE_SYS		0xffffc000
#define AT91C_BASE_ECC		0xffffe800
#define AT91C_BASE_SDRAMC	0xffffea00
#define AT91C_BASE_SMC		0xffffec00
#define AT91C_BASE_MATRIX	0xffffee00
#define AT91C_BASE_CCFG		0xffffef14
#define AT91C_BASE_AIC		0xfffff000
#define AT91C_BASE_DBGU		0xfffff200
#define AT91C_BASE_PIOA		0xfffff400
#define AT91C_BASE_PIOB		0xfffff600
#define AT91C_BASE_PIOC		0xfffff800
#define AT91C_BASE_PMC		0xfffffc00
#define AT91C_BASE_RSTC		0xfffffd00
#define AT91C_BASE_SHDW		0xfffffd10
#define AT91C_BASE_RTT		0xfffffd20
#define AT91C_BASE_PIT		0xfffffd30
#define AT91C_BASE_WDT		0xfffffd40
#define AT91C_BASE_GPBR		0xfffffd50

/*
 * Internal Memory common on all these SoCs
 */
#define AT91C_BASE_BOOT		0x00000000	/* Boot mapped area */
#define AT91C_BASE_ROM		0x00100000	/* Internal ROM base address(32k) */
#define AT91C_BASE_SRAM0	0x00200000	/* Internal SRAM0(16k) */
#define AT91C_BASE_SRAM1	0x00300000	/* Internal SRAM1(16k) */
#define AT91C_UHP_BASE		0x00500000	/* USB Host controller(16k) */

/*
 * External memory
 */
#define AT91C_BASE_CS0		0x10000000
#define AT91C_BASE_CS1		0x20000000	/* SDRAMC */
#define AT91C_BASE_CS2		0x30000000
#define AT91C_BASE_CS3		0x40000000	/* NANDFlash */
#define AT91C_BASE_CS4		0x50000000	/* Compact Flash Slot0 */
#define AT91C_BASE_CS5		0x60000000	/* Compact Flash Slot1 */
#define AT91C_BASE_CS6		0x70000000
#define AT91C_BASE_CS7		0x80000000

/*
 * Other misc defines
 */
#define AT91_BASE_SYS		AT91C_BASE_SYS
#define AT91_SMC		(AT91C_BASE_SMC	- AT91_BASE_SYS)

/*
 * SoC specific defines
 */
# define AT91C_CPU_NAME		"AT91SAM9G20"

#endif	/* #ifndef __AT91SAM9G20_H__ */
