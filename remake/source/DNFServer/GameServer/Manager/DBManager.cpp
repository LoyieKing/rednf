// df_manager_r — CDBManager（ORIG DBManager.cpp）
#include "DBManager.h"

#include <stdio.h>

#include "DNFMySql.h"
#include "DNFTableBase.h"

CDBManager::CDBManager()
{
    m_app = 0;
    m_handles[1] = 0;
    m_handles[2] = 0;
    m_handles[4] = 0;
}

CDBHandle* CDBManager::GetDBHandle(ENUM_DB_HANDLE_IDX idx)
{
    return m_handles[idx];
}

CDBManager::~CDBManager()
{
    Close();
    for (int i = 0; i <= 0x10; i++)
    {
        if (m_handles[i])
        {
            delete m_handles[i];
            m_handles[i] = 0;
        }
    }
}

void CDBManager::Open(ENUM_DB_HANDLE_IDX idx, const char* host, const char* user,
                      const char* pass, const char* db)
{
    ((CMySql*)m_handles[idx])->open(host, user, pass, db);
}

void CDBManager::Close()
{
    for (int i = 0; i <= 0x10; i++)
    {
        if (m_handles[i])
            m_handles[i]->close();
    }
}

bool CDBManager::UpdateQueryCount(unsigned int idx, int count, int time)
{
    bool b;
    CDBHandle* h = m_handles[4];
    if (count <= 0)
        return 0;
    h->set_query(0x4e2c,
                 "inSert into log_query_stat(occ_time,q_id,total,response_time) values(now(),%d,%d,%d)",
                 idx, count, time);
    b = h->exec(0x4e2c);
    if (b == 0)
        return 0;
    return 1;
}

char CDBManager::SelectTest()
{
    bool b;
    int j = 0;
    int i = 0;
    CDBHandle* h = m_handles[2];
    if (!h->set_query(0x4e21,
                      "seLect m_id, charac_no from charac_info where m_id = 1001024"))
    {
        puts("select login_status, m_channel_no from login_account");
        return 0;
    }
    b = h->exec(0x4e21);
    if (b == 0)
        return 0;
    b = h->fetch();
    if (b == 0)
        return 0;
    b = h->get_uint(0, (unsigned int&)j);
    if (b == 0)
        return 0;
    b = h->get_uint(1, (unsigned int&)i);
    return 1;
}

void CDBManager::Init(ENUM_DB_KIND kind, CApplication* app)
{
    m_app = app;
    if (kind == 1)
    {
        for (int i = 0; i <= 0x10; i++)
        {
            m_handles[i] = new (std::nothrow) CMySql;
            if (!m_handles[i])
                throw CDNFException("CDBManager::Init() DB Handle New Error\n");
            if (!m_handles[i]->init())
                throw CDNFException("CDBManager::Init() DB Handle Error\n");
        }
    }
    else
        throw CDNFException("CDBManager::Init() DB Handle Create Error\n");
}
