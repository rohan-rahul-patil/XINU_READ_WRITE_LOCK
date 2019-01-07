#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <stdio.h>
#include<lock.h>

SYSCALL lcreate()
{
        STATWORD ps;
        int     lock;

        disable(ps);
        if ((lock = newlock())==SYSERR) {
                restore(ps);
                return(SYSERR);
        }
        
        restore(ps);
        return(lock);
}


int newlock()
{
        int     lock;
        int     i;

        for (i=0 ; i<NLOCK ; i++) {
                lock=nextlock--;
                if (nextlock < 0)
                        nextlock = NLOCK-1;
                if (locks[lock].lstate==LFREE) {
                        locks[lock].lstate = LUSED;
                        return(lock);
                }
        }
        return(SYSERR);
}


