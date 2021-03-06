/*****************************************************************************/
/*
 * Linux driver for Easylon USB Interfaces
 *
 * Copyright (c)2003 Gesytec GmbH, Volker Schober (info@gesytec.de)
 * Licensend for use with Easylon USB Interfaces by Gesytec
 *
 * Please compile lpclinux.c
 *
 */
 /*****************************************************************************/

#ifdef CONFIG_LPC_DEBUG
        static int debug = 0xFF;
#else
        static int debug;
#endif


#ifdef LPC_MAJOR
static int major = LPC_MAJOR;
#else
static int major = 0;
#endif



/* Module paramaters */
MODULE_PARM(debug, "i");
MODULE_PARM_DESC(debug, "Debug enabled or not");


/* table of devices that work with this driver */
static struct pci_device_id lpp_pci_tbl[] = {
	{ GESYTEC_VENDOR_ID, LPP_PRODUCT_ID, PCI_ANY_ID, PCI_ANY_ID, 0, 0, },
	{ },
};
MODULE_DEVICE_TABLE(pci, lpp_pci_tbl);

#ifdef MODULE

static char *isa;
MODULE_PARM(major, "i");
MODULE_PARM(isa, "s");

#endif



/* array of pointers to our devices that are currently connected */
static struct _DEVICE_EXTENSION          *minor_table[MAX_DEVICES];

/* lock to protect the minor_table structure */
static DECLARE_MUTEX (minor_table_mutex);

#if 0
/*
 * File operations needed when we register this driver.
 * This assumes that this driver NEEDS file operations,
 * of course, which means that the driver is expected
 * to have a node in the /dev directory. If the USB
 * device were for a network interface then the driver
 * would use "struct net_driver" instead, and a serial
 * device would use "struct tty_driver".
 */
static struct file_operations lonusb_fops = {
        /*
         * The owner field is part of the module-locking
         * mechanism. The idea is that the kernel knows
         * which module to increment the use-counter of
         * BEFORE it calls the device's open() function.
         * This also means that the kernel can decrement
         * the use-counter again before calling release()
         * or should the open() function fail.
         *
         * Not all device structures have an "owner" field
         * yet. "struct file_operations" and "struct net_device"
         * do, while "struct tty_driver" does not. If the struct
         * has an "owner" field, then initialize it to the value
         * THIS_MODULE and the kernel will handle all module
         * locking for you automatically. Otherwise, you must
         * increment the use-counter in the open() function
         * and decrement it again in the release() function
         * yourself.
         */
        owner:          THIS_MODULE,

        read:           lonusb_read,
        write:          lonusb_write,
        poll:           lonusb_poll,
        ioctl:          lonusb_ioctl,
        open:           lonusb_open,
        release:        lonusb_release,
};
#endif

#if LINUX_VERSION_CODE < 0x020400

static struct file_operations lpc_fops =
{
	NULL,				// llseek
	lpc_read,
	lpc_write,
	NULL,				// readdir
	lpc_poll,
	lpc_ioctl,
	NULL,				// mmap
	lpc_open,
	NULL,				// flush
	lpc_release,
	NULL,				// fsync
	NULL,				// fasync
	NULL,				// check_media_change
	NULL,				// revalidate
	NULL,				// lock
};

#else

static struct file_operations lpc_fops =
    {
    owner:          THIS_MODULE,
                    
    read:           lpc_read,
    write:          lpc_write,
    poll:           lpc_poll,
    ioctl:          lpc_ioctl,
    open:           lpc_open,
    release:        lpc_release,
/*
                THIS_MODULE,			// owner
                NULL,				// llseek
                lpc_read,
                lpc_write,
                NULL,				// readdir
                lpc_poll,
                lpc_ioctl,
                NULL,				// mmap
                lpc_open,
                NULL,				// flush
                lpc_release,
                NULL,				// fsync
                NULL,				// fasync
                NULL,				// lock
                NULL,				// readv
                NULL,				// writev
                NULL,				// sendpage
                NULL,				// get_unmapped_area
*/
    };

#endif


//static const char driver_version[] = DRIVER_VERSION;

static const u8 irq_table[16] = {
//  0    1    2   *3*   4   *5*   6   *7*
  0x00,0x00,0x00,0x80,0x00,0x90,0x00,0xA0,
//  8   *9* *10* *11* *12*  13   14  *15*
  0x00,0xB0,0xC0,0xD0,0xE0,0x00,0x00,0xF0
};

//static DEV *devs[MAX_LPCS+MAX_LPPS];

static u16 lpcs[MAX_LPCS];// = { 0x5340, 0x7344 };


static struct pci_driver lpp_driver = {
	.name         = DRIVER_MOD_NAME,
	.id_table     = lpp_pci_tbl,
	.probe        =	lpp_init_one,
	.remove       = lpp_remove_one,
#ifdef CONFIG_PM
	.resume       = lpp_resume,
	.suspend      = lpp_suspend,
#endif
};
