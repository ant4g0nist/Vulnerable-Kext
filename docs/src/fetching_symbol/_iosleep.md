# _IOSleep

Function starts with this signature
~~~
01 48 88 52 E1 01 A0 72
~~~

We'll search for this signature in the text(__TEXT_EXEC:__text) section. 

The text section for me is:
~~~
__TEXT_EXEC:__text	FFFFFFF007BDC000	FFFFFFF0090BAA74	R	.	X	.	L	para	0D	public	CODE	64	00	0D
~~~

A simple script like this should find it:

~~~
_IOSleep = "01 48 88 52 E1 01 A0 72"

def search_signature(signature):
    start_address = 0xFFFFFFF007BDC000
    end_address   = 0xFFFFFFF0090BAA74

    data = idc.get_bytes(start_address, end_address - start_address)

    address = start_address

    while address < end_address and address != idc.BADADDR:
        address = idc.find_binary(address, idc.SEARCH_DOWN, signature, 0x10)

        if address != idc.BADADDR:
            func = idaapi.get_func(address)
            try:
                if func.start_ea == address:
                    print(f"{address:x}")
            except:
                pass
        
        address += 0x10

print("_IOSleep")
search_signature(_IOSleep)
~~~