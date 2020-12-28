# _stack_chk_fail

Just search for `Kernel stack memory corruption detected` and we end in _stack_chk_fail at [stack_protector.c](https://github.com/apple/darwin-xnu/blob/master/libkern/stack_protector.c#L35])

~~~
void __noreturn _stack_chk_fail()
{
  panic("\"Kernel stack memory corruption detected\"");
}
~~~

___stack_chk_fail = 0xFFFFFFF00821F40C

_stack_chk_fail is called after ___stack_chk_guard is compared with the canary.
So, the calls/xrefs to `___stack_chk_fail` should look like this:

~~~
  if ( qword_FFFFFFF009275908 != v26 )          <----- __stack_chk_guard = qword_FFFFFFF009275908
    j_stack_check_fail();
~~~

- ___stack_chk_guard = 0xFFFFFFF009275908