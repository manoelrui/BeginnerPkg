#!/bin/bash

# Initial values. These can be overwritten by user's command line args
QEMU_EXEC='qemu-system-x86_64'
QEMU_ARGS=""
FIRMWARE="OVMF.fd"
VFAT_DIR="hda"
SERIAL_OUTPUT="serial_output.txt"
USB_DEVICES=""
MEMORY="3G"
NETWORK=""
HDA_IMG=""

CPY_IMAGE_SCRIPT_DIR=""

# Get user's custom and additional options
while getopts "f:u:m:s:w:d:h:c:b" opt; do
    case $opt in
        f)
            FIRMWARE="$OPTARG"
            ;;
        u)
            USB_DEVICES="$USB_DEVICES -usbdevice host:$OPTARG"
            ;;
        m)
            MEMORY="$OPTARG"
            ;;
        s)
            SERIAL_OUTPUT="$OPTARG"
            ;;
        d)
            VFAT_DIR="$OPTARG"
            ;;
        b)
            NETWORK="-net nic -net bridge,br=br0"
            ;;
        c)
            CPY_IMAGE_SCRIPT_DIR="$OPTARG"
            ;;
        h)
            HDA_IMG="-drive file=$OPTARG,format=raw"
            ;;
    esac
done

# firmware image file
QEMU_ARGS="$QEMU_ARGS -drive file=$FIRMWARE,format=raw,if=pflash"

# virtual fat file system
QEMU_ARGS="$QEMU_ARGS -drive id=nvme0,file=fat:wr:$VFAT_DIR,format=raw,if=none"
QEMU_ARGS="$QEMU_ARGS -device nvme,drive=nvme0,serial=1234"

# machine config
QEMU_ARGS="$QEMU_ARGS -m $MEMORY -machine q35"

# serial output file
QEMU_ARGS="$QEMU_ARGS -serial file:$SERIAL_OUTPUT"

# real usb devices passthrough
QEMU_ARGS="$QEMU_ARGS $USB_DEVICES"

# network config
QEMU_ARGS="$QEMU_ARGS $NETWORK"

# HDA config
QEMU_ARGS="$QEMU_ARGS $HDA_IMG"

if [ ! -z "$CPY_IMAGE_SCRIPT_DIR" -a "$CPY_IMAGE_SCRIPT_DIR" != "" ]; then
    source "$CPY_IMAGE_SCRIPT_DIR"
fi

##
## At last! let's run QEMU
##
$QEMU_EXEC $QEMU_ARGS
