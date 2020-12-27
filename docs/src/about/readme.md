# Vulnerable iOS/macOS Kext

In this guide, we'll look at loading a kext that is vulnerable by design in an iPhone and trigger a heap overflow vulnerability.

The kext is available at [Vulnerable-Kext](https://github.com/ant4g0nist/Vulnerable-Kext)

The kext provides these following vulnerbilities to play with:

```
#define CRASH             0x1
#define HEAP_OVERFLOW     0x2
#define INFO_LEAK         0x3
#define BUFFER_OVERFLOW   0x4
#define USE_AFTER_FREE    0x5   //todo
#define INTEGER_OVERFLOW  0x6   //todo
#define DOUBLE_FETCH      0x7
```

Before we proceed, we need to collect some symbols from the kernel that are required for the kext.
