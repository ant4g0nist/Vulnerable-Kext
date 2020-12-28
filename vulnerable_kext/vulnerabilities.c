#include <sys/types.h>
#include <mach/mach_types.h>

#include "kernel_imports.h"
#include "third_party/kern_control.h"
#include "vulnerabilities.h"

extern proc_t kernproc;

#define EINVAL          22		/* Invalid argument */
#define M_TEMP          80      /* misc temporary data buffers */

/*
 * flags to malloc
 */
#define M_WAITOK        0x0000
#define M_NOWAIT        0x0001
#define M_ZERO          0x0004          /* bzero the allocation */
#define M_NULL          0x0008          /* return NULL if space is unavailable*/

enum sopt_dir { SOPT_GET, SOPT_SET };

struct sockopt {
	enum	sopt_dir sopt_dir; /* is this a get or a set? */
	int	sopt_level;	/* second arg of [gs]etsockopt */
	int	sopt_name;	/* third arg of [gs]etsockopt */
	user_addr_t sopt_val;	/* fourth arg of [gs]etsockopt */
	size_t	sopt_valsize;	/* (almost) fifth arg of [gs]etsockopt */
	struct	proc *sopt_p;	/* calling process or null if kernel */
};

/*
 * Perhaps this routine, and sooptcopyout(), below, ought to come in
 * an additional variant to handle the case where the option value needs
 * to be some kind of integer, but not a specific size.
 * In addition to their use here, these functions are also called by the
 * protocol-level pr_ctloutput() routines.
 *
 * Returns:	0			Success
 *		EINVAL
 *	copyin:EFAULT
 */
int sooptcopyin(struct sockopt *sopt, void *buf, size_t len, size_t minlen)
{
	size_t	valsize;

	/*
	 * If the user gives us more than we wanted, we ignore it,
	 * but if we don't get the minimum length the caller
	 * wants, we return EINVAL.  On success, sopt->sopt_valsize
	 * is set to however much we actually retrieved.
	 */
	if ((valsize = sopt->sopt_valsize) < minlen)
		return (EINVAL);

	if (valsize > len)
		sopt->sopt_valsize = valsize = len;

	if (sopt->sopt_p != kernproc)
		return (copyin(sopt->sopt_val, buf, valsize));

	bcopy(CAST_DOWN(caddr_t, sopt->sopt_val), buf, valsize);
	return (0);
}

int
sooptcopyout(struct sockopt *sopt, void *buf, size_t len)
{
	int     error;
	size_t  valsize;

	error = 0;

	/*
	 * Documented get behavior is that we always return a value,
	 * possibly truncated to fit in the user's buffer.
	 * Traditional behavior is that we always tell the user
	 * precisely how much we copied, rather than something useful
	 * like the total amount we had available for her.
	 * Note that this interface is not idempotent; the entire answer must
	 * generated ahead of time.
	 */
	valsize = min(len, sopt->sopt_valsize);
	sopt->sopt_valsize = valsize;
	if (sopt->sopt_val != USER_ADDR_NULL) {
		if (sopt->sopt_p != kernproc) {
			error = copyout(buf, sopt->sopt_val, valsize);
		} else {
			bcopy(buf, CAST_DOWN(caddr_t, sopt->sopt_val), valsize);
		}
	}
    
	return error;
}

errno_t trigger_crash(uint32_t a)
{
	void (*fp)(uint32_t ) = (void(*)(uint32_t ))0x51515151;
	(*fp)(a);

	return KERN_FAILURE;
}

// uninitialized
errno_t trigger_info_leak(kern_ctl_ref ctlref, uint32_t unit, void *unitinfo, int opt, void *data, size_t data_len)
{   

    struct sockopt sopt;
	bzero(&sopt, sizeof(sopt));

	sopt.sopt_dir       = SOPT_GET;
	sopt.sopt_val       = CAST_USER_ADDR_T(data);
	sopt.sopt_valsize   = data_len;
	sopt.sopt_p         = kernproc;

    void * _data = NULL;
    MALLOC(_data, void *, sopt.sopt_valsize, M_TEMP, M_WAITOK);

    if (_data == NULL)
    {   
        panic("[info_leak] === Failed to MALLOC");
    }

    // sooptcopyin(&sopt, 	_data, sopt.sopt_valsize, sopt.sopt_valsize);
    sooptcopyout(&sopt, _data, sopt.sopt_valsize);

    FREE(_data, M_TEMP);

    return KERN_SUCCESS;
}

//buffer overflow
errno_t trigger_buffer_overflow(void *data, size_t data_len)
{
	char buffer[255];
    struct sockopt sopt;
	bzero(&sopt, sizeof(sopt));

	sopt.sopt_dir       = SOPT_GET;
	sopt.sopt_val       = CAST_USER_ADDR_T(data);
	sopt.sopt_valsize   = data_len;
	sopt.sopt_p         = kernproc;

	sooptcopyin(&sopt, buffer, sopt.sopt_valsize, sopt.sopt_valsize);

	return KERN_SUCCESS;
}

// heap overflow
errno_t trigger_heap_overflow(void *data, size_t data_len)
{
    struct sockopt sopt;
	bzero(&sopt, sizeof(sopt));

	sopt.sopt_dir       = SOPT_SET;
	sopt.sopt_val       = CAST_USER_ADDR_T(data);
	sopt.sopt_valsize   = data_len;
	sopt.sopt_p         = kernproc;

    void * _data = NULL;
    MALLOC(_data, void *, 255, M_TEMP, M_WAITOK);

    if (_data == NULL)
    {   
        panic("[heap_overflow] === Failed to MALLOC ===");
    }

	sooptcopyin(&sopt, _data, sopt.sopt_valsize, sopt.sopt_valsize);

	FREE(_data, M_TEMP);
	
	return KERN_SUCCESS;
}

//arbitrary write
errno_t trigger_arbitrary_write(void *data, size_t data_len)
{
	uint64_t * stuff = (uint64_t *) data;

    struct sockopt sopt;
	bzero(&sopt, sizeof(sopt));

	sopt.sopt_dir       = SOPT_SET;
	sopt.sopt_val       = CAST_USER_ADDR_T(&stuff[1]);
	sopt.sopt_valsize   = data_len;
	sopt.sopt_p         = kernproc;

	uint64_t * write_where = (uint64_t *)stuff[0];
	sooptcopyin(&sopt, write_where, sopt.sopt_valsize, sopt.sopt_valsize);

	return KERN_SUCCESS;
}

//trigger double fetch + heap overflow
errno_t trigger_double_fetch(void *data, size_t data_len)
{
	struct overflow {
		uint32_t size;
		char * buffer;
	};

	void * _data = NULL;
    MALLOC(_data, void *, 255, M_TEMP, M_WAITOK);

    if (_data == NULL)
    {   
        panic("[heap_overflow] === Failed to MALLOC ===");
    }

	size_t size = ((struct overflow* )data) -> size;

	if (size > 255 && size >= data_len)
	{
		FREE(_data, M_TEMP);
		return KERN_FAILURE;
	}

	struct overflow* _overflow = (struct overflow* )data;

	struct sockopt sopt;
	bzero(&sopt, sizeof(sopt));

	sopt.sopt_dir       = SOPT_SET;
	sopt.sopt_val       = CAST_USER_ADDR_T(_overflow->buffer);
	sopt.sopt_valsize   = _overflow->size;
	sopt.sopt_p         = kernproc;	

	sooptcopyin(&sopt, _data, sopt.sopt_valsize, sopt.sopt_valsize);

	FREE(_data, M_TEMP);
	return KERN_SUCCESS;
}

//trigger integer overflow
errno_t trigger_integer_overflow(void *data, size_t data_len)
{
	// TODO
	return KERN_SUCCESS;
}


//trigger use-after-free
errno_t trigger_use_after_free(kern_ctl_ref ctlref, uint32_t unit, void *unitinfo, int opt, void *data, size_t data_len)
{
	// TODO
	return KERN_SUCCESS;
}
