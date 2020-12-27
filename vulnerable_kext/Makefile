TARGET = vulnerable_kext.ikext

ARCH = arm64
SDK  = iphoneos
VERSION = 0.1.0

SYSROOT := $(shell xcrun --sdk $(SDK) --show-sdk-path)
ifeq ($(SYSROOT),)
$(error Could not find SDK $(SDK))
endif
CLANG := $(shell xcrun --sdk $(SDK) --find clang)
CC := $(CLANG) -isysroot $(SYSROOT) -arch $(ARCH)

CFLAGS  = -fno-builtin -fno-common -mkernel
CFLAGS += -Isource
CFLAGS += -O2
CFLAGS += -Wall -Werror -Wpedantic -Wno-gnu -Wno-language-extension-token -Wunused-function

# TODO: -Wconversion

DEFINES = $(GLOBAL_DEFINES) -DVERSION='"$(VERSION)"'

LDFLAGS  = -Xlinker -kext -nostdlib -Xlinker -fatal_warnings

SOURCES = vulnerable_kext.c \
			vulnerabilities.c

HEADERS = kernel_imports.h \
		  vulnerabilities.h

$(TARGET): $(SOURCES) $(HEADERS)
	$(CC) $(CFLAGS) $(DEFINES) $(LDFLAGS) -o $@ $(SOURCES)

symbols: $(TARGET)
	@nm -u $(TARGET)

clean:
	rm -rf -- $(TARGET).$(ARCH).ld
	rm -f -- $(TARGET)
