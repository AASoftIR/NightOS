/*
 * NightOS - Simple Filesystem (RAM Disk)
 * 
 * Basic in-memory filesystem for file operations
 */

#ifndef FS_H
#define FS_H

#include "types.h"

/* Filesystem constants */
#define FS_MAX_FILES        64
#define FS_MAX_FILENAME     32
#define FS_MAX_FILESIZE     4096
#define FS_BLOCK_SIZE       512

/* File types */
#define FS_TYPE_FREE        0
#define FS_TYPE_FILE        1
#define FS_TYPE_DIRECTORY   2

/* File flags */
#define FS_FLAG_READ        0x01
#define FS_FLAG_WRITE       0x02
#define FS_FLAG_HIDDEN      0x04
#define FS_FLAG_SYSTEM      0x08

/* File entry structure */
typedef struct {
    char name[FS_MAX_FILENAME];
    uint8_t type;
    uint8_t flags;
    uint32_t size;
    uint32_t created;       /* Timestamp */
    uint32_t modified;      /* Timestamp */
    uint8_t* data;          /* Pointer to file data */
} fs_file_t;

/* Directory entry for listing */
typedef struct {
    char name[FS_MAX_FILENAME];
    uint8_t type;
    uint32_t size;
} fs_dirent_t;

/* File handle for open files */
typedef struct {
    fs_file_t* file;
    uint32_t position;      /* Current read/write position */
    uint8_t mode;           /* Open mode (read/write) */
    bool in_use;
} fs_handle_t;

/* Initialize filesystem */
void fs_init(void);

/* File operations */
int fs_create(const char* name, uint8_t type);
int fs_delete(const char* name);
int fs_open(const char* name, uint8_t mode);
void fs_close(int handle);
int fs_read(int handle, void* buffer, uint32_t size);
int fs_write(int handle, const void* buffer, uint32_t size);
int fs_seek(int handle, uint32_t position);

/* File info */
fs_file_t* fs_find(const char* name);
bool fs_exists(const char* name);
uint32_t fs_size(const char* name);

/* Directory operations */
int fs_list(fs_dirent_t* entries, int max_entries);
int fs_count(void);

/* Utility */
void fs_format(void);
uint32_t fs_free_space(void);
uint32_t fs_used_space(void);

#endif /* FS_H */
