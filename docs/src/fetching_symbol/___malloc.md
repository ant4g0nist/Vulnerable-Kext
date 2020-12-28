# ___MALLOC


Code from (kern_malloc.c)[https://github.com/apple/darwin-xnu/blob/master/bsd/kern/kern_malloc.c#L573]
~~~
void *
__MALLOC(
	size_t          size,
	int             type,
	int             flags,
	vm_allocation_site_t *site)
{
	void    *addr = NULL;
	vm_size_t       msize = size;

	if (type >= M_LAST) {
		panic("_malloc TYPE");
	}

	if (size == 0) {
		return NULL;
	}

	if (msize != size) {
		panic("Requested size to __MALLOC is too large (%llx)!\n", (uint64_t)size);
	}

	if (flags & M_NOWAIT) {
		addr = (void *)kalloc_canblock(&msize, FALSE, site);
	} else {
		addr = (void *)kalloc_canblock(&msize, TRUE, site);
		if (addr == NULL) {
			/*
			 * We get here when the caller told us to block waiting for memory, but
			 * kalloc said there's no memory left to get.  Generally, this means there's a
			 * leak or the caller asked for an impossibly large amount of memory. If the caller
			 * is expecting a NULL return code then it should explicitly set the flag M_NULL.
			 * If the caller isn't expecting a NULL return code, we just panic. This is less
			 * than ideal, but returning NULL when the caller isn't expecting it doesn't help
			 * since the majority of callers don't check the return value and will just
			 * dereference the pointer and trap anyway.  We may as well get a more
			 * descriptive message out while we can.
			 */
			if (flags & M_NULL) {
				return NULL;
			}
			panic("_MALLOC: kalloc returned NULL (potential leak), size %llu", (uint64_t) size);
		}
	}
	if (!addr) {
		return 0;
	}

	if (flags & M_ZERO) {
		bzero(addr, size);
	}

	return addr;
}
~~~

Again, xrefs to `_malloc TYPE` will lead you to `__MALLOC`

- __MALLOC = 0xFFFFFFF00800CDC0