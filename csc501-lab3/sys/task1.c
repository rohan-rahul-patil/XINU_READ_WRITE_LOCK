#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <lock.h>
#include <stdio.h>
#include <lock.h>


#define DEFAULT_LOCK_PRIO 20


void reader3 (char *msg, int lck)
{
        int     ret;
	kprintf ("  %s: to acquire lock\n", msg);
        lock (lck, READ, DEFAULT_LOCK_PRIO);
        kprintf ("  %s: acquired lock at time %d\n", msg, ctr1000);
        kprintf ("  %s: to release lock\n", msg);
	
        releaseall (1, lck);
}

void writer3 (char *msg, int lck)
{
        kprintf ("  %s: to acquire lock\n", msg);
        lock (lck, WRITE, DEFAULT_LOCK_PRIO);
        kprintf ("  %s: acquired lock, at time %d\n", msg, ctr1000);
        sleep (10);
        kprintf ("  %s: to release lock\n", msg);
        releaseall (1, lck);
}

void proc1 (char *msg, int sem) {
	int     ret;
        kprintf ("  %s: to acquire semaphore\n", msg);
        wait(sem);
        kprintf ("  %s: acquired semapore at time %d\n", msg, ctr1000);
	sleep(10);
        kprintf ("  %s: to release semaphore\n", msg);

        signal(sem);
}

void proc2 (char *msg, int sem) {
        int     ret;
        kprintf ("  %s: to acquire semaphore\n", msg);
        wait(sem);
        kprintf ("  %s: acquired semapore at time %d\n", msg, ctr1000);
        kprintf ("  %s: to release semaphore\n", msg);

        signal(sem);
}

void process (char *msg) {
	kprintf("process started at time %d", ctr1000);
	sleep(10);
	kprintf("process ended\n");
}

void test_lock()
{
        int     lck;
        int     rd1, rd2;
        int     wr1;

        kprintf("\nTest 3: test the basic priority inheritence\n");
        lck  = lcreate ();
        assert (lck != SYSERR, "Test 3 failed");

        rd2 = create(reader3, 2000, 30, "reader3", 2, "reader B", lck);
        wr1 = create(writer3, 2000, 20, "writer3", 2, "writer", lck);
	p = create(process, 2000, 25, "process", 1, 'P'); 
	
	kprintf("-start writer, then sleep 1s. lock granted to write (prio 20)\n");
        resume(wr1);
        sleep (1);

        kprintf("-start reader B, then sleep 1s. reader B(prio 30) blocked on the lock\n");
        resume (rd2);
	sleep (1);
	
	kprintf("-start process P, then sleep 1s, process P(prio 25)\n");
	resume(p);

        sleep (8);
}

void test_sem()
{
        int     sem;
        int     p1, p2;
        int     p;

        kprintf("\nTest 3: test the basic priority inheritence\n");
        sem  = screate ();
        assert (sem != SYSERR, "Test 3 failed");

        p = create(process, 2000, 25, "process", 1, 'P');
        p1 = create(process, 2000, 20, "process1", 1, "P1");
        p2 = create(process, 2000, 30, "process2", 1, "P2");

        kprintf("-start process1, then sleep 1s. semaphore granted to process1 (prio 20)\n");
        resume(p1);
        sleep (1);

        kprintf("-start process2, then sleep 1s. process2(prio 30) blocked on the semaphore\n");
        resume (p2);
        sleep (1);

        kprintf("-start process P, then sleep 1s, process P(prio 25)\n");
        resume(p);

        sleep (8);
}

int main( )
{
        /* These test cases are only used for test purpose.
 *          * The provided results do not guarantee your correctness.
 *                   * You need to read the PA2 instruction carefully.
 *                            */
	test_sem();
	test_lock();

        /* The hook to shutdown QEMU for process-like execution of XINU.
 *          * This API call exists the QEMU process.
 *                   */
        shutdown();
}
