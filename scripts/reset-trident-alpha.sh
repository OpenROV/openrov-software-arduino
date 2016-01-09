#!/bin/sh

echo "Initiating SAMD21 Reset..."

echo "Pulling down the reset pin (GPIO50)"
echo 0 > /sys/class/gpio/gpio50/value

sleep 0.25

echo "Releasing reset pin"
echo 1 > /sys/class/gpio/gpio50/value

echo "SAMD21 reset!"
