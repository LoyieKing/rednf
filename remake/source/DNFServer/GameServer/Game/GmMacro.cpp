// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 GmMacro/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// GmMacro.cpp — game_master::CMacro / stMacroData 完整还原（第2批）。
// 对照 docs/class_func_reports/game_master__CMacro/ 10 函数报告 + stMacroData ctor/dtor。
#include "GameMasterCmd.h"
#include "CUser.h"
#include "CStreamGuard.h"
#include "StreamPool.h"
#include "MsgQueueMgr.h"
#include "GlobalData.h"
#include <string.h>

namespace game_master
{

#include "GmMacro/Reset.inc"
{
    if (!m_data.empty())
    {
        for (size_t i = 0; i < m_data.size(); ++i)
        {
            if (m_data[i].m_data != 0)                  // ORIG：operator[] 两次取 data 判空
            {
                delete[] m_data[i].m_data;              // ORIG operator delete[]
            }
        }
        m_data.clear();
    }
    m_curIdx = 0;
    m_state = 0;
}

// ---- stMacroData ----
#include "GmMacro/stMacroData.inc"
{
    m_cmd = 0;
    m_sub = 0;
    m_data = 0;
    m_len = 0;
}

// b28d-solo-GmMacro：stMacroData 无用户析构（隐式 trivial）。
// ORIG nm 0x084b5db4 W 0x5 = 编译器隐式 trivial D1；ORIG Recording 0x084a8802 正常
// 路径无 dtor call、vector 清理走 _Destroy_aux<true>（trivial 分支）。我方旧用户空
// dtor 使 4.4 判 non-trivial：正常路径多 1 次 dtor call + push_back 处 EH landing
// （~stMacroData ×2 + _Unwind_Resume + push esi/ebx），Recording 53 vs ORIG 35。
// m_data 释放仍由 CMacro::Reset 循环负责（浅拷贝 push_back，析构释放会 double-free）。

// ---- CMacro ----

#include "GmMacro/StartMacro.inc"
{
    Reset();
    m_state = 1;                                        // recording
}

#include "GmMacro/StopMacro.inc"
{
    m_state = 0;
}

#include "GmMacro/RudMacro.inc"
{
    m_state = 2;                                        // running
    for (size_t i = 0; i < m_data.size(); ++i)
    {
        // b28d-D42：ORIG 0x084a8686 只调一次 operator[] 并把结果存 -0xc 槽，
        // 之后逐字段 `mov -0xc(%ebp),%eax` 重读（非 P157 的逐点重算形）；
        // 4.4 对逐点表达式改把 m_len 缓到 %ebx（push %ebx + 帧 -4）⇒ 用具名引用。
        stMacroData& cur = m_data[i];
        if (cur.m_data != 0)
        {
            SendPacket(cur.m_data, cur.m_len);
        }
    }
}

#include "GmMacro/NextCmd.inc"
{
    if (m_curIdx >= m_data.size())
    {
        m_state = 0;
        m_curIdx = 0;
        return;
    }
    stMacroData& cur = m_data[m_curIdx];
    if (cur.m_data != 0)
    {
        SendPacket(cur.m_data, cur.m_len);
    }
    if (static_cast<unsigned int>(m_curIdx + 1) < m_data.size())
    {
        ++m_curIdx;
        if (m_data[m_curIdx].m_data != 0)
        {
            NextCmd();                          // ORIG：新槽 data 非空才尾递归
        }
    }
}

#include "GmMacro/GetCurCmd.inc"
{
    return &m_data[m_curIdx];
}

// b28d-solo-GmMacro verdict=AE ORIG 0x084a8802 orig/ours=35/36（1 条尾 nop，
// §6.1 对齐填充豁免），p167 CLEAR 4/4。修法：删除 stMacroData 用户空析构
// （GameMasterCmd.h 声明 + 本文件定义）——ORIG nm 0x084b5db4 W 0x5 系编译器隐式
// trivial D1，ORIG 正常路径无 dtor call、vector 清理走 _Destroy_aux<true>；我方旧
// 用户 dtor 使 4.4 判 non-trivial，正常路径多 1 次 dtor call 且 push_back 处布
// EH landing（~stMacroData ×2 + _Unwind_Resume + push esi/ebx），53 vs 35。
// 旧 b28 判词 N-EH/N83（P216 throw() 法不可用）作废：非平凡性源自我方用户 dtor，
// 删声明即复现 ORIG（P12/P71 判据；dtor 符号由 vector 使用点隐式发射，nm 仍在）。
#include "GmMacro/Recording.inc"
{
    stMacroData rec;
    rec.m_cmd = static_cast<unsigned char>(type);
    rec.m_sub = static_cast<unsigned char>(sub);
    if (data != 0 && len != 0)
    {
        rec.m_data = new char[len];                     // ORIG operator new[](unsigned)
        memcpy(rec.m_data, data, len);
        rec.m_len = len;
    }
    m_data.push_back(rec);
}

#include "GmMacro/OnRecv.inc"
{
    if (m_state == 1)
    {
        Recording(type, sub, data, len);
    }
}

#include "GmMacro/OnSend.inc"
{
    switch (m_state)
    {
    case 1:
        Recording(type, sub, 0, 0);
        break;
    case 2:
    {
        stMacroData* cur = GetCurCmd();
        if (static_cast<int>(cur->m_cmd) == type &&
            static_cast<int>(cur->m_sub) == sub)
        {
            NextCmd();
        }
        break;
    }
    }
}

// b28d-solo-GmMacro verdict=AE ORIG 0x084a8924 orig/ours=97/97，p167 CLEAR 18/18。
// 修法：P222 正向包裹 + P171 外层已判后的死重测照抄 + 三支 Acquire 内联直传；
// 本轮 p167 命中 1 处重载选错：operator<<(GetUID()) 原码选 operator<<(int)
// （ORIG +0x104 call 0x080c8c56；不 cast 会选 0x080e2292 unsigned 版），
// 实参处 static_cast<int> 后调用点差集清零（线格式同为 4 字节，位模式不变）。
#include "GmMacro/SendPacket.inc"
{
    int total = len + 8;
    CStreamGuard guard(true);
    // P0：ORIG 无 Stream* 槽——Set(pool->Acquire(...)) 三支各自内联直传。
    // 第二分支首条 `cmpl $0xfff/-0xc; jle big` 为外层已判后的死重测（P171 照抄）；
    // 正向包裹形（P222/探针 /tmp/bl5/probeB.cpp f3）使中段 `cmpl $0x3fff; jg→big岛、
    // fall-through mid(0xf1)` 与 ORIG 块序一致。
    if (total < 0x1000)
    {
        guard.Set(GlobalData::s_small_stream_pool->Acquire("GameMaster.cpp", 0xed));
    }
    else if (total >= 0x1000 && total <= 0x3fff)
    {
        guard.Set(GlobalData::s_stream_pool->Acquire("GameMaster.cpp", 0xf1));
    }
    else
    {
        guard.Set(GlobalData::s_big_stream_pool->Acquire("GameMaster.cpp", 0xf5));
    }
    // ORIG 每 << 先 operator*（+0xc7/+0xf0），uid 经 %ebx 跨 call 直传无槽。
    // CStreamGuard::operator* 返回 this 指针（CStreamGuard.h:27-28）；链式显式调用
    // 令 op<< 的 this=operator* 返回值（%eax→(%esp)），对齐 ORIG +0xf0/+0x10f。
    guard.operator*()->operator<<(0);
    // P167：ORIG +0x104 call 0x080c8c56 = operator<<(int)（GetUID 返回 unsigned int，
    // 原码在实参处显式转 int）；不 cast 会选 operator<<(unsigned int) 0x080e2292。
    guard.operator*()->operator<<(static_cast<int>(m_user->GetUID()));
    guard.operator->()->put_binary(const_cast<char*>(data), len);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
}

} // namespace game_master
