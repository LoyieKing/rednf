//
// Created by Loyie King on 2024/3/10.
//


#include "CoreDump.h"

#include <iostream>
#include "DNFFunctionLib.h"
#include "CommonConstants.h"

#include <sys/resource.h>

#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "GlobalInstance.h"

typedef void func_void_int(int);

bool regist_signals();

CCoreDump::CCoreDump() {
    // 原始：call regist_signals 后 xor eax,1; test al,al; jne（== false 提前返回形态）
    if (regist_signals() == false) {
        return;
    }
    rlimit rl;
    int ret = getrlimit(RLIMIT_CORE, &rl);  // 原始：结果存局部变量
    rl.rlim_cur = 0xffffffff;
    ret = setrlimit(RLIMIT_CORE, &rl);
}

CCoreDump::~CCoreDump() {
}

void omll_dump_core_file() {
    // 来自反编译/DWARF 的推断：原始实现为 abort()
    abort();
}

void signal_handler(int signal) {
    REDNF_LOG("Recieve %d signal.", signal);
    switch (signal) {
        case SIGTERM:
            puts("RECV SIGTERM");
            break;
        case SIGUSR2:
            global_instance::shutdown = true;
            break;
        case SIGABRT:
        case SIGSEGV:
            puts("Recv SIGSEGV signal --> make Dump Core file.");
            omll_dump_core_file();
            break;
        case SIGFPE:
            puts("Recv FPE signal");
            omll_dump_core_file();
            break;
        case SIGILL:
        case SIGBUS:
        case SIGSTKFLT:
        case SIGURG:
        case SIGXCPU:
        case SIGXFSZ:
        case SIGSYS:
            printf("Recv %d signal --> process is terminated abnormaly.\n", signal);
            omll_dump_core_file();
            exit(-1);
    }
}

bool regist_signal(int signal, func_void_int *handler) {
    struct sigaction sa;
    sa.sa_handler = handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;  // 原始：直接操作结构体字段（mov eax,[ebp]; or eax,imm; mov [ebp],eax）
    if (signal == SIGALRM) {
        sa.sa_flags |= SA_INTERRUPT /*0x20000000*/;
    } else {
        sa.sa_flags |= SA_RESTART /*0x10000000*/;
    }
    struct sigaction uap;
    // 原始：call sigaction 后 shr eax,0x1f + test al,al + je（调用结果直接入条件 < 0）
    if (sigaction(signal, &sa, &uap) < 0) {
        // EUC-KR："%d번 signal 등록 실패"
        printf("%d\271\370 signal \265\356\267\317 \275\307\306\320\n", signal);
        return 0;
    }
    return 1;
}

bool regist_signals() {
    // 原始：bool 初始化 + 前 3 个直接检查调用结果（xor eax,1），后 10 个经变量存取
    bool register_success = false;

#define REGIST_SIGNAL(signal, handler)                     \
    {                                                      \
        register_success = regist_signal(signal, handler); \
        if (register_success != 1) {                       \
            return false;                                  \
        }                                                  \
    }

    if (regist_signal(SIGTERM, signal_handler) != 1) {
        return false;
    }
    if (regist_signal(SIGUSR1, signal_handler) != 1) {
        return false;
    }
    if (regist_signal(SIGUSR2, signal_handler) != 1) {
        return false;
    }
    REGIST_SIGNAL(SIGPIPE, (func_void_int *)0x1);
    REGIST_SIGNAL(SIGSEGV, signal_handler);
    REGIST_SIGNAL(SIGFPE, signal_handler);
    REGIST_SIGNAL(SIGILL, signal_handler);
    REGIST_SIGNAL(SIGBUS, signal_handler);
    REGIST_SIGNAL(SIGSTKFLT, signal_handler);
    REGIST_SIGNAL(SIGURG, signal_handler);
    REGIST_SIGNAL(SIGXCPU, signal_handler);
    REGIST_SIGNAL(SIGXFSZ, signal_handler);
    REGIST_SIGNAL(SIGSYS, signal_handler);

#undef REGIST_SIGNAL

    return true;
}
