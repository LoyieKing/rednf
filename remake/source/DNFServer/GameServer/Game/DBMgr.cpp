// df_game_r 还原 —— DBMgr（D4 批次）
// 参照 docs/class_func_reports/DBMgr.md；源文件 DBMgr.cpp。
#include "DBMgr.h"

#include <new>

#include "CQueryCounter.h"
#include "GameBase.h"

DBMgr::DBMgr()
{
    m_dbHandle = new MySQL[0x11];
    if (m_dbHandle == 0)
    {
        cMyTrace trace("DBMgr::DBMgr()", 0x301, 5);
        trace("[%s][%d]", "DBMgr::DBMgr()", 0x301);
    }
    for (int i = 0; i < 0x11; i = i + 1)
    {
        m_dbHandle[i].init();
    }
    m_bActiveGlobalDB = false;
    m_userIdx = -1;
}

DBMgr::~DBMgr()
{
    Close();
    delete[] m_dbHandle;
    for (std::map<ENUM_SERVER_GROUP, std::map<ENUM_DB_HANDLE_IDX, MySQL*> >::iterator outer =
             m_dbHandleMap.begin();
         outer != m_dbHandleMap.end(); ++outer)
    {
        for (std::map<ENUM_DB_HANDLE_IDX, MySQL*>::iterator inner = outer->second.begin();
             inner != outer->second.end(); ++inner)
        {
            MySQL* p = inner->second;
            if (p != 0)
            {
                delete p;
            }
        }
    }
}

bool DBMgr::Open(ENUM_DB_HANDLE_IDX idx, char* host, char* db, char* user,
                 char* passwd, ENUM_SERVER_GROUP group)
{
    if (group != 0)
    {
        if (m_dbHandleMap[group][idx] != 0)
        {
            return false;
        }
        m_dbHandleMap[group][idx] = new MySQL;
        if (!m_dbHandleMap[group][idx]->init())
        {
            return false;
        }
        if (!m_dbHandleMap[group][idx]->open(host, db, user, passwd))
        {
            return false;
        }
        return true;
    }
    bool ret = m_dbHandle[idx].open(host, db, user, passwd);
    return ret;
}

bool DBMgr::Open(ENUM_DB_HANDLE_IDX idx, char* host, unsigned int port,
                 char* db, char* user, char* passwd, ENUM_SERVER_GROUP group)
{
    if (group != 0)
    {
        if (m_dbHandleMap[group][idx] != 0)
        {
            return false;
        }
        m_dbHandleMap[group][idx] = new MySQL;
        if (!m_dbHandleMap[group][idx]->init())
        {
            return false;
        }
        if (!m_dbHandleMap[group][idx]->open(host, port, db, user, passwd))
        {
            return false;
        }
        return true;
    }
    bool ret = m_dbHandle[idx].open(host, port, db, user, passwd);
    return ret;
}

void DBMgr::Close()
{
    for (int i = 0; i < 0x11; i = i + 1)
    {
        m_dbHandle[i].close();
    }
}

void DBMgr::Close(ENUM_DB_HANDLE_IDX idx)
{
    if (idx >= 0 && idx < 0x11)
    {
        m_dbHandle[idx].close();
    }
}

int DBMgr::GetServerType(int userIdx)
{
    __gnu_cxx::hash_map<int, stUserDBInfo_t>::const_iterator it =
        m_userDBInfoMap.find(userIdx);
    if (it != m_userDBInfoMap.end())
    {
        return it->second.m_serverGroup;
    }
    return 0;
}

void DBMgr::addUserDBInfo(int userIdx, const stUserDBInfo_t& info)
{
    std::pair<__gnu_cxx::hash_map<int, stUserDBInfo_t>::const_iterator, bool> ins =
        m_userDBInfoMap.insert(std::make_pair(userIdx, info));
    if (!ins.second)
    {
        m_userDBInfoMap[userIdx] = info;
    }
}

void DBMgr::removeUserDBInfo(int userIdx)
{
    if (GlobalData::s_db_mgr->isActiveGlobalDB() != true)
    {
        return;
    }
    m_userDBInfoMap.erase(userIdx);
}

MySQL* DBMgr::GetDBHandleServerGroup(ENUM_DB_HANDLE_IDX idx, ENUM_SERVER_GROUP group)
{
    if (group == 0)
    {
        __gnu_cxx::hash_map<int, stUserDBInfo_t>::const_iterator it =
            m_userDBInfoMap.find(m_userIdx);
        if (it == m_userDBInfoMap.end())
        {
            if (m_userIdx == -1)
            {
                return &m_dbHandle[idx];
            }
            return 0;
        }
        group = (ENUM_SERVER_GROUP)it->second.m_serverGroup;
    }
    return m_dbHandleMap[group][idx];
}

void DBMgr::SetQueryCounterPointer(CQueryCounter* pCounter)
{
    for (std::map<ENUM_SERVER_GROUP, std::map<ENUM_DB_HANDLE_IDX, MySQL*> >::iterator outer =
             m_dbHandleMap.begin();
         outer != m_dbHandleMap.end(); ++outer)
    {
        for (std::map<ENUM_DB_HANDLE_IDX, MySQL*>::iterator inner = outer->second.begin();
             inner != outer->second.end(); ++inner)
        {
            MySQL* p = inner->second;
            if (p != 0)
            {
                p->SetQueryCounter(pCounter);
            }
        }
    }
}

MySQL* DBMgr::GetDBHandle(ENUM_DB_HANDLE_IDX idx, ENUM_SERVER_GROUP group)
{
    MySQL* base = m_dbHandle;
    if (!m_bActiveGlobalDB)
    {
        return &m_dbHandle[idx];
    }
    int fallbackIdx = 4;
    if (idx == 2 || idx == 3)
    {
        MySQL* p = GetDBHandleServerGroup(idx, group);
        if (p == 0)
        {
            return &base[4];
        }
        return p;
    }
    return &base[idx];
}
