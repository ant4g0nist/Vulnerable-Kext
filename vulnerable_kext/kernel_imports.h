#ifndef KERNEL_FUNCTIONS_H
#define KERNEL_FUNCTIONS_H

#include "third_party/kern_control.h"

typedef char * caddr_t;
typedef unsigned long size_t;
typedef struct proc * proc_t;
typedef u_int64_t	user_addr_t;
typedef void (*thread_continue_t)(void *parameter, int wait_result);


#define min(a, b)	(a) < (b) ? a : b
#define bcopy(a, b, c) memmove(b,a,c)
#define KERNEL_EXTERN extern __attribute__((weak_import))

#define CAST_USER_ADDR_T(a_ptr)   ((user_addr_t)((uintptr_t)(a_ptr)))

#define CAST_DOWN( type, addr ) \
    ( ((type)((uintptr_t) (addr)/(sizeof(type) < sizeof(uintptr_t) ? 0 : 1))) )

#define MALLOC(space, cast, size, type, flags)                   \
	({ VM_ALLOC_SITE_STATIC(0, 0);                               \
	(space) = (cast)__MALLOC(size, type, flags, &site); })

#define REALLOC(space, cast, addr, size, type, flags)            \
	({ VM_ALLOC_SITE_STATIC(0, 0);                               \
	(space) = (cast)__REALLOC(addr, size, type, flags, &site); })

#define _MALLOC(size, type, flags)                               \
	({ VM_ALLOC_SITE_STATIC(0, 0);                               \
	__MALLOC(size, type, flags, &site); })
#define _REALLOC(addr, size, type, flags)                        \
	({ VM_ALLOC_SITE_STATIC(0, 0);                               \
	__REALLOC(addr, size, type, flags, &site); })

#define _MALLOC_ZONE(size, type, flags)                          \
	({ VM_ALLOC_SITE_STATIC(0, 0);                               \
	__MALLOC_ZONE(size, type, flags, &site); })


#define FREE(addr, type) \
_Pragma("clang diagnostic push") \
_Pragma("clang diagnostic ignored \"-Wshadow\"") \
	do { \
	        _Static_assert(sizeof (addr) == sizeof (void *) || sizeof (addr) == sizeof (mach_vm_address_t), "addr is not a pointer"); \
	        void *__tmp_addr = (void *)addr; \
	        int __tmp_type = type; \
	        addr = (typeof(addr)) NULL; \
	        _FREE(__tmp_addr, __tmp_type); \
	} while (0) \
_Pragma("clang diagnostic pop")


#define NULL 0

#define	bzero(a,c)	memset(a,0,c)


KERNEL_EXTERN char * strncpy(char * dst, const char * src, size_t len);
KERNEL_EXTERN void * memset(void *b, int c, size_t len);
KERNEL_EXTERN void __stack_chk_fail(void);
KERNEL_EXTERN unsigned long __stack_chk_guard;
KERNEL_EXTERN void IOSleep(unsigned milliseconds);
KERNEL_EXTERN void thread_deallocate(thread_t thread);
KERNEL_EXTERN int kernel_thread_start(thread_continue_t continuation, void *parameter, thread_t *thread);
KERNEL_EXTERN void panic(const char *str, ...);
KERNEL_EXTERN void IOLog(const char *format, ...);
KERNEL_EXTERN void * memmove(void *dst, const void *src, size_t len);
KERNEL_EXTERN void     *__MALLOC(
	size_t                size,
	int                   type,
	int                   flags,
	vm_allocation_site_t *site)  __attribute__((alloc_size(1)));

KERNEL_EXTERN void
_FREE(
	void            *addr,
	int             type);

/* Move arbitrarily-aligned data from a user space to kernel space */
KERNEL_EXTERN int copyin(const user_addr_t user_addr, void *kernel_addr, vm_size_t nbytes);

/* Move arbitrarily-aligned data from a kernel space to user space */
extern int copyout(const void *kernel_addr, user_addr_t user_addr, vm_size_t nbytes);
	
KERNEL_EXTERN struct proc * current_proc(void);

#endif
