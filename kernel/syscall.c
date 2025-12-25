/*
 * NightOS - System Calls Implementation
 * 
 * Kernel-user interface through software interrupts
 */

#include "../include/syscall.h"
#include "../include/idt.h"
#include "../include/vga.h"
#include "../include/memory.h"
#include "../include/timer.h"
#include "../include/process.h"
#include "../include/fs.h"
#include "../include/rtc.h"

/* System call table */
typedef int (*syscall_fn_t)(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);

/* Forward declarations */
static int sys_exit_handler(uint32_t code, uint32_t, uint32_t, uint32_t, uint32_t);
static int sys_write_handler(uint32_t fd, uint32_t buf, uint32_t count, uint32_t, uint32_t);
static int sys_read_handler(uint32_t fd, uint32_t buf, uint32_t count, uint32_t, uint32_t);
static int sys_open_handler(uint32_t path, uint32_t flags, uint32_t, uint32_t, uint32_t);
static int sys_close_handler(uint32_t fd, uint32_t, uint32_t, uint32_t, uint32_t);
static int sys_getpid_handler(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);
static int sys_sleep_handler(uint32_t ms, uint32_t, uint32_t, uint32_t, uint32_t);
static int sys_time_handler(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);
static int sys_malloc_handler(uint32_t size, uint32_t, uint32_t, uint32_t, uint32_t);
static int sys_free_handler(uint32_t ptr, uint32_t, uint32_t, uint32_t, uint32_t);
static int sys_yield_handler(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);
static int sys_kill_handler(uint32_t pid, uint32_t sig, uint32_t, uint32_t, uint32_t);

/* System call table */
static syscall_fn_t syscall_table[] = {
    [SYS_EXIT]   = sys_exit_handler,
    [SYS_WRITE]  = sys_write_handler,
    [SYS_READ]   = sys_read_handler,
    [SYS_OPEN]   = sys_open_handler,
    [SYS_CLOSE]  = sys_close_handler,
    [SYS_GETPID] = sys_getpid_handler,
    [SYS_SLEEP]  = sys_sleep_handler,
    [SYS_TIME]   = sys_time_handler,
    [SYS_MALLOC] = sys_malloc_handler,
    [SYS_FREE]   = sys_free_handler,
    [SYS_YIELD]  = sys_yield_handler,
    [SYS_KILL]   = sys_kill_handler,
};

#define NUM_SYSCALLS (sizeof(syscall_table) / sizeof(syscall_table[0]))

/* Syscall interrupt handler */
static void syscall_isr(registers_t* regs) {
    uint32_t syscall_num = regs->eax;
    
    if (syscall_num >= NUM_SYSCALLS || !syscall_table[syscall_num]) {
        regs->eax = -1;  /* Invalid syscall */
        return;
    }
    
    /* Call handler with arguments from registers */
    int ret = syscall_table[syscall_num](
        regs->ebx,  /* arg1 */
        regs->ecx,  /* arg2 */
        regs->edx,  /* arg3 */
        regs->esi,  /* arg4 */
        regs->edi   /* arg5 */
    );
    
    regs->eax = ret;
}

/* Initialize system calls */
void syscall_init(void) {
    /* Register syscall interrupt handler (INT 0x80) */
    register_interrupt_handler(SYSCALL_INT, syscall_isr);
}

/* ========== System Call Implementations ========== */

static int sys_exit_handler(uint32_t code, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5) {
    UNUSED(a2); UNUSED(a3); UNUSED(a4); UNUSED(a5);
    process_exit(code);
    return 0;
}

static int sys_write_handler(uint32_t fd, uint32_t buf, uint32_t count, uint32_t a4, uint32_t a5) {
    UNUSED(a4); UNUSED(a5);
    
    if (fd == 1 || fd == 2) {  /* stdout or stderr */
        const char* str = (const char*)buf;
        for (uint32_t i = 0; i < count && str[i]; i++) {
            vga_putchar(str[i]);
        }
        return count;
    }
    
    /* File write */
    return fs_write(fd - 3, (void*)buf, count);
}

static int sys_read_handler(uint32_t fd, uint32_t buf, uint32_t count, uint32_t a4, uint32_t a5) {
    UNUSED(a4); UNUSED(a5);
    
    if (fd == 0) {  /* stdin */
        /* Would read from keyboard buffer */
        return 0;
    }
    
    /* File read */
    return fs_read(fd - 3, (void*)buf, count);
}

static int sys_open_handler(uint32_t path, uint32_t flags, uint32_t a3, uint32_t a4, uint32_t a5) {
    UNUSED(a3); UNUSED(a4); UNUSED(a5);
    
    int handle = fs_open((const char*)path, flags);
    if (handle >= 0) {
        return handle + 3;  /* Offset by stdin/stdout/stderr */
    }
    return handle;
}

static int sys_close_handler(uint32_t fd, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5) {
    UNUSED(a2); UNUSED(a3); UNUSED(a4); UNUSED(a5);
    
    if (fd <= 2) return -1;  /* Can't close std streams */
    
    fs_close(fd - 3);
    return 0;
}

static int sys_getpid_handler(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5) {
    UNUSED(a1); UNUSED(a2); UNUSED(a3); UNUSED(a4); UNUSED(a5);
    return process_getpid();
}

static int sys_sleep_handler(uint32_t ms, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5) {
    UNUSED(a2); UNUSED(a3); UNUSED(a4); UNUSED(a5);
    process_sleep(ms);
    return 0;
}

static int sys_time_handler(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5) {
    UNUSED(a1); UNUSED(a2); UNUSED(a3); UNUSED(a4); UNUSED(a5);
    return timer_get_seconds();
}

static int sys_malloc_handler(uint32_t size, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5) {
    UNUSED(a2); UNUSED(a3); UNUSED(a4); UNUSED(a5);
    return (int)kmalloc(size);
}

static int sys_free_handler(uint32_t ptr, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5) {
    UNUSED(a2); UNUSED(a3); UNUSED(a4); UNUSED(a5);
    kfree((void*)ptr);
    return 0;
}

static int sys_yield_handler(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5) {
    UNUSED(a1); UNUSED(a2); UNUSED(a3); UNUSED(a4); UNUSED(a5);
    process_yield();
    return 0;
}

static int sys_kill_handler(uint32_t pid, uint32_t sig, uint32_t a3, uint32_t a4, uint32_t a5) {
    UNUSED(sig); UNUSED(a3); UNUSED(a4); UNUSED(a5);
    return process_kill(pid);
}

/* Public syscall wrappers */
void sys_exit(int code) {
    syscall1(SYS_EXIT, code);
}

int sys_write(int fd, const void* buf, uint32_t count) {
    return syscall3(SYS_WRITE, fd, (uint32_t)buf, count);
}

int sys_read(int fd, void* buf, uint32_t count) {
    return syscall3(SYS_READ, fd, (uint32_t)buf, count);
}

int sys_open(const char* path, int flags) {
    return syscall2(SYS_OPEN, (uint32_t)path, flags);
}

int sys_close(int fd) {
    return syscall1(SYS_CLOSE, fd);
}

uint32_t sys_getpid(void) {
    return syscall0(SYS_GETPID);
}

void sys_sleep(uint32_t ms) {
    syscall1(SYS_SLEEP, ms);
}

uint32_t sys_time(void) {
    return syscall0(SYS_TIME);
}

void* sys_malloc(uint32_t size) {
    return (void*)syscall1(SYS_MALLOC, size);
}

void sys_free(void* ptr) {
    syscall1(SYS_FREE, (uint32_t)ptr);
}

void sys_yield(void) {
    syscall0(SYS_YIELD);
}

int sys_kill(uint32_t pid, int signal) {
    return syscall2(SYS_KILL, pid, signal);
}
