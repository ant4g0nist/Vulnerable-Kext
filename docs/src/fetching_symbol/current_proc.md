# current_proc

Code for (current_proc)[bsd/kern/bsd_stubs.c]

~~~
struct proc *
current_proc(void)
{
	/* Never returns a NULL */
	struct uthread * ut;
	struct proc * p;
	thread_t thread = current_thread();

	ut = (struct uthread *)get_bsdthread_info(thread);
	if (ut && (ut->uu_flag & UT_VFORK) && ut->uu_proc) {
		p = ut->uu_proc;
		if ((p->p_lflag & P_LINVFORK) == 0) {
			panic("returning child proc not under vfork");
		}
		if (p->p_vforkact != (void *)thread) {
			panic("returning child proc which is not cur_act");
		}
		return p;
	}

	p = (struct proc *)get_bsdtask_info(current_task());

	if (p == NULL) {
		return kernproc;
	}

	return p;
}
~~~

Xrefs will again land you in `current_proc()`

- current_proc = 0xFFFFFFF0081025E4

The functions `copyin` and `copyout` are defined in [copyio.c](https://github.com/apple/darwin-xnu/blob/master/osfmk/arm64/copyio.c). 
Xrefs to these 2 functions can be found by searching for example `necp_client_claim copyin client_id error` and `%s: %s copyout() error %d`
respectively.
