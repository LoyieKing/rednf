// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CGenUniqueNo/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__ZN8WongWork12CGenUniqueNo12m_nTimeCountE`（ORIG 0x84b7850）；
// 类静态数据成员定义已是本 TU 首强符号。
#include <iostream>

#include "CGenUniqueNo.h"
#include "CSystemTime.h"
#include "GlobalData.h"
#include "CEnvironment.h"
#include "OS_API.h"

#include <stdio.h>
#include <time.h>

namespace WongWork
{
unsigned short CGenUniqueNo::m_nTimeCount = 0;

#include "CGenUniqueNo/_updateTimeCount.inc"
{
    ++m_nTimeCount;
    if (m_nTimeCount > 49999)
        m_nTimeCount = 1;
}

#include "CGenUniqueNo/GenUniqueNo.inc"
{
    // b28d-solo-CGenUniqueNo（AE）：ORIG 0x084b76d8 首实参 = Server_Envir.m_serverGroup
    // (+0x378)、次实参 = m_channelNo (+0x1b0)（双证：ORIG get_server_group 0x08106ce0
    // 读 +0x378、get_channel_no 0x0814aaca 读 +0x1b0）——原码把 serverGroup 具名物化到
    // -0xc、channelNo 内联作第二实参。旧代码两字段写反（真语义 bug：唯一号两个 %02d 段
    // 互换）。调用目标同 ORIG OS_API::snprintf（_ZN6OS_API8snprintfEPcjPKcz @0x0858c81c），
    // 旧为 libc snprintf（p167 各 MISSING/EXTRA 1）。sec@-0x10、serverGroup@-0xc、
    // count→%ebx、帧 push %ebx/sub $0x34 全对齐。
    _updateTimeCount();
    int sec = GlobalData::s_systemTime_.getCurSec();
    int serverGroup = G_CEnvironment()->m_serverEnvir.m_serverGroup;
    OS_API::snprintf(out, 0x14, "%02d%02d%10d%05d",
                     serverGroup,
                     G_CEnvironment()->m_serverEnvir.m_channelNo,
                     sec, (unsigned int)m_nTimeCount);
    return out;
}

#include "CGenUniqueNo/genIPGNo.inc"
{
    // b28d-solo-CGenUniqueNo（AE）：ORIG 0x084b7750。typeConst 是 ORIG 函数内局部静态
    // （_ZZN8WongWork12CGenUniqueNo8genIPGNoENS0_15ENUM_IPGNO_TYPEEjPcE9typeConst
    // @0x093abb44 d），故移入函数体，消除同名文件级静态发明符号。env/year 无具名槽
    // （env 内联、year 以 `lea 0x76c(%eax)` 直入 %ecx）；调用目标 OS_API::snprintf。
    // ORIG 该局部静态 nm size=8（无 NUL）；4.4 拒绝 `char[8]="CLQBVFOE"`，用逐字符列表同尺寸。
    static const char typeConst[8] = { 'C', 'L', 'Q', 'B', 'V', 'F', 'O', 'E' };
    time_t curSec = (time_t)GlobalData::s_systemTime_.getCurSec();
    struct tm local_tm;
    struct tm* cur = localtime_r(&curSec, &local_tm);
    OS_API::snprintf(out, 0x14, "%c%02d%02d%02d%02d%010d",
                     (int)(signed char)typeConst[(unsigned int)type],
                     G_CEnvironment()->m_serverEnvir.m_channelNo,
                     (cur->tm_year + 1900) % 2000, cur->tm_mon + 1,
                     cur->tm_mday, no);
    return out;
}
}
