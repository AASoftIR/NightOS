/*
 * NightOS - Process Management
 * 
 * Simple cooperative multitasking
 */

#ifndef PROCESS_H
#define PROCESS_H

#include "types.h"

/* Process constants */
#define MAX_PROCESSES       16
#define PROCESS_STACK_SIZE  4096
#define PROCESS_NAME_LEN    32

/* Process states */
typedef enum {
    PROC_STATE_FREE = 0,
    PROC_STATE_READY,
    PROC_STATE_RUNNING,
    PROC_STATE_BLOCKED,
    PROC_STATE_ZOMBIE
} proc_state_t;

/* Process priority */
typedef enum {
    PROC_PRIORITY_LOW = 0,
    PROC_PRIORITY_NORMAL,
    PROC_PRIORITY_HIGH,
    PROC_PRIORITY_REALTIME
} proc_priority_t;

/* CPU context for context switching */
typedef struct {
    uint32_t eax, ebx, ecx, edx;
    uint32_t esi, edi;
    uint32_t esp, ebp;
    uint32_t eip;
    uint32_t eflags;
} cpu_context_t;

/* Process Control Block (PCB) */
typedef struct {
    uint32_t pid;                       /* Process ID */
    char name[PROCESS_NAME_LEN];        /* Process name */
    proc_state_t state;                 /* Current state */
    proc_priority_t priority;           /* Priority level */
    cpu_context_t context;              /* Saved CPU context */
    uint8_t* stack;                     /* Stack pointer */
    uint32_t stack_size;                /* Stack size */
    uint32_t parent_pid;                /* Parent process ID */
    uint32_t created_time;              /* Creation timestamp */
    uint32_t cpu_time;                  /* Total CPU time used */
    void (*entry)(void);                /* Entry point function */
} process_t;

/* Process information for listing */
typedef struct {
    uint32_t pid;
    char name[PROCESS_NAME_LEN];
    proc_state_t state;
    uint32_t cpu_time;
} proc_info_t;

/* Initialize process manager */
void process_init(void);

/* Process creation/destruction */
int process_create(const char* name, void (*entry)(void), proc_priority_t priority);
void process_exit(int code);
int process_kill(uint32_t pid);

/* Process control */
void process_yield(void);
void process_sleep(uint32_t ms);
void process_block(uint32_t pid);
void process_unblock(uint32_t pid);

/* Process information */
process_t* process_current(void);
uint32_t process_getpid(void);
int process_list(proc_info_t* info, int max_count);
int process_count(void);

/* Scheduler */
void scheduler_init(void);
void scheduler_tick(void);
void schedule(void);

#endif /* PROCESS_H */
