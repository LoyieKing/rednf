// df_game_r 还原 —— CQueryCounter（D4 批次）
// 参照 docs/class_func_reports/CQueryCounter.md；源文件 QueryCounter.cpp。
#include "CQueryCounter.h"

#include <stdio.h>
#include <string.h>
#include <time.h>

#include "CStreamGuard.h"
#include "GameBase.h"
#include "MsgQueueMgr.h"
#include "StreamPool.h"

// ---- ORIG 全局工具函数（属 DB 工具 TU，0x083fbc66）----
int GetIdentityFromDB(MySQL* mysql);

CQueryCounter::CQueryCounter()
{
    m_current = m_queryInfoMap.end();
    m_totalQueryCount = 0;
}

CQueryCounter::~CQueryCounter()
{
}

void CQueryCounter::Reset()
{
    for (std::map<int, QUERY_INFO>::iterator it = m_queryInfoMap.begin();
         it != m_queryInfoMap.end(); ++it)
    {
        it->second.count = 0;
        it->second.responseTime = 0.0;
    }
    m_totalQueryCount = 0;
}

void CQueryCounter::IncreaseQueryCount(const std::string& query)
{
    if (query.find("log_query_ref", 0) != std::string::npos)
    {
        return;
    }
    if (query.find("from db_connect", 0) != std::string::npos)
    {
        return;
    }
    std::map<std::string, std::map<int, QUERY_INFO>::iterator>::iterator it =
        m_queryIDMap.find(query);
    if (it != m_queryIDMap.end())
    {
        m_current = it->second;
        if (m_current->second.count == 0)
        {
            m_totalQueryCount = m_totalQueryCount + 1;
        }
        m_current->second.count = m_current->second.count + 1;
    }
    else if (!m_bLogDB)
    {
        int qid = 0;
        if (SaveQuery(query.c_str(), qid))
        {
            RegisterQuery(query.c_str(), qid, 0);
        }
    }
    else
    {
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire("QueryCounter.cpp", 0x1d2), true);
        **guard << 0xbe;
        **guard << -1;
        **guard << (int)query.size();
        guard->put_binary((void*)query.c_str(), (int)query.size());
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    }
}

void CQueryCounter::UpdateQueryCounter()
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("QueryCounter.cpp", 0x1f1), true);
    **guard << 0x7d;
    **guard << -1;
    int curTime = time(0);
    **guard << curTime;
    **guard << m_totalQueryCount;
    for (std::map<int, QUERY_INFO>::iterator it = m_queryInfoMap.begin();
         it != m_queryInfoMap.end(); ++it)
    {
        int qid = it->first;
        int count = it->second.count;
        int response = (int)(it->second.responseTime * 1000.0);
        if (count > 0)
        {
            **guard << qid;
            **guard << count;
            **guard << response;
        }
    }
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q, guard);
    Reset();
}

void CQueryCounter::SetResponseTime(double responseTime)
{
    if (m_current != m_queryInfoMap.end())
    {
        m_current->second.responseTime += responseTime;
        m_current = m_queryInfoMap.end();
    }
}

bool CQueryCounter::LoadQueryTable()
{
    MySQL* mysql = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6, E_SERVER_GROUP_NONE);
    int ret = 0;
    char query[0x6001];
    mysql->set_query("seLect q_id,query from log_query_ref");
    if (!mysql->exec(true))
    {
        return false;
    }
    int nRows = mysql->get_n_rows();
    for (int i = 0; i < nRows; i = i + 1)
    {
        int qid = 0;
        memset(query, 0, 0x6001);
        if (!mysql->fetch())
        {
            cMyTrace trace("bool CQueryCounter::LoadQueryTable()", 0x22f, 5);
            trace("LoadQueryTable, fatch ERROR");
            return false;
        }
        if (!mysql->get_int(0, qid))
        {
            cMyTrace trace("bool CQueryCounter::LoadQueryTable()", 0x235, 5);
            trace("LoadQueryTable, get_int(0) ERROR");
            return false;
        }
        if (!mysql->get_str(1, query, 0x6000))
        {
            cMyTrace trace("bool CQueryCounter::LoadQueryTable()", 0x23b, 5);
            trace("LoadQueryTable, get_str(1) ERROR");
            return false;
        }
        if (!RegisterQuery(query, qid, 0))
        {
            return false;
        }
    }
    return true;
}

bool CQueryCounter::SaveQuery(const char* query, int& qid)
{
    MySQL* mysql = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6, E_SERVER_GROUP_NONE);
    char escaped[0xc001];
    memset(escaped, 0, 0xc001);
    mysql->escape_string(escaped, query);
    mysql->set_query("inSert into log_query_ref(query, query_hash) values('%s',password('%s'))",
                     escaped, escaped);
    if (mysql->exec(true))
    {
        qid = GetIdentityFromDB(mysql);
        return true;
    }
    mysql->set_query("seLect q_id from log_query_ref where query_hash=password('%s')",
                     escaped, escaped);
    if (mysql->exec(true))
    {
        if (mysql->get_n_rows() == 0)
        {
            return false;
        }
        if (mysql->fetch())
        {
            if (mysql->get_int(0, qid))
            {
                return true;
            }
            cMyTrace trace("bool CQueryCounter::SaveQuery(const char*, int&)", 0x277, 5);
            trace("SaveQuery, get_int(0) ERROR");
            return false;
        }
        cMyTrace trace("bool CQueryCounter::SaveQuery(const char*, int&)", 0x271, 5);
        trace("SaveQuery, fetch() ERROR");
        return false;
    }
    cMyTrace trace("bool CQueryCounter::SaveQuery(const char*, int&)", 0x265, 5);
    trace("SaveQuery, exec() ERROR");
    return false;
}

bool CQueryCounter::RegisterQuery(const char* query, int qid, int type)
{
    QUERY_INFO info;
    info.count = type;
    std::pair<std::map<int, QUERY_INFO>::iterator, bool> ins =
        m_queryInfoMap.insert(std::make_pair(qid, info));
    if (!ins.second)
    {
        return false;
    }
    std::pair<std::map<std::string, std::map<int, QUERY_INFO>::iterator>::iterator, bool> ins2 =
        m_queryIDMap.insert(std::make_pair(query, ins.first));
    if (!ins2.second)
    {
        cMyTrace trace("bool CQueryCounter::RegisterQuery(const char*, int, int)", 0x291, 0);
        trace("LoadQueryTable,  query_id_map_.insert ERROR");
        return false;
    }
    return true;
}
