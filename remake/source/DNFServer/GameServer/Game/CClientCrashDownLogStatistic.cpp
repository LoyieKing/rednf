#include "CClientCrashDownLogStatistic.h"
#include "CEnvironment.h"
#include "CServerProxyMgr.h"
#include "CStatisticServerProxy.h"
#include "DBMgr.h"
#include "GlobalData.h"
#include "MySQL.h"
#include "Packet_Reason_Crash_Down_Info.h"

#include <cstdio>
#include <cstring>
#include <ctime>

// ---- ORIG 符号（nm -S --defined-only df_game_r）----
// 类本体：
//   RegisterClientCrashDown(int)  T 0x0860d038
//   CarryClientCrashDown()        T 0x0860d072
//   MakeClientCrashDownQuery(...) T 0x0860d1fe
//   UpdateClientCrashDown()       T 0x0860d444
//   Init()                        W 0x086102d4
//   CClientCrashDownLogStatistic() W 0x086102da
//   ~CClientCrashDownLogStatistic() W 0x08610800
// record：
//   stClientCrashDownLog_t()        W 0x08610846（仅构造 m_reasonCount，不初始化 m_occTime）
//   ~stClientCrashDownLog_t()       W 0x0861085c
//   stClientCrashDownLog_t(const&)  W 0x08612dfa
//   operator=(const&)               W 0x08612ea8
// 报告：docs/class_func_reports/CClientCrashDownLogStatistic/*.md（4 份全读）

CClientCrashDownLogStatistic::stClientCrashDownLog_t::stClientCrashDownLog_t()
    : m_reasonCount()
{
}

CClientCrashDownLogStatistic::stClientCrashDownLog_t::stClientCrashDownLog_t(
    const stClientCrashDownLog_t& other)
    : m_occTime(other.m_occTime), m_reasonCount(other.m_reasonCount)
{
}


CClientCrashDownLogStatistic::stClientCrashDownLog_t::~stClientCrashDownLog_t()
{
}

CClientCrashDownLogStatistic::stClientCrashDownLog_t&
CClientCrashDownLogStatistic::stClientCrashDownLog_t::operator=(
    const stClientCrashDownLog_t& other)
{
    m_occTime = other.m_occTime;
    m_reasonCount = other.m_reasonCount;
    return *this;
}

CClientCrashDownLogStatistic::CClientCrashDownLogStatistic()
    : m_reasonCount(), m_logRecords()
{
}

CClientCrashDownLogStatistic::~CClientCrashDownLogStatistic()
{
}

void CClientCrashDownLogStatistic::Init()
{
}

void CClientCrashDownLogStatistic::RegisterClientCrashDown(int reason)
{
    int v = m_reasonCount[reason];
    m_reasonCount[reason] = ++v;
}

int CClientCrashDownLogStatistic::CarryClientCrashDown()
{
    const time_t now = GlobalData::s_systemTime_.getCurSec();
    if (!m_reasonCount.empty())
    {
        Packet_Reason_Crash_Down_Info packet;
        packet.m_occTime = static_cast<int>(now);
        packet.m_channel = G_CEnvironment()->get_channel_no();
        for (std::map<int, int>::iterator it = m_reasonCount.begin();
             it != m_reasonCount.end(); ++it)
        {
            packet.m_reason = it->first;
            packet.m_cnt = it->second;
            GlobalData::s_statistic_proxy_mgr->GetServerProxy(E_SERVER_GROUP_NONE)
                ->SendPacket(reinterpret_cast<char*>(&packet), 0x1a);
        }

        stClientCrashDownLog_t record;
        record.m_occTime = static_cast<int>(now);
        record.m_reasonCount = m_reasonCount;
        m_logRecords.push_back(record);
        m_reasonCount.clear();
        Init();
    }
    struct tm tmBuf;
    struct tm* pTm = localtime_r(&now, &tmBuf);
    (void)pTm;
    return 1;
}

void CClientCrashDownLogStatistic::MakeClientCrashDownQuery(
    std::vector<std::string>& queries, int queryPrefixLength)
{
    std::string values;
    for (unsigned int i = 0; i < m_logRecords.size(); ++i)
    {
        values.clear();
        for (std::map<int, int>::iterator it = m_logRecords[i].m_reasonCount.begin(),
                                          end = m_logRecords[i].m_reasonCount.end();
             it != end; )
        {
            if (queryPrefixLength <= 0x6000 - static_cast<int>(values.size()))
            {
                if (!values.empty()) values += ",";
                char value[0xff] = { '\0' };
                std::sprintf(value, "(from_unixtime(%d),%d,%d,%d)",
                             m_logRecords[i].m_occTime,
                             G_CEnvironment()->get_channel_no(), it->first,
                             it->second);
                values += value;
                ++it;
            }
            else
                break;
        }
        if (!values.empty()) queries.push_back(values);
    }
}

int CClientCrashDownLogStatistic::UpdateClientCrashDown()
{
    if (!m_logRecords.empty())
    {
        MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_4,
                                                       E_SERVER_GROUP_NONE);
        std::vector<std::string> queries;
        char prefix[] =
            "inSert into log_client_ting(occ_time,channel_no,reason,cnt) values%s";
        MakeClientCrashDownQuery(queries, strlen(prefix));
        for (unsigned int i = 0; i < queries.size(); ++i)
        {
            if (queries[i].size() != 0)
            {
                db->set_query(prefix, queries[i].c_str());
                db->exec(true);
            }
        }
        m_logRecords.clear();
    }
    return 1;
}
// ----------------------------------------------------------------------------
// GetInstanceClientCrashDownLogStatistic —— ORIG T 0x0860cfa5：
// 函数级静态 CClientCrashDownLogStatistic（guard 0x94732e8 / 实例
// 0x9473420，cxa_guard_acquire → C1 → release → __cxa_atexit(~D1)，
// P20 范式），返回静态实例指针。DB_TimerStatisticsLog::dispatch 消费。
// ----------------------------------------------------------------------------
CClientCrashDownLogStatistic* GetInstanceClientCrashDownLogStatistic()
{
    static CClientCrashDownLogStatistic instance;
    return &instance;
}
