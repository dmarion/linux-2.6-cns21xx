/*
 *  Copyright (c) 2008 Cavium Networks
 *  Copyright (c) 2010 Gabor Juhos <juhosg@openwrt.org>
 *
 *  This file is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License, Version 2, as
 *  published by the Free Software Foundation.
 */

#ifndef _CNS21XX_IO_H
#define _CNS21XX_IO_H

#define IO_SPACE_LIMIT	0xffffffff

#define __io(p)		__typesafe_io(p)
#define __mem_pci(a)	(a)

#endif /* _CNS21XX_IO_H */
