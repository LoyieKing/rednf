#include "ArchiveLog.h"
#include "CommunityServer.h"
#include "ConfigFileReader.h"
#include "CoreDump.h"
#include "GlobalFunction.h"
#include "GlobalInstance.h"
#include "ProcessManager.h"
#include "SessionManager.h"
#include <cstdio>
#include <exception>
#include <iostream>
#include <cstring>
#include <cerrno>
#include <cstdlib>
#include "ReverseEngineerLib.h"

#include "DNFFunctionLib.h"
#include "CommonConstants.h"

bool command(const char *processName, const char *cmd) {
    CProcessManager processManager;
    // 原始：nofork != 0 外层（strcmp 结果直接 test；nofork 相等时 je 到共享 return 1 尾部）
    if (strcmp("nofork", cmd) != 0) {
        if (strcmp("start", cmd) == 0) {
            // 来自反编译/DWARF 的推断：check_pidfile 返回非0表示PID文件已存在，应退出
            if (processManager.check_pidfile(processName)) {
                puts("exist pid file!");
                return 0;
            }
            processManager.Daemon();
            processManager.WritePID(processName);
        } else if (strcmp("stop", cmd) == 0) {
            processManager.SendShutdownSignal(processName);
            return 0;
        }
    }
    return 1;
}

int main(int argc, char **argv) {
    try {
        puts("**********************************************************");
        puts("* +---   +-+ +  +----         DUNGEON & FIGHTER          *");
        printf("* |   |  | | |  |                  ver %3d.%3d.%3d.%3d   *\n", 1, 0, 0, 0);
        puts("* |   |  | | |  +----                                    *");
        puts("* |   |  | | |  |      Open Beta Test on 2005.08.10~     *");
        puts("* +---   + +-+  |      Copyright(c) 2004,2005 Neople Co. *");
        puts("**********************************************************");
        CConfigFileReader cfg;
        cfg.add_tag("ip");
        cfg.add_tag("port");
        cfg.add_tag("domain");
        if (argc == 3) {
            char buffer[255];
            // 与原始一致：按地址对齐展开的清 0 循环（非 memset）
            // 原始：p/size 保持在寄存器（edx/ebx），无栈溢写
            register char *p = buffer;
            register unsigned int size = 255;
            register bool odd = ((unsigned int)p & 1) != 0;
            if (odd) {
                buffer[0] = '\0';
                p = buffer + 1;
                size = 254;
            }
            if (((unsigned int)p & 2) != 0) {
                p[0] = '\0';
                p[1] = '\0';
                p += 2;
                size -= 2;
            }
            for (register unsigned int i = size >> 2; i != 0; i--) {
                p[0] = '\0';
                p[1] = '\0';
                p[2] = '\0';
                p[3] = '\0';
                p += 4;
            }
            if ((size & 2) != 0) {
                p[0] = '\0';
                p[1] = '\0';
                p += 2;
            }
            if (!odd) {
                *p = '\0';
            }
            sprintf(buffer, "./cfg/%s.cfg", argv[1]);
            cfg.ReadConfigFile(buffer);
            // 原始：command == false 直接 return 0（xor eax,1; test/je 形态）
            if (command(argv[1], argv[2]) == false) {
                return 0;
            }
            CCoreDump coreDump;
            CSessionManager sessionManager;
            sessionManager.CreateEvents(100);
            int port = atoi(cfg.get_value("port"));
            const char* ip = cfg.get_value("ip");
            // 原始：TryListen == false 直接 return 0
            if (sessionManager.TryListen(ip, port) == false) {
                return 0;
            }
            ArchiveLog("Listen Socket IP:%s, PORT:%s", cfg.get_value("ip"), cfg.get_value("port"));
            printf("Community Server(IP:%s, PORT:%s)\nStart\n", cfg.get_value("ip"), cfg.get_value("port"));
            while (!global_instance::shutdown) {
                global_instance::current_time = time(NULL);
                sessionManager.WaitForSessionEvent(10);
                global_function::sleep(1);
            }
            // 原始：关闭时删除 PID 文件；失败打印错误
            char pidPath[30];
            memset(pidPath, 0, 30);
            sprintf(pidPath, "./pid/%s.pid", argv[1]);
            if (remove(pidPath) == -1) {
                printf("FAIL TO DELETE PID FILE ERROR: %s\n", strerror(errno));
            }
            return 0;
        }
        // 原始：argc != 3 时直落到 puts（if 后无 else）
        puts("wrong parameter!");
        puts("[execute filename]  [cfg filename]  [command]");
        return 0;
    } catch (std::exception& e) {
        // 原始：std::cerr << "error: " << e.what() << "\n"（_ZStls 三次输出）
        std::cerr << "error: " << e.what() << "\n";
        return 1;
    } catch (...) {
        std::cerr << "Exception of unknown type!\n";
        return 0;
    }
}
