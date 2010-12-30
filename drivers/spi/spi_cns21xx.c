/*******************************************************************************
 *
 *  Copyright (c) 2008 Cavium Networks 
 *  Copyright (c) 2010 Damjan Marion 
 * 
 *  This file is free software; you can redistribute it and/or modify 
 *  it under the terms of the GNU General Public License, Version 2, as 
 *  published by the Free Software Foundation. 
 *
 *  This file is distributed in the hope that it will be useful, 
 *  but AS-IS and WITHOUT ANY WARRANTY; without even the implied warranty of 
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE, TITLE, or 
 *  NONINFRINGEMENT.  See the GNU General Public License for more details. 
 *
 *  You should have received a copy of the GNU General Public License 
 *  along with this file; if not, write to the Free Software 
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA or 
 *  visit http://www.gnu.org/licenses/. 
 *
 *  This file may also be available under a different license from Cavium. 
 *  Contact Cavium Networks for more information
 *
 ******************************************************************************/

#include <linux/init.h>
#include <linux/spinlock.h>
#include <linux/workqueue.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/errno.h>
#include <linux/err.h>
#include <linux/clk.h>
#include <linux/platform_device.h>

#include <linux/spi/spi.h>
#include <linux/spi/spi_bitbang.h>
#include <linux/mtd/partitions.h>

#include <asm/io.h>
#include <asm/dma.h>
#include <mach/hardware.h>
#include <mach/cns21xx.h>

#define CNS21XX_SPI_DEBUG

struct cns21xx_spi {
	/* bitbang has to be first */
	struct spi_bitbang	 bitbang;
	struct completion	 done;

	int			 len;
	int			 count;

	/* data buffers */
	const unsigned char	*tx;
	unsigned char		*rx;

	struct spi_master	*master;
	struct device		*dev;
	struct spi_device	*spi_dev[4];

	int			board_count;
	struct spi_board_info	board_info[4];
};

extern u32 APB_clock;

static inline u8 cns21xx_spi_bus_idle(void)
{
	return ((SPI_SERVICE_STATUS_REG & 0x1) ? 0 : 1);
}

static inline u8 cns21xx_spi_tx_buffer_empty(void)
{
	return ((SPI_INTERRUPT_STATUS_REG & (0x1 << 3)) ? 1 : 0);
}

static inline u8 cns21xx_spi_rx_buffer_full(void)
{
	return ((SPI_INTERRUPT_STATUS_REG & (0x1 << 2)) ? 1 : 0);
}

static u8 cns21xx_spi_tx_rx(u8 tx_channel, u8 tx_eof_flag, u32 tx_data, u32 *rx_data)
{
	printk("DAMJAN cns21xx_spi_tx_rx start\n");
	
	u8 rx_channel;
	u8 rx_eof_flag;

	while (!cns21xx_spi_bus_idle())
		; // do nothing

	while (!cns21xx_spi_tx_buffer_empty())
		; // do nothing

	SPI_TRANSMIT_CONTROL_REG &= ~(0x7);
	SPI_TRANSMIT_CONTROL_REG |= (tx_channel & 0x3) | ((tx_eof_flag & 0x1) << 2);

	SPI_TRANSMIT_BUFFER_REG = tx_data;

	while (!cns21xx_spi_rx_buffer_full())
		; // do nothing

	rx_channel = (SPI_RECEIVE_CONTROL_REG & 0x3);

	rx_eof_flag = (SPI_RECEIVE_CONTROL_REG & (0x1 << 2)) ? 1 : 0;

	*rx_data = SPI_RECEIVE_BUFFER_REG;

	if ((tx_channel != rx_channel) || (tx_eof_flag != rx_eof_flag)) {
		return 0;
	} else {
		return 1;
	}
	printk("DAMJAN cns21xx_spi_tx_rx end\n");
}

static inline struct cns21xx_spi *to_hw(struct spi_device *sdev)
{
	printk("DAMJAN to_hw start\n");
	return spi_master_get_devdata(sdev->master);
	printk("DAMJAN to_hw start\n");
}

static void cns21xx_spi_chipselect(struct spi_device *spi, int value)
{
	unsigned int spi_config;
	int i;
	printk("DAMJAN cns21xx_spi_chipselect start\n");

	switch (value) {
	case BITBANG_CS_INACTIVE:
		break;

	case BITBANG_CS_ACTIVE:
		spi_config = SPI_CONFIGURATION_REG;
		if (spi->mode & SPI_CPHA)
			spi_config |= (0x1 << 13);
		else
			spi_config &= ~(0x1 << 13);

		if (spi->mode & SPI_CPOL)
			spi_config |= (0x1 << 14);
		else
			spi_config &= ~(0x1 << 14);

		/* write new configration */
		SPI_CONFIGURATION_REG = spi_config;

		SPI_TRANSMIT_CONTROL_REG &= ~(0x7);
		SPI_TRANSMIT_CONTROL_REG |= (spi->chip_select & 0x3);

		for (i = 0; i < 8; i++) {
			if (spi->max_speed_hz > (APB_clock >> i))
				break;
		}
#ifdef CNS21XX_SPI_DEBUG
		printk("[CNS21XX_SPI_DEBUG] APB_clock:%u\n", APB_clock);
		printk("[CNS21XX_SPI_DEBUG] spi->max_speed_hz:%u\n", spi->max_speed_hz);
		printk("[CNS21XX_SPI_DEBUG] SPI bit rate control val:%d\n", i);
#endif
		SPI_BIT_RATE_CONTROL_REG = i;

		break;
	}
	printk("DAMJAN cns21xx_spi_chipselect end\n");
}

static int cns21xx_spi_setup(struct spi_device *spi)
{
	printk("DAMJAN cns21xx_spi_setup start\n");
	
	if (!spi->bits_per_word)
		spi->bits_per_word = 8;

	return 0;
	printk("DAMJAN cns21xx_spi_setup end\n");
}

static int cns21xx_spi_txrx(struct spi_device *spi, struct spi_transfer *t)
{
	struct cns21xx_spi *hw = to_hw(spi);
	int error = 0;

	hw->tx = t->tx_buf;
	hw->rx = t->rx_buf;
	hw->len = t->len;
	hw->count = 0;
	printk("DAMJAN cns21xx_spi_txrx start\n");

#ifdef CNS21XX_SPI_DEBUG
	printk("[CNS21XX_SPI_DEBUG] txrx: tx %p, rx %p, len %d\n", t->tx_buf, t->rx_buf, t->len);
	if (hw->tx) {
		int i;
		for (i = 0; i < t->len; i++) {
			printk("[CNS21XX_SPI_DEBUG] t->tx_buf[%02d]: 0x%02x\n", i, hw->tx[i]);
		}
	}
#endif
	if (hw->tx) {
		int i;
		u32 rx_data;
		for (i = 0; i < (hw->len - 1); i++) {
			cns21xx_spi_tx_rx(spi->chip_select, 0, hw->tx[i], &rx_data);
			if (hw->rx) {
				hw->rx[i] = rx_data;
#ifdef CNS21XX_SPI_DEBUG
				printk("[CNS21XX_SPI_DEBUG] hw->rx[%02d]:0x%02x\n", i, hw->rx[i]);
#endif
			}
		}
		if (t->last_in_message_list) {
			cns21xx_spi_tx_rx(spi->chip_select, 1, hw->tx[i], &rx_data);
			if (hw->rx) {
				hw->rx[i] = rx_data;
#ifdef CNS21XX_SPI_DEBUG
				printk("[CNS21XX_SPI_DEBUG] hw->rx[%02d]:0x%02x\n", i, hw->rx[i]);
#endif
			}
		} else {
			cns21xx_spi_tx_rx(spi->chip_select, 0, hw->tx[i], &rx_data);
		}
		goto done;
	}

	if (hw->rx) {
		int i;
		u32 rx_data;
		for (i = 0; i < (hw->len - 1); i++) {
			cns21xx_spi_tx_rx(spi->chip_select, 0, 0xff, &rx_data);
			hw->rx[i] = rx_data;
#ifdef CNS21XX_SPI_DEBUG
			printk("[CNS21XX_SPI_DEBUG] hw->rx[%02d]:0x%02x\n", i, hw->rx[i]);
#endif
		}
		if (t->last_in_message_list) {
			cns21xx_spi_tx_rx(spi->chip_select, 1, 0xff, &rx_data);
			hw->rx[i] = rx_data;
#ifdef CNS21XX_SPI_DEBUG
			printk("[CNS21XX_SPI_DEBUG] hw->rx[%02d]:0x%02x\n", i, hw->rx[i]);
#endif
		} else {
			cns21xx_spi_tx_rx(spi->chip_select, 0, 0xff, &rx_data);
			hw->rx[i] = rx_data;
#ifdef CNS21XX_SPI_DEBUG
			printk("[CNS21XX_SPI_DEBUG] hw->rx[%02d]:0x%02x\n", i, hw->rx[i]);
#endif
		}
	}

done:
	printk("DAMJAN cns21xx_spi_txrx end\n");
	return t->len;
}

static int cns21xx_spi_setupxfer(struct spi_device *spi,  struct spi_transfer *t)
{
	printk("DAMJAN cns21xx_spi_setupxfer\n");
	return 0;
}


static int __init cns21xx_spi_probe(struct platform_device *pdev)
{
	struct cns21xx_spi *hw;
	struct spi_master *master;
	unsigned int receive_data;
	int err = 0;
	printk("DAMJAN cns21xx_spi_probe start\n");
	master = spi_alloc_master(&pdev->dev, sizeof(struct cns21xx_spi));
	if (master == NULL) {
		dev_err(&pdev->dev, "No memory for spi_master\n");
		err = -ENOMEM;
		goto err_nomem;
	}

	master->bus_num = 1;
	master->num_chipselect = 4;

	hw = spi_master_get_devdata(master);
	memset(hw, 0, sizeof(struct cns21xx_spi));

	hw->master = spi_master_get(master);
	hw->dev = &pdev->dev;

	platform_set_drvdata(pdev, hw);
	init_completion(&hw->done);

	// Clear spurious interrupt sources
	SPI_INTERRUPT_STATUS_REG = (0xF << 4);

	// Disable SPI interrupt
	SPI_INTERRUPT_ENABLE_REG = 0;

	// Enable SPI
	SPI_CONFIGURATION_REG |= (0x1 << 31);
	/* setup the state for the bitbang driver */

	//master->mode_bits          = SPI_MODE_0;
	//master->num_chipselect     = hw->pdata->num_cs;
	//master->bus_num            = hw->pdata->bus_num;
	//hw->bitbang.setup_transfer = nuc900_spi_setupxfer;


	hw->bitbang.master         = hw->master;
	hw->bitbang.chipselect     = cns21xx_spi_chipselect;
	hw->bitbang.txrx_bufs      = cns21xx_spi_txrx;
	hw->bitbang.master->setup  = cns21xx_spi_setup;
	hw->bitbang.setup_transfer = cns21xx_spi_setupxfer;

	dev_dbg(hw->dev, "bitbang at %p\n", &hw->bitbang);

	/* register our spi controller */

	err = spi_bitbang_start(&hw->bitbang);
	if (err) {
		dev_err(&pdev->dev, "Failed to register SPI master\n");
		goto err_register;
	}

#ifdef CNS21XX_SPI_DEBUG
{
	int i;
	u32 rx_data1, rx_data2, rx_data3;

	cns21xx_spi_tx_rx(0, 0, 0x9f, &rx_data1);
	cns21xx_spi_tx_rx(0, 0, 0xff, &rx_data1);
	cns21xx_spi_tx_rx(0, 0, 0xff, &rx_data2);
	cns21xx_spi_tx_rx(0, 1, 0xff, &rx_data3);
	printk("[CNS21XX_SPI_DEBUG] manufacturer: %x\n", rx_data1);
	printk("[CNS21XX_SPI_DEBUG] device:       %x\n", ((rx_data2 & 0xff) << 8) | (u16) (rx_data3 & 0xff));

	cns21xx_spi_tx_rx(0, 0, 0x03, &rx_data1);
	cns21xx_spi_tx_rx(0, 0, 0x00, &rx_data1);
	cns21xx_spi_tx_rx(0, 0, 0x00, &rx_data1);
	cns21xx_spi_tx_rx(0, 0, 0x00, &rx_data1);
	for (i = 0; i < 15; i++) {
		cns21xx_spi_tx_rx(0, 0, 0xff, &rx_data1);
		printk("[CNS21XX_SPI_DEBUG] flash[%02d]:0x%02x\n", i, rx_data1 & 0xff);
	}
	cns21xx_spi_tx_rx(0, 1, 0xff, &rx_data1);
	printk("[CNS21XX_SPI_DEBUG] flash[%02d]:0x%02x\n", i, rx_data1 & 0xff);
}
#endif
	printk("DAMJAN cns21xx_spi_probe end\n");

	return 0;

err_register:
	spi_master_put(hw->master);;

err_nomem:
	return err;
}

static int cns21xx_spi_remove(struct platform_device *dev)
{
	struct cns21xx_spi *hw = platform_get_drvdata(dev);

	platform_set_drvdata(dev, NULL);

	spi_unregister_master(hw->master);

	//cns21xx_spi_clk_disable();

	spi_master_put(hw->master);

	return 0;
}


#ifdef CONFIG_PM

static int cns21xx_spi_suspend(struct platform_device *pdev, pm_message_t msg)
{
	struct cns21xx_spi *hw = platform_get_drvdata(pdev);

	//cns21xx_spi_clk_disable();
	return 0;
}

static int cns21xx_spi_resume(struct platform_device *pdev)
{
	struct cns21xx_spi *hw = platform_get_drvdata(pdev);

	//cns21xx_spi_clk_enable()
	return 0;
}

#else
#define cns21xx_spi_suspend	NULL
#define cns21xx_spi_resume	NULL
#endif

static void __init cns21xx_spi_hw_init(void)
{
	u32 receive_data;

	// Enable SPI pins
	HAL_MISC_ENABLE_SPI_PINS();

	// Enable SPI clock
	HAL_PWRMGT_ENABLE_SPI_CLOCK();

	// Disable SPI serial flash access through 0x30000000 region
	HAL_MISC_DISABLE_SPI_SERIAL_FLASH_BANK_ACCESS();

	/*
	 * Note SPI is NOT enabled after this function is invoked!!
	 */
	SPI_CONFIGURATION_REG =
		(((0x0 & 0x3) << 0) | /* 8bits shift length */
		 (0x0 << 9) | /* general SPI mode */
		 (0x0 << 10) | /* disable FIFO */
		 (0x1 << 11) | /* SPI master mode */
		 (0x0 << 12) | /* disable SPI loopback mode */
		 (0x0 << 13) | /* clock phase */
		 (0x0 << 14) | /* clock polarity */
		 (0x0 << 24) | /* disable SPI Data Swap */
		 (0x0 << 30) | /* disable SPI High Speed Read for BootUp */
		 (0x0 << 31)); /* disable SPI */

	SPI_BIT_RATE_CONTROL_REG = 0x1; // PCLK/2

	// Configure SPI's Tx channel
	SPI_TRANSMIT_CONTROL_REG = 0;

	// Configure Tx FIFO Threshold
	SPI_FIFO_TRANSMIT_CONFIG_REG &= ~(0x03 << 4);
	SPI_FIFO_TRANSMIT_CONFIG_REG |= ((0x0 & 0x03) << 4);

	// Configure Rx FIFO Threshold
	SPI_FIFO_RECEIVE_CONFIG_REG &= ~(0x03 << 4);
	SPI_FIFO_RECEIVE_CONFIG_REG |= ((0x0 & 0x03) << 4);

	SPI_INTERRUPT_ENABLE_REG = 0;

	// Clear spurious interrupt sources
	SPI_INTERRUPT_STATUS_REG = (0xF << 4);

	receive_data = SPI_RECEIVE_BUFFER_REG;

	return;
}

static struct platform_driver cns21xx_spidrv = {
	.remove		= __devexit_p(cns21xx_spi_remove),
	.suspend	= cns21xx_spi_suspend,
	.resume		= cns21xx_spi_resume,
	.driver		= {
		.name	= "cns21xx-spi",
		.owner	= THIS_MODULE,
	},
};

static int __init cns21xx_spi_init(void)
{
	printk("STR8100 SPI: init\n");
	cns21xx_spi_hw_init();

	return platform_driver_probe(&cns21xx_spidrv, cns21xx_spi_probe);
}

static void __exit cns21xx_spi_exit(void)
{
	platform_driver_unregister(&cns21xx_spidrv);
}

module_init(cns21xx_spi_init);
module_exit(cns21xx_spi_exit);

MODULE_DESCRIPTION("CNS21xx SPI Driver");
MODULE_AUTHOR("Cavium Networks");
MODULE_LICENSE("GPL");
