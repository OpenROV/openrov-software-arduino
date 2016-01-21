#!/bin/bash

set -e

/opt/openrov/firmware/scripts/build-firmware-trident-alpha.sh

sleep 0.25

/opt/openrov/firmware/scripts/upload-firmware-trident-alpha.sh