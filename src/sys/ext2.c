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

#include "libc/stdlib.h"
#include "libc/string.h"
#include "libc/stdio.h"

#include "ext2.h"

#define INODES_PER_GROUP state.sb.s_group_inodes
#define BGD_SIZE sizeof(struct ext2_bgd)
#define BLOCK_SIZE state.sb.s_block_size
#define INODE_SIZE state.sb.s_inode_size

int ext2_init() {
    impl_read_sector(state.buf, 1);

    // Cache superblock
    memcpy((void*)&state.sb, state.buf, sizeof(struct ext2_sb));

    if (state.sb.s_signature != 0xef53)
        return 1;

    return 0;
}

int ext2_get_inode(struct ext2_inode* inode, uint32_t index) {
    uint32_t inode_off = ((index - 1) / INODES_PER_GROUP) * BGD_SIZE;
    uint32_t block_size = 1024 << BLOCK_SIZE;

    uint32_t inode_bg_lba = inode_off / block_size;
    uint32_t inode_bg_off = inode_off % block_size;

    impl_read_sector(state.buf, state.sb.s_sb_lba + 1 + inode_bg_lba);

    struct ext2_bgd* bgd = (struct ext2_bgd*)(state.buf + inode_bg_off);

    uint32_t inode_table_idx = (index - 1) % INODES_PER_GROUP;
    uint32_t inode_table_start_lba = bgd->s_inode_table_lba;
    uint32_t inode_table_start_off = inode_table_idx * INODE_SIZE;
    uint32_t inode_table_lba_off = inode_table_start_off / block_size;
    uint32_t inode_table_off = inode_table_start_off % block_size;

    impl_read_sector(state.buf, inode_table_start_lba + inode_table_lba_off);

    memcpy(inode, state.buf + inode_table_off, sizeof(struct ext2_inode));

    return 0;
}

int ext2_search_directory(struct ext2_inode* inode, const char* path) {
    int i = 0;

    size_t path_len = 0;

    while (path[path_len] != '/' && path[path_len] != '\0')
        ++path_len;

    while (inode->s_direct[i]) {
        impl_read_sector(state.buf, inode->s_direct[i++]);

        for (int j = 0; j < EXT2_SECTOR_SIZE;) {
            struct ext2_dirent* dirent = (struct ext2_dirent*)(state.buf + j);

            j += dirent->s_size;

            if (!dirent->s_inode)
                continue;

            if (dirent->s_name_len != path_len)
                continue;

            if (!strncmp(dirent->s_name, path, path_len)) {
                ext2_get_inode(inode, dirent->s_inode);

                return 0;
            }
        }
    }

    return 1;
}

int ext2_search(struct ext2_inode* inode, const char* path) {
    if (*path != '/') {
        printf("Path \'%s\' is not absolute\n", path);

        return 1;
    }

    ext2_get_inode(inode, 2);

    ++path;

    while (1) {
        while (*path == '/')
            ++path;

        if (*path == '\0')
            return 0;

        if (ext2_search_directory(inode, path))
            return 1;

        const char* name = path;

        while (*path != '/' && *path != '\0')
            ++path;
        
        if (*path == '/' && (inode->s_tp & 0xf000) != INODE_DIRECTORY) {
            printf("Path \'%s\' is not a directory\n", name);

            return 1;
        }
    }
}

int ext2_dir_search_entry(struct ext2_inode* inode, struct ext2_dirent* dirent, int index) {
    if ((inode->s_tp & 0xf000) != INODE_DIRECTORY) {
        printf("Path is not a directory\n");

        return -1;
    }

    int i = 0;

    while (inode->s_direct[i] && (i != index)) {
        impl_read_sector(state.buf, inode->s_direct[i++]);

        for (int j = 0; j < EXT2_SECTOR_SIZE;) {
            struct ext2_dirent* entry = (struct ext2_dirent*)(state.buf + j);

            j += entry->s_size;

            if (!entry->s_inode)
                continue;

            memcpy(dirent, entry, sizeof(struct ext2_dirent));

            return 1;
        }
    }

    return 0;
}

int ext2_dir_iterate(struct ext2_inode* inode, void (*iterate_func)(struct ext2_dirent*)) {
    char buf[EXT2_SECTOR_SIZE];

    if ((inode->s_tp & 0xf000) != INODE_DIRECTORY) {
        printf("Path is not a directory\n");

        return -1;
    }

    int i = 0;

    while (inode->s_direct[i]) {
        impl_read_sector(buf, inode->s_direct[i++]);

        for (int j = 0; j < EXT2_SECTOR_SIZE;) {
            struct ext2_dirent* entry = (struct ext2_dirent*)(buf + j);

            j += entry->s_size;

            if (!entry->s_inode)
                continue;

            iterate_func(entry);
        }
    }

    return 0;
}

/* Filesystem interface */
int ext2_stat(const char* path);

int ext2_fopen(struct ext2_fd* file, const char* path, const char* mode) {
    if (ext2_search(&file->inode, path))
        return 1;

    if ((file->inode.s_tp & 0xf000) == INODE_DIRECTORY)
        return 1;

    file->cur = 0;

    return 0;
}

int ext2_fseek(struct ext2_fd* file, uint32_t offset, int whence) {
    if (whence == SEEK_SET) {
        file->cur = offset;
    } else if (whence == SEEK_CUR) {
        file->cur += offset;
    } else {
        file->cur = file->inode.s_sizel + offset;
    }
}

size_t ext2_ftell(struct ext2_fd* file) {
    return file->cur;
}

int ext2_feof(struct ext2_fd* file) {
    return file->cur > file->inode.s_sizel;
}

uint32_t ext2_off_2_lba(struct ext2_inode* inode, uint32_t offset) {
    int index = offset / EXT2_SECTOR_SIZE;

    // Direct
    if (index < 12) {
        return inode->s_direct[index];

    // Singly indirect
    } else if (index >= 12 && index <= 268) {
        impl_read_sector(state.buf, inode->s_indirect_s);
        
        index -= 12;

        return ((uint32_t*)state.buf)[index];
    }

    // To-do: Support doubly and triply indirect

    return 0;
}

int ext2_fread(struct ext2_fd* file, void* buf, int size) {
    struct ext2_inode* inode = &file->inode;

    uint8_t* ptr = buf;

    uint32_t lba = ext2_off_2_lba(inode, file->cur);
    uint32_t offset = file->cur % EXT2_SECTOR_SIZE;

    impl_read_sector(state.buf, lba);

    for (int i = 0; i < size; i++) {
        if (ext2_feof(file)) {
            *ptr++ = 0;

            ++file->cur;

            continue;
        }

        if (offset >= EXT2_SECTOR_SIZE) {
            lba = ext2_off_2_lba(inode, file->cur);
            offset = file->cur % EXT2_SECTOR_SIZE;

            impl_read_sector(state.buf, lba);
        }

        *ptr++ = state.buf[offset++];

        ++file->cur;
    }
}

void ext2_fclose(struct ext2_fd* file) {
    file->cur = 0;
}
