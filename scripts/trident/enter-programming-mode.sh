#!/bin/sh

echo "Initiating SAMD21 Reset..."
echo "Pulling down the reset pin (GPIO50)"
echo 0 > /sys/class/gpio/gpio50/value

echo "Pulling down the programming enable pin (GPIO60)"
echo 0 > /sys/class/gpio/gpio60/value

sleep 0.25

echo "Releasing reset pin"
echo 1 > /sys/class/gpio/gpio50/value

sleep 0.25

echo "Releasing programming enable pin"
echo 1 > /sys/class/gpio/gpio60/value

echo "SAMD21 should now be in the SAM-BA bootloader, ready for programming."
echo "Device can be programmed using ttyO4 (115200B 8N1)"


