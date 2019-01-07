/* chprio.c - chprio */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <stdio.h>
#include <lock.h>

/*------------------------------------------------------------------------
 * chprio  --  change the scheduling priority of a process
 *------------------------------------------------------------------------
 */
SYSCALL chprio(int pid, int newprio)
{
	STATWORD ps;    
	struct	pentry	*pptr;
	struct lentry *lptr;
	int pr, maxprio;

	disable(ps);
	if (isbadpid(pid) || newprio<=0 ||
	    (pptr = &proctab[pid])->pstate == PRFREE) {
		restore(ps);
		return(SYSERR);
	}
	pptr->pprio = newprio;

	lptr = &locks[pptr->lock_id];
	if(nonempty(lptr->lqhead)) {
        	maxprio = -1;
                pr = q[lptr->lqhead].qnext;
               	while(pr != lptr->lqtail) {
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

       	priority_deheritance(pptr->lock_id);


	restore(ps);
	if(pptr->pinh > 0) {
		return pptr->pinh;
	}
	return(newprio);
}
