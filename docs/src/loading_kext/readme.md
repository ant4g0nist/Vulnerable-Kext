# Loading the kext on the device

We use the kext loader from [ktrw](https://github.com/googleprojectzero/ktrw), an iOS kernel debugger made by [@bazad](https://twitter.com/bazad).
He uses [checkra1n](https://checkra.in/) and the [pongoOS](https://github.com/checkra1n/pongoOS) to load a kext.

Our setup now consists of 2 components. 1) kext loader from ktrw and 2) vulnerable kext

These can be built by running make on the projects root directory.

To load the vulnerable kext, we'll run 2 utilities: checkra1n and the kext_loader.

Running the following command causes checkra1n to listen for attached iOS devices in DFU mode and boot pongoOS:
~~~
/Applications/checkra1n.app/Contents/MacOS/checkra1n -c -p
~~~

Run `run.sh` to build kext_loader and the vulnerable kext and to start kext_loader.

~~~
./run.sh
~~~

Note for advanced Usage:
- Disable the patches (jailbreak) by checkra1n, modify DISABLE_CHECKRA1N_KERNEL_PATCHES to 1 in Makefile before running `run.sh`.
- This makes checkra1n just inject the vulnerable kext driver and boot into xnu without modiying or disabling any security features inside XNU.
- This can be then be used to write a full chain exploit to jailbreak for teaching/practice! :)

kext_loader waits for a device that's booted pongo shell!

Finally, connect an iOS device in DFU mode using a USB cable.
Now, checkra1n will boot pongoOS, then kext_loader will insert the vulnerable kext, and boot to XNU.
