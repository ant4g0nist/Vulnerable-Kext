#include <sys/types.h>
#include <sys/syscall.h>
#include <mach/mach_types.h>
#include <mach/kern_return.h>

#include "third_party/kernel_imports.h"
#include "third_party/kern_control.h"
#include "third_party/kmod.h"
#include "vulnerabilities.h"

KMOD_DECL(fuzzing_science, VERSION)

#define	CONTROL_NAME "fuzzing.science.vulnkext"

proc_t kernproc;

static kern_ctl_ref _ctlref;

errno_t connect(kern_ctl_ref ctlref, struct sockaddr_ctl *addr, void **unitinfo)
{
	return 0;
}

errno_t disconnect(kern_ctl_ref ctlref, uint32_t unit, void *unitinfo)
{
	return 0;
}

errno_t send(kern_ctl_ref ctlref, uint32_t unit, void *unitinfo, mbuf_t data, int flags)
{
	return 0;
}

errno_t setopt(kern_ctl_ref ctlref, uint32_t unit, void *unitinfo, int opt, void *data, size_t data_len)
{
	errno_t res = KERN_SUCCESS;

	switch	(opt)
	{
		case CRASH:
			res = trigger_crash(opt);
			break;

		case HEAP_OVERFLOW:
			res = trigger_heap_overflow(data, data_len);
			break;

		case INFO_LEAK:
			res = trigger_info_leak(ctlref, unit, unitinfo, opt, data, data_len);
			break;

		case BUFFER_OVERFLOW:
			res = trigger_buffer_overflow(data, data_len);
			break;

		case USE_AFTER_FREE:
			res = trigger_use_after_free(ctlref, unit, unitinfo, opt, data, data_len);
			break;

		case INTEGER_OVERFLOW:
			res = trigger_integer_overflow(data, data_len);
			break;

		case DOUBLE_FETCH:
			res = trigger_double_fetch(data, data_len);
			break;

		case ARBITRARY_WRITE:
			res = trigger_arbitrary_write(data, data_len);
			break;

		default:
			break;			
	}

	return res;
}

errno_t getopt(kern_ctl_ref ctlref, uint32_t unit, void *unitinfo, int opt, void *data, size_t *data_len)
{
	return KERN_SUCCESS;
}

static void insert_vuln_ctl(__unused void *param, __unused int wr )
{
	if (KEXT_ACTIVATION_DELAY > 0)
	{
		IOSleep(KEXT_ACTIVATION_DELAY * 1000);
	}

	int err = 0 ;

	/* a kern_ctl_reg is an application form for a kernel control */
	struct kern_ctl_reg kern_ctl;
	
	bzero(&kern_ctl, sizeof(kern_ctl));

	kern_ctl.ctl_id 	= 0; /* ask for a dynamically allocated id */
	kern_ctl.ctl_unit 	= 0; /* ditto for unit numbers */

	strncpy(kern_ctl.ctl_name, CONTROL_NAME, sizeof(kern_ctl.ctl_name)); // = 96;
	kern_ctl.ctl_name[sizeof(kern_ctl.ctl_name) - 1] = 0;

	//limit the size?
	kern_ctl.ctl_sendsize                            = 512 * 1024; /* enough? */
	kern_ctl.ctl_recvsize                            = 512 * 1024;	

	//callbacks
	kern_ctl.ctl_connect 		= connect;
	kern_ctl.ctl_disconnect 	= disconnect;
	kern_ctl.ctl_send 			= send;
	kern_ctl.ctl_setopt 		= setopt;
	kern_ctl.ctl_getopt 		= getopt;

	err = ctl_register(&kern_ctl, &_ctlref);
	
	if (err)
		panic("Failed to load fuzzing.science");

	kernproc = current_proc();

	return;
}

static int
fuzzing_science_module_start(struct kmod_info *kmod, void *data) {

	thread_t thread;

	int kr = kernel_thread_start(insert_vuln_ctl, NULL, &thread);

	if (kr != KERN_SUCCESS) {
		return KERN_FAILURE;
	}

	return KERN_SUCCESS;
}

static int
fuzzing_science_module_stop(struct kmod_info *kmod, void *data) {
	return KERN_SUCCESS;
}
