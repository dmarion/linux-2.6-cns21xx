/*******************************************************************************
 *
 *  Copyright (c) 2008 Cavium Networks 
 *  Copyright (c) 2010 Damjan Marion <damjan.marion@gmail.com>
 * 
 *  Based on "ehci-fsl.c" by Randy Vinson <rvinson@mvista.com>
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

#include <linux/platform_device.h>
#include <mach/hardware.h>
#include <mach/cns21xx-regs.h>

static int ehci_cns21xx_reset(struct usb_hcd *hcd)
{
	struct ehci_hcd *ehci = hcd_to_ehci(hcd);
	int retval;

	/* Reinitialize EHCI Host Controller */
	/*  * Command Register: Master Enable & Operation Register Access Enable */
	__raw_writel(0x06, CNS21XX_USB20_CONFIG_BASE_VIRT + 0x04);
	/*  * Operational Mode Enable Register: Set FIFO threshold to 512 bytes */
	__raw_writel(0x60, CNS21XX_USB20_CONFIG_BASE_VIRT + 0x40);
	msleep(100);

	ehci->caps = hcd->regs;
	ehci->regs = hcd->regs + HC_LENGTH(ehci_readl(ehci, &ehci->caps->hc_capbase));
	dbg_hcs_params(ehci, "reset");
	dbg_hcc_params(ehci, "reset");

	/* cache this readonly data; minimize chip reads */
	ehci->hcs_params = ehci_readl(ehci, &ehci->caps->hcs_params);
	ehci->sbrn = 0x20;

	retval = ehci_halt(ehci);
	if (retval)
		return retval;

	retval =  ehci_init(hcd);
	if (retval)
		return retval;

	ehci_reset(ehci);

	ehci_port_power(ehci, 0);
	pr_debug(" CNS21XX EHCI USB Controller reset done.\n");
	return 0;
}

static const struct hc_driver ehci_cns21xx_hc_driver = {
	.description 		= hcd_name,
        .product_desc 		= "Cavium ECONA CNS21xx On-Chip EHCI Host Controller",
        .hcd_priv_size 		= sizeof(struct ehci_hcd),
	.irq 			= ehci_irq,
	.flags 			= HCD_MEMORY | HCD_USB2,
	.reset 			= ehci_cns21xx_reset, 
	.start 			= ehci_run,
	.stop			= ehci_stop,
	.shutdown		= ehci_shutdown,
	.urb_enqueue		= ehci_urb_enqueue,
	.urb_dequeue		= ehci_urb_dequeue,
	.endpoint_disable	= ehci_endpoint_disable,
	.endpoint_reset		= ehci_endpoint_reset,
	.get_frame_number	= ehci_get_frame,
	.hub_status_data	= ehci_hub_status_data,
	.hub_control		= ehci_hub_control,
#if defined(CONFIG_PM)
	.bus_suspend		= ehci_bus_suspend,
	.bus_resume		= ehci_bus_resume,
#endif
	.relinquish_port	= ehci_relinquish_port,
	.port_handed_over	= ehci_port_handed_over,

	.clear_tt_buffer_complete	= ehci_clear_tt_buffer_complete,
};

static int __devinit ehci_cns21xx_probe(struct platform_device *pdev)
{
	const struct hc_driver *driver = &ehci_cns21xx_hc_driver;
	struct usb_hcd *hcd;
	struct resource *res;
	int retval = 0;
	int irq;

	pr_debug("Initializing CNS21XX EHCI USB Controller driver\n");

	if (usb_disabled()) {
		retval = -ENODEV;
		goto err1;
	}

	res = platform_get_resource(pdev, IORESOURCE_IRQ, 0);
	if (!res) {
		dev_err(&pdev->dev,
			"Found HC with no IRQ assigned. Check %s setup!\n",
			dev_name(&pdev->dev));
		retval = -ENODEV;
		goto err1;
	}
	irq = res->start;

	hcd = usb_create_hcd(driver, &pdev->dev, dev_name(&pdev->dev));
	if (!hcd) {
		retval = -ENOMEM;
		goto err1;
	}

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!res) {
		dev_err(&pdev->dev,
			"Found HC with no register addr. Check %s setup!\n",
			dev_name(&pdev->dev));
		retval = -ENODEV;
		goto err2;
	}

	hcd->rsrc_start	= res->start;
	hcd->rsrc_len	= res->end - res->start + 1;

	if (!request_mem_region(hcd->rsrc_start, hcd->rsrc_len,
				driver->description)) {
		dev_dbg(&pdev->dev, "controller already in use\n");
		retval = -EBUSY;
		goto err2;
	}
	hcd->regs = ioremap(hcd->rsrc_start, hcd->rsrc_len);

	if (hcd->regs == NULL) {
		dev_dbg(&pdev->dev, "error mapping memory\n");
		retval = -EFAULT;
		goto err3;
	}

	msleep(100);

	retval = usb_add_hcd(hcd, irq, IRQF_DISABLED);
	if (retval != 0)
		goto err4;

	platform_set_drvdata(pdev, hcd);

	return retval;

      err4:
	iounmap(hcd->regs);
      err3:
	release_mem_region(hcd->rsrc_start, hcd->rsrc_len);
      err2:
	usb_put_hcd(hcd);
      err1:
	return retval;
}

static int __devexit ehci_cns21xx_remove(struct platform_device *pdev)
{
	struct usb_hcd *hcd = platform_get_drvdata(pdev);

	usb_remove_hcd(hcd);
	iounmap(hcd->regs);
	release_mem_region(hcd->rsrc_start, hcd->rsrc_len);
	usb_put_hcd(hcd);
	platform_set_drvdata(pdev, NULL);

	return 0;
}

MODULE_ALIAS("platform:cns21xx-ehci");

static struct platform_driver ehci_cns21xx_driver = {
	.probe = ehci_cns21xx_probe,
	.remove = __devexit_p(ehci_cns21xx_remove),
	.driver = {
		.name  = "cns21xx-ehci",
		.owner = THIS_MODULE,
	},
};
