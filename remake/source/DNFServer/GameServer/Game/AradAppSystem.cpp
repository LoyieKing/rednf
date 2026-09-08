// df_game_r 还原 —— AradAppSystem（启动/日志/脚本装载入口）。
// ORIG 符号（docs/class_func_reports/AradAppSystem/）：
//   AradAppInit(int, char**)   T 0x0817de00
//   script_load()              T 0x0817de9a
//   open_log(char*)            T 0x0817deea
// 逐函数按 ORIG objdump 转写（c6root gcc4.4.7 -m32 -O0）。
// 三方法 ORIG 均按静态成员编译（无 this：AradAppInit 的 argc 在 0x8(%ebp)、
// open_log 的 name 在 0x8(%ebp)、调用点只压显式实参）。

#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <iostream>

#include "AradAppSystem.h"
#include "CEnvironment.h"   // G_CEnvironment / set_file_name / get_file_name
#include "CDataManager.h"   // G_CDataManager / CDataManager::init
#include "CLog.h"           // CLog::instance / initiate
#include "HistoryLog.h"     // HistoryLog::Init

// [b28d-32 N(4.1 死块): ORIG cmp+无条件 jmp 跳过 argc 错误块（死代码），4.4 恒条件化并 DCE 死块，合法 C++ 不可造（探针 /tmp/lane32_probe1.cpp） | ORIG 0x0817de00]
int AradAppSystem::AradAppInit(int argc, char** argv)
{
    std::cerr << "[!] Server Starting...\n";
    srand(time(0));
    if (argc != 2)
    {
        std::cerr << "[!] Check Args Fail!!\n";
        return 0;
    }
    if (!open_log(argv[1]))
        return 0;
    if (!HistoryLog::Init())
        return 0;
    if (!script_load())
        return 0;
    return 1;
}

bool AradAppSystem::script_load()
{
    std::cerr << "[!] script loading...\n";
    if (!G_CDataManager()->init())
        return false;
    std::cerr << "[!] script loading Complete!!\n";
    return true;
}

bool AradAppSystem::open_log(char* name)
{
    G_CEnvironment()->set_file_name(name);
    char local_d4[200] = { '\0' };
    sprintf(local_d4, "./log/%s/", G_CEnvironment()->get_file_name());
    CLog::instance()->initiate(local_d4, (char*)0, 0);
    std::cerr << "[!] Log file init Sucess!!\n";
    return true;
}
