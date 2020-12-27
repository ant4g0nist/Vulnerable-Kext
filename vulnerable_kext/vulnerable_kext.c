#include <sys/types.h>
#include <sys/syscall.h>
#include <mach/mach_types.h>
#include <mach/kern_return.h> /* does one of the above drag this in? */

#include "kernel_imports.h"
#include "third_party/kern_control.h"
#include "third_party/kmod.h"
#include "vulnerabilities.h"

/*
KMOD_DECL takes 2 arguments.
 arg0: if sample_module_start/sample_module_stop are the start/stop methods, `sample` is argument 1 
 arg1: random version that is loaded from Makefile
*/

KMOD_DECL(fuzzing_science, VERSION)

#define	CONTROL_NAME "fuzzing.science.vulnkext"

proc_t kernproc;
static kern_ctl_ref _ctlref; /* an opaque reference to the control */

/*
Here be callbacks. The first parameter to each is a kern_ctl_ref which
is a bit like self/this in an OO program - it tells the function exactly
which control is being operated, so you can have a single instance of a
callback routine service multiple controls. You also get either a unit
number (or a sockaddr_ctl structure, which includes it as its sc_unit
member - myself, i would have left the unit number as a parameter too,
but hey), so you can (if i understand this right) tell which instance of
that control is being operated - controls are a bit like server sockets
in networking, and units are like sockets opened from them. You also get
this unitinfo variable, which allows you to stash some data with each
unit; in connect, it's a pointer-to-pointer-to-void, ie it leads you to
a secret place where you can place a pointer to your data (as a void*),
and in the other routines, it's a pointer-to-void, and you get whatever
it was you put there in connect. The rest's pretty self-explanatory, i 
think. For the return value, 0 means success, as always, otherwise pick 
something from errno.h; EINVAL seems to be the catch-all error number in 
the Apple example code.
*/

errno_t connect(kern_ctl_ref ctlref, struct sockaddr_ctl *addr, void **unitinfo)
{
	/* nothing much of interest in addr except sc_unit */
	/* stash stuff into unitinfo here */
	
	return 0 ;
}

errno_t disconnect(kern_ctl_ref ctlref, uint32_t unit, void *unitinfo)
{
	return 0 ;
}

errno_t send(kern_ctl_ref ctlref, uint32_t unit, void *unitinfo, mbuf_t data, int flags)
{
	/*
	There's a strong chance that you'll want to send a reply to the 
	client round here; for that, you want the ctl_enqueuedata routine in 
	kern_control.h, which looks like:
	
	errno_t ctl_enqueuedata(kern_ctl_ref ctlref, u_int32_t unit, void *data, size_t data_len, u_int32_t flags) ;
	
	Which does exactly what it says on the tin - puts some data on the
	queue of client-bound messages on the socket. The only flag you
	might want is CTL_DATA_NOWAKEUP, which means 'put the data on the
	queue, but don't wake the client up so it can read it', a low act if
	ever there was one.
	*/
	return 0 ;
}

errno_t setopt(kern_ctl_ref ctlref, uint32_t unit, void *unitinfo, int opt, void *data, size_t data_len)
{
	errno_t res = KERN_SUCCESS;

	switch	(opt)
	{
		case CRASH:
			res = trigger_crash(opt);
			break;

		case INFO_LEAK:
			res = trigger_info_leak(ctlref, unit, unitinfo, opt, data, data_len);
			break;

		case BUFFER_OVERFLOW:
			res = trigger_buffer_overflow(data, data_len);
			break;
		
		case HEAP_OVERFLOW:
			res = trigger_heap_overflow(data, data_len);
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
		panic("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n Failed to load fuzzing.science");

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
