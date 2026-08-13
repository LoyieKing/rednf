// df_statics_r — DNFServerConfig（ORIG DNFServerConfig.o 拆分）
#include <stdio.h>
#include <stdlib.h>

#include "DNFServerConfig.h"
#include "DNFTableBase.h"
#include "DNFFileLog.h"
#include "DNFFunctionLib.h"

bool CServerConfig::Parse_Table(char* line, int idx)
{
    if (line[0] == '#')
    {
        return 0;
    }
    char* tok[5];
    int n;  // 死局部：与 ORIG 栈布局对齐（tok 槽位 -0x24..-0x14）
    if (DNFFLib::ExplodeString(line, " \t\r\n\"", tok, 5) == 5)
    {
        if (idx < 0xff)
        {
            ST_ServerInfo* s = &m_servers[idx];
            s->m_serverType = (char)atoi(tok[0]);
            s->m_id = (char)atoi(tok[1]);
            s->m_serverIndex = (char)atoi(tok[2]);
            s->m_string = tok[3];
            s->m_ushort = (unsigned short)atoi(tok[4]);
            return 1;
        }
    }
    return 0;
}
void CServerConfig::Load_Table(const std::string& path)
{
    int rc = Load_Txt_Table_Data(path.c_str(), 0xff);
    if (0 < rc && rc < 0xff)
    {
        return;
    }
    DNF_LOG_SCOPE_LINE(0x39, "./log/TableError", "Server Config Table - ReturnCode = %d\n", rc);
    throw CDNFException("CServerConfig::Load_Setup_Table() Exception Break!");
}
ST_ServerInfo::~ST_ServerInfo()
{
}
CServerConfig::~CServerConfig()
{
}
ST_ServerInfo* CServerConfig::GetServerInfo()
{
    return m_servers;
}
ST_ServerInfo::ST_ServerInfo()
    : m_serverType(0), m_id(0), m_serverIndex(0xff), m_ushort(0)
{
}
CServerConfig::CServerConfig()
{
}
