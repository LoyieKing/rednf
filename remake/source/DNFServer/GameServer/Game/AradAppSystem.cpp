// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 AradAppSystem/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
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
// [S5AradDisp 2026-09-11 复核维持 N：直接读 ORIG 原始字节 —— 0x0817de2e `83 7d 08 02`
//  cmpl $0x2,0x8(%ebp) 后跟 0x0817de32 `eb 1b` **jmp** 0x0817de4f（跳过 0x0817de34..
//  0x0817de4d 错误块；全函数无任何跳转指向 0x34 ⇒ 该块不可达 = 死块）。
//  本波探针 /tmp/probe_ai.cpp（v1 if(argc!=2){} / v2 if(argc==2){}else{} / v3 switch(argc)）
//  同旗标同头集实测恒发 `74 1b` **je**（cmp 后必条件化），合法 C++ 在 -O0 下不可能产出
//  「cmp + 无条件 jmp + 保留死块」⇒ 维持 N。我方 gap 0（orig 42 / our 42），
//  残差仅该 1 处 opcode（je vs jmp）+ .o 重定位占位（§6.3 假阳性）。]
#include "AradAppSystem/AradAppInit.inc"
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

#include "AradAppSystem/script_load.inc"
{
    std::cerr << "[!] script loading...\n";
    if (!G_CDataManager()->init())
        return false;
    std::cerr << "[!] script loading Complete!!\n";
    return true;
}

#include "AradAppSystem/open_log.inc"
{
    G_CEnvironment()->set_file_name(name);
    char local_d4[200] = { '\0' };
    sprintf(local_d4, "./log/%s/", G_CEnvironment()->get_file_name());
    CLog::instance()->initiate(local_d4, (char*)0, 0);
    std::cerr << "[!] Log file init Sucess!!\n";
    return true;
}
