/*
 *  Copyright (c) 2008 Cavium Networks
 *  Copyright (c) 2010 Gabor Juhos <juhosg@openwrt.org>
 *
 *  This file is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License, Version 2, as
 *  published by the Free Software Foundation.
 */

#include <linux/platform_device.h>

#include <mach/cns21xx.h>

#define DRIVER_NAME	"cns21xx-ohci"

static int __devinit cns21xx_ohci_start(struct usb_hcd *hcd)
{
	struct ohci_hcd *ohci = hcd_to_ohci(hcd);
	int ret;

	ret = ohci_init(ohci);
	if (ret)
		return ret;

	ret = ohci_run(ohci);
	if (ret) {
		err("can't start %s", ohci_to_hcd(ohci)->self.bus_name);
		goto err;
	}

	return 0;

err:
	ohci_stop(hcd);
	return ret;
}

static const struct hc_driver ohci_cns21xx_hc_driver = {
	.description		= hcd_name,
	.product_desc		= "cns21xx-ohci",
	.hcd_priv_size		= sizeof(struct ohci_hcd),

	/*
	 * generic hardware linkage
	 */
	.irq			= ohci_irq,
	.flags			= HCD_USB11 | HCD_MEMORY,

	/*
	 * basic lifecycle operations
	 */
	.start			= cns21xx_ohci_start,
	.stop			= ohci_stop,
	.shutdown		= ohci_shutdown,

	/*
	 * managing i/o requests and associated device resources
	 */
	.urb_enqueue		= ohci_urb_enqueue,
	.urb_dequeue		= ohci_urb_dequeue,
	.endpoint_disable	= ohci_endpoint_disable,

	/*
	 * scheduling support
	 */
	.get_frame_number	= ohci_get_frame,

	/*
	 * root hub support
	 */
	.hub_status_data	= ohci_hub_status_data,
	.hub_control		= ohci_hub_control,
	.start_port_reset	= ohci_start_port_reset,
};

static void cns21xx_ohci_init_hc(void)
{
	__raw_writel(0x146, CNS21XX_OHCI_CONFIG_BASE_VIRT + 0x04);
	__raw_writel(0x200, CNS21XX_OHCI_CONFIG_BASE_VIRT + 0x44);
	msleep(100);
}

static int ohci_cns21xx_probe(struct platform_device *pdev)
{
	struct usb_hcd *hcd;
	struct resource *res;
	struct ohci_hcd *ohci;
	int irq;
	int ret;

	if (usb_disabled())
		return -ENODEV;

	res = platform_get_resource(pdev, IORESOURCE_IRQ, 0);
	if (!res) {
		dev_dbg(&pdev->dev, "no IRQ specified for %s\n",
			dev_name(&pdev->dev));
		return -ENODEV;
	}
	irq = res->start;

	hcd = usb_create_hcd(&ohci_cns21xx_hc_driver, &pdev->dev,
			     dev_name(&pdev->dev));
	if (!hcd)
		return -ENOMEM;

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!res) {
		dev_dbg(&pdev->dev, "no base address specified for %s\n",
			dev_name(&pdev->dev));
		ret = -ENODEV;
		goto err_put_hcd;
	}
	hcd->rsrc_start	= res->start;
	hcd->rsrc_len	= res->end - res->start + 1;

	if (!request_mem_region(hcd->rsrc_start, hcd->rsrc_len, hcd_name)) {
		dev_dbg(&pdev->dev, "controller already in use\n");
		ret = -EBUSY;
		goto err_put_hcd;
	}

	hcd->regs = ioremap(hcd->rsrc_start, hcd->rsrc_len);
	if (!hcd->regs) {
		dev_dbg(&pdev->dev, "error mapping memory\n");
		ret = -EFAULT;
		goto err_release_region;
	}

	cns21xx_ohci_init_hc();

	ohci = hcd_to_ohci(hcd);
	ohci->flags |= OHCI_QUIRK_INIT_FMINTERVAL;
	ohci_hcd_init(ohci);

	ret = usb_add_hcd(hcd, irq,  IRQF_DISABLED);
	if (ret)
		goto err_unmap;

	platform_set_drvdata(pdev, hcd);
	return 0;

err_unmap:
	iounmap(hcd->regs);
err_release_region:
	release_mem_region(hcd->rsrc_start, hcd->rsrc_len);
err_put_hcd:
	usb_put_hcd(hcd);
	return ret;
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

static struct platform_driver ohci_cns21xx_driver = {
	.probe		= ohci_cns21xx_probe,
	.remove		= ohci_cns21xx_remove,
	.shutdown	= usb_hcd_platform_shutdown,
	.driver		= {
		.owner	= THIS_MODULE,
		.name	= DRIVER_NAME,
	},
};

MODULE_ALIAS("platform:" DRIVER_NAME);
