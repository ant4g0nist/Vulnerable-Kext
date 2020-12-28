# _FREE

Code from (kern_malloc.c)[https://github.com/apple/darwin-xnu/blob/master/bsd/kern/kern_malloc.c#L624]
~~~
void
_FREE(
	void            *addr,
	int             type)
{
	if (type >= M_LAST) {
		panic("_free TYPE");
	}

	if (!addr) {
		return; /* correct (convenient bsd kernel legacy) */
	}
	kfree_addr(addr);
}
~~~

Xrefs to `_free TYPE` will lead you to `__FREE`

- __FREE = 0xFFFFFFF00800CE6C