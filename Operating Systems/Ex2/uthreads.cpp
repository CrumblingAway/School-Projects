/**
 * @file uthreads.cpp
 *
 * @section DESCRIPTION
 * The implementation for Uthread library, to create and manage threads.
 */


// ------------------------------ Includes ------------------------------
#include "uthreads.h"
#include "Thread.h"
#include <vector>
#include <iostream>
#include <stdio.h>
#include <setjmp.h>
#include <signal.h>
#include <sys/time.h>
#include <map>

// ---------------------------- Magic Numbers ---------------------------
#define RUNNING 2
#define BLOCKED -1
#define TERMINATED -2
#define LONG_RET 1
#define ERROR -1
#define MAIN_ID 0
#define MAIN_PRIORITY 0
#define SCALE 1000000
#define SYS_ERR "system error: "
#define TIMER_ERR "setitimer failed"
#define SA_ERR "signal handler installation error"
#define ID_ERROR "No thread with id "
#define MEMORY_ERR "Memory allocation failed"
#define LIBRARY_ERR "thread library error: "
#define INIT_ERR "Bad init args given"
#define SPAWN_ERR "Could not create thread"
#define PRIORITY_ERR "Bad priority or id given"
#define TERMINATE_ERR "Could not terminate thread"
#define BLOCK_ERR "Could not block thread"
#define RESUME_ERR "Could not resume thread"
#define GETQ_ERR "Could not return quantums of this thread"

// -------------------------- Global Variables -------------------------
static int threadsCounter;
static std::map<int, Thread *> allThreads;
static std::vector<Thread *> readyThreads;
static int runningState;
static Thread *runningThread;
static bool takenIndices[MAX_THREAD_NUM];
int *quantumUsecs;
static int totalQuantums;
static int maxPriority;
struct itimerval timer;
struct sigaction sa;

// ------------------------------ Functions -----------------------------


/**
 * Free all library resources.
 */
void freeLibrary()
{
    for (auto pt : allThreads)
    {
        delete pt.second;
    }
    delete[] quantumUsecs;
}

/**
 * Find next available index for thead ID
 * @return thread ID if successful, otherwise -1
 */
int nextIndex()
{
    for (int i = 1; i < MAX_THREAD_NUM; ++i)
    {
        if (!takenIndices[i])
        {
            takenIndices[i] = true;
            return i;
        }
    }

    return -1;
}

/**
 * Sets the timer, according to given quantum (in useconds).
 */
void setTimer(int quantum)
{
    int sec = quantum / SCALE;
    int micSec = quantum - sec * SCALE;
    timer.it_value.tv_sec = sec;
    timer.it_value.tv_usec = micSec;
    timer.it_interval.tv_sec = sec;
    timer.it_interval.tv_usec = micSec;
    if (setitimer(ITIMER_VIRTUAL, &timer, nullptr))
    {
		sigprocmask(SIG_UNBLOCK, &sa.sa_mask, nullptr);
    	std::cerr << SYS_ERR << TIMER_ERR << std::endl;
    	freeLibrary();
    	exit(EXIT_FAILURE);
    }
}

/**
 * After pointing running thread to the appropriate thread increment thread's and
 * global quantum counters and set timer to quantum of thread
 */
void runThread()
{
	runningState = RUNNING;
    totalQuantums++;
	runningThread->incIteration();
    setTimer(quantumUsecs[runningThread->getPriority()]);
}

/**
 * Pop thread at the frount of readyThreads into runningThread
 */
void popFromReady()
{
    runningThread = readyThreads.front();
    readyThreads.erase(readyThreads.begin());
}

/**
 * Our signal handler, responsible for activating the thread, managing the ready threads list and
 * also sets timer to expire according to new thread's quantum.
 */
void switchThreads(int sig)
{
	// Only main thread exists, no action necessary
    if (threadsCounter == 1)
    {
        return;
    }
    // Quantum expired "naturally"
    if (runningState == RUNNING)
    {
        readyThreads.push_back(runningThread);
    }
    // Only save context is running thread still exists
    if (runningState != TERMINATED)
    {
        if (sigsetjmp(runningThread->getSjb(), LONG_RET))
        {
            return;
        }

    }
    popFromReady();
    runThread();
	siglongjmp(runningThread->getSjb(), LONG_RET);
}


/**
 * Creates a thread, stores it in threads array and add it to ready threads queue.
 */
void createThread(int idx, int priority, void (*f)())
{
    auto *t = new(std::nothrow) Thread(idx, priority, STACK_SIZE, f);
    if (t == nullptr)
    {
        std::cerr << SYS_ERR << MEMORY_ERR << std::endl;
        freeLibrary();
        exit(EXIT_FAILURE);
    }
    allThreads.insert(std::pair<int, Thread *>(idx, t));
    takenIndices[idx] = true;
    readyThreads.push_back(t);
    threadsCounter++;
}


/**
 * Sets the handler function of SIGVTALRM to switchThreads.
 */
int setSignalHandler()
{
    sa.sa_handler = &switchThreads;
    if (sigemptyset(&sa.sa_mask) == ERROR)
    {
        return ERROR;
    }
    if (sigaddset(&sa.sa_mask, SIGVTALRM) == ERROR)
    {
        return ERROR;
    }
    if (sigaction(SIGVTALRM, &sa, nullptr) < 0)
    {
        printf("sigaction error."); //TODO: see requirement for error in case of handling SIGVTALRM
        return ERROR;
    }
    return EXIT_SUCCESS;
}

/**
 * Initialize all global variables.
 */
void initGlobalVars(const int *quantum_usecs, int size)
{
    threadsCounter = 0;
    totalQuantums = 1;
    maxPriority = size - 1;
    quantumUsecs = new(std::nothrow) int[size];
    if (quantumUsecs == nullptr)
    {
        std::cerr << SYS_ERR << MEMORY_ERR << std::endl;
        freeLibrary();
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < size; i++)
    {
        quantumUsecs[i] = quantum_usecs[i];
    }
    for (bool &index : takenIndices)
    {
        index = false;
    }
}

/**
 * Removes the Thread with id tid from ready queue.
 */
void removeFromReady(int tid)
{
    for (int i = 0; i < (int) readyThreads.size(); i++)
    {
        if (readyThreads[i]->getId() == tid)
        {
            readyThreads.erase(readyThreads.begin() + i);
        }
    }
}

/**
 * Delete thread from all data structures
 * @param tid
 * @return
 */
int deleteThread(int tid)
{
	if(!takenIndices[tid])
	{
		std::cerr << ID_ERROR << tid << std::endl;
		return ERROR;
	}
	removeFromReady(tid);
	delete allThreads[tid];
	allThreads.erase(tid);
	takenIndices[tid] = false;
	threadsCounter--;
	return EXIT_SUCCESS;
}

int uthread_init(int *quantum_usecs, int size)
{
    if (size <= 0)
    {
        std::cerr << LIBRARY_ERR << INIT_ERR << std::endl;
        return ERROR;
    }
    for (int i = 0; i < size; i++)
    {
        if (quantum_usecs[i] <= 0)
        {
            std::cerr << LIBRARY_ERR << INIT_ERR << std::endl;
            return ERROR;
        }
    }
    
    initGlobalVars(quantum_usecs, size);
    
    // Initialize main thread
    createThread(MAIN_ID, MAIN_PRIORITY, nullptr);
	allThreads[MAIN_ID]->incIteration();
    runningThread = allThreads[MAIN_ID];
    runningState = RUNNING;
    readyThreads.erase(readyThreads.begin());
    
    // Initialize signal handler and start timer
    if (setSignalHandler() == ERROR)
    {
        freeLibrary();
        std::cerr << SYS_ERR << SA_ERR << std::endl;
        exit(EXIT_FAILURE);
    }
    setTimer(quantumUsecs[MAIN_PRIORITY]);
    
    return EXIT_SUCCESS;
}


int uthread_spawn(void (*f)(void), int priority)
{
    if (threadsCounter == MAX_THREAD_NUM || (priority < 0 || priority > maxPriority))
    {
        std::cerr << LIBRARY_ERR << SPAWN_ERR << std::endl;
        return ERROR;
    }
    sigprocmask(SIG_BLOCK, &sa.sa_mask, nullptr);
    int idx = nextIndex();
    createThread(idx, priority, f);
    sigprocmask(SIG_UNBLOCK, &sa.sa_mask, nullptr);
    return idx;
}


int uthread_change_priority(int tid, int priority)
{
    if (priority > maxPriority || priority < 0 || tid < 0 || tid > MAX_THREAD_NUM - 1 || !takenIndices[tid])
    {
        std::cerr << LIBRARY_ERR << PRIORITY_ERR << std::endl;
        return ERROR;
    }
    sigprocmask(SIG_BLOCK, &sa.sa_mask, nullptr);
    allThreads[tid]->setPriority(priority);
    sigprocmask(SIG_UNBLOCK, &sa.sa_mask, nullptr);
    return EXIT_SUCCESS;
}


int uthread_terminate(int tid)
{
    if (tid < 0 || tid > MAX_THREAD_NUM - 1 || !takenIndices[tid])
    {
        std::cerr << LIBRARY_ERR << TERMINATE_ERR << std::endl;
        return ERROR;
    }
    sigprocmask(SIG_BLOCK, &sa.sa_mask, nullptr);
    if (tid == MAIN_ID)
    {
		sigprocmask(SIG_UNBLOCK, &sa.sa_mask, nullptr);
        freeLibrary();
        exit(EXIT_SUCCESS);
    }
    if (runningThread->getId() == tid) //running thread terminates itself
    {
        runningState = TERMINATED;
        popFromReady();
        runThread();
        deleteThread(tid);
        sigprocmask(SIG_UNBLOCK, &sa.sa_mask, nullptr);
        siglongjmp(runningThread->getSjb(), tid);
    }
	deleteThread(tid);
    sigprocmask(SIG_UNBLOCK, &sa.sa_mask, nullptr);
    return EXIT_SUCCESS;
}

int uthread_block(int tid)
{
    if (tid <= 0 || tid > MAX_THREAD_NUM - 1 || !takenIndices[tid])
    {
        std::cerr << LIBRARY_ERR << BLOCK_ERR << std::endl;
        return ERROR;
    }
    sigprocmask(SIG_BLOCK, &sa.sa_mask, nullptr);
	allThreads[tid]->setBlocked(true);
    if (runningThread->getId() == tid) //if running thread blocked itself
    {
        runningState = BLOCKED;
        switchThreads(0);
    }
    removeFromReady(tid);
    sigprocmask(SIG_UNBLOCK, &sa.sa_mask, nullptr);
    return EXIT_SUCCESS;
}

int uthread_resume(int tid)
{
    if (tid < 0 || tid > MAX_THREAD_NUM - 1 || !takenIndices[tid])
    {
        std::cerr << LIBRARY_ERR << RESUME_ERR << std::endl;
        return ERROR;
    }
    sigprocmask(SIG_BLOCK, &sa.sa_mask, nullptr);
    if (allThreads[tid]->getBlocked())
    {
		allThreads[tid]->setBlocked(false);
        readyThreads.push_back(allThreads[tid]);
    }
    sigprocmask(SIG_UNBLOCK, &sa.sa_mask, nullptr);
    return EXIT_SUCCESS;
}

int uthread_get_tid()
{
    return runningThread->getId();
}

int uthread_get_total_quantums()
{
    return totalQuantums;
}

int uthread_get_quantums(int tid)
{
    sigprocmask(SIG_BLOCK, &sa.sa_mask, nullptr);
    if (tid < 0 || tid > MAX_THREAD_NUM - 1 || !takenIndices[tid])
    {
        std::cerr << LIBRARY_ERR << GETQ_ERR << std::endl;
        return ERROR;
    }
    int q = allThreads[tid]->getIteration();
    sigprocmask(SIG_UNBLOCK, &sa.sa_mask, nullptr);
    return q;
}

