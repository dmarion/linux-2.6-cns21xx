/*******************************************************************************
 *
 *  Copyright (c) 2008 Cavium Networks
 *  Copyright (c) 2011 Damjan Marion
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
#include <linux/mtd/partitions.h>

#include <asm/io.h>
#include <asm/dma.h>
#include <mach/hardware.h>
#include <mach/cns21xx-spi.h>
#include <mach/cns21xx-misc.h>
#include <mach/cns21xx-pm.h>

struct cns21xx_spi {
	struct spi_master	*master;
	struct work_struct	work;
	spinlock_t		lock;
	struct list_head	queue;
	struct workqueue_struct	*workqueue;

	int			 len;
	int			 count;

	/* data buffers */
	const unsigned char	*tx;
	unsigned char		*rx;
};

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
}

static void cns21xx_spi_chipselect_active(struct spi_device *spi)
{
	unsigned int spi_config;
	int i;

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
		if (spi->max_speed_hz > (cns21xx_get_apb_freq() >> i))
			break;
	}
	dev_dbg(&spi->dev, "APB freq:%lu\n", cns21xx_get_apb_freq());
	dev_dbg(&spi->dev, "spi->max_speed_hz:%u\n", spi->max_speed_hz);
	dev_dbg(&spi->dev, "SPI bit rate control val:%d\n", i);
	SPI_BIT_RATE_CONTROL_REG = i;
}

static int cns21xx_spi_setup(struct spi_device *spi)
{
	if (!spi->bits_per_word)
		spi->bits_per_word = 8;
	return 0;
}

static int cns21xx_spi_txrx(struct spi_device *spi, struct spi_transfer *t, int last_in_message_list)
{
	struct cns21xx_spi *hw = spi_master_get_devdata(spi->master);
	hw->tx = t->tx_buf;
	hw->rx = t->rx_buf;
	hw->len = t->len;
	hw->count = 0;

	dev_dbg(&spi->dev, "txrx: tx %p, rx %p, len %d\n", t->tx_buf, t->rx_buf, t->len);
	if (hw->tx) {
		int i;
		u32 rx_data;
		for (i = 0; i < (hw->len - 1); i++) {
			cns21xx_spi_tx_rx(spi->chip_select, 0, hw->tx[i], &rx_data);
			if (hw->rx)
				hw->rx[i] = rx_data;
		}
		if (last_in_message_list) {
			cns21xx_spi_tx_rx(spi->chip_select, 1, hw->tx[i], &rx_data);
			if (hw->rx)
				hw->rx[i] = rx_data;
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
		}
		if (last_in_message_list) {
			cns21xx_spi_tx_rx(spi->chip_select, 1, 0xff, &rx_data);
		} else {
			cns21xx_spi_tx_rx(spi->chip_select, 0, 0xff, &rx_data);
		}
		hw->rx[i] = rx_data;
	}

done:
	return t->len;
}

static void cns21xx_spi_work_one(struct cns21xx_spi *c, struct spi_message *m)
{
	struct spi_device 	*spi = m->spi;
	struct 			spi_transfer *t;
	unsigned 		nsecs = 100;
	unsigned 		cs_change = 1;
	int			status;

	list_for_each_entry (t, &m->transfers, transfer_list) {

		if (cs_change) {
			cns21xx_spi_chipselect_active(spi);
			ndelay(nsecs);
		}
		cs_change = t->cs_change;

		if (!t->tx_buf && !t->rx_buf && t->len) {
			status = -EINVAL;
			break;
		}

		if (t->len) {
			if (!m->is_dma_mapped)
				t->rx_dma = t->tx_dma = 0;

			if (t->transfer_list.next == &m->transfers) {
				/* Last trasfer, CNS21xx will deselect CS automatically */
				status = cns21xx_spi_txrx(spi, t, 1);
			} else {
				status = cns21xx_spi_txrx(spi, t, 0);
			}
		}
		if (status > 0)
			m->actual_length += status;
		if (status != t->len) {
			/* always report some kind of error */
			if (status >= 0)
				status = -EREMOTEIO;
			break;
		}
		status = 0;

		if (!cs_change)
			continue;
		if (t->transfer_list.next == &m->transfers)
			break;

		ndelay(nsecs);
	}

	m->status = status;
	m->complete(m->context);
}

static void cns21xx_spi_work(struct work_struct *work)
{
	struct cns21xx_spi *c = container_of(work, struct cns21xx_spi, work);
	unsigned long flags;

	spin_lock_irqsave(&c->lock, flags);
	while (!list_empty(&c->queue)) {
		struct spi_message *m;

		m = container_of(c->queue.next, struct spi_message, queue);
		list_del_init(&m->queue);
		spin_unlock_irqrestore(&c->lock, flags);

		cns21xx_spi_work_one(c, m);

		spin_lock_irqsave(&c->lock, flags);
	}
	spin_unlock_irqrestore(&c->lock, flags);
}

static int cns21xx_spi_transfer(struct spi_device *spi, struct spi_message *m)
{
	struct cns21xx_spi	*c;
	unsigned long		flags;
	int			status = 0;

	m->actual_length = 0;
	m->status = -EINPROGRESS;

	c = spi_master_get_devdata(spi->master);

	spin_lock_irqsave(&c->lock, flags);
	if (!spi->max_speed_hz)
		status = -ENETDOWN;
	else {
		list_add_tail(&m->queue, &c->queue);
		queue_work(c->workqueue, &c->work);
	}
	spin_unlock_irqrestore(&c->lock, flags);

	return status;

}

static int __init cns21xx_spi_probe(struct platform_device *pdev)
{
	struct spi_master	*master;
	struct cns21xx_spi	*c;
	int ret;

	master = spi_alloc_master(&pdev->dev, sizeof *c);
	if (!master) {
		dev_err(&pdev->dev, "master allocation failed\n");
		return -ENOMEM;
	}

	master->bus_num = 1;
	master->num_chipselect = 4;
	master->setup = cns21xx_spi_setup;
	master->transfer = cns21xx_spi_transfer;
	master->cleanup = NULL;
	master->mode_bits = SPI_CPOL | SPI_CPHA | SPI_CS_HIGH;

	dev_set_drvdata(&pdev->dev, master);

	c = spi_master_get_devdata(master);
	c->master = master;

	INIT_WORK(&c->work, cns21xx_spi_work);
	spin_lock_init(&c->lock);
	INIT_LIST_HEAD(&c->queue);

	c->workqueue = create_singlethread_workqueue(
			dev_name(master->dev.parent));
	if (c->workqueue == NULL) {
		dev_err(&pdev->dev, "failed to create workqueue\n");
		ret = -EBUSY;
		goto err0;
	}

	// Clear spurious interrupt sources
	SPI_INTERRUPT_STATUS_REG = (0xF << 4);

	// Disable SPI interrupt
	SPI_INTERRUPT_ENABLE_REG = 0;

	// Enable SPI
	SPI_CONFIGURATION_REG |= (0x1 << 31);

	// Register Master
	ret = spi_register_master(master);
	if (ret)
		goto err1;
	return 0;

err1:
	destroy_workqueue(c->workqueue);
err0:
	platform_set_drvdata(pdev, NULL);
	spi_master_put(master);
	return ret;
}

static int __exit cns21xx_remove(struct platform_device *dev)
{
	struct spi_master *master = spi_master_get(platform_get_drvdata(dev));
	struct cns21xx_spi *c = spi_master_get_devdata(master);

	spi_unregister_master(master);
	destroy_workqueue(c->workqueue);
	platform_set_drvdata(dev, NULL);
	spi_master_put(master);
	return 0;
}

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

MODULE_ALIAS("platform:cns21xx-spi");

static struct platform_driver cns21xx_driver = {
	.remove = __exit_p(cns21xx_remove),
	.driver = {
		.name = "cns21xx-spi",
		.owner = THIS_MODULE,
	},
};

static int __init cns21xx_spi_init(void)
{
	cns21xx_spi_hw_init();
	return platform_driver_probe(&cns21xx_driver, cns21xx_spi_probe);
}

static void __exit cns21xx_spi_exit(void)
{
	platform_driver_unregister(&cns21xx_driver);
}

module_init(cns21xx_spi_init);
module_exit(cns21xx_spi_exit);

MODULE_DESCRIPTION("CNS21xx SPI Driver");
MODULE_AUTHOR("Cavium networks");
MODULE_AUTHOR("Damjan Marion <damjan.marion@gmail.com>");
MODULE_LICENSE("GPL");

