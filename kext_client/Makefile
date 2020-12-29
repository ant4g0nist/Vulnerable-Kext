CC = `xcrun --sdk iphoneos --find gcc`
SDK=`xcrun --sdk iphoneos --show-sdk-path`

CFLAGS = -O2 -Wimplicit -isysroot $(SDK) -F$(SDK)/System/Library/Frameworks

.DEFAULT_GOAL := build

kext_client: kext_client.c
	$(CC) $(CFLAGS) -arch arm64 -o $@ $^ 

build: kext_client
	codesign -f --entitlements ent.xml -s - kext_client
	scp -P 2222 kext_client root@localhost:/var/root/
	ssh -p 2222 root@localhost "/var/root/kext_client"

.PHONY : clean

clean:
	rm kext_client
