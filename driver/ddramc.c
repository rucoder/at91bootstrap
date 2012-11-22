/* ----------------------------------------------------------------------------
 *         ATMEL Microcontroller Software Support
 * ----------------------------------------------------------------------------
 * Copyright (c) 2007, Stelian Pop <stelian.pop@leadtechdesign.com>
 * Copyright (c) 2007 Lead Tech Design <www.leadtechdesign.com>
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the disclaiimer below.
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
#include "common.h"
#include "hardware.h"
#include "arch/at91_pmc.h"
#include "arch/at91_ddrsdrc.h"
#include "arch/at91_ccfg.h"
#include "debug.h"
#include "ddramc.h"

/* write DDRC register */
static void write_ddramc(unsigned int address,
			unsigned int offset,
			const unsigned int value)
{
	writel(value, (address + offset));
}

/* read DDRC registers */
static unsigned int read_ddramc(unsigned int address, unsigned int offset)
{
	return readl(address + offset);
}

static int ddramc_decodtype_is_seq(unsigned int ddramc_cr)
{
#if defined(AT91SAM9X5) || defined(AT91SAM9N12) || defined(AT91SAMA5D3X)
	if (ddramc_cr & AT91C_DDRC2_DECOD_INTERLEAVED)
		return 0;
#endif
	return 1;
}

int ddram_initialize(unsigned int base_address,
			unsigned int ram_address,
			struct ddramc_register *ddramc_config)
{
	unsigned int ba_offset;
	unsigned int cr = 0;

	/* compute BA[] offset according to CR configuration */
	ba_offset = (ddramc_config->cr & AT91C_DDRC2_NC) + 9;
	if (ddramc_decodtype_is_seq(ddramc_config->cr))
		ba_offset += ((ddramc_config->cr & AT91C_DDRC2_NR) >> 2) + 11;

	ba_offset += (ddramc_config->mdr & AT91C_DDRC2_DBW) ? 1 : 2;

	dbg_log(3, " ba_offset = %x ...\n\r", ba_offset);

	/*
	 * Step 1: Program the memory device type into the Memory Device Register
	 */
	write_ddramc(base_address, HDDRSDRC2_MDR, ddramc_config->mdr);

	/* 
	 * Step 2: Program the feature of DDR2-SDRAM device into 
	 * the Timing Register, and into the Configuration Register
	 */
	write_ddramc(base_address, HDDRSDRC2_CR, ddramc_config->cr);

	write_ddramc(base_address, HDDRSDRC2_T0PR, ddramc_config->t0pr);
	write_ddramc(base_address, HDDRSDRC2_T1PR, ddramc_config->t1pr);
	write_ddramc(base_address, HDDRSDRC2_T2PR, ddramc_config->t2pr);

	/*
	 * Step 3: An NOP command is issued to the DDR2-SDRAM
	 */
	write_ddramc(base_address, HDDRSDRC2_MR, AT91C_DDRC2_MODE_NOP_CMD);
	*((unsigned volatile int *)ram_address) = 0;
	/* Now, clocks which drive the DDR2-SDRAM device are enabled */

	/* A minimum pause wait 200 us is provided to precede any signal toggle.
	(6 core cycles per iteration, core is at 396MHz: min 13340 loops) */
	delay(13340);

	/*
	 * Step 4:  An NOP command is issued to the DDR2-SDRAM
	 */
	write_ddramc(base_address, HDDRSDRC2_MR, AT91C_DDRC2_MODE_NOP_CMD);
	*((unsigned volatile int *)ram_address) = 0;
	/* Now, CKE is driven high */
	/* wait 400 ns min */
	delay(27);

	/*
	 * Step 5: An all banks precharge command is issued to the DDR2-SDRAM.
	 */
	write_ddramc(base_address, HDDRSDRC2_MR, AT91C_DDRC2_MODE_PRCGALL_CMD);
	*((unsigned volatile int *)ram_address) = 0;

	/* wait 2 cycles min (of tCK) = 15 ns min */
	delay(2);

	/*
	 * Step 6: An Extended Mode Register set(EMRS2) cycle is issued to chose between commercial or high
	 * temperature operations.
	 * Perform a write access to DDR2-SDRAM to acknowledge this command.
	 * The write address must be chosen so that BA[1] is set to 1 and BA[0] is set to 0.
	 */
	write_ddramc(base_address, HDDRSDRC2_MR, AT91C_DDRC2_MODE_EXT_LMR_CMD);
	*((unsigned int *)(ram_address + (0x2 << ba_offset))) = 0;

	/* wait 2 cycles min (of tCK) = 15 ns min */
	delay(2);

	/*
	 * Step 7: An Extended Mode Register set(EMRS3) cycle is issued
	 * to set the Extended Mode Register to "0".
	 * Perform a write access to DDR2-SDRAM to acknowledge this command.
	 * The write address must be chosen so that BA[1] is set to 1 and BA[0] is set to 1.
	 */
	write_ddramc(base_address, HDDRSDRC2_MR, AT91C_DDRC2_MODE_EXT_LMR_CMD);
	*((unsigned int *)(ram_address + (0x3 << ba_offset))) = 0;

	/* wait 2 cycles min (of tCK) = 15 ns min */
	delay(2);

	/*
	 * Step 8: An Extened Mode Register set(EMRS1) cycle is issued to enable DLL,
	 * and to program D.I.C(Output Driver Impedance Control)
	 * Perform a write access to DDR2-SDRAM to acknowledge this command.
	 * The write address must be chosen so that BA[1] is set to 0 and BA[0] is set to 1.
	 */
	write_ddramc(base_address, HDDRSDRC2_MR, AT91C_DDRC2_MODE_EXT_LMR_CMD);
	*((unsigned int *)(ram_address + (0x1 << ba_offset))) = 0;

	/* An additional 200 cycles of clock are required for locking DLL */
	delay(100);

	/*
	 * Step 9: Program DLL field into the Configuration Register to high(Enable DLL reset)
	 */
	cr = read_ddramc(base_address, HDDRSDRC2_CR);
	write_ddramc(base_address, HDDRSDRC2_CR, cr | AT91C_DDRC2_DLL_RESET_ENABLED);

	/*
	 * Step 10: A Mode Register set(MRS) cycle is issied to reset DLL.
	 * Perform a write access to DDR2-SDRAM to acknowledge this command.
	 * The write address must be chosen so that BA[1:0] bits are set to 0.
	 */
	write_ddramc(base_address, HDDRSDRC2_MR, AT91C_DDRC2_MODE_LMR_CMD);
	*((unsigned int *)(ram_address + (0x0 << ba_offset))) = 0;

	/* wait 2 cycles min (of tCK) = 15 ns min */
	delay(2);

	/*
	 * Step 11: An all banks precharge command is issued to the DDR2-SDRAM.
	 */
	write_ddramc(base_address, HDDRSDRC2_MR, AT91C_DDRC2_MODE_PRCGALL_CMD);
	*(((unsigned volatile int *)ram_address)) = 0;

	/* wait 400 ns min (not needed on certain DDR2 devices) */
	delay(27);

	/*
	 * Step 12: Two auto-refresh (CBR) cycles are provided.
	 * Program the auto refresh command (CBR) into the Mode Register.
	 */
	write_ddramc(base_address, HDDRSDRC2_MR, AT91C_DDRC2_MODE_RFSH_CMD);
	*(((unsigned volatile int *)ram_address)) = 0;

	/* wait TRFC cycles min (135 ns min) extended to 400 ns */
	delay(27);

	/* Set 2nd CBR */
	write_ddramc(base_address, HDDRSDRC2_MR, AT91C_DDRC2_MODE_RFSH_CMD);
	*(((unsigned volatile int *)ram_address)) = 0;

	/* wait TRFC cycles min (135 ns min) extended to 400 ns */
	delay(27);

	/*
	 * Step 13: Program DLL field into the Configuration Register to low(Disable DLL reset).
	 */
	cr = read_ddramc(base_address, HDDRSDRC2_CR);
	write_ddramc(base_address, HDDRSDRC2_CR, cr & (~AT91C_DDRC2_DLL_RESET_ENABLED));

	/*
	 * Step 14: A Mode Register set (MRS) cycle is issued to program
	 * the parameters of the DDR2-SDRAM devices, in particular CAS latency,
	 * burst length and to disable DDL reset.
	 * Perform a write access to DDR2-SDRAM to acknowledge this command.
	 * The write address must be chosen so that BA[1:0] bits are set to 0.
	 */
	write_ddramc(base_address, HDDRSDRC2_MR, AT91C_DDRC2_MODE_LMR_CMD);
	*((unsigned int *)(ram_address + (0x0 << ba_offset))) = 0;

	/* wait 2 cycles min (of tCK) = 15 ns min */
	delay(2);

	/*
	 * Step 15: Program OCD field into the Configuration Register
	 * to high (OCD calibration default).
	 */
	cr = read_ddramc(base_address, HDDRSDRC2_CR);
	write_ddramc(base_address, HDDRSDRC2_CR, cr | AT91C_DDRC2_OCD_DEFAULT);

	/* wait 2 cycles min (of tCK) = 15 ns min */
	delay(2);

	/*
	 * Step 16: An Extended Mode Register set (EMRS1) cycle is issued to OCD default value.
	 * Perform a write access to DDR2-SDRAM to acknowledge this command.
	 * The write address must be chosen so that BA[1] is set to 0 and BA[0] is set to 1.
	 */
	write_ddramc(base_address, HDDRSDRC2_MR, AT91C_DDRC2_MODE_EXT_LMR_CMD);
	*((unsigned int *)(ram_address + (0x1 << ba_offset))) = 0;

	/* wait 2 cycles min (of tCK) = 15 ns min */
	delay(2);

	/*
	 * Step 17: Program OCD field into the Configuration Register
	 * to low (OCD calibration mode exit).
	 */
	cr = read_ddramc(base_address, HDDRSDRC2_CR);
	write_ddramc(base_address, HDDRSDRC2_CR, cr & (~AT91C_DDRC2_OCD_DEFAULT));

	/* wait 2 cycles min (of tCK) = 15 ns min */
	delay(2);

	/*
	 * Step 18: An Extended Mode Register set (EMRS1) cycle is issued to enable OCD exit.
	 * Perform a write access to DDR2-SDRAM to acknowledge this command.
	 * The write address must be chosen so that BA[1] is set to 0 and BA[0] is set to 1.
	 */
	write_ddramc(base_address, HDDRSDRC2_MR, AT91C_DDRC2_MODE_EXT_LMR_CMD);
	*((unsigned int *)(ram_address + (0x1 << ba_offset))) = 0;

	/* wait 2 cycles min (of tCK) = 15 ns min */
	delay(2);

	/*
	 * Step 19: A Nornal mode command is provided.
	 */
	write_ddramc(base_address, HDDRSDRC2_MR, AT91C_DDRC2_MODE_NORMAL_CMD);
	*(((unsigned volatile int *)ram_address)) = 0;

	/*
	 * Step 20: Perform a write access to any DDR2-SDRAM address
	 */
	*(((unsigned volatile int *)ram_address)) = 0;

	/*
	 * Step 21: Write the refresh rate into the count field in the Refresh Timer register.
	 */
	write_ddramc(base_address, HDDRSDRC2_RTR, ddramc_config->rtr);

	/*
	 * Now we are ready to work on the DDRSDR
	 *  wait for end of calibration
	 */
	delay(500);

	return 0;
}
