#!/bin/bash
set -e

SCRIPTS_DIR=${PROJECT_DIR}/scripts/cb25

${SCRIPTS_DIR}/build-usblinker.sh
${SCRIPTS_DIR}/upload-usblinker.sh

# Flash the hex file onto the escs
