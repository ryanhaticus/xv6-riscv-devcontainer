#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

// Including the MLFQProcInfo structure from `mlfq.h`, as required by Project 1C.
#include "mlfq.h"

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return wait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int n;

  argint(0, &n);
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  argint(0, &n);
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

// Determines whether the MLFQ scheduler should be ran (1) over the Round Robin scheduler (0), as required by Project 1C.
// External declaration. The initialization falls in `proc.c` of the kernel.
extern int mlfqSchedulerEnabled;

// Determines the number of levels in the MLFQ scheduler, as required by Project 1C.
// External declaration. The initialization falls in `proc.c` of the kernel.
extern int mlfqNumLevels;

// Determines the maximum number of ticks a process can run at the bottom level of the MLFQ scheduler (level m-1), as required by Project 1C.
// External declaration. The initialization falls in `proc.c` of the kernel.
extern int mlfqMaxTicks;

// Enables the MLFQ scheduler, as required by Project 1C.
uint64 sys_startMLFQ(void) {
  if(mlfqSchedulerEnabled == 1) {
    return -1;
  }

  int numLevel;
  argint(0, &numLevel);

  if(numLevel < 1) {
    return -1;
  }

  int maxTicks;
  argint(1, &maxTicks);

  if(maxTicks < 1) {
    return -1;
  }

  mlfqSchedulerEnabled = 1;
  mlfqNumLevels = numLevel;
  mlfqMaxTicks = maxTicks;
  return 0;
}

// Disables the MLFQ scheduler, as required by Project 1C.
uint64 sys_stopMLFQ(void) {
  if(mlfqSchedulerEnabled == 0) {
    return -1;
  }

  mlfqSchedulerEnabled = 0;
  return 0;
}

// This function is used to get the MLFQ information of the current process.
// Implemented as required by Project 1C.
uint64 sys_getMLFQInfo(void) {
  uint64 arg_addr;

  argaddr(0, &arg_addr);

  struct proc *p = myproc();

  if(copyout(
    p->pagetable,
    arg_addr,
    (char *)&(p->mlfqInfo.report),
    sizeof(struct MLFQInfoReport)
  ) < 0) {
    return -1;
  }

  return 0;
}