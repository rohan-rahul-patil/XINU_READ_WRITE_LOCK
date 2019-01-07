#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <lock.h>
#include <stdio.h>
#include <lock.h>


#define DEFAULT_LOCK_PRIO 20

int t_sem_1;
int t_sem_2;
int t_lock_1;
int t_lock_2;

void reader3 (char *msg, int lck)
{
        int     ret;
        kprintf ("  %s: to acquire lock\n", msg);
        lock (lck, READ, DEFAULT_LOCK_PRIO);
        kprintf ("  %s: acquired lock at time %d\n", msg, t_lock_2 = ctr1000);
        kprintf ("  %s: to release lock\n", msg);

        releaseall (1, lck);
}

void writer3 (char *msg, int lck)
{
	int i, count;
        kprintf ("  %s: to acquire lock\n", msg);
        lock (lck, WRITE, DEFAULT_LOCK_PRIO);
        kprintf ("  %s: acquired lock, at time %d\n", msg, t_lock_1 = ctr1000);
        sleep (1);
	for(i = 0; i < 1000000000; i++) {
		count = count + 1;
	}
        kprintf ("  %s: to release lock\n", msg);
        releaseall (1, lck);
}

void proc1 (char *msg, int sem) {
        int     ret, i, count;
        kprintf ("  %s: to acquire semaphore\n", msg);
        wait(sem);
        kprintf ("  %s: acquired semapore at time %d\n", msg, t_sem_1 = ctr1000);
        sleep(1);
        kprintf ("  %s: to release semaphore\n", msg);
	for(i = 0; i < 1000000000; i++) {
		count = count + 1;
	}
        signal(sem);
}

void proc2 (char *msg, int sem) {
        int     ret;
        kprintf ("  %s: to acquire semaphore\n", msg);
        wait(sem);
        kprintf ("  %s: acquired semapore at time %d\n", msg, t_sem_2 = ctr1000);
        kprintf ("  %s: to release semaphore\n", msg);

        signal(sem);
}

void process (char *msg) {
        kprintf("process %s started at time %d\n", msg, ctr1000);
       	int i, count;
	for(i =0 ; i < 1000000000;i++) {
		count = count + 1;
	}
        kprintf("%s ended\n", msg);
}

void test_lock()
{
        int     lck;
        int     p, rd2;
        int     wr1;

        kprintf("\nTest 3: test the basic priority inheritence\n");
        lck  = lcreate ();


        rd2 = create(reader3, 2000, 30, "reader3", 2, "reader", lck);
        wr1 = create(writer3, 2000, 20, "writer3", 2, "writer", lck);
        p = create(process, 2000, 25, "process", 1, "M");

        kprintf("-start writer, then sleep 1s. lock granted to write (prio 20)\n");
        resume(wr1);
        sleep (1);

        kprintf("-start reader B, then sleep 1s. reader(prio 30) blocked on the lock\n");
        resume (rd2);
        sleep (1);

        kprintf("-start process P, then sleep 1s, process P(prio 25)\n");
        resume(p);
	sleep(10);

	kprintf("lock test done\n");
	kprintf("time required for reader to aquire lock = %d ms\n", t_lock_2 - t_lock_1);
	sleep(10);

}

void test_sem()
{
        int     sem;
        int     p1, p2;
        int     p;

        kprintf("\nTest 3: test the basic priority inheritence\n");
        sem  = screate (1);


        p = create(process, 2000, 25, "process", 1, "M");
        p1 = create(proc1, 2000, 20, "process1", 2, "P1", sem);
        p2 = create(proc2, 2000, 30, "process2", 2, "P2", sem);

        kprintf("-start process1, then sleep 1s. semaphore granted to P1 (prio 20)\n");
        resume(p1);
        sleep (1);

        kprintf("-start process2, then sleep 1s. P2(prio 30) blocked on the semaphore\n");
        resume (p2);
        sleep (1);

        kprintf("-start process P, then sleep 1s, process P(prio 25)\n");
        resume(p);

	sleep(10);	

	kprintf("semaphore test done\n");
	kprintf("time required for P2 to aquire lock = %d ms\n", t_sem_2 - t_sem_1);


	sleep(10);
}

int main( )
{
        /* These test cases are only used for test purpose.
 *  *          * The provided results do not guarantee your correctness.
 *   *                   * You need to read the PA2 instruction carefully.
 *    *                            */

	kprintf("\n\n___________________________semaphore test________________________________________________\n\n");
        test_sem();
	kprintf("\n\n___________________________lock test_____________________________________________________\n\n");
        test_lock();
	kprintf("\n\n");
        /* The hook to shutdown QEMU for process-like execution of XINU.
 *  *          * This API call exists the QEMU process.
 *   *                   */
        shutdown();
}

