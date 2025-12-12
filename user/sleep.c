#include "../kernel/types.h"
#include "user.h"


int main(int argc, char *argv[])
{
  if (argc < 2)
  {
    printf("USAGE: sleep <ticks>");
  }
  int ticks = atoi(argv[1]);

  pause(ticks); // -> Goes to usys.S (loads syscall id into a7, and calls ecall.)
                // ecall jumps to trampoline.S, which saves all user registers and jumps usertrap (trap.c)
                // trap.c saves user pc and moves it to next instruction and calls syscall() from syscall.c
                // syscall.c's syscall() function gets SYSCALL_ID from a7 register saved in the TRAPFRAME
                // It then gets the syscall according to syscall id and calls it (sys_pause) which jumps to sysproc.c
                // In sys_pause, the code acquires lock and then puts this process to SLEEPING state if less time
                // has passed (ticks).
                // The code then calls the scheduler and finds another process which is RUNNABLE to run
}
