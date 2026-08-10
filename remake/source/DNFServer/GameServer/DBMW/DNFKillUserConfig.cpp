// df_dbmw_r - DNFKillUserConfig (ORIG DNFKillUserConfig.cpp)
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
CKillUSRConfig::CKillUSRConfig() {}
CKillUSRConfig::~CKillUSRConfig()
{
    Clear_Table();
}
int CKillUSRConfig::Load_Table(const std::string& fileName)
{
    int n = Load_Txt_Table_Data(fileName.c_str(), 0x64);
    if (n > 0 && n <= 0x64)
        return n;
    CMyFileLog log("Load_Table", 0x5b);
    log("./log/TableError", "Kill USR Config Table - ReturnCode = %d\n", n);
    throw CDNFException(
        std::string("CKillUSRConfig::Load_Setup_Table() Exception break!"));
}
int CKillUSRConfig::Parse_Table(char* data, int size)
{
    if (data[0] == '#')
        return 0;
    char* fields[4];
    if (DNFFLib::ExplodeString(data, " \t\r\n\"", fields, 4) == 4)
    {
        ST_KillUSRConfig* kc = new (std::nothrow) ST_KillUSRConfig;
        if (!kc)
            return 0;
        kc->m_type = atoi(fields[0]);
        kc->m_field4 = atoi(fields[1]);
        kc->m_field8 = atoi(fields[2]);
        kc->m_fieldC = atoi(fields[3]);
        m_list.push_back(kc);
        return 1;
    }
    return 0;
}
void CKillUSRConfig::Clear_Table()
{
    if (m_list.empty())
        return;
    for (std::vector<ST_KillUSRConfig*>::iterator it = m_list.begin();
         it != m_list.end(); ++it)
    {
        ST_KillUSRConfig* p = *it;
        delete p;
        p = 0;
    }
    m_list.clear();
}
void* CKillUSRConfig::GetInfo() const { return (void*)&m_list; }
ST_KillUSRConfig::ST_KillUSRConfig()
{
    *(int*)((char*)this + 0) = 0;
    *(int*)((char*)this + 4) = 0;
    *(int*)((char*)this + 8) = 0;
    *(int*)((char*)this + 0xc) = 0;
}
