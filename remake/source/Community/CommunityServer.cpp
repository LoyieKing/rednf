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
        // 原始：argc != 3 错误分支在前（ORIG cmp; je <main body>，错误路径内联），
        // if/else 结构而非 if 后直落。
        if (argc != 3) {
            puts("wrong parameter!");
            puts("[execute filename]  [cfg filename]  [command]");
            return 0;
        } else {
            // 原始：char buffer[255] = {0}; 初始化展开为运行时对齐分拆块清 0
            // （and $0x1 / and $0x2 + rep stosd + 尾 2/1 字节，edx=地址 ebx=0xff），
            // 与 CreateLogFile 的 char buffer[1024] = {0} 同一编译器形态。
            char buffer[255] = {0};
            sprintf(buffer, "./cfg/%s.cfg", argv[1]);
            cfg.ReadConfigFile(buffer);
            // 原始：command == false 直接 return 0（xor eax,1; test/je 形态）
            if (command(argv[1], argv[2]) == false) {
                return 0;
            }
            CCoreDump coreDump;
            CSessionManager sessionManager;
            sessionManager.CreateEvents(100);
            // 原始：get_value/atoi 直接内联为 TryListen/ArchiveLog/printf 实参
            // （ORIG 每次重新求值，无 port/ip 命名局部）。
            // 原始：TryListen == false 直接 return 0
            if (sessionManager.TryListen(cfg.get_value("ip"), atoi(cfg.get_value("port"))) == false) {
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
        }
    } catch (std::exception& e) {
        // 原始：std::cerr << "error: " << e.what() << "\n"（_ZStls 三次输出）
        std::cerr << "error: " << e.what() << "\n";
        return 1;
    } catch (...) {
        std::cerr << "Exception of unknown type!\n";
    }
    // 原始：main 末尾公共 return 0（ORIG 反汇编：正常落出 try + catch(...) 落出后
    // 共享 esi=0 收尾，指令形态 test %ebx 归零链 + mov %esi,%eax）
    return 0;
}
