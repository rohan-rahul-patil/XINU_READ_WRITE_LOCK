#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <stdio.h>
#include<lock.h>

int lock(int ldes1, int type, int priority) {
        STATWORD ps;
        struct  lentry  *lptr;
        struct  pentry  *pptr;
	int lock = ldes1;
	int highest_priority, current_process_priority, lock_process_priority, i;
	

        disable(ps);
        if (isbadlock(lock) || ((lptr= &locks[lock])->lstate==LFREE) || (proctab[currpid].pwaitret == DELETED)) {
                restore(ps);
                return(SYSERR);
        }
	
	pptr = &proctab[currpid];
	if(pptr->pinh == 0) {
        	current_process_priority = pptr->pprio;
        }
       	else {
               	current_process_priority = pptr->pinh;
        }

	
	if (type == READ) {
		if(lptr->type == WRITE) {
			pptr->pstate = PRWAIT;

			priority_inheritance(lock, currpid);

        	        insert(currpid,lptr->lqhead, priority);
               		pptr->pwaitret = OK;
			pptr->wait_time = ctr1000;

			
			
			pptr->lock_type = type;	
		
			if(isempty(lptr->lqhead)) {
				lptr->lprio = current_process_priority;
			}
			else {
				if(current_process_priority > lptr->lprio)
					lptr->lprio = current_process_priority;
			}
		
			pptr->lock_id = lock;
                	resched();

			if(proctab[currpid].pwaitret == DELETED) {
				return SYSERR;
			}			
		}
		else if(lptr->type == READ) {
			if(nonempty(lptr->lqhead)) {
				highest_priority = lastkey(lptr->lqtail);
				if(priority < highest_priority) {
					pptr->pstate = PRWAIT;

					priority_inheritance(lock, currpid);

		                        insert(currpid,lptr->lqhead, priority);
                		        pptr->pwaitret = OK;
					pptr->wait_time = ctr1000;

			
					pptr->lock_type = type;

					 if(isempty(lptr->lqhead)) {
		                                lptr->lprio = current_process_priority;
                		        }
                       			 else {
                                		if(current_process_priority > lptr->lprio)
                                        		lptr->lprio = current_process_priority;
                       			}

					pptr->lock_id = lock;
                        		resched();

					if(proctab[currpid].pwaitret == DELETED) {
						return SYSERR;
					}
				}
			
  			}
		}
	}
	else if(type == WRITE) {
		if(lptr->type == READ || lptr->type == WRITE) {
			pptr->pstate = PRWAIT;
			priority_inheritance(lock, currpid);

               		insert(currpid,lptr->lqhead, priority);
	
        	        pptr->pwaitret = OK;
			pptr->wait_time = ctr1000;

		

			pptr->lock_type = type;

			if(isempty(lptr->lqhead)) {
				lptr->lprio = current_process_priority;
               		 }
                	else {
                		if(current_process_priority > lptr->lprio)
                        		lptr->lprio = current_process_priority;
               		 }
		
			pptr->lock_id = lock;

                	resched();

			if(proctab[currpid].pwaitret == DELETED) {
				return SYSERR;
			}
		}
	}

	lptr->process_held[currpid] = 1;
	lptr->type = type;		

	pptr->lock_type = NONE;
	pptr->locks_held[lock] = 1;	
	pptr->wait_time = 0;	
	pptr->lock_id = -1;	
        restore(ps);
	

        return(OK);
}


void priority_inheritance(int ldes, int pid) {
	int i;
	struct  lentry  *lptr;
        struct  pentry  *pptr;

	if(ldes==-1) {		
       		return;
	}
	
	pptr = &proctab[pid];
	lptr = &locks[ldes];

	for(i = 0; i < NPROC; i++) {

        	if(lptr->process_held[i] == 1) {                	
                	if(pptr->pinh > 0) {
                        	if(proctab[i].pinh > 0) {
                                	if(pptr->pinh > proctab[i].pinh)
                                        	proctab[i].pinh = pptr->pinh;
                        	}

                        	else {
                                	if(pptr->pinh > proctab[i].pprio)
                                        	proctab[i].pinh = pptr->pinh;

                        	}

                	}
                	else {
                        	if(proctab[i].pinh > 0) {
                                	if(pptr->pprio > proctab[i].pinh)
                                        	proctab[i].pinh = pptr->pprio;
                        	}
                        	else {
     		                	  if(pptr->pprio > proctab[i].pprio)
                                        	proctab[i].pinh = pptr->pprio;
                        	}

                	}			
                	priority_inheritance(proctab[i].lock_id,i);

        	}

	} 	
}
