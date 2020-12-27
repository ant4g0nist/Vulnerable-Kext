#ifndef KPI_KERN_CONTROL_H
#define KPI_KERN_CONTROL_H

#define IOCPARM_MASK    0x1fff          /* parameter length, at most 13 bits */
#define IOCPARM_LEN(x)  (((x) >> 16) & IOCPARM_MASK)
#define IOCBASECMD(x)   ((x) & ~(IOCPARM_MASK << 16))
#define IOCGROUP(x)     (((x) >> 8) & 0xff)

#define IOCPARM_MAX     (IOCPARM_MASK + 1)      /* max size of ioctl args */
/* no parameters */
#define IOC_VOID        (__uint32_t)0x20000000
/* copy parameters out */
#define IOC_OUT         (__uint32_t)0x40000000
/* copy parameters in */
#define IOC_IN          (__uint32_t)0x80000000
/* copy parameters in and out */
#define IOC_INOUT       (IOC_IN|IOC_OUT)
/* mask for IN/OUT/VOID */
#define IOC_DIRMASK     (__uint32_t)0xe0000000

#define _IOC(inout, group, num, len) \
	(inout | ((len & IOCPARM_MASK) << 16) | ((group) << 8) | (num))
#define _IO(g, n)        _IOC(IOC_VOID,	(g), (n), 0)
#define _IOR(g, n, t)     _IOC(IOC_OUT,	(g), (n), sizeof(t))
#define _IOW(g, n, t)     _IOC(IOC_IN,	(g), (n), sizeof(t))
/* this should be _IORW, but stdio got there first */

/*!
 *       @defined CTLIOCGCOUNT
 *   @discussion The CTLIOCGCOUNT ioctl can be used to determine the
 *       number of kernel controllers registered.
 */
#define CTLIOCGCOUNT    _IOR('N', 2, int)               /* get number of control structures registered */

/*!
 *       @defined CTLIOCGINFO
 *   @discussion The CTLIOCGINFO ioctl can be used to convert a kernel
 *       control name to a kernel control id.
 */
#define CTLIOCGINFO     _IOWR('N', 3, struct ctl_info)  /* get id from name */


#define MAX_KCTL_NAME   96

struct ctl_info {
	u_int32_t   ctl_id;                             /* Kernel Controller ID  */
	char        ctl_name[MAX_KCTL_NAME];            /* Kernel Controller Name (a C string) */
};

struct sockaddr_ctl {
	u_char      sc_len;     /* depends on size of bundle ID string */
	u_char      sc_family;  /* AF_SYSTEM */
	u_int16_t   ss_sysaddr; /* AF_SYS_KERNCONTROL */
	u_int32_t   sc_id;      /* Controller unique identifier  */
	u_int32_t   sc_unit;    /* Developer private unit number */
	u_int32_t   sc_reserved[5];
};

#endif