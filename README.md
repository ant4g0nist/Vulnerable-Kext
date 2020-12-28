# Vulnerable Kext
    A WIP "Vulnerable by Design" kext for iOS/macOS to play/learn with iOS kernel exploitation

# Usage:

Please check the guide at [fuzzing.science](https://fuzzing.science/vulnerable-kext) to find in depth usage details

Running the following command causes checkra1n to listen for attached iOS devices in DFU mode and boot pongoOS:
~~~
/Applications/checkra1n.app/Contents/MacOS/checkra1n -c -p
~~~

Run `run.sh` to build kext_loader, pongo_module, and the vulnerable kext and to start kext_loader
kext_loader waits for a device that's booted pongo shell!

~~~
./run.sh
~~~

for more details about ktrw, check [ktrw](https://github.com/googleprojectzero/ktrw)


## TODO
- [ ] Add IOKit stuff
- [ ] Add vulnerabilities from reported XNU/IOKit bugs? 🤔
- [ ] Maybe improve stability of loading kexts

## credits
- [@_bazad](https://twitter.com/_bazad) for the super awesome [ktrw](https://github.com/googleprojectzero/ktrw)
- checkra1n team for the jailbreak
