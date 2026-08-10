// df_relay_r — Thread/LinuxSystem（GCC 4.1.2, 无 DWARF — Ghidra 反汇编还原）
#include <pthread.h>
#include <sys/select.h>

#include "ThreadLock.h"

ThreadLock_linux::ThreadLock_linux()
{
    pthread_mutex_init(&handle_, 0);
}

ThreadLock_linux::~ThreadLock_linux()
{
    pthread_mutex_destroy(&handle_);
}

