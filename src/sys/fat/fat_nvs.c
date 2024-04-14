// DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
//                    Version 2, December 2004
//  
// Copyright (C) 2004 Sam Hocevar <sam@hocevar.net>
// 
// Everyone is permitted to copy and distribute verbatim or modified
// copies of this license document, and changing it is allowed as long
// as the name is changed.
//  
//            DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
//   TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION
// 
//  0. You just DO WHAT THE FUCK YOU WANT TO.

#include "hw/nvs.h"
#include "fs.h"

#include "libc/string.h"

#define FAT32_NVS_MAX_SECTORS 1
#define FAT32_SECTOR_SIZE 512

// Support reading max 8 sectors
uint8_t sector[NVS_SECTOR_SIZE * FAT32_NVS_MAX_SECTORS];

u8 disk_get_status(disk_e disk) {
	// Disk present
	if (nvs_get_status(disk) & NVS_STAT_PROBE) {
		nvs_read_ident(disk, sector);

		nvs_id* id = (nvs_id*)sector;

		return id->type ? 1 : 0;
	}

	return 0;
}

u8 disk_initialize(disk_e disk) {
	memset(sector, 0, NVS_SECTOR_SIZE);

	// Disk initialized by BIOS
	return 1;
}

u8 disk_read(disk_e disk, u8* buffer, u32 lba, u32 count) {
	// To-do: Support reading multiple sectors at once
	nvs_read_sector(disk, sector, lba);

	memcpy(buffer, sector, FAT32_SECTOR_SIZE);

	return 1;
}

u8 disk_write(disk_e disk, const u8* buffer, u32 lba, u32 count) {
	while (count--) {
		memcpy(sector, buffer, FAT32_SECTOR_SIZE);

		nvs_write_sector(disk, sector, lba);

		buffer += FAT32_SECTOR_SIZE;
	}

	return 1;
}
