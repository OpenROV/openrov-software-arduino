#!/bin/bash
set -e

# Path variables
PROJECT_DIR=/opt/openrov/firmware
BIN_DIR=${PROJECT_DIR}/bin/cb25
SCRIPTS_DIR=${PROJECT_DIR}/scripts/cb25
ESC_FIRMWARE=$BIN_DIR/afro/afro_nfet.hex

# Build and upload the firmware which turns the MCU into a flashing tool
${SCRIPTS_DIR}/build-usblinker.sh
${SCRIPTS_DIR}/upload-usblinker.sh

# Flash the hex file onto the escs

# Stop the cockpit service
/etc/init.d/openrov stop

# Set UART settings
stty -F /dev/ttyO1 raw 19200

# Flash ESC1
avrdude -p m8 -b 19200 -P /dev/ttyO1 -c avrispv2 -e -U flash:w:${ESC_FIRMWARE}:i -vv

# Flash ESC2
echo -n "\$M<P52" > /dev/ttyO1
avrdude -p m8 -b 19200 -P /dev/ttyO1 -c avrispv2 -e -U flash:w:${ESC_FIRMWARE}:i -vv

# Flash ESC3
echo -n "\$M<P53" > /dev/ttyO1
avrdude -p m8 -b 19200 -P /dev/ttyO1 -c avrispv2 -e -U flash:w:${ESC_FIRMWARE}:i -vv

# Reflash the OpenROV firmware onto the MCU
${SCRIPTS_DIR}/build.sh
${SCRIPTS_DIR}/upload.sh

# Restart the cockpit service
/etc/init.d/openrov restart
