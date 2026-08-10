// df_manager_r — CServerConfig / ST_ServerInfo（ORIG DNFServerConfig.cpp）
#include "DNFServerConfig.h"

#include <stdio.h>
#include <stdlib.h>

#include "DNFTableBase.h"
#include "DNFFileLog.h"
#include "DNFFunctionLib.h"

ST_ServerInfo::ST_ServerInfo()
{
    m_index = 0;
    m_type = 0xff;
    m_port = 0;
}

ST_ServerInfo::~ST_ServerInfo() {}

CServerConfig::CServerConfig() {}
CServerConfig::~CServerConfig() {}

int CServerConfig::Load_Table(const std::string& fileName)
{
    int n = Load_Txt_Table_Data(fileName.c_str(), 0x65);
    if (n > 0 && n <= 0x64)
        return 1;
    CMyFileLog log("Load_Table", 0x38);
    log("./log/TableError.log", "Server Config Table - ReturnCode = %d\n", n);
    throw CDNFException("CServerConfig::Load_Setup_Table() Exception Break!");
}
int CServerConfig::Parse_Table(char* data, int size)
{
    if (data[0] == '#')
        return 0;
    char* fields[4];
    if (DNFFLib::ExplodeString(data, " \t\r\n\"", fields, 4) == 4 && size <= 0x64)
    {
        ST_ServerInfo* info = &m_servers[size];
        info->m_index = (char)atoi(fields[0]);
        info->m_type = (char)atoi(fields[1]);
        info->m_name = fields[2];
        info->m_port = (unsigned short)atoi(fields[3]);
        return 1;
    }
    return 0;
}

void* CServerConfig::GetServerInfo() { return &m_servers; }
