// Simple ext2 filesystem driver written in ANSI C
//
// MIT License
//
// Copyright (c) 2024 Allkern (https://github.com/allkern)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef EXT2_H
#define EXT2_H

#ifdef __cplusplus
extern "C" {
#endif

#include "libc/stdint.h"

/*
0 	3 	4 	Total number of inodes in file system
4 	7 	4 	Total number of blocks in file system
8 	11 	4 	Number of blocks reserved for superuser (see offset 80)
12 	15 	4 	Total number of unallocated blocks
16 	19 	4 	Total number of unallocated inodes
20 	23 	4 	Block number of the block containing the superblock (also the starting block number, NOT always zero.)
24 	27 	4 	log2 (block size) - 10. (In other words, the number to shift 1,024 to the left by to obtain the block size)
28 	31 	4 	log2 (fragment size) - 10. (In other words, the number to shift 1,024 to the left by to obtain the fragment size)
32 	35 	4 	Number of blocks in each block group
36 	39 	4 	Number of fragments in each block group
40 	43 	4 	Number of inodes in each block group
44 	47 	4 	Last mount time (in POSIX time)
48 	51 	4 	Last written time (in POSIX time)
52 	53 	2 	Number of times the volume has been mounted since its last consistency check (fsck)
54 	55 	2 	Number of mounts allowed before a consistency check (fsck) must be done
56 	57 	2 	Ext2 signature (0xef53), used to help confirm the presence of Ext2 on a volume
58 	59 	2 	File system state (see below)
60 	61 	2 	What to do when an error is detected (see below)
62 	63 	2 	Minor portion of version (combine with Major portion below to construct full version field)
64 	67 	4 	POSIX time of last consistency check (fsck)
68 	71 	4 	Interval (in POSIX time) between forced consistency checks (fsck)
72 	75 	4 	Operating system ID from which the filesystem on this volume was created (see below)
76 	79 	4 	Major portion of version (combine with Minor portion above to construct full version field)
80 	81 	2 	User ID that can use reserved blocks
82 	83 	2 	Group ID that can use reserved blocks 
*/

struct ext2_sb {
    uint32_t s_total_inodes;
    uint32_t s_total_blocks;
    uint32_t s_resv_su_blocks;
    uint32_t s_free_blocks;
    uint32_t s_free_inodes;
    uint32_t s_sb_lba;
    uint32_t s_block_size;
    uint32_t s_fragment_size;
    uint32_t s_group_blocks;
    uint32_t s_group_fragments;
    uint32_t s_group_inodes;
    uint32_t s_last_mount_time;
    uint32_t s_last_write_time;
    uint16_t s_mount_count;
    uint16_t s_mounts_remaining;
    uint16_t s_signature;
    uint16_t s_fs_state;
    uint16_t s_err_action;
    uint16_t s_version_minor;
    uint32_t s_last_check_time;
    uint32_t s_check_interval;
    uint32_t s_creator_os;
    uint32_t s_version_major;
    uint16_t s_resv_user;
    uint16_t s_resv_group;

    /* Extended fields */
    uint32_t s_first_free_inode;
    uint16_t s_inode_size;
    uint16_t s_sb_group;
    uint32_t s_opt_features;
    uint32_t s_req_features;
    uint32_t s_ro_features;
    char s_fs_id[16];
    char s_vol_name[16];
    char s_vol_last_mounted_path[64];
    uint32_t s_compression_algorithms;
    uint8_t s_file_prealloc_blocks;
    uint8_t s_dir_prealloc_blocks;
    uint16_t s_unused;
    char s_journal_id[16];
    uint32_t s_journal_inode;
    uint32_t s_journal_dev;
    uint32_t s_orphan_inode_list_head;
};

/*
0 	3 	4 	Block address of block usage bitmap
4 	7 	4 	Block address of inode usage bitmap
8 	11 	4 	Starting block address of inode table
12 	13 	2 	Number of unallocated blocks in group
14 	15 	2 	Number of unallocated inodes in group
16 	17 	2 	Number of directories in group
18 	31 	X 	(Unused)
*/

struct ext2_bgd {
    uint32_t s_block_usage_lba;
    uint32_t s_inode_usage_lba;
    uint32_t s_inode_table_lba;
    uint16_t s_group_free_blocks;
    uint16_t s_group_free_inodes;
    uint16_t s_group_dir_count;
    char s_unused[32-18];
};

/*
0 	1 	2 	Type and Permissions (see below)
2 	3 	2 	User ID
4 	7 	4 	Lower 32 bits of size in bytes
8 	11 	4 	Last Access Time (in POSIX time)
12 	15 	4 	Creation Time (in POSIX time)
16 	19 	4 	Last Modification time (in POSIX time)
20 	23 	4 	Deletion time (in POSIX time)
24 	25 	2 	Group ID
26 	27 	2 	Count of hard links (directory entries) to this inode. When this reaches 0, the data blocks are marked as unallocated.
28 	31 	4 	Count of disk sectors (not Ext2 blocks) in use by this inode, not counting the actual inode structure nor directory entries linking to the inode.
32 	35 	4 	Flags (see below)
36 	39 	4 	Operating System Specific value #1
40 	43 	4 	Direct Block Pointer 0
44 	47 	4 	Direct Block Pointer 1
48 	51 	4 	Direct Block Pointer 2
52 	55 	4 	Direct Block Pointer 3
56 	59 	4 	Direct Block Pointer 4
60 	63 	4 	Direct Block Pointer 5
64 	67 	4 	Direct Block Pointer 6
68 	71 	4 	Direct Block Pointer 7
72 	75 	4 	Direct Block Pointer 8
76 	79 	4 	Direct Block Pointer 9
80 	83 	4 	Direct Block Pointer 10
84 	87 	4 	Direct Block Pointer 11
88 	91 	4 	Singly Indirect Block Pointer (Points to a block that is a list of block pointers to data)
92 	95 	4 	Doubly Indirect Block Pointer (Points to a block that is a list of block pointers to Singly Indirect Blocks)
96 	99 	4 	Triply Indirect Block Pointer (Points to a block that is a list of block pointers to Doubly Indirect Blocks)
100 	103 	4 	Generation number (Primarily used for NFS)
104 	107 	4 	In Ext2 version 0, this field is reserved. In version >= 1, Extended attribute block (File ACL).
108 	111 	4 	In Ext2 version 0, this field is reserved. In version >= 1, Upper 32 bits of file size (if feature bit set) if it's a file, Directory ACL if it's a directory
112 	115 	4 	Block address of fragment
116 	127 	12 	Operating System Specific Value #2
*/

struct ext2_inode {
    uint16_t s_tp;
    uint16_t s_user_id;
    uint32_t s_sizel;
    uint32_t s_access_time;
    uint32_t s_creation_time;
    uint32_t s_mod_time;
    uint32_t s_deletion_time;
    uint16_t s_group_id;
    uint16_t s_hard_links;
    uint32_t s_sector_count;
    uint32_t s_flags;
    uint32_t s_os1;
    uint32_t s_direct[12];
    uint32_t s_indirect_s;
    uint32_t s_indirect_d;
    uint32_t s_indirect_t;
    uint32_t s_generation_number;
    uint32_t s_extended_attribute_block;
    uint32_t s_sizeh;
    uint32_t s_fragment_block;
    char s_os2[12];
};

#define INODE_PIPE        0x1000
#define INODE_CHARDEV     0x2000
#define INODE_DIRECTORY   0x4000
#define INODE_BLKDEV      0x6000
#define INODE_FILE        0x8000
#define INODE_SYMLINK     0xa000
#define INODE_UNIX_SOCKET 0xc000

#define PERM_OTHER_X 0x001
#define PERM_OTHER_W 0x002
#define PERM_OTHER_R 0x004
#define PERM_GROUP_X 0x008
#define PERM_GROUP_W 0x010
#define PERM_GROUP_R 0x020
#define PERM_USER_X 0x040
#define PERM_USER_W 0x080
#define PERM_USER_R 0x100
#define PERM_STICKY 0x200
#define PERM_SET_GID 0x400
#define PERM_SET_UID 0x800

/*
0 	3 	4 	Inode
4 	5 	2 	Total size of this entry (Including all subfields)
6 	6 	1 	Name Length least-significant 8 bits
7 	7 	1 	Type indicator (only if the feature bit for "directory entries have file type byte" is set, else this is the most-significant 8 bits of the Name Length)
8 	8+N-1 	N 	Name characters
*/

struct ext2_dirent {
    uint32_t s_inode;
    uint16_t s_size;
    uint8_t s_name_len;
    uint8_t s_type;
    char s_name[];
};

#define DIRENT_UNKNOWN   0
#define DIRENT_FILE      1
#define DIRENT_DIRECTORY 2
#define DIRENT_CHARDEV   3
#define DIRENT_BLKDEV    4
#define DIRENT_PIPE      5
#define DIRENT_SOCKET    6
#define DIRENT_SYMLINK   7

#define EXT2_SECTOR_SIZE 1024

enum {
    FSTAT_OK
};

struct ext2_fd {
    struct ext2_inode inode;
    uint32_t cur;
};

struct ext2_state {
    uint8_t buf[EXT2_SECTOR_SIZE];

    struct ext2_sb sb;

    int status;
} static state;

int impl_read_sector(void* buf, uint32_t lba);
int impl_write_sector(void* buf, uint32_t lba);

int ext2_init();
int ext2_get_inode(struct ext2_inode* inode, uint32_t index);
int ext2_search(struct ext2_inode* inode, const char* path);
int ext2_dir_search_entry(struct ext2_inode* inode, struct ext2_dirent* dirent, int index);
int ext2_dir_iterate(struct ext2_inode* inode, void (*iterate_func)(struct ext2_dirent*));

/* Filesystem interface */
int ext2_stat(const char* path);
int ext2_fopen(struct ext2_fd* file, const char* path, const char* mode);
int ext2_fseek(struct ext2_fd* file, uint32_t offset, int whence);
uint32_t ext2_ftell(struct ext2_fd* file);
int ext2_feof(struct ext2_fd* file);
int ext2_fread(struct ext2_fd* file, void* buf, int size);
void ext2_fclose(struct ext2_fd* file);

#ifdef __cplusplus
}
#endif

#endif