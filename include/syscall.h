/*
 * NightOS - System Calls
 * 
 * User-kernel interface
 */

#ifndef SYSCALL_H
#define SYSCALL_H

#include "types.h"

/* System call numbers */
#define SYS_EXIT        0
#define SYS_WRITE       1
#define SYS_READ        2
#define SYS_OPEN        3
#define SYS_CLOSE       4
#define SYS_GETPID      5
#define SYS_FORK        6
#define SYS_EXEC        7
#define SYS_WAIT        8
#define SYS_SLEEP       9
#define SYS_TIME        10
#define SYS_MALLOC      11
#define SYS_FREE        12
#define SYS_YIELD       13
#define SYS_KILL        14
#define SYS_STAT        15

/* System call interrupt number */
#define SYSCALL_INT     0x80

/* Initialize system calls */
void syscall_init(void);

/* System call handler */
void syscall_handler(void);

/* System call implementations */
void sys_exit(int code);
int sys_write(int fd, const void* buf, uint32_t count);
int sys_read(int fd, void* buf, uint32_t count);
int sys_open(const char* path, int flags);
int sys_close(int fd);
uint32_t sys_getpid(void);
int sys_fork(void);
int sys_exec(const char* path, char* const argv[]);
int sys_wait(int* status);
void sys_sleep(uint32_t ms);
uint32_t sys_time(void);
void* sys_malloc(uint32_t size);
void sys_free(void* ptr);
void sys_yield(void);
int sys_kill(uint32_t pid, int signal);

/* User-space syscall wrappers (inline assembly) */
static inline int syscall0(int num) {
    int ret;
    __asm__ volatile(
        "int $0x80"
        : "=a"(ret)
        : "a"(num)
    );
    return ret;
}

static inline int syscall1(int num, uint32_t arg1) {
    int ret;
    __asm__ volatile(
        "int $0x80"
        : "=a"(ret)
        : "a"(num), "b"(arg1)
    );
    return ret;
}

static inline int syscall2(int num, uint32_t arg1, uint32_t arg2) {
    int ret;
    __asm__ volatile(
        "int $0x80"
        : "=a"(ret)
        : "a"(num), "b"(arg1), "c"(arg2)
    );
    return ret;
}

static inline int syscall3(int num, uint32_t arg1, uint32_t arg2, uint32_t arg3) {
    int ret;
    __asm__ volatile(
        "int $0x80"
        : "=a"(ret)
        : "a"(num), "b"(arg1), "c"(arg2), "d"(arg3)
    );
    return ret;
}

#endif /* SYSCALL_H */
