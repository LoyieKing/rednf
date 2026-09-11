// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 W6_HackPunishDeps/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include "W6_HackPunishDeps.h"

#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>

// ----------------------------------------------------------------------------
// 【临时】CUser.h 当前引用但尚未有任何权威头提供声明的类型（Wave6 进行中）；
// 仅本 TU 编译期可见：引用参数所需的最小前向 + 枚举占位（不带语义假设）。
// 权威头落地后删除本块并改 include。
// ----------------------------------------------------------------------------
#ifndef DNF_W6_HP_USER_FWD_TYPES
#define DNF_W6_HP_USER_FWD_TYPES
class CodeHackChecksum;
class CodeHackChecksumEx;
class CodeHackCheckRangeEx;
// [dedup] 两枚举权威定义见 CUser.h 守卫段（include 后即可见）。
#endif

#include "CUser.h"   // CUser::m_historyLog（+0x79700 cUserHistoryLog）

// Divide_C_ClassIP_EndIP 权威声明见 Divide_C_ClassIP_EndIP.h（W7 批次新建 TU）；
// ORIG 返回 void（_Z22Divide_C_ClassIP_EndIPPcS_S_ @0x086b1dd5），原 char* 声明修正。
#include "Divide_C_ClassIP_EndIP.h"

namespace WongWork
{

#include "W6_HackPunishDeps/HackIPCounter_t.inc"


#include "W6_HackPunishDeps/HackFullIPCounter_t.inc"


#include "W6_HackPunishDeps/stIPInfo_t.inc"


#include "W6_HackPunishDeps/addBlackIP.inc"


#include "W6_HackPunishDeps/isBlackIP.inc"


#include "W6_HackPunishDeps/getBlackIPCount.inc"


#include "W6_HackPunishDeps/setHackCnt.inc"


#include "W6_HackPunishDeps/setTotalTrade.inc"


#include "W6_HackPunishDeps/GetExceptionCnt.inc"


#include "W6_HackPunishDeps/setIPCounterData.inc"


#include "W6_HackPunishDeps/_findIPCount.inc"
                                             char const* ipStr)
{
    // b28d-solo-_findIPCount: ORIG 循环体对 it->second 做 0x18 字节按值拷贝到
    // -0x38（6 条 mov 对），prefix 用 4 条 movl 内联清零，且 m_b 与 strncmp 为
    // 两个独立守卫落同一 continue 出口。残差 1 条（gap=+1，dual_rank 唯一候选）：
    // ORIG `lea -0x48(%ebp),%eax; movzbl (%eax),%eax`（4.1 数组元素读恒发地址），
    // 4.4 恒折 `movzbl -0x48(%ebp),%eax`。本轮再探针三形（/tmp/probe_lea.cpp：
    // `prefix[0]`、`*prefix`、`*(char*)((unsigned int)prefix)`）**全部折 disp**；
    // 具名指针/引用形能出 lea 但多占 4B 槽破坏帧 0x58 ⇒ N117/N102 封顶。
    // ORIG 0x080f9322。
    typedef std::multimap<unsigned short, HackIPCounter_t> IpCounterMap;
    IpCounterMap::const_iterator it = m_ipCounterMap.lower_bound((unsigned short)a);
    IpCounterMap::const_iterator endIt = m_ipCounterMap.upper_bound((unsigned short)a);

    for (; it != endIt; ++it)
    {
        HackIPCounter_t entry = it->second;
        if ((unsigned int)entry.m_b == b)
        {
            char prefix[0x10] = {0};
            Divide_C_ClassIP_EndIP(prefix, (char*)0, (char*)ipStr);
            if (prefix[0] != '\0')
            {
                if (strncmp(prefix, entry.m_ip, 0xc) == 0)
                {
                    return (int)entry.m_cnt;
                }
            }
        }
    }
    return 0;
}

#include "W6_HackPunishDeps/WriteIPCounterLog.inc"


#include "W6_HackPunishDeps/ResetIpCounterData.inc"


#include "W6_HackPunishDeps/setFullIPCounterData.inc"


#include "W6_HackPunishDeps/_findFullIPCount.inc"
                                                 char const* ipStr)
{
    // b28d-solo-_findFullIPCount: ORIG 循环体对 it->second 做 0x18 字节按值拷贝到
    // -0x38（6 条 mov 对），m_b 与 strncmp 为两个独立守卫落同一 continue 出口。
    // ORIG 0x080f9574。
    typedef std::multimap<unsigned short, HackFullIPCounter_t> FullCounterMap;
    FullCounterMap::const_iterator it = m_fullIPCounterMap.lower_bound((unsigned short)a);
    FullCounterMap::const_iterator endIt = m_fullIPCounterMap.upper_bound((unsigned short)a);

    for (; it != endIt; ++it)
    {
        HackFullIPCounter_t entry = it->second;
        if ((unsigned int)entry.m_b == b)
        {
            if (strncmp(ipStr, entry.m_ip, 0x10) == 0)
            {
                return (int)entry.m_cnt;
            }
        }
    }
    return 0;
}

#include "W6_HackPunishDeps/WriteFullIPCounterLog.inc"


#include "W6_HackPunishDeps/ResetFullIpCounterData.inc"


#include "W6_HackPunishDeps/_checkApplyFlag.inc"


#include "W6_HackPunishDeps/checkMaxHackCntOver.inc"


#include "W6_HackPunishDeps/checkTotalTradeRuleOver.inc"


}  // namespace WongWork
