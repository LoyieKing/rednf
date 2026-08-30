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
stMacroData::stMacroData()                              // 0x084b3f6a
{
    m_cmd = 0;
    m_sub = 0;
    m_data = 0;
    m_len = 0;
}

stMacroData::~stMacroData()                            // 0x084b5db4（ORIG D1 空体：push ebp/mov/pop/ret）
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
        if (m_data[i].m_data != 0)
        {
            SendPacket(m_data[i].m_data, m_data[i].m_len);
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

void CMacro::Recording(int type, int sub, char* data, int len)  // 0x084a8802
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
    if (m_state == 1)
    {
        Recording(type, sub, 0, 0);
    }
    else if (m_state == 2)
    {
        stMacroData* cur = GetCurCmd();
        if (static_cast<int>(cur->m_cmd) == type &&
            static_cast<int>(cur->m_sub) == sub)
        {
            NextCmd();
        }
    }
}

void CMacro::SendPacket(char const* data, int len)      // 0x084a8924
{
    int total = len + 8;
    CStreamGuard guard(true);
    Stream* s = 0;
    if (total <= 0xfff)
    {
        s = GlobalData::s_small_stream_pool->Acquire("GameMaster.cpp", 0xed);
    }
    else if (total <= 0x3fff)
    {
        s = GlobalData::s_stream_pool->Acquire("GameMaster.cpp", 0xf1);
    }
    else
    {
        s = GlobalData::s_big_stream_pool->Acquire("GameMaster.cpp", 0xf5);
    }
    guard.Set(s);
    guard << 0;                                          // 包头占位（ORIG operator<< int）
    guard << static_cast<int>(m_user->GetUID());
    guard.put_binary(const_cast<char*>(data), len);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);  // ORIG QUEUE_IDX=1
}

} // namespace game_master
