#include "threads.h"

static int currentThread = -1;
static int nThreads = 0;
static TCB threadList[MAXTHREADS];
static int ran = 0;

void initStatistics(TCB::statistics* stat, int id) {
    assert(stat != nullptr);
    stat->ID = id;
    stat->state = TCB::DED;
    stat->burst = 0;
    stat->total_exec_time = 0;
    stat->total_slp_time = 0;
    stat->avg_time_quant = 0;
    stat->avg_wait_time = 0;
    stat->RedTimeTotal = 0;
}

void deleteThread(int threadID) {
    nThreads--;
    threadList[threadID].stat.state = TCB::DED;
    cout << "Thread with ID " << threadID << " deleted" << endl;
}

DWORD WINAPI threadWrapper(LPVOID lpParam) {
    TCB* tcb = static_cast<TCB*>(lpParam);
    if (tcb->tType == 1) {
        tcb->retVal = tcb->f2(tcb->args);
    } else {
        tcb->f1();
    }
    cout << "Thread " << tcb->stat.ID << " exited" << endl;
    deleteThread(tcb->stat.ID);
    return 0;
}

int createWithArgs(void *(*f)(void *), void *args) {
    int id = -1;
    if (ran == 0) {
        for (int i = 0; i < MAXTHREADS; i++)
            initStatistics(&(threadList[i].stat), i);
        ran = 1;
    }
    for (int i = 0; i < MAXTHREADS; i++) {
        if (threadList[i].stat.state == TCB::DED) {
            id = i;
            break;
        }
    }
    if (id == -1) {
        cerr << "Error: Cannot allocate ID" << endl;
        return id;
    }
    assert(id >= 0 && id < MAXTHREADS);
    threadList[id].tType = 1;
    threadList[id].f2 = f;
    threadList[id].args = args;
    threadList[id].stat.state = TCB::RED;
    threadList[id].context.ContextFlags = CONTEXT_ALL;

    #ifdef _WIN64
    threadList[id].context.Rip = (DWORD64)threadWrapper;
    #else
    threadList[id].context.Eip = (DWORD)threadWrapper;
    #endif
    #ifdef _WIN64
    threadList[id].context.Rsp = (DWORD64)(threadList[id].stack + STACK_SIZE);
    #else
    threadList[id].context.Esp = (DWORD)(threadList[id].stack + STACK_SIZE);
    #endif
    #ifdef _WIN64
    threadList[id].context.Rbp = (DWORD64)(threadList[id].stack + STACK_SIZE);
    #else
    threadList[id].context.Ebp = (DWORD)(threadList[id].stack + STACK_SIZE);
    #endif

    threadList[id].context.ContextFlags = CONTEXT_FULL;
    nThreads++;
    CreateThread(NULL, 0, threadWrapper, &threadList[id], 0, NULL);
    return id;
}

int create(void (*f)(void)) {
    int id = -1;
    if (ran == 0) {
        for (int i = 0; i < MAXTHREADS; i++)
            initStatistics(&(threadList[i].stat), i);
        ran = 1;
    }
    for (int i = 0; i < MAXTHREADS; i++) {
        if (threadList[i].stat.state == TCB::DED) {
            id = i;
            break;
        }
    }
    if (id == -1) {
        cerr << "Error: Cannot allocate ID" << endl;
        return id;
    }
    assert(id >= 0 && id < MAXTHREADS);
    threadList[id].tType = 0;
    threadList[id].f1 = f;
    threadList[id].stat.state = TCB::NEW;
    threadList[id].context.ContextFlags = CONTEXT_ALL;

    #ifdef _WIN64
    threadList[id].context.Rip = (DWORD64)threadWrapper;
    #else
    threadList[id].context.Eip = (DWORD)threadWrapper;
    #endif
    #ifdef _WIN64
    threadList[id].context.Rsp = (DWORD64)(threadList[id].stack + STACK_SIZE);
    #else
    threadList[id].context.Esp = (DWORD)(threadList[id].stack + STACK_SIZE);
    #endif
    #ifdef _WIN64
    threadList[id].context.Rbp = (DWORD64)(threadList[id].stack + STACK_SIZE);
    #else
    threadList[id].context.Ebp = (DWORD)(threadList[id].stack + STACK_SIZE);
    #endif

    threadList[id].context.ContextFlags = CONTEXT_FULL;
    nThreads++;
    CreateThread(NULL, 0, threadWrapper, &threadList[id], 0, NULL);
    return id;
}

void run(int tid) {
    threadList[tid].stat.state = TCB::RED;
}

int getID() {
    return currentThread;
}

void sleep(int secs) {
    Sleep(secs * SECOND);
}

void start() {
    while (1);
}
