// df_coserver_r — CKillUSRConfig/ST_KillUSRConfig（ORIG DNFKillUserConfig.cpp）
#include <stdio.h>
#include <stdlib.h>

#include "DNFKillUserConfig.h"
#include "DNFFileLog.h"
#include "DNFFunctionLib.h"

ST_KillUSRConfig::ST_KillUSRConfig()
{
    m_field0 = 0;
    m_field1 = 0;
    m_field2 = 0;
    m_field3 = 0;
}

CKillUSRConfig::CKillUSRConfig()
{
}

CKillUSRConfig::~CKillUSRConfig()
{
    Clear_Table();
}

void CKillUSRConfig::Clear_Table()
{
    if (!m_infos.empty())
    {
        for (std::vector<ST_KillUSRConfig*>::iterator it = m_infos.begin(); it != m_infos.end();
             ++it)
        {
            ST_KillUSRConfig* p = *it;
            ::operator delete(p);
            p = 0;
        }
        m_infos.clear();
    }
}

bool CKillUSRConfig::Parse_Table(char* line, int idx)
{
    if (line[0] == '#')
    {
        return 0;
    }
    char* tok0;
    char* tok1;
    char* tok2;
    char* tok3;
    if (DNFFLib::ExplodeString(line, " \t\r\n\"", &tok0, 4) == 4)
    {
        ST_KillUSRConfig* p = new (std::nothrow) ST_KillUSRConfig;
        if (p != 0)
        {
            p->m_field0 = atoi(tok0);
            p->m_field1 = atoi(tok1);
            p->m_field2 = atoi(tok2);
            p->m_field3 = atoi(tok3);
            m_infos.push_back(p);
            return 1;
        }
    }
    return 0;
}

void CKillUSRConfig::Load_Table(const std::string& path)
{
    int rc = Load_Txt_Table_Data(path.c_str(), 100);
    if (0 < rc && rc < 0x65)
    {
        return;
    }
    DNF_LOG_SCOPE_LINE(0x5b, "./log/TableError", "Kill USR Config Table - ReturnCode = %d\n", rc);
    throw CDNFException("CKillUSRConfig::Load_Setup_Table() Exception break!");
}

std::vector<ST_KillUSRConfig*>* CKillUSRConfig::GetInfo() const
{
    return const_cast<std::vector<ST_KillUSRConfig*>*>(&m_infos);
}
