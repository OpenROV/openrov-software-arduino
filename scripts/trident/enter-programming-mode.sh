#!/bin/sh
set -e

# Load the config variables for the attached board
. /opt/openrov/cockpit/linux/scripts/get-board-config.sh

echo "Resetting SAMD21 into programming mode..."

echo 0 > /sys/class/gpio/gpio${MCU_RESET_PIN}/value
echo 0 > /sys/class/gpio/gpio${MCU_PROG_PIN}/value

sleep 0.25

echo 1 > /sys/class/gpio/gpio${MCU_RESET_PIN}/value

sleep 0.25

echo 1 > /sys/class/gpio/gpio${MCU_PROG_PIN}/value

echo "SAMD21 should now be in the SAM-BA bootloader, ready for programming."
echo "Device can be programmed using ttyO4 (115200B 8N1)"


