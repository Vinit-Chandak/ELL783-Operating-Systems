#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "syscall.h"

extern void print_count(void);
extern void toggle(void);
extern int add(int, int);
extern void ps();
extern int send(int, int, void *);
extern int recv(void *);
extern int send_multi(int, int, void *);

enum trace_state{TRACE_OFF=0, TRACE_ON=1};
int trace_flag = TRACE_OFF; 
int syscallcount[29];				// array to store the invocation count of different syscalls, size = no of syscalls + 1 as 0 is not assigned to any
int totalSysCalls = 28;				//total number of syscalls in the system(at the time of writing trace, changed afterwards, make necessary chaneges everywhere if you want to update this

//array used to print syscall names
static char* syscallnames[] = {
[SYS_fork]    "sys_fork",
[SYS_exit]    "sys_exit",
[SYS_wait]    "sys_wait",
[SYS_pipe]    "sys_pipe",
[SYS_read]    "sys_read",
[SYS_kill]    "sys_kill",
[SYS_exec]    "sys_exec",
[SYS_fstat]   "sys_fstat",
[SYS_chdir]   "sys_chdir",
[SYS_dup]     "sys_dup",
[SYS_getpid]  "sys_getpid",
[SYS_sbrk]    "sys_sbrk",
[SYS_sleep]   "sys_sleep",
[SYS_uptime]  "sys_uptime",
[SYS_open]    "sys_open",
[SYS_write]   "sys_write",
[SYS_mknod]   "sys_mknod",
[SYS_unlink]  "sys_unlink",
[SYS_link]    "sys_link",
[SYS_mkdir]   "sys_mkdir",
[SYS_close]   "sys_close",
[SYS_print_count]	"sys_print_count",
[SYS_toggle]	"sys_toggle",
[SYS_add]		"sys_add",
[SYS_ps]		"sys_ps",
[SYS_send]		"sys_send",
[SYS_recv]		"sys_recv",
[SYS_send_multi]	"sys_send_multi",
};

//array used to print the syscall names alphabetically
static int alphabeticalsyscallmap[] = {
SYS_add, SYS_chdir, SYS_close, SYS_dup, SYS_exec, SYS_exit, SYS_fork, SYS_fstat, SYS_getpid, SYS_kill, SYS_link, SYS_mkdir, SYS_mknod, SYS_open, SYS_pipe, SYS_ps, SYS_read, SYS_recv, SYS_sbrk, SYS_send, SYS_send_multi, SYS_sleep, SYS_unlink, SYS_uptime, SYS_wait, SYS_write
};
int alphaSize = 26;	//size of the alphabeticalsyscallmap[] array, no of elem + 1

extern int (*syscalls[])(void);

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
  return myproc()->pid;
}

int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
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
    if(myproc()->killed){
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


void
sys_print_count(void){
	//loops the alphabeticalsyscallmap array
	for(int i=0; i<alphaSize; i++){
		if(syscallcount[alphabeticalsyscallmap[i]]!=0)
			cprintf("%s %d \n", syscallnames[alphabeticalsyscallmap[i]], syscallcount[alphabeticalsyscallmap[i]]);
	}
	return;
}

void
sys_toggle(void){
	if(trace_flag){
		trace_flag=TRACE_OFF;
		//clear the sys call counts after toggle from TRACE_ON to TRACE_OFF
		for(int i=1; i<totalSysCalls; i++){
  			syscallcount[i]=0;
  		}
	}
	else{
		trace_flag=TRACE_ON;
	}
	return;
}

int 
sys_add(void){
	int firstNum, secondNum;
	argint(0, &firstNum);
	argint(1, &secondNum);
	return add(firstNum, secondNum);	
}

void
sys_ps(void){
	ps();
	return;
}

int
sys_send(void){
	int s_id, r_id;
	char *message;
	
	if(argint(0, &s_id) < 0 || argint(1, &r_id) < 0 || argptr(2, &message, sizeof(char*)) < 0)
		return -1;
	return send(s_id, r_id, message);
}

int
sys_recv(void){
	char *message;
	
	if(argptr(0, &message, sizeof(char*)) < 0)
		return -1;
	//cprintf("helo");
	return recv(message);
}


int
sys_send_multi(void){
	int s_id;
	int *r_id;
	char *message;
	
	//argptr: Fetch the nth word−sized system call argument as a pointer
	// to a block of memory of size bytes.
	
	if(argint(0, &s_id) < 0 || argptr(1, (void *)&r_id, sizeof(int *)) < 0 || argstr(2, &message) < 0)
		return -1;
	//cprintf("lmao");
	int length = 8; //no of recv processes
	
	r_id = (int *)r_id;
	for(int i=0; i<length; i++){
		//cprintf("message sent to %d is %s \n", (int)r_id[i], message);
		send(s_id, (int)(r_id[i]), message);
		//cprintf("message sent to %d is %s\n", (int)r_id[i], message);
	}
	
	return 0;
}
	
