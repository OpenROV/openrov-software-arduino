#!/bin/bash

BOOTLOADER_PATH=/opt/openrov/arduino/hardware/openrov/samd/bootloaders/samd21

openocd -f ${BOOTLOADER_PATH}/openocd.cfg -c "program ${BOOTLOADER_PATH}/samba_both.hex; reset; exit"
