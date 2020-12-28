# Fetching Symbols

I'll explain below how to collect the required symbols for iPhone X on iOS version 13.4.1.

Download the firmware from https://ipsw.me/download/iPhone10,3/17E262. Unzip the ipsw. 

Now we'll use [jtool2](http://newosxbook.com/tools/jtool2.tgz) by Jonathan to decompress the kernel cache

```
#  /Users/ant4g0nist/tools/jtool2/jtool2 -dec kernelcache.release.iphone10b
Decompressed kernel written to /tmp/kernel
#  mv /tmp/kernel kernelcache.decompressed
```

Open the decompressed kernel in IDA pro or Binary Ninja or whatever you choose and wait for it to finish the analysis.

The symbols we need are:

- _IOSleep
- _kernel_map
- _kernel_thread_start
- _panic
- _strncpy
- _memset
- _memmove
- ___stack_chk_fail
- ___stack_chk_guard
- _ctl_register
- ___MALLOC
- __FREE
- _current_proc
- _copyin
- _copyout 
