// df_manager_r — CAppInit / CAppStartInit（ORIG DNFAppStartInit.cpp）
#include "DNFAppStartInit.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include "DNFAppConfig.h"
#include "DNFApplication.h"
#include "DNFKillUserConfig.h"
#include "DNFServerConfig.h"
#include "DNFTableBase.h"

CAppStartInit::CAppStartInit() {}
CAppStartInit::~CAppStartInit() {}

// CAppInit ctor 定义置于 CAppStartInit ctor 之后（理由同 SystemTimeHandler.cpp：
// ORIG CAppStartInitC1 栈帧 0x18，定义在前会退化为 sub $0x4）。
CAppInit::CAppInit() {}
CAppInit::~CAppInit() {}

bool CAppStartInit::Save_pid(const std::string& path)
{
    // R10: ORIG 为 "./pid/" + path + ".pid" 的 operator+ 链（无显式 string 临时），
    // 且返回类型为 bool（调用侧 ORIG 为 mov %eax,%ebx; xor $1,%ebx 形态）。
    std::string full = "./pid/" + path + ".pid";
    // R10: ORIG 槽位 fd@-0xc / n@-0x10；先声明 n 使槽位与 ORIG 对齐。
    int n;
    int fd = open(full.c_str(), 0x42, 0x1a4);
    if (fd < 0)
        return 0;
    char buf[0x400];
    memset(buf, 0, 0x400);
    sprintf(buf, "%ld\n", (long)getpid());
    n = write(fd, buf, strlen(buf));
    if (n < 0)
    {
        close(fd);
        return 0;
    }
    return 1;
}

int CAppStartInit::Init_Daemon(int argc, char** argv)
{
    // R10: ORIG 先把 argv[2] 存入局部再 strcmp（复现 -0x10 槽位存取）。
    char* arg = argv[2];
    if (strcmp(arg, "start") == 0)
    {
        int pid = fork();
        if (pid < 0)
            return -1;
        // R10: ORIG 为 cmpl $0; je 形态（if (pid != 0) exit），非 jle。
        if (pid != 0)
            exit(0);
        setsid();
        chdir("./");
        umask(0);
    }
    if (!Save_pid(std::string(argv[1])))
        return -1;
    return 0;
}

void CAppStartInit::Init(CApplication* app, int argc, char** argv)
{
    srand((unsigned int)time(0));
    app->m_appConfig = new CAppConfig;
    static_cast<CAppConfig*>(app->m_appConfig)->Check_FileName(std::string(argv[1]));
    app->m_serverConfig = new CServerConfig;
    app->m_killUsrConfig = new CKillUSRConfig;
    if (Init_Daemon(argc, argv) == -1)
        throw CDNFException("CAppStartInit::Init() Demon Init Exception Break!");
}
