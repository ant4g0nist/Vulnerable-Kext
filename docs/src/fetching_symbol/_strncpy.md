# _strncpy

We can find strncpy by searching for xrefs of some strings as we did above or search for the signature:

signature = "F6 03 00 AA E0 03 13 AA E1 03 15 AA E2 03 16 AA"

Pseudo code indeed resembles implementation from [strncpy.c](https://github.com/apple/darwin-xnu/blob/master/osfmk/arm/strncpy.c#L33) :

~~~
char *__cdecl sub_FFFFFFF007D2BC4C(char *__dst, const char *__src, size_t __n)
{
  unsigned __int64 v6; // x0
  unsigned __int64 v7; // x22

  v6 = str_len(__src, __n);
  if ( v6 >= __n )
  {
    memmove(__dst, __src, __n);
  }
  else
  {
    v7 = v6;
    memmove(__dst, __src, v6);                  <---- memmove
    memset(&__dst[v7], 0, __n - v7);            <---- memset
  }
  return __dst;
}
~~~

- strncpy = sub_FFFFFFF007D2BC4C
- memmove = sub_FFFFFFF00820B550
- memset  = sub_FFFFFFF00820B780