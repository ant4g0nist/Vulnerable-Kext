# kernel_thread_start

Sample usage of kernel_thread_start from XNU kernel: (bsd/kern/kern_memorystatus_freeze.c)
~~~
__private_extern__ void
memorystatus_freeze_init(void)
{
	kern_return_t result;
	thread_t thread;

	freezer_lck_grp_attr = lck_grp_attr_alloc_init();
	freezer_lck_grp = lck_grp_alloc_init("freezer", freezer_lck_grp_attr);

	lck_mtx_init(&freezer_mutex, freezer_lck_grp, NULL);

	/*
	 * This is just the default value if the underlying
	 * storage device doesn't have any specific budget.
	 * We check with the storage layer in memorystatus_freeze_update_throttle()
	 * before we start our freezing the first time.
	 */
	memorystatus_freeze_budget_pages_remaining = (memorystatus_freeze_daily_mb_max * 1024 * 1024) / PAGE_SIZE;

	result = kernel_thread_start(memorystatus_freeze_thread, NULL, &thread);
	if (result == KERN_SUCCESS) {
		proc_set_thread_policy(thread, TASK_POLICY_INTERNAL, TASK_POLICY_IO, THROTTLE_LEVEL_COMPRESSOR_TIER2);
		proc_set_thread_policy(thread, TASK_POLICY_INTERNAL, TASK_POLICY_PASSIVE_IO, TASK_POLICY_ENABLE);
		thread_set_thread_name(thread, "VM_freezer");

		thread_deallocate(thread);
	} else {
		panic("Could not create memorystatus_freeze_thread");
	}
}
~~~

Assuming the strings are available, search for "Could not create memorystatus_freeze_thread". I found 1 xref in IDA inside sub_FFFFFFF00802E71C function.

~~~
__int64 sub_FFFFFFF00802E71C()
{
  _DWORD *v0; // x0
  _DWORD *v1; // x20
  __int64 v2; // x0
  __int64 v3; // x19
  unsigned int v4; // w8
  __int64 v5; // x19
  __int64 result; // x0
  unsigned int v7; // w9
  unsigned int v8; // off
  __int64 v9; // [xsp+0h] [xbp-30h] BYREF
  unsigned __int64 v10; // [xsp+8h] [xbp-28h] BYREF

  v9 = 0LL;
  v10 = 4LL;
  v0 = kalloc_canblock(&v10, 1LL, &unk_FFFFFFF0090C85D8);
  v1 = v0;
  if ( v0 )
    *v0 = (dword_FFFFFFF0092557C0 >> 1) & 1;
  qword_FFFFFFF009270068 = v0;
  v10 = 168LL;
  v2 = kalloc_canblock(&v10, 1LL, &unk_FFFFFFF0090C85F0);
  v3 = v2;
  if ( v2 )
    sub_FFFFFFF007C224E0(v2, "freezer", v1);
  qword_FFFFFFF009270070 = v3;
  qword_FFFFFFF0092DAC28 = 570425344LL;
  qword_FFFFFFF0092DAC20 = 0LL;
  v4 = atomic_fetch_add_explicit((v3 + 16), 1u, memory_order_relaxed);
  if ( !v4 )
    sub_FFFFFFF00821F2FC(v3 + 16);
  if ( v4 >= 0xFFFFFFF )
    sub_FFFFFFF00821F318(v3 + 16);
  atomic_fetch_add_explicit((v3 + 24), 1u, memory_order_relaxed);
  qword_FFFFFFF009270030 = (dword_FFFFFFF0090E7AE4 << 6) & 0x3FFC0;
  if ( sub_FFFFFFF007C4F540(sub_FFFFFFF00802E8C0, 0LL, &v9) )                       <----- As it can be observed, this is the call to kernel_thread_start. thread_t thread = v9.
    sub_FFFFFFF0090BAA08("\"Could not create memorystatus_freeze_thread\"");        <----- this should be panic
  v5 = v9;                                                                          <----- v5 = v9 = thread
  sub_FFFFFFF007C59340(v9, 0LL, 35LL, 2LL);                                         <----- sub_FFFFFFF007C59340 = proc_set_thread_policy
  result = sub_FFFFFFF007C59340(v5, 0LL, 36LL, 1LL);
  if ( v5 )
  {
    result = *(v5 + 960);
    if ( result )
      result = sub_FFFFFFF00809100C(result, "VM_freezer");                          <---- sub_FFFFFFF00809100C = thread_set_thread_name
    v7 = atomic_fetch_add_explicit((v5 + 204), 0xFFFFFFFF, memory_order_release);   
    if ( v7 == 1 )
    {
      v8 = __ldar((v5 + 204));
      result = sub_FFFFFFF007C4DCAC(v5);                                            <------ thread_deallocate_complete
    }
    else if ( !v7 )
    {
      sub_FFFFFFF00821F2E0((v5 + 204));
    }
  }
  return result;
}
~~~

~~~
kernel_thread_start = 0xFFFFFFF007C4F540
panic               = 0xFFFFFFF0090BAA08
~~~
