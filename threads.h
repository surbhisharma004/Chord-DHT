#ifndef THREADS_H
#define THREADS_H

#include <iostream>
#include <Windows.h>
#include <cassert>

#define SECOND 1000 // Milliseconds
#define STACK_SIZE 4096
#define MAXTHREADS 10

using namespace std;

struct TCB {
    CONTEXT context;
    char stack[STACK_SIZE];
    enum { NEW, RED, RUN, SUS, SLP, DED } state;
    int tType;
    union {
        void (*f1)(void);
        void *(*f2)(void *);
    };
    void *args;
    void *retVal;
    DWORD RunTimeStart;
    DWORD RedTimeStart;
    int burst;
    struct statistics {
        int ID;
        int state;
        int burst;
        int total_exec_time;
        int total_slp_time;
        float avg_time_quant;
        float avg_wait_time;
        int RedTimeTotal;
    } stat;
};

void initStatistics(TCB::statistics* stat, int id);
void deleteThread(int threadID);
void dispatch(int sig);
void yield();
void alarm_handler(int sig);
int createWithArgs(void *(*f)(void *), void *args);
int create(void (*f)(void));
int getID();
void sleep(int secs);
void run(int tid);
void start();

#endif // THREADS_H
