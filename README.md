# bootloader
Orange Pi (H3) bootloader

# Make a bootable SD Card
## Find the SD Card
* lsblk

## If the SD card is connected via USB and is sdX (replace X for a correct letter)
* export card=/dev/sdX
* export p=""

## If the SD card is connected via mmc and is mmcblk0
* export card=/dev/mmcblk0
* export p=p

## Clean first Megabyte
* dd if=/dev/zero of=${card} bs=1M count=1

## If you wish to keep the partition table, run:
* dd if=/dev/zero of=${card} bs=1k count=1023 seek=1

## Copy the bootloader
* dd if=bin/bootloader.sunxi of=${card} bs=1024 seek=8

## Create base FAT32 Partition
* fdisk ${card}

  -> n -> p -> 1 -> 2048 -> +2G
  
  -> t -> c

## Create second FAT32 Partition with remaing free memory
* fdisk ${card}

  -> n -> p -> 2 -> "default" -> "default"

  -> t -> c

# Bootloader commands
## **help** cmd
  Print help for the specified command
## **load** type addr size/file
  Load file to RAM.

  Types: sd or serial
## **go** <*core*> addr arg0 arg1
  Start execution of specified CPU core to the specifed address and set the argument registers

  Use <*> to start all cores
## **read** addr
  Read memory address
## **write** addr value
  Write to specified memory address
