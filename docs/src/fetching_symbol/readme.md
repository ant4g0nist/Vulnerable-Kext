# Fetching Symbols

I'll explain below how I managed to collect the required symbols for iPhone X on iOS version 13.4.1.

Download the firmware from https://ipsw.me/download/iPhone10,3/17E262. 

Open in IDA pro or Binary Ninja or whatever you choose and wait for it to finish the analysis.

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
