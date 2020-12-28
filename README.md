# Vulnerable Kext

[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](https://github.com/ant4g0nist/Vulnerable-Kext/blob/master/LICENSE)
[![Github Stars](https://img.shields.io/github/stars/ant4g0nist/Vulnerable-Kext)](https://github.com/ant4g0nist/Vulnerable-Kext/stargazers)
[![PRs Welcome](https://img.shields.io/badge/PRs-welcome-brightgreen.svg)](https://github.com/ant4g0nist/Vulnerable-Kext/pulls)

A WIP (work-in progress) "Vulnerable by Design" kext for iOS/macOS to play/learn with *OS kernel exploitation


## Usage

* Documentation can be found at [https://fuzzing.science/vulnerable-kext](https://fuzzing.science/vulnerable-kext/)

* Basic setup requirements
  * iOS device that can be jailbroken with [checkra1n](https://checkra.in/)
  * Currently the make files are made to be used on a Mac. So, a macOS device or a VM.

* Running the following command causes checkra1n to listen for attached iOS devices in DFU mode and boot pongoOS:

```bash
/Applications/checkra1n.app/Contents/MacOS/checkra1n -c -p
```

* Run `run.sh` to build kext_loader, pongo_module, and the vulnerable kext and to start kext_loader
kext_loader waits for a device that's booted pongo shell!

```bash
./run.sh
```

For more details about ktrw, check [ktrw](https://github.com/googleprojectzero/ktrw)

## Disclaimer

> [Vulnerable-Kext](https://github.com/ant4g0nist/Vulnerable-Kext) is an intentionally vulnerable kext for iOS/macOS, meant for educational purpose only.

## TODO

* [ ] Add IOKit stuff
* [ ] Add vulnerabilities from reported XNU/IOKit bugs? 🤔
* [ ] Maybe improve stability of loading kexts
* [ ] Fix the bugs in the vulnerabilities I implemented 🧐
* [ ] Add Writeups for exploitation

## credits

* [@_bazad](https://twitter.com/_bazad) for the super awesome [ktrw](https://github.com/googleprojectzero/ktrw)
* checkra1n team for the jailbreak
* Used the kext template from [twic](https://urchin.earth.li/~twic/Kernel_Extensions_for_OS_X.html)
