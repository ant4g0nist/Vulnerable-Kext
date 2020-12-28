# Vulnerable Kext
    A WIP "Vulnerable by Design" kext for iOS/macOS to play/learn with iOS kernel exploitation

# Usage:

Please check the guide at [fuzzing.science](https://fuzzing.science/vulnerable-kext) to find in depth usage details

Basic setup requirements:
- iOS device that can be jailbroken with [checkra1n](https://checkra.in/)
- Currently the make files are made to be used on a Mac. So, a macOS device or a VM.

Running the following command causes checkra1n to listen for attached iOS devices in DFU mode and boot pongoOS:
~~~
/Applications/checkra1n.app/Contents/MacOS/checkra1n -c -p
~~~

Run `run.sh` to build kext_loader, pongo_module, and the vulnerable kext and to start kext_loader
kext_loader waits for a device that's booted pongo shell!

~~~
./run.sh
~~~

For more details about ktrw, check [ktrw](https://github.com/googleprojectzero/ktrw)

## TODO
- [ ] Add IOKit stuff
- [ ] Add vulnerabilities from reported XNU/IOKit bugs? 🤔
- [ ] Maybe improve stability of loading kexts

## credits
- [@_bazad](https://twitter.com/_bazad) for the super awesome [ktrw](https://github.com/googleprojectzero/ktrw)
- checkra1n team for the jailbreak
- Used the kext template from [twic](https://urchin.earth.li/~twic/Kernel_Extensions_for_OS_X.html)
