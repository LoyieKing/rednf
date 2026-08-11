// df_coserver_r — CServerConfig/ST_ServerInfo（ORIG DNFServerConfig.cpp）
#include <stdio.h>
#include <stdlib.h>

#include "DNFServerConfig.h"
#include "DNFFileLog.h"
#include "DNFFunctionLib.h"

CServerConfig::CServerConfig()
{
}

CServerConfig::~CServerConfig()
{
}

bool CServerConfig::Parse_Table(char* line, int idx)
{
    if (line[0] == '#')
    {
        return 0;
    }
    char* tok[5];
    char* tok5;
    (void)tok5;
    if (DNFFLib::ExplodeString(line, " \t\r\n\"", tok, 5) == 5)
    {
        if (idx < 0x649b)
        {
            ST_ServerInfo* s = &m_servers[idx];
            s->m_field0 = (char)atoi(tok[0]);
            s->m_field1 = (char)atoi(tok[1]);
            s->m_field2 = (char)atoi(tok[2]);
            s->m_string = tok[3];
            s->m_ushort = (unsigned short)atoi(tok[4]);
            return 1;
        }
    }
    return 0;
}

void CServerConfig::Load_Table(const std::string& path)
{
    int rc = Load_Txt_Table_Data(path.c_str(), 0x649b);
    if (0 < rc && rc < 0x649b)
    {
        return;
    }
    DNF_LOG_SCOPE_LINE(0x39, "./log/TableError", "Server Config Table - ReturnCode = %d\n", rc);
    throw CDNFException("CServerConfig::Load_Setup_Table() Exception Break!");
}

ST_ServerInfo* CServerConfig::GetServerInfo()
{
    return m_servers;
}

ST_ServerInfo::ST_ServerInfo()
    : m_field0(0), m_field1(0), m_field2(0xff), m_ushort(0)
{
}

ST_ServerInfo::~ST_ServerInfo()
{
}
