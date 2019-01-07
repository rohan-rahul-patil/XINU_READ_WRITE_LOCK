#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <stdio.h>
#include<lock.h>


void linit() {
	int i, j;
	struct  lentry  *lptr;


	for (i=0 ; i<NLOCK ; i++) {
                (lptr = &locks[i])->lstate = LFREE;
               	lptr->lqtail = 1 + (lptr->lqhead = newqueue());
		lptr->lprio = -1;
		lptr->type = NONE; 
		for(j = 0; j < NPROC; j++) {
			lptr->process_held[j] = 0;
		}
        }
 }
