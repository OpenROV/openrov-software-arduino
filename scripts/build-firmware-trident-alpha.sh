#!/bin/bash

# This script uses the arduino-builder command line tool to build the source contained in OpenROVSam/src/
echo "Building firmware for OpenROV SAMD21..."

# Path variables
PROJECT_DIR=/opt/openrov/firmware
ARDUINO_DIR=/opt/openrov/arduino
SCRIPTS_DIR=${PROJECT_DIR}/scripts
SOURCE_DIR=${PROJECT_DIR}/sketches/OpenROV
HARDWARE_DIR=${ARDUINO_DIR}/hardware
TOOLS_DIR=${HARDWARE_DIR}/tools
BIN_DIR=${PROJECT_DIR}/bin
TOOLCHAIN_DIR=/usr/bin

# Build variables
FQ_BOARDNAME="openrov:samd:trident_alpha"
SOURCE_FILENAME="OpenROV"
SOURCE_EXT=".ino"

# Create temp build directory
mkdir ${PROJECT_DIR}/build
BUILD_DIR=${PROJECT_DIR}/build
#echo "Created temporary build directory: ${BUILD_DIR}"


arduino-builder -verbose -debug-level=1 -compile -build-path ${BUILD_DIR}/ -hardware ${HARDWARE_DIR}/ -tools ${TOOLCHAIN_DIR} -fqbn ${FQ_BOARDNAME} ${SOURCE_DIR}/${SOURCE_FILENAME}${SOURCE_EXT}

if [ $? -eq 0 ]
then
	echo "Successfully built firmware!"
	RET=0

	echo "Copied ${SOURCE_FILENAME}.bin to ${BIN_DIR}!"
	# Copy the compiled .bin file to the bin directory
	cp ${BUILD_DIR}/${SOURCE_FILENAME}${SOURCE_EXT}.bin ${BIN_DIR}/${SOURCE_FILENAME}.bin
else
	echo "Firmware build failed!"
	RET=1
fi

# Delete the temporary build folder
rm -rf ${BUILD_DIR}

# Finished successfully
exit ${RET}
