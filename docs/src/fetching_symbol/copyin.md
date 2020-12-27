# copyin

~~~
__int64 __fastcall copyin(__int64 *a1, __int64 *a2, unsigned __int64 a3)
{
  __int64 result; // x0

  if ( !a3 )
    return 0LL;
  result = copy_validate(a1, a2, a3, 5);                        <-----  5 = COPYIO_IN | COPYIO_ALLOW_KERNEL_TO_KERNEL
  if ( result == 18 )
  {
    memmove(a2, a1, a3);
    result = 0LL;
  }
  else if ( !result )
  {
    __asm { MSR             #4, #0 }
    result = sub_FFFFFFF00821CABC(a1, a2, a3);
    __asm { MSR             #4, #1 }
  }
  return result;
}
~~~