#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <stdio.h>
#include<lock.h>


SYSCALL ldelete(int lock)
{
        STATWORD ps;
        int     pid;
        struct  lentry  *lptr;


        disable(ps);
        if (isbadlock(lock) || locks[lock].lstate==LFREE) {
                restore(ps);
                return(SYSERR);
        }
        lptr = &locks[lock];
        lptr->lstate = LFREE;
        if (nonempty(lptr->lqhead)) {
                while( (pid=getfirst(lptr->lqhead)) != EMPTY)
                  {
                    proctab[pid].pwaitret = DELETED;
                    ready(pid,RESCHNO);
                  }
                resched();
        }
        restore(ps);
        return(DELETED);
}

