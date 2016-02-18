#!/bin/sh

error_exit ()
{

#       ----------------------------------------------------------------
#       Function for exit due to fatal program error
#               Accepts 1 argument:
#                       string containing descriptive error message
#       ----------------------------------------------------------------


        echo "${1:-"Unknown Error"}" 1>&2
        exit 1
}

COUNTER=0

export UPLOAD_TO_ATMEGA_COMMAND="avrdude -P /dev/spidev1.0 -c linuxspi -vvv -p m2560 -U flash:w:../../bin/OpenROV.hex"
export LINUX_RESET_GPIO=30
export UPLOAD_REQUIRES_RESET=false


while [ $COUNTER -lt 9 ]; do
    echo $COUNTER

    EXITCODE=`$UPLOAD_TO_ATMEGA_COMMAND 1>&2`
        if [ $? -eq 0 ]
                then
                        echo upload successfull! 1>&2
                        exit 0
                fi
        COUNTER=`expr $COUNTER + 1`
        echo upload failed, trying again. 1>&2
done
error_exit "Failed to upload after numerous tries. Aborting."
