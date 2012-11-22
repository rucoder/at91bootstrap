/* ----------------------------------------------------------------------------
 *         ATMEL Microcontroller Software Support
 * ----------------------------------------------------------------------------
 * Copyright (c) 2008, Atmel Corporation

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
#include "common.h"
#include "hardware.h"
#include "arch/at91_ccfg.h"
#include "arch/at91_matrix.h"
#include "arch/at91_wdt.h"
#include "arch/at91_rstc.h"
#include "arch/at91_pmc.h"
#include "arch/at91_smc.h"
#include "arch/at91_pio.h"
#include "arch/at91_sdramc.h"
#include "spi.h"
#include "gpio.h"
#include "pmc.h"
#include "dbgu.h"
#include "debug.h"
#include "sdramc.h"
#include "at91sam9g20ek.h"

#ifdef CONFIG_USER_HW_INIT
extern void hw_init_hook(void);
#endif

#ifdef CONFIG_DEBUG
static void at91_dbgu_hw_init(void)
{
	/* Configure DBGU pin */
	const struct pio_desc dbgu_pins[] = {
		{"RXD", AT91C_PIN_PB(14), 0, PIO_DEFAULT, PIO_PERIPH_A},
		{"TXD", AT91C_PIN_PB(15), 0, PIO_DEFAULT, PIO_PERIPH_A},
		{(char *)0, 0, 0, PIO_DEFAULT, PIO_PERIPH_A},
	};

	pio_configure(dbgu_pins);

	/*  Configure the dbgu pins */
	writel((1 << AT91C_ID_PIOB), (PMC_PCER + AT91C_BASE_PMC));
}

static void initialize_dbgu(void)
{
	at91_dbgu_hw_init();

	dbgu_init(BAUDRATE(MASTER_CLOCK, 115200));
}
#endif /* #ifdef CONFIG_DEBUG */

#ifdef CONFIG_SDRAM
static void sdramc_hw_init(void)
{
	/* Configure sdramc pins */
	const struct pio_desc sdramc_pins[] = {
		{"D16", AT91C_PIN_PC(16), 0, PIO_DEFAULT, PIO_PERIPH_A},
		{"D17", AT91C_PIN_PC(17), 0, PIO_DEFAULT, PIO_PERIPH_A},
		{"D18", AT91C_PIN_PC(18), 0, PIO_DEFAULT, PIO_PERIPH_A},
		{"D19", AT91C_PIN_PC(19), 0, PIO_DEFAULT, PIO_PERIPH_A},
		{"D20", AT91C_PIN_PC(20), 0, PIO_DEFAULT, PIO_PERIPH_A},
		{"D21", AT91C_PIN_PC(21), 0, PIO_DEFAULT, PIO_PERIPH_A},
		{"D22", AT91C_PIN_PC(22), 0, PIO_DEFAULT, PIO_PERIPH_A},
		{"D23", AT91C_PIN_PC(23), 0, PIO_DEFAULT, PIO_PERIPH_A},
		{"D24", AT91C_PIN_PC(24), 0, PIO_DEFAULT, PIO_PERIPH_A},
		{"D25", AT91C_PIN_PC(25), 0, PIO_DEFAULT, PIO_PERIPH_A},
		{"D26", AT91C_PIN_PC(26), 0, PIO_DEFAULT, PIO_PERIPH_A},
		{"D27", AT91C_PIN_PC(27), 0, PIO_DEFAULT, PIO_PERIPH_A},
		{"D28", AT91C_PIN_PC(28), 0, PIO_DEFAULT, PIO_PERIPH_A},
		{"D29", AT91C_PIN_PC(29), 0, PIO_DEFAULT, PIO_PERIPH_A},
		{"D30", AT91C_PIN_PC(30), 0, PIO_DEFAULT, PIO_PERIPH_A},
		{"D31", AT91C_PIN_PC(31), 0, PIO_DEFAULT, PIO_PERIPH_A},
		{(char *) 0, 0, 0, PIO_DEFAULT, PIO_PERIPH_A},
	};

	/* Configure the SDRAMC PINs */
	writel((1 << AT91C_ID_PIOC), (PMC_PCER + AT91C_BASE_PMC));
	pio_configure(sdramc_pins);
}

static void sdramc_init(void)
{
	struct sdramc_register sdramc_config;

	sdramc_config.cr = AT91C_SDRAMC_NC_9 | AT91C_SDRAMC_NR_13 | AT91C_SDRAMC_CAS_3
				| AT91C_SDRAMC_NB_4_BANKS | AT91C_SDRAMC_DBW_32_BITS
				| AT91C_SDRAMC_TWR_3 | AT91C_SDRAMC_TRC_9
				| AT91C_SDRAMC_TRP_3 | AT91C_SDRAMC_TRCD_3
				| AT91C_SDRAMC_TRAS_6 | AT91C_SDRAMC_TXSR_10;

	sdramc_config.tr = (MASTER_CLOCK * 7) / 1000000;
	sdramc_config.mdr = AT91C_SDRAMC_MD_SDRAM;

	sdramc_hw_init();

	/* Initialize the matrix (memory voltage = 3.3) */
	writel((readl(AT91C_BASE_CCFG + CCFG_EBICSA))
		| AT91C_EBI_CS1A_SDRAMC | AT91C_VDDIOM_SEL_33V,
		AT91C_BASE_CCFG + CCFG_EBICSA);

	sdramc_initialize(&sdramc_config, AT91C_BASE_CS1);
}
#endif /* #ifdef CONFIG_SDRAM */

#if defined(CONFIG_NANDFLASH_RECOVERY) || defined(CONFIG_DATAFLASH_RECOVERY)
static void recovery_buttons_hw_init(void)
{
	/* Configure recovery button PINs */
	const struct pio_desc recovery_button_pins[] = {
		{"RECOVERY_BUTTON", CONFIG_SYS_RECOVERY_BUTTON_PIN, 0, PIO_PULLUP, PIO_INPUT},
		{(char *)0, 0, 0, PIO_DEFAULT, PIO_PERIPH_A},
	};

	writel((1 << AT91C_ID_PIOA), PMC_PCER + AT91C_BASE_PMC);
	pio_configure(recovery_button_pins);
}
#endif /* #if defined(CONFIG_NANDFLASH_RECOVERY) || defined(CONFIG_DATAFLASH_RECOVERY) */

#ifdef CONFIG_HW_INIT
void hw_init(void)
{
	/* Disable watchdog */
	writel(AT91C_WDTC_WDDIS, AT91C_BASE_WDT + WDTC_MR);

	/*
	 * At this stage the main oscillator is supposed to be enabled
	 * PCK = MCK = MOSC
	 */
	writel(0x00, AT91C_BASE_PMC + PMC_PLLICPR);

	/* Configure PLLA = MOSC * (PLL_MULA + 1) / PLL_DIVA */
	pmc_cfg_plla(PLLA_SETTINGS, PLL_LOCK_TIMEOUT);

	/* PCK = PLLA/2 = 3 * MCK */
	pmc_cfg_mck(MCKR_SETTINGS, PLL_LOCK_TIMEOUT);

	/* Switch MCK on PLLA output */
	pmc_cfg_mck(MCKR_CSS_SETTINGS, PLL_LOCK_TIMEOUT);

	/* Configure PLLB  */
	//pmc_cfg_pllb(PLLB_SETTINGS, PLL_LOCK_TIMEOUT);

	/* Enable External Reset */
	writel(((0xA5 << 24) | AT91C_RSTC_URSTEN), AT91C_BASE_RSTC + RSTC_RMR);

	/* Configure the EBI Slave Slot Cycle to 64 */
	writel((readl((AT91C_BASE_MATRIX + MATRIX_SCFG3)) & ~0xFF) | 0x40,
		(AT91C_BASE_MATRIX + MATRIX_SCFG3));

#ifdef CONFIG_DEBUG
	/* Initialize dbgu */
	initialize_dbgu();
#endif

#ifdef CONFIG_SDRAM
	/* Initlialize sdram controller */
	sdramc_init();
#endif

#ifdef CONFIG_USER_HW_INIT
	hw_init_hook();
#endif

#if defined(CONFIG_NANDFLASH_RECOVERY) || defined(CONFIG_DATAFLASH_RECOVERY)
	/* Init the recovery buttons pins */
	recovery_buttons_hw_init();
#endif
}
#endif /* #ifdef CONFIG_HW_INIT */

#ifdef CONFIG_DATAFLASH
void at91_spi0_hw_init(void)
{
	/* Configure spi0 PINs */
	const struct pio_desc spi0_pins[] = {
		{"MISO",	AT91C_PIN_PA(0),	0, PIO_DEFAULT, PIO_PERIPH_A},
		{"MOSI",	AT91C_PIN_PA(1),	0, PIO_DEFAULT, PIO_PERIPH_A},
		{"SPCK",	AT91C_PIN_PA(2),	0, PIO_DEFAULT, PIO_PERIPH_A},
		{"NPCS",	CONFIG_SYS_SPI_PCS,	1, PIO_PULLUP, PIO_OUTPUT},
		{(char *)0, 0, 0, PIO_DEFAULT, PIO_PERIPH_A},
	};

	/* Configure the spi0 pins */
	pio_configure(spi0_pins);
#if (AT91C_SPI_PCS_DATAFLASH == AT91C_SPI_PCS0_DATAFLASH)
	writel((1 << AT91C_ID_PIOA), (PMC_PCER + AT91C_BASE_PMC));
#endif

#if (AT91C_SPI_PCS_DATAFLASH == AT91C_SPI_PCS1_DATAFLASH)
	writel(((1 << AT91C_ID_PIOA) | (1 << AT91C_ID_PIOC)), (PMC_PCER + AT91C_BASE_PMC));
#endif

	/* Enable the spi0 clock */
	writel((1 << AT91C_ID_SPI0), (PMC_PCER + AT91C_BASE_PMC));
}
#endif /* #ifdef CONFIG_DATAFLASH */

#ifdef CONFIG_SDCARD
void at91_mci0_hw_init(void)
{
	const struct pio_desc mci_pins[] = {
		{"MCCK",	AT91C_PIN_PA(8), 0, PIO_PULLUP, PIO_PERIPH_A},
		{"MCCDA",	AT91C_PIN_PA(7), 0, PIO_PULLUP, PIO_PERIPH_A},
		{"MCDA0",	AT91C_PIN_PA(6), 0, PIO_PULLUP, PIO_PERIPH_A},
		{"MCDA1",	AT91C_PIN_PA(9), 0, PIO_PULLUP, PIO_PERIPH_A},
		{"MCDA2",	AT91C_PIN_PA(10), 0, PIO_PULLUP, PIO_PERIPH_A},
		{"MCDA3",	AT91C_PIN_PA(11), 0, PIO_PULLUP, PIO_PERIPH_A},
		{(char *)0, 0, 0, PIO_DEFAULT, PIO_PERIPH_A},

	};

	/* Configure the PIO controller */
	writel((1 << AT91C_ID_PIOA), (PMC_PCER + AT91C_BASE_PMC));
	pio_configure(mci_pins);

	/* Enable the clock */
	writel((1 << AT91C_ID_MCI), (PMC_PCER + AT91C_BASE_PMC));
}
#endif /* #ifdef CONFIG_SDCARD */

#ifdef CONFIG_NANDFLASH
void nandflash_hw_init(void)
{
	unsigned int reg;

	/* Configure PIOs */
	const struct pio_desc nand_pins[] = {
		{"NANDCS",	CONFIG_SYS_NAND_ENABLE_PIN,	1, PIO_PULLUP, PIO_OUTPUT},
		{(char *)0, 	0, 0, PIO_DEFAULT, PIO_PERIPH_A},
	};

	/* Setup Smart Media, first enable the address range of CS3 in HMATRIX user interface  */
	reg = readl(AT91C_BASE_CCFG + CCFG_EBICSA);
	reg |= AT91C_EBI_CS3A_SM;

	writel(reg, AT91C_BASE_CCFG + CCFG_EBICSA);

	/* Configure SMC CS3 */
	writel((AT91C_SMC_NWESETUP_(2)
		| AT91C_SMC_NCS_WRSETUP_(0)
		| AT91C_SMC_NRDSETUP_(2)
		| AT91C_SMC_NCS_RDSETUP_(0)),
		AT91C_BASE_SMC + SMC_SETUP3);

	writel((AT91C_SMC_NWEPULSE_(4)
		| AT91C_SMC_NCS_WRPULSE_(4)
		| AT91C_SMC_NRDPULSE_(4)
		| AT91C_SMC_NCS_RDPULSE_(4)),
		AT91C_BASE_SMC + SMC_PULSE3);

	writel((AT91C_SMC_NWECYCLE_(7)
		|  AT91C_SMC_NRDCYCLE_(7)),
		AT91C_BASE_SMC + SMC_CYCLE3);

	writel((AT91C_SMC_READMODE
		| AT91C_SMC_WRITEMODE
		| AT91C_SMC_NWAITM_NWAIT_DISABLE
		| AT91C_SMC_DBW_WIDTH_BITS_16
		| AT91_SMC_TDF_(3)),
		AT91C_BASE_SMC + SMC_CTRL3);

	/* Configure the PIO controller */
	pio_configure(nand_pins);
	writel((1 << AT91C_ID_PIOC), PMC_PCER + AT91C_BASE_PMC);
}

void nandflash_config_buswidth(unsigned char busw)
{
	unsigned long csa;

	csa = readl(AT91C_BASE_SMC + SMC_CTRL3);

	if (busw == 0)
		csa |= AT91C_SMC_DBW_WIDTH_BITS_8;
	else
		csa |= AT91C_SMC_DBW_WIDTH_BITS_16;

	writel(csa, AT91C_BASE_SMC + SMC_CTRL3);
}
#endif /* #ifdef CONFIG_NANDFLASH */

