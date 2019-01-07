/* lock.h - isbadlock */

#ifndef _LOCK_H_
#define _LOCK_H_

#define READ	1
#define WRITE	2 
#define NONE	-1

#define NLOCK            50      

/*#ifndef NPROC            
#define NPROC           30 
#endif*/


#define LFREE   '\01'           /* this lock is free               */
#define LUSED   '\02'           /* this lock is used               */

struct  lentry  {               	/* lcok table entry                */
        char    lstate;         	/* the state SFREE or SUSED             */
        int     type;         		/* read or write            */
        int     lqhead;       	  	/* q index of head of list              */
        int     lqtail;         	/* q index of tail of list              */
	int	lprio;			/* maximum priority of all processes waiting in locks wating list */
	int	process_held[NPROC];	/* processes holding this lock*/

};

extern  struct  lentry  locks[];
extern  int     nextlock;


void linit();
SYSCALL lcreate();
int newlock();
int lock(int ldes1, int type, int priority);
void priority_inheritance(int ldes, int pid);
int releaseall (int numlocks, int ldes1, ...);
SYSCALL ldelete(int lock);
void priority_deheritance(int ldes);

#define isbadlock(l)     (l<0 || l>=NLOCK)

#endif

