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
#include <mach/cns21xx.h>


static int ehci_cns21xx_setup(struct usb_hcd *hcd)
{
	struct ehci_hcd *ehci = hcd_to_ehci(hcd);
	int retval;

	ehci->caps = hcd->regs;
	ehci->regs = hcd->regs + HC_LENGTH(ehci_readl(ehci, &ehci->caps->hc_capbase));
	dbg_hcs_params(ehci, "reset");
	dbg_hcc_params(ehci, "reset");

	/* cache this readonly data; minimize chip reads */
	ehci->hcs_params = ehci_readl(ehci, &ehci->caps->hcs_params);

	retval = ehci_halt(ehci);
	if (retval)
		return retval;

	retval =  ehci_init(hcd);
	
	return retval;
}

#ifdef	CONFIG_PM
//mkl070226: the functionality of suspend/resume is not complete
static int ehci_suspend (struct usb_hcd *hcd)
{
	printk("%s: not implemented, just pass it\n",__FUNCTION__);
	return 0;
}
static int ehci_resume (struct usb_hcd *hcd)
{
	printk("%s: not implemented, just pass it\n",__FUNCTION__);
	return 0;
}
#endif

static const struct hc_driver ehci_cns21xx_hc_driver = {
	.description 		= hcd_name,
        .product_desc 		= "Cavium ECONA CNS21xx On-Chip EHCI Host Controller",
        .hcd_priv_size 		= sizeof(struct ehci_hcd),
	.irq 			= ehci_irq,
	.flags 			= HCD_MEMORY | HCD_USB2,
	.reset 			= ehci_cns21xx_setup, 
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
	struct usb_hcd *hcd;
	const struct hc_driver *driver = &ehci_cns21xx_hc_driver;
	int retval = 0;
	
	if (usb_disabled())
		return -ENODEV;

	/* CNS21xx USB20 Register Init */
	__raw_writel(0x106, SYSVA_USB20_CONFIG_BASE_ADDR + 0x04);
	__raw_writel((3 << 5) | 0x20000, SYSVA_USB20_OPERATION_BASE_ADDR + 0x40);
	mdelay(100);

	hcd = usb_create_hcd(driver, &pdev->dev, dev_name(&pdev->dev));
	if (!hcd) { 
		retval = -ENOMEM;
		return retval;
	}

	hcd->regs = (unsigned int *)SYSVA_USB20_OPERATION_BASE_ADDR;
	hcd->rsrc_start = SYSPA_USB20_OPERATION_BASE_ADDR;
	hcd->rsrc_len = 4096;
	hcd->driver = driver;
	retval = usb_add_hcd(hcd, IRQ_USB20, IRQF_SHARED);
	if (retval == 0) {
		return retval;
	}
	printk("str8100 ehci init fail, %d\n", retval);
	usb_put_hcd(hcd);
	return retval;
}

static int ehci_cns21xx_remove(struct platform_device *pdev)
{
	struct usb_hcd *hcd = platform_get_drvdata(pdev);
	usb_remove_hcd(hcd);
	usb_put_hcd(hcd);
	return 0;
}

MODULE_ALIAS("platform:cns21xx-ehci");

static struct platform_driver ehci_cns21xx_driver = {
	.probe = ehci_cns21xx_probe,
	.remove = ehci_cns21xx_remove,
	.driver = {
		.name = "cns21xx-ehci",
	},
};

