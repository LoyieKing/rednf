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

// ---- stMacroData ----
stMacroData::stMacroData() throw()                      // 0x084b3f6a
{
    m_cmd = 0;
    m_sub = 0;
    m_data = 0;
    m_len = 0;
}

stMacroData::~stMacroData() throw()                    // 0x084b5db4（ORIG D1 空体：push ebp/mov/pop/ret）
{
    // m_data 释放由 CMacro::Reset 循环负责（ORIG operator delete[] 实证）；
    // 析构不释放：vector<stMacroData> push_back 为浅拷贝，释放会导致 double-free。
}

// ---- CMacro ----

void CMacro::Reset()                                    // 0x084a85b4
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

void CMacro::StartMacro()                               // 0x084a865c
{
    Reset();
    m_state = 1;                                        // recording
}

void CMacro::StopMacro()                                // 0x084a8678
{
    m_state = 0;
}

void CMacro::RudMacro()                                 // 0x084a8686
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

void CMacro::NextCmd()                                  // 0x084a86fe
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

stMacroData* CMacro::GetCurCmd()                        // 0x084a87e2
{
    return &m_data[m_curIdx];
}

// 0x084a8802。【b28 判词 N-EH】ORIG 35 = ours 正常路径逐条全等；额外 18 条为
// 4.4 对 new[]/memcpy/push_back throwing call 恒发的 landing cleanup 块
// （stMacroData ctor/dtor 已 throw() 仍发；push %esi/%ebx 随之），4.1 同形源码
// 不布 guard-cleanup——「跨版本 new-expression landing-pad 抑制」负例在册。
// b28d-D42 verdict=DIFF [N-EH/N83] ORIG 0x084a8802 orig/ours=35/53 ndiff=50。
// P216 throw() 法不可用：涉事 callee 为 libstdc++ operator new[]/memcpy/
// vector::push_back（禁改头），4.4 恒布 landing ⇒ 语义等价封顶。
void CMacro::Recording(int type, int sub, char* data, int len)
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

void CMacro::OnRecv(int type, int sub, char* data, int len)     // 0x084a8870
{
    if (m_state == 1)
    {
        Recording(type, sub, data, len);
    }
}

void CMacro::OnSend(int type, int sub)                  // 0x084a88aa
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

// b28d-D42 verdict=AE(IDENTICAL) ORIG 0x084a8924 orig/ours=97/97 ndiff=0。
// 修法：P222 正向包裹 + P171 外层已判后的死重测照抄 + 三支 Acquire 内联直传。
void CMacro::SendPacket(char const* data, int len)      // 0x084a8924
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
    guard.operator*()->operator<<(m_user->GetUID());
    guard.operator->()->put_binary(const_cast<char*>(data), len);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
}

} // namespace game_master
