/*
 * NightOS - Simple RAM Filesystem Implementation
 * 
 * In-memory filesystem for basic file operations
 */

#include "../include/fs.h"
#include "../include/memory.h"
#include "../include/string.h"
#include "../include/timer.h"

/* Filesystem state */
static fs_file_t files[FS_MAX_FILES];
static fs_handle_t handles[16];
static bool fs_initialized = false;

/* Initialize filesystem */
void fs_init(void) {
    memset(files, 0, sizeof(files));
    memset(handles, 0, sizeof(handles));
    
    /* Create root directory entry */
    strcpy(files[0].name, "/");
    files[0].type = FS_TYPE_DIRECTORY;
    files[0].flags = FS_FLAG_READ | FS_FLAG_SYSTEM;
    files[0].created = timer_get_seconds();
    files[0].data = NULL;
    
    fs_initialized = true;
}

/* Find a file by name */
fs_file_t* fs_find(const char* name) {
    for (int i = 0; i < FS_MAX_FILES; i++) {
        if (files[i].type != FS_TYPE_FREE) {
            if (strcmp(files[i].name, name) == 0) {
                return &files[i];
            }
        }
    }
    return NULL;
}

/* Check if file exists */
bool fs_exists(const char* name) {
    return fs_find(name) != NULL;
}

/* Get file size */
uint32_t fs_size(const char* name) {
    fs_file_t* file = fs_find(name);
    return file ? file->size : 0;
}

/* Find free file slot */
static int find_free_slot(void) {
    for (int i = 0; i < FS_MAX_FILES; i++) {
        if (files[i].type == FS_TYPE_FREE) {
            return i;
        }
    }
    return -1;
}

/* Find free handle */
static int find_free_handle(void) {
    for (int i = 0; i < 16; i++) {
        if (!handles[i].in_use) {
            return i;
        }
    }
    return -1;
}

/* Create a file or directory */
int fs_create(const char* name, uint8_t type) {
    if (!fs_initialized) return -1;
    if (fs_exists(name)) return -2;  /* Already exists */
    if (strlen(name) >= FS_MAX_FILENAME) return -3;
    
    int slot = find_free_slot();
    if (slot < 0) return -4;  /* No free slots */
    
    strncpy(files[slot].name, name, FS_MAX_FILENAME - 1);
    files[slot].name[FS_MAX_FILENAME - 1] = '\0';
    files[slot].type = type;
    files[slot].flags = FS_FLAG_READ | FS_FLAG_WRITE;
    files[slot].size = 0;
    files[slot].created = timer_get_seconds();
    files[slot].modified = files[slot].created;
    
    if (type == FS_TYPE_FILE) {
        files[slot].data = (uint8_t*)kmalloc(FS_MAX_FILESIZE);
        if (!files[slot].data) {
            files[slot].type = FS_TYPE_FREE;
            return -5;  /* Out of memory */
        }
        memset(files[slot].data, 0, FS_MAX_FILESIZE);
    } else {
        files[slot].data = NULL;
    }
    
    return 0;
}

/* Delete a file */
int fs_delete(const char* name) {
    fs_file_t* file = fs_find(name);
    if (!file) return -1;
    if (file->flags & FS_FLAG_SYSTEM) return -2;  /* Can't delete system files */
    
    if (file->data) {
        kfree(file->data);
    }
    
    memset(file, 0, sizeof(fs_file_t));
    return 0;
}

/* Open a file */
int fs_open(const char* name, uint8_t mode) {
    fs_file_t* file = fs_find(name);
    if (!file) return -1;
    if (file->type != FS_TYPE_FILE) return -2;
    
    int h = find_free_handle();
    if (h < 0) return -3;
    
    handles[h].file = file;
    handles[h].position = 0;
    handles[h].mode = mode;
    handles[h].in_use = true;
    
    return h;
}

/* Close a file handle */
void fs_close(int handle) {
    if (handle >= 0 && handle < 16) {
        handles[handle].in_use = false;
        handles[handle].file = NULL;
    }
}

/* Read from file */
int fs_read(int handle, void* buffer, uint32_t size) {
    if (handle < 0 || handle >= 16) return -1;
    if (!handles[handle].in_use) return -2;
    if (!(handles[handle].mode & FS_FLAG_READ)) return -3;
    
    fs_handle_t* h = &handles[handle];
    fs_file_t* f = h->file;
    
    if (h->position >= f->size) return 0;  /* EOF */
    
    uint32_t to_read = size;
    if (h->position + to_read > f->size) {
        to_read = f->size - h->position;
    }
    
    memcpy(buffer, f->data + h->position, to_read);
    h->position += to_read;
    
    return to_read;
}

/* Write to file */
int fs_write(int handle, const void* buffer, uint32_t size) {
    if (handle < 0 || handle >= 16) return -1;
    if (!handles[handle].in_use) return -2;
    if (!(handles[handle].mode & FS_FLAG_WRITE)) return -3;
    
    fs_handle_t* h = &handles[handle];
    fs_file_t* f = h->file;
    
    if (h->position + size > FS_MAX_FILESIZE) {
        size = FS_MAX_FILESIZE - h->position;
    }
    
    if (size == 0) return 0;
    
    memcpy(f->data + h->position, buffer, size);
    h->position += size;
    
    if (h->position > f->size) {
        f->size = h->position;
    }
    
    f->modified = timer_get_seconds();
    return size;
}

/* Seek in file */
int fs_seek(int handle, uint32_t position) {
    if (handle < 0 || handle >= 16) return -1;
    if (!handles[handle].in_use) return -2;
    
    if (position > handles[handle].file->size) {
        position = handles[handle].file->size;
    }
    
    handles[handle].position = position;
    return 0;
}

/* List files */
int fs_list(fs_dirent_t* entries, int max_entries) {
    int count = 0;
    
    for (int i = 0; i < FS_MAX_FILES && count < max_entries; i++) {
        if (files[i].type != FS_TYPE_FREE) {
            strncpy(entries[count].name, files[i].name, FS_MAX_FILENAME);
            entries[count].type = files[i].type;
            entries[count].size = files[i].size;
            count++;
        }
    }
    
    return count;
}

/* Count files */
int fs_count(void) {
    int count = 0;
    for (int i = 0; i < FS_MAX_FILES; i++) {
        if (files[i].type != FS_TYPE_FREE) {
            count++;
        }
    }
    return count;
}

/* Format filesystem (clear all) */
void fs_format(void) {
    for (int i = 0; i < FS_MAX_FILES; i++) {
        if (files[i].data) {
            kfree(files[i].data);
        }
    }
    memset(files, 0, sizeof(files));
    memset(handles, 0, sizeof(handles));
    
    /* Re-create root */
    strcpy(files[0].name, "/");
    files[0].type = FS_TYPE_DIRECTORY;
    files[0].flags = FS_FLAG_READ | FS_FLAG_SYSTEM;
}

/* Get free space */
uint32_t fs_free_space(void) {
    int free_slots = 0;
    for (int i = 0; i < FS_MAX_FILES; i++) {
        if (files[i].type == FS_TYPE_FREE) {
            free_slots++;
        }
    }
    return free_slots * FS_MAX_FILESIZE;
}

/* Get used space */
uint32_t fs_used_space(void) {
    uint32_t used = 0;
    for (int i = 0; i < FS_MAX_FILES; i++) {
        if (files[i].type == FS_TYPE_FILE) {
            used += files[i].size;
        }
    }
    return used;
}
