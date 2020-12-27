#!/bin/bash
make
./kext_loader/kext_loader -l pongo_module/kextload.pongo-module -s kernel_symbols/ -k vulnerable_kext/vulnerable_kext.ikext