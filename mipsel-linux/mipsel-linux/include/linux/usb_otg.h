/**
 * <linux/usb_otg.h>
 *
 * Copyright 2004 ADVANCED MICRO DEVICES, INC.
 *
 * karsten.boge@amd.com
 */
#ifndef __LINUX_USB_OTG_H
#define __LINUX_USB_OTG_H


/*
 * OTG device flags
 */
#define USB_OTG_CONNECT_VALID		0x01	/* connected to OTG port     */
#define USB_OTG_HNP_ENABLED		0x02	/* device is ready for HNP   */

/*
 * SET_FEATURE data
 */
#define USB_OTG_B_HNP_ENABLE		0x03	/* enables HNP in B-device   */
#define USB_OTG_A_HNP_SUPPORT		0x04	/* signals HNP capability    */
#define USB_OTG_A_ALT_HNP_SUPPORT	0x05	/* HNP, but no OTG connect   */

/*
 * OTG state definitions
 */
#define OTG_STATE_MASK                  0xF0

#define OTG_NOT_ASSIGNED                0
#define OTG_NO_B_DEVICE_A               (0x6 | OTG_NOT_ASSIGNED)
#define OTG_NO_B_DEVICE_B               (0x4 | OTG_NOT_ASSIGNED)

#define OTG_A_IDLE                      0x10
#define OTG_A_IDLE_WAIT_DP              (1 | OTG_A_IDLE)
#define OTG_A_IDLE_WAIT_VP              (2 | OTG_A_IDLE)
#define OTG_A_IDLE_WAIT_MP              (3 | OTG_A_IDLE)
#define OTG_A_IDLE_WAIT_DV              (4 | OTG_A_IDLE)
#define OTG_A_WAIT_VRISE                0xB0
#define OTG_A_WAIT_BCON                 0x30
#define OTG_A_WAIT_BCON_VB              (1 | OTG_A_WAIT_BCON)
#define OTG_A_HOST                      0x50
#define OTG_A_SUSPEND                   0x70
#define OTG_A_PERIPHERAL                0x90
#define OTG_A_VBUS_ERROR                0xF0
#define OTG_A_WAIT_VFALL                0xD0
#define OTG_A_WAIT_VFALL_DN             (1 | OTG_A_WAIT_VFALL)
#ifdef CONFIG_USB_OTG_FORCE_SUSPEND
#define OTG_A_WAIT_BDISCON              0x40
#define OTG_A_WAIT_BSUSPEND             (1 | OTG_A_WAIT_BDISCON)
#endif

#define OTG_B_IDLE                      0x20
#define OTG_B_PERIPHERAL                0x80
#define OTG_B_PERIPHERAL_WT             (1 | OTG_B_PERIPHERAL)
#define OTG_B_PERIPHERAL_DC             (2 | OTG_B_PERIPHERAL)
#define OTG_B_WAIT_ACON                 0xA0
#define OTG_B_HOST                      0xE0
#define OTG_B_SRP_INIT                  0xC0
#define OTG_B_SRP_INIT_WAIT_SE0         (1 | OTG_B_SRP_INIT)
#define OTG_B_SRP_INIT_D_PULSE          (2 | OTG_B_SRP_INIT)
#define OTG_B_SRP_INIT_V_PULSE          (3 | OTG_B_SRP_INIT)
#define OTG_B_SRP_INIT_V_DCHRG          (4 | OTG_B_SRP_INIT)
#define OTG_B_SRP_INIT_WAIT_VBUS        (5 | OTG_B_SRP_INIT)
#ifdef CONFIG_USB_OTG_FORCE_SUSPEND
#define OTG_B_WAIT_ADISCON              0x60
#define OTG_B_WAIT_ASUSPEND             (1 | OTG_B_WAIT_ADISCON)
#endif

/*
 * typical timer values (ms)
 */
/* !!! make sure none of the values below is less then the SYSTEM LATENCY !!!*/

#define OTG_TMR_WAIT_VFALL     10        /* A waits for VBus                 */
#define OTG_TMR_A_WAIT_VRISE   100       /* A waits for VBus                 */
#define OTG_TMR_A_WAIT_BCON    200       /* A waits for B-connect (1.. s)    */
#define OTG_TMR_A_IDLE_BDIS    250       /* A waits for B-disc (200.. ms)    */
#define OTG_TMR_B_ASE0_BRST    5         /* B waits for A-conn (3.125.. ms)  */
#define OTG_TMR_B_AIDL_BDIS    50        /* B waits before dc (5..150ms)     */
#define OTG_TMR_SRP_WAIT_SE0   2         /* B SRP idle wait                  */
#define OTG_TMR_SRP_WAIT_DP    8         /* B SRP D_PULSE (5..10ms)          */
#define OTG_TMR_SRP_WAIT_VP    80        /* B SRP V_PULSE (5..100ms)         */
#define OTG_TMR_SRP_DCHRG_V    30        /* B SRP VBus discharge             */
#define OTG_TMR_SRP_WAIT_VRS   5800      /* B SRP waits for VBus (5..6s)     */
#define OTG_TMR_ASRP_WAIT_MP   4         /* A SRP min. pulse                 */
#define OTG_TMR_ASRP_WAIT_DP   10        /* A SRP D_PULSE TO                 */
#define OTG_TMR_ASRP_WAIT_VP   200       /* A SRP V_PULSE TO                 */
#define OTG_TMR_ASRP_WAIT_DV   200       /* A SRP waits for V_PULSE          */
#define OTG_TMR_A_BCON_VB      50        /* A waits for VBus after connect   */
#ifdef CONFIG_USB_OTG_FORCE_SUSPEND
#define OTG_TMR_HOST_WAIT_SUSP 5         /* USB 2.0 ( ..3ms)                 */
#define OTG_TMR_A_FS_BDIS      (OTG_TMR_HOST_WAIT_SUSP + OTG_TMR_A_IDLE_BDIS)
#define OTG_TMR_B_WAIT_ADIS    OTG_TMR_A_IDLE_BDIS
#define OTG_TMR_B_FS_ADIS      (OTG_TMR_HOST_WAIT_SUSP + OTG_TMR_B_WAIT_ADIS)
#endif

/*
 * OTG state parameters
 */
#define OTG_HOST_READY         (1<<20)   /* indicates a USB host driver is   */
                                         /* running                          */
#define OTG_GADGET_READY       (1<<21)   /* indicates a USB gadget driver is */
                                         /* running                          */
#define OTG_A_BUS_REQ          (1<<22)   /* used by appl-SW to request a     */
                                         /* VBus rise, auto-reset by driver  */
#define OTG_A_BUS_DROP         (1<<23)   /* used by appl-SW to request a     */
                                         /* VBus drop, auto-reset by driver  */
#define OTG_A_bHNP_EN          (1<<24)   /* aapl-SW can use this to enable   */
                                         /* HNP / indicates HNP is enabled   */
                                         /* reset on USB port reset or SEnd  */
#define OTG_A_CLR_ERR          (1<<25)   /* used by appl-SW to request VBerr */
                                         /* clean-up, auto-reset by driver   */
#define OTG_AB_HNP_REQ         (1<<26)   /* used by appl-SW to initiate      */
                                         /* HNP, auto-reset by driver        */
#define OTG_B_BUS_REQ          (1<<27)   /* used by appl-SW to request       */
                                         /* B-device functionality, ...      */
#define OTG_B_BUS_DIS          (1<<28)   /* used by appl-SW to request       */
                                         /* disable B-device functionality   */
#define OTG_B_aSSN_REQ         (1<<29)   /* used by appl-SW to initiate SRP, */
                                         /* auto-reset by the driver         */
#define OTG_B_SRP_ERROR        (1<<30)   /* indicates invalid HW conditions  */
                                         /* during SRP, reset by writing "1" */
#define OTG_A_VBUS_FAILED      (1<<31)   /* indicates a VBus error, reset by */
                                         /* writing "1", when setting        */
                                         /* CLR_ERR or when leaving A-states */

#ifdef CONFIG_USB_OTG_FORCE_SUSPEND
#define SW_REQUEST_MASK        (OTG_A_BUS_REQ | OTG_A_BUS_DROP | \
                                OTG_A_bHNP_EN | OTG_A_CLR_ERR | \
                                OTG_B_BUS_REQ | OTG_B_BUS_DIS | \
                                OTG_B_aSSN_REQ | OTG_AB_HNP_REQ)
#else
#define SW_REQUEST_MASK        (OTG_A_BUS_REQ | OTG_A_BUS_DROP | \
                                OTG_A_bHNP_EN | OTG_A_CLR_ERR | \
                                OTG_B_BUS_REQ | OTG_B_BUS_DIS | \
                                OTG_B_aSSN_REQ)
#endif

/*********************************************************************/

/*
 * gadget events for notify function
 */
#define OTG_GADGET_EVT_SVDROP  1         /* Session valid drop       */
#define OTG_GADGET_EVT_SVALID  2         /* Session valid            */

/*** HS-A0 WA: gadget phy suspend issue                            ***/
#define OTG_FLAGS_UDC_SUSP     (1<<17)   /* gadget phy suspended     */

/*********************************************************************/

#ifdef __KERNEL__
/*
 * fsm variables
 */
struct otg_ctl {
	int  act_state;		/* OTG actual state */
	int  prv_state;		/* OTG previous state */
	int  params;		/* OTG state parameters */
};

struct otg {
	spinlock_t        lock;
	int               hostcount;
	int               gadgetflag;
	struct usb_device *device;
	unsigned          enabled : 1,
	                  got_irq : 1,
	                  region : 1,
	                  selfpowered : 1;
	u16               chiprev;
	/* pci state used to access those endpoints */
	struct pci_dev   *pdev;
	struct otg_regs  *regs;
	struct otg_ctl   *otg_ctl;
};

/*
 * To be allocated once for one OTG controller
 */
struct usbcore_otg_extension {
	int  (*bind) (struct usb_bus *);
	int  (*unbind) (struct usb_bus *);
	int  (*request) (u32);	/* function call for state change requests */
	u32  (*query) (int);	/* function call to query state */
};

struct usb_gadget_otg_extension {
	int  (*bind) (void);
	int  (*unbind) (void);
	int  (*request) (u32);	/* function call for state change requests */
	u32  (*query) (int);	/* function call to query state */
	int  (*notify) (int);   /* report events to the gadget driver */
};

/*
 * To be allocated for each OTG capabel host (assuming 1 OTG port)
 */
struct usb_otg_host {
	int   otgport;		/* port supporting OTG (-1 for none) */
	void *otgpriv;		/* OTG controller private data */
};


int usb_bus_register_otg (struct usbcore_otg_extension *);
int usb_bus_deregister_otg (struct usbcore_otg_extension *);
int usb_gadget_register_otg (struct usb_gadget_otg_extension *);
int usb_gadget_unregister_otg (struct usb_gadget_otg_extension *);
int usb_otg_enable_b_hnp (struct usb_device *);

int usb_check_otg_support (struct pci_dev *, struct usb_bus *);
void usb_release_otg_host (struct usb_bus *);

#endif

#endif /* __LINUX_USB_OTG_H */
