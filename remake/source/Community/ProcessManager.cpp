//
// Created by Loyie King on 2024/3/7.
//

#include "ProcessManager.h"

#include <iostream>
#include "DNFFunctionLib.h"
#include "CommonConstants.h"
#include "GuildConstants.h"

#include <dirent.h>
#include <signal.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <unistd.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>

CProcessManager::CProcessManager() {}
CProcessManager::~CProcessManager() {}

void CProcessManager::_makeDir(char *dirName) {
    DIR *dir = opendir(dirName);
    if (dir == NULL) {
        mkdir(dirName, S_IRWXU | S_IRWXG | S_IRWXO /* 0x1ff,777*/);
    }
}

bool CProcessManager::check_pidfile(const char *processName) {
    char buffer[30];
    memset(buffer, 0, 30);
    sprintf(buffer, "./pid/%s.pid", processName);
    puts(buffer);
    // 原始：call access 后 test eax,eax; sete al; test al,al; je（调用结果直接入条件）
    if (access(buffer, F_OK) == 0) {
        return 1;
    }
    return 0;
}

int CProcessManager::Daemon() {
    pid_t pid = fork();
    if (pid < 0) {
        return 0xffffffff;
    } else {
        if (pid != 0) {
            exit(0);
        }
        setsid();
        chdir("./");
        umask(0);
    }
    return 0;
}

void CProcessManager::SendShutdownSignal(const char *processName) {
    char buffer[30];
    memset(buffer, 0, 30);
    sprintf(buffer, "pid/%s.pid", processName);
    FILE *file = fopen(buffer, "r");
    if (file == NULL) {
        // EUC-KR："실패"
        printf("%s process id file open \275\307\306\320\n", buffer);
    } else {
        int pid;
        fscanf(file, "%d", &pid);
        if (pid < 1) {
            fclose(file);
            // EUC-KR："번의 잘못된"
            printf("%d\271\370\300\307 \300\337\270\370\265\310 process id\n", pid);
        } else {
            int result = kill(pid, SIGUSR2);
            if (result < 0) {
                fclose(file);
                // EUC-KR："번 process로 종료 signal 송신 실패"
                printf("%d\271\370 process\267\316 \301\276\267\341 signal \274\333\275\305 \275\307\306\320", pid);
            } else {
                printf("SEND SHUTDOWN SIGNAL TO %d\n", pid);
                fclose(file);
            }
        }
    }
}

int CProcessManager::WritePID(const char *processName) {
    char pidBuffer[512];
    char pidPath[30];
    // 原始：_makeDir("./pid") 常量（0x80b58e3）
    _makeDir(const_cast<char*>("./pid"));
    memset(pidPath, 0, 30);
    sprintf(pidPath, "./pid/%s.pid", processName);
    int file = open(pidPath, O_CREAT | O_RDWR /* 0102 */, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH /* 0644 */);
    if (file < 0) {
        return 0;
    } else {
        memset(pidBuffer, 0, 512);
        // 原始：getpid() 直接作为变参
        sprintf(pidBuffer, "%ld\n", getpid());
        // 原始：strlen 内联为 write 参数；write 结果先存局部变量（cmp+jns）
        int w = write(file, pidBuffer, strlen(pidBuffer));
        if (w < 0) {
            close(file);
            return 0;
        } else {
            return 1;
        }
    }
}
