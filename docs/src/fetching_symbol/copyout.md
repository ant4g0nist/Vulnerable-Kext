# copyout

~~~
__int64 __fastcall copyout(__int64 *a1, __int64 *a2, unsigned __int64 a3)
{
  __int64 result; // x0

  if ( !a3 )
    return 0LL;
  result = copy_validate(a2, a1, a3, 6);                        <-----  6 = COPYIO_OUT | COPYIO_ALLOW_KERNEL_TO_KERNEL
  if ( result == 18 )
  {
    memmove(a2, a1, a3);
    return 0LL;
  }
  if ( !result )
  {
    __asm { MSR             #4, #0 }
    result = sub_FFFFFFF00821CC40(a1, a2, a3);
    __asm { MSR             #4, #1 }
  }
  return result;
}
~~~

Now that we have all the required symbols, we create a .txt file inside kernel_symbols folder with these symbols
