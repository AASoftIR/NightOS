/*
 * NightOS - Process Management Implementation
 * 
 * Simple cooperative multitasking system
 */

#include "../include/process.h"
#include "../include/memory.h"
#include "../include/string.h"
#include "../include/timer.h"
#include "../include/vga.h"

/* Process table */
static process_t processes[MAX_PROCESSES];
static uint32_t current_pid = 0;
static uint32_t next_pid = 1;
static bool scheduler_enabled = false;

/* Initialize process manager */
void process_init(void) {
    memset(processes, 0, sizeof(processes));
    
    /* Create kernel process (PID 0) */
    processes[0].pid = 0;
    strcpy(processes[0].name, "kernel");
    processes[0].state = PROC_STATE_RUNNING;
    processes[0].priority = PROC_PRIORITY_REALTIME;
    processes[0].created_time = timer_get_seconds();
    processes[0].parent_pid = 0;
    
    current_pid = 0;
    next_pid = 1;
}

/* Find free process slot */
static int find_free_slot(void) {
    for (int i = 1; i < MAX_PROCESSES; i++) {
        if (processes[i].state == PROC_STATE_FREE) {
            return i;
        }
    }
    return -1;
}

/* Find process by PID */
static process_t* find_process(uint32_t pid) {
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (processes[i].pid == pid && processes[i].state != PROC_STATE_FREE) {
            return &processes[i];
        }
    }
    return NULL;
}

/* Create a new process */
int process_create(const char* name, void (*entry)(void), proc_priority_t priority) {
    int slot = find_free_slot();
    if (slot < 0) return -1;
    
    process_t* proc = &processes[slot];
    
    /* Allocate stack */
    proc->stack = (uint8_t*)kmalloc(PROCESS_STACK_SIZE);
    if (!proc->stack) return -2;
    
    /* Initialize PCB */
    proc->pid = next_pid++;
    strncpy(proc->name, name, PROCESS_NAME_LEN - 1);
    proc->name[PROCESS_NAME_LEN - 1] = '\0';
    proc->state = PROC_STATE_READY;
    proc->priority = priority;
    proc->stack_size = PROCESS_STACK_SIZE;
    proc->parent_pid = current_pid;
    proc->created_time = timer_get_seconds();
    proc->cpu_time = 0;
    proc->entry = entry;
    
    /* Set up initial context */
    proc->context.esp = (uint32_t)(proc->stack + PROCESS_STACK_SIZE - 4);
    proc->context.ebp = proc->context.esp;
    proc->context.eip = (uint32_t)entry;
    proc->context.eflags = 0x202;  /* IF flag set */
    
    return proc->pid;
}

/* Exit current process */
void process_exit(int code) {
    UNUSED(code);
    
    process_t* proc = &processes[current_pid];
    if (proc->pid == 0) return;  /* Can't exit kernel */
    
    proc->state = PROC_STATE_ZOMBIE;
    
    /* Free stack */
    if (proc->stack) {
        kfree(proc->stack);
        proc->stack = NULL;
    }
    
    schedule();
}

/* Kill a process */
int process_kill(uint32_t pid) {
    if (pid == 0) return -1;  /* Can't kill kernel */
    
    process_t* proc = find_process(pid);
    if (!proc) return -2;
    
    proc->state = PROC_STATE_ZOMBIE;
    
    if (proc->stack) {
        kfree(proc->stack);
        proc->stack = NULL;
    }
    
    /* Clean up zombie */
    memset(proc, 0, sizeof(process_t));
    
    return 0;
}

/* Yield to scheduler */
void process_yield(void) {
    schedule();
}

/* Sleep for milliseconds */
void process_sleep(uint32_t ms) {
    /* Simple busy-wait sleep for now */
    msleep(ms);
}

/* Block a process */
void process_block(uint32_t pid) {
    process_t* proc = find_process(pid);
    if (proc && proc->state == PROC_STATE_RUNNING) {
        proc->state = PROC_STATE_BLOCKED;
    }
}

/* Unblock a process */
void process_unblock(uint32_t pid) {
    process_t* proc = find_process(pid);
    if (proc && proc->state == PROC_STATE_BLOCKED) {
        proc->state = PROC_STATE_READY;
    }
}

/* Get current process */
process_t* process_current(void) {
    return &processes[current_pid];
}

/* Get current PID */
uint32_t process_getpid(void) {
    return processes[current_pid].pid;
}

/* List processes */
int process_list(proc_info_t* info, int max_count) {
    int count = 0;
    
    for (int i = 0; i < MAX_PROCESSES && count < max_count; i++) {
        if (processes[i].state != PROC_STATE_FREE) {
            info[count].pid = processes[i].pid;
            strncpy(info[count].name, processes[i].name, PROCESS_NAME_LEN);
            info[count].state = processes[i].state;
            info[count].cpu_time = processes[i].cpu_time;
            count++;
        }
    }
    
    return count;
}

/* Count active processes */
int process_count(void) {
    int count = 0;
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (processes[i].state != PROC_STATE_FREE) {
            count++;
        }
    }
    return count;
}

/* Initialize scheduler */
void scheduler_init(void) {
    scheduler_enabled = true;
}

/* Scheduler tick (called from timer interrupt) */
void scheduler_tick(void) {
    if (!scheduler_enabled) return;
    
    /* Update CPU time for current process */
    processes[current_pid].cpu_time++;
}

/* Simple round-robin scheduler */
void schedule(void) {
    if (!scheduler_enabled) return;
    
    /* Mark current as ready if running */
    if (processes[current_pid].state == PROC_STATE_RUNNING) {
        processes[current_pid].state = PROC_STATE_READY;
    }
    
    /* Find next ready process (round-robin) */
    int next = current_pid;
    for (int i = 0; i < MAX_PROCESSES; i++) {
        next = (next + 1) % MAX_PROCESSES;
        if (processes[next].state == PROC_STATE_READY) {
            break;
        }
    }
    
    /* Switch to next process */
    current_pid = next;
    processes[current_pid].state = PROC_STATE_RUNNING;
    
    /* Context switch would happen here in a real implementation */
}
