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

static int ohci_cns21xx_start(struct usb_hcd *hcd)
{
	struct ohci_hcd	*ohci = hcd_to_ohci(hcd);
	int ret;

	/* Reinitialize OHCI Host Controller */
	/*  * Command Register: Master Enable & Operation Register Access Enable */
	__raw_writel(0x146, CNS21XX_USB11_CONFIG_BASE_VIRT + 0x04);
	/*  * Operational Mode Enable Register: clear */
	__raw_writel(0x200, CNS21XX_USB11_CONFIG_BASE_VIRT + 0x44);
	msleep(100);

	if ((ret = ohci_init(ohci)) < 0)
		return ret;

	if ((ret = ohci_run(ohci)) < 0) {
		err("can't start %s", ohci_to_hcd(ohci)->self.bus_name);
		ohci_stop(hcd);
		return ret;
	}

	return 0;
}

static const struct hc_driver ohci_cns21xx_hc_driver = {
	.description 		= hcd_name,
        .product_desc 		= "Cavium ECONA CNS21xx On-Chip OHCI Host Controller",
        .hcd_priv_size 		= sizeof(struct ohci_hcd),
	.irq 			= ohci_irq,
	.flags 			= HCD_USB11 | HCD_MEMORY,
	.start 			= ohci_cns21xx_start,
	.stop			= ohci_stop,
	.shutdown		= ohci_shutdown,
	.urb_enqueue		= ohci_urb_enqueue,
	.urb_dequeue		= ohci_urb_dequeue,
	.endpoint_disable	= ohci_endpoint_disable,
	.get_frame_number	= ohci_get_frame,
	.hub_status_data	= ohci_hub_status_data,
	.hub_control		= ohci_hub_control,
#if defined(CONFIG_PM)
	.bus_suspend		= ohci_bus_suspend,
	.bus_resume		= ohci_bus_resume,
#endif
	.start_port_reset	= ohci_start_port_reset
};

static int ohci_cns21xx_probe(struct platform_device *pdev)
{
	struct usb_hcd *hcd = 0;
	const struct hc_driver *driver = &ohci_cns21xx_hc_driver;
	struct resource *res;
	struct ohci_hcd *ohci;
	int retval;
	int irq;

	if (usb_disabled())
		return -ENODEV;

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

	ohci = hcd_to_ohci(hcd);
	ohci->flags |= OHCI_QUIRK_INIT_FMINTERVAL;
	ohci_hcd_init(ohci);

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

static int ohci_cns21xx_remove(struct platform_device *pdev)
{
	struct usb_hcd *hcd = platform_get_drvdata(pdev);

	usb_remove_hcd(hcd);
	iounmap(hcd->regs);
	release_mem_region(hcd->rsrc_start, hcd->rsrc_len);
	usb_put_hcd(hcd);	
	platform_set_drvdata(pdev, NULL);

	return 0;
}

MODULE_ALIAS("platform:cns21xx-ohci");

static struct platform_driver ohci_hcd_cns21xx_driver = {
	.probe = ohci_cns21xx_probe,
	.remove = ohci_cns21xx_remove,
	.driver = {
		.name = "cns21xx-ohci",
		.owner = THIS_MODULE,
	},
};

