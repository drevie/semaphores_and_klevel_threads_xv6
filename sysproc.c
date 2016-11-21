#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"

int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return proc->pid;
}

int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = proc->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

int
sys_sleep(void)
{
  int n;
  uint ticks0;
  
  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(proc->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;
  
  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

// BEGIN CHANGES:

int sys_sem_init(void){
  int semId,n;

  // make sure sem is valid
  if(argint(0, &semId) < 0) 
    return -1;
  if(argint(1, &n) < 0) 
    return -1;

  return sem_init(semId,n);
}

int sys_sem_destroy(void){
  int semId;

  // make sure sem is valid
  if(argint(0,&semId) < 0)
    return -1;

  return sem_destroy(semId);
}

int sys_sem_wait(void){
  int semId;

  // make sure sem is valid
  if(argint(0,&semId) < 0) 
    return -1;

  return sem_wait(semId);
}

int sys_sem_signal(void){
  int semId;

  // make sure sem is valid  
  if(argint(0,&semId) < 0) 
    return -1;

  return sem_signal(semId);
}

int sys_clone(void){

  void *x, *y,*stack;
  int temp_val;

  // make sure sem is valid
  if(argint(0, &temp_val) < 0) 
    return -1;

  // make sure sem is valid
  x = (void *)temp_val;
  if(argint(1, &temp_val) < 0) 
    return -1;

  // make sure sem is valid
  y = (void *)temp_val;
  if(argint(2, &temp_val) < 0) 
    return -1;

  // set stack
  stack = (void *)temp_val;

  return clone(x,y,stack);
}



int sys_texit(void){
  void *retval;
  int temp_val;


  if(argint(0,&temp_val) < 0) 
    return -1;

  retval = (void*)temp_val;
  texit(retval);
  
  return 0;
}


int sys_join(void){
  void ** stack,**retval;
  int pid,temp;
  if(argint(0,&pid) < 0) return -1;
  if(argint(1,&temp) < 0) return -1;
  stack = (void **)temp;
  if(argint(2,&temp) < 0) return -1;
  retval = (void **)temp;
  return join(pid,stack,retval);
}
// CHANGES END


// Halt (shutdown) the system by sending a special
// signal to QEMU.
// Based on: http://pdos.csail.mit.edu/6.828/2012/homework/xv6-syscall.html
// and: https://github.com/t3rm1n4l/pintos/blob/master/devices/shutdown.c
int
sys_halt(void)
{
  char *p = "Shutdown";
  for( ; *p; p++)
    outw(0xB004, 0x2000);
  return 0;
}
