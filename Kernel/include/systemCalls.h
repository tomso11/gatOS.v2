#ifndef SYSCALLS_H
#define SYSCALLS_H

typedef unsigned long qword;

qword sys_call_writeC(qword qstdout, qword qmsg, qword qlength, qword rcx, qword r8, qword r9);
qword sys_call_readC(qword qstdin, qword qbuffer, qword qlength, qword rcx, qword r8, qword r9);
qword sys_call_clearC(qword rdi,qword rsi, qword rdx, qword rcx, qword r8, qword r9);
qword sys_call_echoC(qword qon,qword rsi, qword rdx, qword rcx, qword r8, qword r9);
qword  sys_call_runC(qword qprogram, qword rsi, qword rdx, qword rcx, qword r8, qword r9);
qword sys_call_printProcesses(qword rdi, qword rsi, qword rdx, qword rcx, qword r8, qword r9);
qword sys_call_changeModuleEnvironmetC(qword qi, qword qj,qword rdx, qword rcx, qword r8, qword r9);
qword sys_call_undoBackwardsC(qword from, qword j,qword rdx, qword rcx, qword r8, qword r9);
qword sys_call_kill(qword qpid, qword j,qword rdx, qword rcx, qword r8, qword r9);
void setUpSystemCalls();
qword syscallHandler(qword rdi,qword rsi, qword rdx, qword rcx, qword r8, qword r9);
qword sys_call_getAddressOfModuleC(qword qprogram, qword rsi, qword rdx, qword rcx, qword r8, qword r9);
qword sys_call_createThread(qword qentryPoint, qword qargs,qword rdx, qword rcx, qword r8, qword r9);
qword sys_call_sleep(qword time,qword rsi,qword rdx, qword rcx, qword r8, qword r9);
qword sys_call_wait(qword semaphore,qword rsi,qword rdx, qword rcx, qword r8, qword r9);
qword sys_call_signal(qword semaphore,qword rsi,qword rdx, qword rcx, qword r8, qword r9);
qword sys_call_semOpen(qword name,qword rsi,qword rdx, qword rcx, qword r8, qword r9);
qword sys_call_semCreate(qword name , qword start,qword rsi,qword rdx, qword rcx, qword r8, qword r9);
qword sys_call_semClose(qword index,qword rsi,qword rdx, qword rcx, qword r8, qword r9);
qword sys_call_getMutex(qword mutexName,qword rsi,qword rdx, qword rcx, qword r8, qword r9);
qword sys_call_lockMutex(qword mutex,qword rsi,qword rdx, qword rcx, qword r8, qword r9);
qword sys_call_freeMutex (qword mutex,qword rsi,qword rdx, qword rcx, qword r8, qword r9);
qword sys_call_closeMutex(qword index,qword rsi,qword rdx, qword rcx, qword r8, qword r9);
qword sys_call_openPipe(qword name,qword rsi,qword rdx, qword rcx, qword r8, qword r9);
qword sys_call_createPipe(qword name,qword rsi,qword rdx, qword rcx, qword r8, qword r9);
qword sys_call_closePipe(qword pipe,qword rsi,qword rdx, qword rcx, qword r8, qword r9);
qword sys_call_read(qword pipe,qword result,qword bytes, qword rcx, qword r8, qword r9);
qword sys_call_write(qword pipe,qword message,qword rdx, qword rcx, qword r8, qword r9);
#endif
