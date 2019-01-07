/* kill.c - kill */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <sem.h>
#include <mem.h>
#include <io.h>
#include <q.h>
#include <stdio.h>
#include <lock.h>

/*------------------------------------------------------------------------
 * kill  --  kill a process and remove it from the system
 *------------------------------------------------------------------------
 */
SYSCALL kill(int pid)
{
	STATWORD ps;    
	struct	pentry	*pptr;		/* points to proc. table for pid*/
	int	dev;

	disable(ps);
	if (isbadpid(pid) || (pptr= &proctab[pid])->pstate==PRFREE) {
		restore(ps);
		return(SYSERR);
	}
	if (--numproc == 0)
		xdone();

	dev = pptr->pdevs[0];
	if (! isbaddev(dev) )
		close(dev);
	dev = pptr->pdevs[1];
	if (! isbaddev(dev) )
		close(dev);
	dev = pptr->ppagedev;
	if (! isbaddev(dev) )
		close(dev);
	
	send(pptr->pnxtkin, pid);

	freestk(pptr->pbase, pptr->pstklen);
	switch (pptr->pstate) {

	case PRCURR:	pptr->pstate = PRFREE;	/* suicide */
			resched();

	case PRWAIT:	semaph[pptr->psem].semcnt++;
			int i, pr, maxprio;
			struct  lentry  *lptr;

                        for(i = 0; i < NLOCK; i++) {
                                if(pptr->locks_held[i] == 1) {
                                        releaseall(1, i);
                                }
                        }
		
				
			lptr = &locks[pptr->lock_id];
                        dequeue(pid);
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
			
	case PRREADY:	dequeue(pid);
			pptr->pstate = PRFREE;
			break;

	case PRSLEEP:
	case PRTRECV:	unsleep(pid);
						/* fall through	*/
	default:	pptr->pstate = PRFREE;
	}
	restore(ps);
	return(OK);
}

void priority_deheritance(int ldes) {
	int i;
	struct  lentry  *lptr;
	
	if(ldes == -1) {
		return;
	}

	lptr = &locks[ldes];

	for(i = 0; i < NPROC; i++) {
		if(lptr->process_held[i] == 1) {
			if(lptr->lprio > proctab[i].pprio) {
				proctab[i].pinh = lptr->lprio;
			}
			else {
				proctab[i].pinh = 0;
			}
			priority_deheritance(proctab[i].lock_id);
		}
	}
}
