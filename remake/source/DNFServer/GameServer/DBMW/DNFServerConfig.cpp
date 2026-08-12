// df_dbmw_r - DNFServerConfig (ORIG DNFServerConfig.cpp)
#include "DBMWCommon.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <algorithm>
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/resource.h>
#include <signal.h>
#include <fcntl.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <sys/times.h>

#include "DNFFileLog.h"
#include "DNFFunctionLib.h"

int getErrno();

// CTableBase / CAppInit / CAppConfig / CServerConfig
CServerConfig::CServerConfig() {}
CServerConfig::~CServerConfig() {}
int CServerConfig::Load_Table(const std::string& fileName)
{
    int n = Load_Txt_Table_Data(fileName.c_str(), 0xff);
    if (n >= 0 && n <= 0xfe)
        return n;
    CMyFileLog log("Load_Table", 0x39);
    log("./log/TableError.log", "Server Config Table - ReturnCode = %d\n", n);
    throw CDNFException("CServerConfig::Load_Setup_Table() Exception Break!");
}
bool CServerConfig::Parse_Table(char* data, int size)
{
    if (data[0] == '#')
        return 0;
    char* fields[6];
    if (DNFFLib::ExplodeString(data, " \t\r\n\"", fields, 5) == 5)
    {
        if (size <= 0xfe)
        {
            ST_ServerInfo* info = &m_servers[size];
            info->m_type = (char)atoi(fields[0]);
            info->m_flag = (char)atoi(fields[1]);
            info->m_idx = (char)atoi(fields[2]);
            info->m_name = fields[3];
            info->m_port = (unsigned short)atoi(fields[4]);
            return 1;
        }
    }
    return 0;
}
ST_ServerInfo::ST_ServerInfo()
    : m_type(0), m_flag(0), m_idx(0xff), m_name(), m_port(0) {}
ST_ServerInfo::~ST_ServerInfo() {}
void* CServerConfig::GetServerInfo() { return &m_servers; }
