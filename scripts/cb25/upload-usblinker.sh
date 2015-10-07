#!/bin/bash
set -e

# This script uses the arduino-builder command line tool to build the source contained in OpenROVSam/src/
echo "Uploading USB Linker firmware to ATMEGA2560..."

# Path variables
PROJECT_DIR=/opt/openrov/firmware
BIN_DIR=${PROJECT_DIR}/bin/cb25

# Build variables
SOURCE_FILENAME="ArduinoUSBLinker"

avrdude -P /dev/spidev1.0 -c linuxspi -vvv -p m2560 -U flash:w:${BIN_DIR}/${SOURCE_FILENAME}.hex

if [ $? -eq 0 ]
then
        echo "Successfully uploaded firmware!"

        # Reset the MCU to start the program
        /opt/openrov/cockpit/linux/scripts/reset-mcu.sh

        exit 0
else
        echo "Firmware upload failed!"
        exit 1
fi

