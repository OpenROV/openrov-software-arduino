#!/bin/bash

# This script uses the arduino-builder command line tool to build the source contained in OpenROVSam/src/
echo "Uploading firmware to SAMD21..."

# Path variables
PROJECT_DIR=/opt/openrov/firmware
ARDUINO_DIR=/opt/openrov/arduino
SCRIPTS_DIR=${PROJECT_DIR}/scripts
HARDWARE_DIR=${ARDUINO_DIR}/hardware
TOOLS_DIR=${HARDWARE_DIR}/tools
BIN_DIR=${PROJECT_DIR}/bin

# Build variables
SOURCE_FILENAME="OpenROV"

# Run reset script to put SAMD21 into program mode
${SCRIPTS_DIR}/reset-samd21-program-mode.sh

# Upload bin file to SAMD21
$TOOLS_DIR/bossac -e -w -b ${BIN_DIR}/${SOURCE_FILENAME}.bin

if [ $? -eq 0 ]
then
        echo "Successfully uploaded firmware!"
        
	# Reset the SAMD21 to start the program
	${SCRIPTS_DIR}/reset-samd21.sh
	
	exit 0
else
        echo "Firmware upload failed!"
        exit 1
fi
