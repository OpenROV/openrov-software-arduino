#!/bin/bash

PROJECT_DIR=/opt/openrov/firmware
BIN_DIR=${PROJECT_DIR}/bin/trident

BOOTLOADER_PATH=/opt/openrov/arduino/hardware/openrov/samd/bootloaders/samd21

openocd -f ${BOOTLOADER_PATH}/openocd.cfg -c "program ${BIN_DIR}/OpenROV.bin; reset; exit"
