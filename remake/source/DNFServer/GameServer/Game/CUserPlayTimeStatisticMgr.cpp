// CUserPlayTimeStatisticMgr —— 在线用户游玩时长统计实现。
// 报告：docs/class_func_reports/CUserPlayTimeStatisticMgr/{InsertUserPlayTime,UpdateUserPlayTime}.md
// GetInstanceUserPlayTimeStatisticMgr 由 objdump 0x0860cba5 转写（__cxa_guard 单例）。
#include "CUserPlayTimeStatisticMgr.h"

#include <cstring>

#include "CStreamGuard.h"
#include "GlobalData.h"     // s_stream_pool / s_msgq_mgr
#include "MsgQueueMgr.h"
#include "SigTypes.h"       // SIG_USER_PLAY_TIME_STATISTIC（SigTypes_E.h 分片）
#include "StreamPool.h"
#include "CEnvironment.h"   // G_CEnvironment / get_channel_no

#include <cstdio>           // sprintf
#include <cstdlib>          // atoi
#include <string>

// NumberToString（ORIG T 0x081090b4；权威声明 CInventory.h）
char* NumberToString(unsigned int value, int radix);

CUserPlayTimeStatisticMgr::CUserPlayTimeStatisticMgr()
{
    // ORIG C1 0x086102c0（W，0x13 字节）：仅构造内嵌 map。
}

CUserPlayTimeStatisticMgr::~CUserPlayTimeStatisticMgr()
{
    // ORIG D1 0x086107ec（W，0x13 字节）。
}

// [b28d-62] 修：P197④ size()==0 + P103/P132 `**guard`/`guard->` 链。ORIG 0x0860cd14
int CUserPlayTimeStatisticMgr::InsertUserPlayTime()
{
    // ORIG 0x0860cd14：map 为空直接返回 1；否则按 ≤20 条/批经 LOGDB 队列
    // 写 SIG_USER_PLAY_TIME_STATISTIC（+0 计数，+4 起每条 {accId, playSec, count}）。
    // [b28d-62] P197④：ORIG 调 map::size()（非 empty()，后者 out-of-line 且无
    //   sete 段；p167 差集 MISSING size / EXTRA empty 实证）。
    if (m_userPlayInfo.size() == 0)
    {
        return 1;
    }
    CStreamGuard guard(static_cast<Stream*>(0), false);
    SIG_USER_PLAY_TIME_STATISTIC* sig = 0;
    int n = 0;
    for (std::map<unsigned int, stUserPlayInfo_t>::iterator it = m_userPlayInfo.begin();
         it != m_userPlayInfo.end(); ++it)
    {
        unsigned int accId = it->first;
        stUserPlayInfo_t* info = &it->second;
        if (guard.Get() != 0)
        {
            Stream* stream = GlobalData::s_stream_pool->Acquire("Statistics.cpp", 0x24a);
            guard.Set(stream);
            // [b28d-62] P103/P132：ORIG 走 operator* 2 次（`**guard << ...`，
            //   CStreamGuard::operator* 返回 this 指针）+ `guard->GetInBuffer<...>()`
            //   （operator-> 1 次）——p167 差集实证（旧形 `guard << ...` 0 次）。
            **guard << 0xb6;
            **guard << -1;
            sig = guard->GetInBuffer<SIG_USER_PLAY_TIME_STATISTIC>();
            memset(sig, 0, 0xf4);
        }
        sig->m_entries[n].m_accId = accId;
        sig->m_entries[n].m_playSec = info->m_playSec;
        sig->m_entries[n].m_count = info->m_count;
        n = n + 1;
        if (0x13 < n)
        {
            sig->m_count = n;
            GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q, guard);
            guard.Set(static_cast<Stream*>(0));
            n = 0;
        }
    }
    if (0 < n)
    {
        sig->m_count = n;
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q, guard);
        guard.Set(static_cast<Stream*>(0));
        n = 0;
    }
    m_userPlayInfo.clear();
    return 1;
}

// [b28d-62] 修：P167 info 指针缓存（1×operator-> 替 4×ptEv）。ORIG 0x0860cc38
int CUserPlayTimeStatisticMgr::UpdateUserPlayTime(unsigned int accId, int playSec)
{
    // ORIG 0x0860cc38：命中 → +0 累加 playSec、+4 计数 +1；未命中 → 插入
    // {playSec, 1}。
    std::map<unsigned int, stUserPlayInfo_t>::iterator it = m_userPlayInfo.find(accId);
    if (it != m_userPlayInfo.end())
    {
        // [b28d-62] P167/P36：ORIG 0x0860cc90 只发 1 次 operator-> 并把
        //   `&it->second` 缓存到 -0xc（info 指针局部），逐字段经 info-> 访问；
        //   直写 4 次 it->second 会多发 3 次 ptEv call。
        stUserPlayInfo_t* info = &it->second;
        info->m_playSec = info->m_playSec + playSec;
        info->m_count = info->m_count + 1;
    }
    else
    {
        stUserPlayInfo_t info;
        info.m_playSec = playSec;
        info.m_count = 1;
        m_userPlayInfo.insert(std::make_pair(accId, info));
    }
    return 1;
}

CUserPlayTimeStatisticMgr* GetInstanceUserPlayTimeStatisticMgr()
{
    // ORIG 0x0860cba5：__cxa_guard 单例（instance 0x09473444，析构经 __cxa_atexit）。
    static CUserPlayTimeStatisticMgr instance;
    return &instance;
}

// ============================================================================
// CreateUserPlayTimeQuery —— ORIG T 0x084255bb
// 逐条把 SIG_USER_PLAY_TIME_STATISTIC 拼成
//   "(%s,%d,%d,now())"（accId, channel_no, count），条目间逗号分隔。
// 末尾返回 1。
// ============================================================================
// [b28d-62] AE：P214③ char buf={0} 复现 rep stos（去 memset call）。ORIG 0x084255bb
int CreateUserPlayTimeQuery(std::string& query, SIG_USER_PLAY_TIME_STATISTIC* sig)
{
    // [b28d-62] P214③：ORIG 0x084255d7 `rep stos`（0x100 dword）⇒ 源形
    //   `char buf[N] = {0}`；显式 memset 会退化为 call。
    char buf[0x400] = {0};
    for (int i = 0; i < sig->m_count; ++i)
    {
        sprintf(buf, "(%s,%d,%d,now())",
                NumberToString(sig->m_entries[i].m_accId, 0),
                G_CEnvironment()->get_channel_no(),
                sig->m_entries[i].m_playSec);
        query += buf;
        if (i < sig->m_count - 1)
        {
            query += ",";
        }
    }
    return 1;
}

// ============================================================================
// ConvertHHMMStringToInt —— ORIG T 0x088c2748
// 形参按值 string；长度 != 5 返回 0；否则 h=atoi(substr(0,2))、
// m=atoi(substr(3,2))，返回 1。
// ============================================================================
int ConvertHHMMStringToInt(std::string s, unsigned char& h, unsigned char& m)
{
    if (s.size() != 5)
    {
        return 0;
    }
    h = static_cast<unsigned char>(atoi(s.substr(0, 2).c_str()));
    m = static_cast<unsigned char>(atoi(s.substr(3, 2).c_str()));
    return 1;
}
