#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <stdio.h>
#include<lock.h>

int releaseall (int numlocks, int ldes1, ...) {
	STATWORD ps;
        struct  lentry  *lptr;
        struct  pentry  *pptr;

	int lock, error = 0, lock_in_use = 0, i;
	unsigned long *a = (unsigned long *) (&ldes1);
	int w, pr;
	int maxprio, hprio, x, max_id, max_wait_time, write_present;

	pptr = &proctab[currpid];

	while (numlocks--) {
		lock = *a++;
		
		disable(ps);
	        if (isbadlock(lock) || (lptr= &locks[lock])->lstate==LFREE) {
                	error = 1;
			continue;
       		 }
		
		if(pptr->locks_held[lock] == 1) {
			pptr->locks_held[lock] = 0;
			
			lptr->process_held[currpid] = 0;
			pptr->lock_type = NONE;

			/* set new priority for the process */
			maxprio = -1;
			for(i = 0; i < NLOCK; i++) {
				if(pptr->locks_held[i] == 1) {
					if(locks[i].lprio > maxprio) {
						maxprio = locks[i].lprio;
					}
				}
			}

			if(maxprio > 0) {
				pptr->pinh = maxprio;
			}
			else {
				pptr->pinh = 0;
			}

			/* ready waiting processes based on the locking policy */
			if(nonempty(lptr->lqhead)) {
				for(i = 0; i < NPROC; i++) {
					if(lptr->process_held[i] == 1){
						lock_in_use = 1;
					}
				}	
				
				if(lock_in_use == 0) {
					hprio = lastkey(lptr->lqtail);
					pr = q[lptr->lqtail].qprev;
					max_wait_time = -1;
					max_id = -1;
                                        while(pr != lptr->lqhead && q[pr].qkey == hprio){
                                        	w = ctr1000 - proctab[pr].wait_time;
                                                if(w > max_wait_time) {
                                                	max_wait_time = w;
                                                        max_id = pr;
                                                }       
                                                pr = q[pr].qprev;
                                        }
					
					w = ctr1000 - proctab[q[lptr->lqtail].qprev].wait_time;
					write_present = 0; 
					if(max_wait_time - w < 1000) {
						pr = q[lptr->lqtail].qprev;
                	                        while(pr != max_id){
                                	                if(proctab[pr].lock_type == WRITE) {
                                        	                ready(dequeue(pr), RESCHYES);
								write_present = 1;
								break;
                                                        }
							pr = q[pr].qprev;
                                                }
                                        }
					if(write_present == 0) {
						ready(dequeue(max_id), RESCHYES);
					}

					lptr->type = NONE;
				}

			}
			if(nonempty(lptr->lqhead)) {
				maxprio = -1;
				pr = q[lptr->lqhead].qnext;
                        	while(pr != lptr->lqtail){
                                	if(proctab[pr].pinh == 0) {
                                        	if(proctab[pr].pprio > maxprio) {
							maxprio = proctab[pr].pprio;
						}	
					}
					else {
						if(proctab[pr].pinh > maxprio) {
							maxprio = proctab[pr].pinh;
						}
                                       	 }
                               		pr = q[pr].qnext;
                       		}

				lptr->lprio = maxprio;
			}
			
			else {
				lptr->lprio = -1;
			}
				
		}		
		else{
			error = 1;
			continue;
		}
	}	

        if(error == 1) {
		return SYSERR;
	}

        restore(ps);
        return(OK);
}		
