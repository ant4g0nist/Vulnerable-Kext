# _FREE

Code from (kern_malloc.c)[bsd/kern/kern_malloc.c]
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