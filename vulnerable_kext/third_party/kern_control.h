/*
 * Copyright (c) 2000-2004, 2012-2016 Apple Inc. All rights reserved.
 *
 * @APPLE_OSREFERENCE_LICENSE_HEADER_START@
 * 
 * This file contains Original Code and/or Modifications of Original Code
 * as defined in and that are subject to the Apple Public Source License
 * Version 2.0 (the 'License'). You may not use this file except in
 * compliance with the License. The rights granted to you under the License
 * may not be used to create, or enable the creation or redistribution of,
 * unlawful or unlicensed copies of an Apple operating system, or to
 * circumvent, violate, or enable the circumvention or violation of, any
 * terms of an Apple operating system software license agreement.
 * 
 * Please obtain a copy of the License at
 * http://www.opensource.apple.com/apsl/ and read it before using this file.
 * 
 * The Original Code and all software distributed under the License are
 * distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT.
 * Please see the License for the specific language governing rights and
 * limitations under the License.
 * 
 * @APPLE_OSREFERENCE_LICENSE_HEADER_END@
 */
/*!
	@header kern_control.h
	This header defines an API to communicate between a kernel
	extension and a process outside of the kernel.
 */

#ifndef KPI_KERN_CONTROL_H
#define KPI_KERN_CONTROL_H

#include <sys/types.h>
#include "mbuf.h"

typedef	struct mbuf*		mbuf_t;

#define MAX_KCTL_NAME	96

/*!
	@struct sockaddr_ctl
	@discussion The controller address structure is used to establish
		contact between a user client and a kernel controller. The
		sc_id/sc_unit uniquely identify each controller. sc_id is a
		unique identifier assigned to the controller. The identifier can
		be assigned by the system at registration time or be a 32-bit
		creator code obtained from Apple Computer. sc_unit is a unit
		number for this sc_id, and is privately used by the kernel
		controller to identify several instances of the controller.
	@field sc_len The length of the structure.
	@field sc_family AF_SYSTEM.
	@field ss_sysaddr AF_SYS_KERNCONTROL.
	@field sc_id Controller unique identifier.
	@field sc_unit Kernel controller private unit number.
	@field sc_reserved Reserved, must be set to zero.
*/
struct sockaddr_ctl {
    u_char	sc_len;		/* depends on size of bundle ID string */
    u_char	sc_family;	/* AF_SYSTEM */
    u_int16_t 	ss_sysaddr;	/* AF_SYS_KERNCONTROL */
    u_int32_t	sc_id; 		/* Controller unique identifier  */
    u_int32_t 	sc_unit;	/* Developer private unit number */
    u_int32_t 	sc_reserved[5];
};


/*!
	@typedef kern_ctl_ref
	@discussion A control reference is used to track an attached kernel
		control. Registering a kernel control will create a kernel
		control reference. This reference is required for sending data
		or removing the kernel control. This reference will be passed to
		callbacks for that kernel control.
*/
typedef void * kern_ctl_ref;

/*!
	@typedef ctl_connect_func
	@discussion The ctl_connect_func is used to receive
		notification of a client connecting to the kernel control.
	@param kctlref The control ref for the kernel control the client is
		connecting to.
	@param sac The address used to connect to this control. The field sc_unit 
		contains the unit number of the kernel control instance the client is
		connecting to. If CTL_FLAG_REG_ID_UNIT was set when the kernel control 
		was registered, sc_unit is the ctl_unit of the kern_ctl_reg structure. 
		If CTL_FLAG_REG_ID_UNIT was not set when the kernel control was 
		registered, sc_unit is the dynamically allocated unit number of 
		the new kernel control instance that is used for this connection.
	@param unitinfo A placeholder for a pointer to the optional user-defined
		private data associated with this kernel control instance.  This
		opaque info will be provided to the user when the rest of the
		callback routines are executed.  For example, it can be used
		to pass a pointer to an instance-specific data structure in
		order for the user to keep track of the states related to this
		kernel control instance.
 */
typedef errno_t (*ctl_connect_func)(kern_ctl_ref kctlref, 
									struct sockaddr_ctl *sac,
									void **unitinfo);


/*!
	@typedef ctl_disconnect_func
	@discussion The ctl_disconnect_func is used to receive notification
		that a client has disconnected from the kernel control. This
		usually happens when the socket is closed. If this is the last
		socket attached to your kernel control, you may unregister your
		kernel control from this callback.
	@param kctlref The control ref for the kernel control instance the client has
		disconnected from.
	@param unit The unit number of the kernel control instance the client has
		disconnected from.  
	@param unitinfo The user-defined private data initialized by the
		ctl_connect_func callback.
 */
typedef errno_t (*ctl_disconnect_func)(kern_ctl_ref kctlref, u_int32_t unit, void *unitinfo);

/*!
	@typedef ctl_send_func
	@discussion The ctl_send_func is used to receive data sent from
		the client to the kernel control.
	@param kctlref The control ref of the kernel control.
	@param unit The unit number of the kernel control instance the client has
		connected to.
	@param unitinfo The user-defined private data initialized by the
		ctl_connect_func callback.
	@param m The data sent by the client to the kernel control in an
		mbuf chain. Your function is responsible for releasing the
		mbuf chain.
	@param flags The flags specified by the client when calling
		send/sendto/sendmsg (MSG_OOB/MSG_DONTROUTE).
 */
typedef errno_t (*ctl_send_func)(kern_ctl_ref kctlref, u_int32_t unit, void *unitinfo,
								 mbuf_t m, int flags);


/*!
	@typedef ctl_setopt_func
	@discussion The ctl_setopt_func is used to handle set socket option
		calls for the SYSPROTO_CONTROL option level.
	@param kctlref The control ref of the kernel control.
	@param unit The unit number of the kernel control instance.
	@param unitinfo The user-defined private data initialized by the
		ctl_connect_func callback.
	@param opt The socket option.
	@param data A pointer to the socket option data. The data has
		already been copied in to the kernel for you.
	@param len The length of the socket option data.
 */
typedef errno_t (*ctl_setopt_func)(kern_ctl_ref kctlref, u_int32_t unit, void *unitinfo,
								   int opt, void *data, size_t len);

/*!
	@typedef ctl_getopt_func
	@discussion The ctl_getopt_func is used to handle client get socket
		option requests for the SYSPROTO_CONTROL option level. A buffer
		is allocated for storage and passed to your function. The length
		of that buffer is also passed. Upon return, you should set *len
		to length of the buffer used. In some cases, data may be NULL.
		When this happens, *len should be set to the length you would
		have returned had data not been NULL. If the buffer is too small,
		return an error.
	@param kctlref The control ref of the kernel control.
	@param unit The unit number of the kernel control instance.
	@param unitinfo The user-defined private data initialized by the
		ctl_connect_func callback.
	@param opt The socket option.
	@param data A buffer to copy the results in to. May be NULL, see
		discussion.
	@param len A pointer to the length of the buffer. This should be set
		to the length of the buffer used before returning.
 */
typedef errno_t (*ctl_getopt_func)(kern_ctl_ref kctlref, u_int32_t unit, void *unitinfo,
								   int opt, void *data, size_t *len);


/*!
	@struct kern_ctl_reg
	@discussion This structure defines the properties of a kernel
		control being registered.
	@field ctl_name A Bundle ID string of up to MAX_KCTL_NAME bytes (including the ending zero).
		This string should not be empty.
	@field ctl_id The control ID may be dynamically assigned or it can be a
		32-bit creator code assigned by DTS. 
		For a DTS assigned creator code the CTL_FLAG_REG_ID_UNIT flag must be set. 
		For a dynamically assigned control ID, do not set the CTL_FLAG_REG_ID_UNIT flag.
		The  value of the dynamically assigned control ID is set to this field 
		when the registration succeeds.
	@field ctl_unit A separate unit number to register multiple units that 
		share the same control ID with DTS assigned creator code when 
		the CTL_FLAG_REG_ID_UNIT flag is set.
		This field is ignored for a dynamically assigned control ID.
	@field ctl_flags CTL_FLAG_PRIVILEGED and/or CTL_FLAG_REG_ID_UNIT.
	@field ctl_sendsize Override the default send size. If set to zero,
		the default send size will be used, and this default value 
		is set to this field to be retrieved by the caller. 
	@field ctl_recvsize Override the default receive size. If set to
		zero, the default receive size will be used, and this default value 
		is set to this field to be retrieved by the caller. 
	@field ctl_connect Specify the  function to be called whenever a client
		connects to the kernel control. This field must be specified.
	@field ctl_disconnect Specify a function to be called whenever a
		client disconnects from the kernel control.
	@field ctl_send Specify a function to handle data send from the
		client to the kernel control.
	@field ctl_setopt Specify a function to handle set socket option
		operations for the kernel control.
	@field ctl_getopt Specify a function to handle get socket option
		operations for the kernel control.
*/
struct kern_ctl_reg
{
	/* control information */
	char		ctl_name[MAX_KCTL_NAME];
	u_int32_t	ctl_id;
	u_int32_t	ctl_unit;

    /* control settings */
    u_int32_t	ctl_flags;
    u_int32_t	ctl_sendsize;
    u_int32_t	ctl_recvsize;

    /* Dispatch functions */
    ctl_connect_func	ctl_connect;
    ctl_disconnect_func	ctl_disconnect;
    ctl_send_func		ctl_send;
    ctl_setopt_func		ctl_setopt;
    ctl_getopt_func		ctl_getopt;

};

errno_t
ctl_register(struct kern_ctl_reg *userkctl, kern_ctl_ref *kctlref);	

#define CTL_FLAG_PRIVILEGED	0x1
/*!
	@defined CTL_FLAG_REG_ID_UNIT
    @discussion The CTL_FLAG_REG_ID_UNIT flag is passed to indicate that
    	the ctl_id specified should be used. If this flag is not
    	present, a unique ctl_id will be dynamically assigned to your
    	kernel control. The CTLIOCGINFO ioctl can be used by the client
    	to find the dynamically assigned id based on the control name
    	specified in ctl_name.
*/
#define CTL_FLAG_REG_ID_UNIT	0x2

///start mach/vm_types.h
typedef uint16_t vm_tag_t;

struct vm_allocation_total {
	vm_tag_t tag;
	uint64_t total;
};

struct vm_allocation_zone_total {
	uint64_t  total;
	uint64_t  peak;
	uint32_t  waste;
	uint32_t  wastediv;
};
typedef struct vm_allocation_zone_total vm_allocation_zone_total_t;

struct vm_allocation_site {
	uint64_t  total;
#if DEBUG || DEVELOPMENT
	uint64_t  peak;
#endif /* DEBUG || DEVELOPMENT */
	uint64_t  mapped;
	int16_t   refcount;
	vm_tag_t  tag;
	uint16_t  flags;
	uint16_t  subtotalscount;
	struct vm_allocation_total subtotals[0];
	/* char      name[0]; -- this is placed after subtotals, see KA_NAME() */
};
typedef struct vm_allocation_site vm_allocation_site_t;

#define VM_ALLOC_SITE_STATIC(iflags, itag)                                          \
	static vm_allocation_site_t site __attribute__((section("__DATA, __data"))) \
	 = { .refcount = 2, .tag = (itag), .flags = (iflags) };
	 
/// end mach/vm_types.h

#endif
