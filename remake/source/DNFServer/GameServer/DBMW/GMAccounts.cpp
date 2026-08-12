// df_dbmw_r - GMAccounts (ORIG GMAccounts.cpp)
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
int get_day_interval(struct tm* a, struct tm* b);

// ---- CGuildManager / WongWork ----
WongWork::CGMAccounts::CGMAccounts() {}
WongWork::CGMAccounts::~CGMAccounts() {}
bool WongWork::CGMAccounts::stGMInfo_t::operator==(const stGMInfo_t& other) const
{
    return m_field0 == other.m_field0;
}
int WongWork::CGMAccounts::loadGMAccounts(char const* path) { return 1; }
bool WongWork::CGMAccounts::isGM(unsigned int id)
{
    stGMInfo_t key = {};
    key.m_field1 = 3;
    key.m_field0 = (int)id;
    std::list<stGMInfo_t>::iterator it =
        std::find(m_list.begin(), m_list.end(), key);
    return it != m_list.end();
}
WongWork::CGMAccounts::stGMInfo_t WongWork::CGMAccounts::getGMInfo(
    unsigned int id) const
{
    stGMInfo_t key = {};
    key.m_field1 = 3;
    key.m_field0 = (int)id;
    std::list<stGMInfo_t>::const_iterator it =
        std::find(m_list.begin(), m_list.end(), key);
    stGMInfo_t result;
    if (it != m_list.end())
    {
        result = *it;
    }
    else
    {
        result.m_field0 = 0;
        result.m_field1 = 3;
    }
    return result;
}
int WongWork::CGMAccounts::appendGM(unsigned int id, unsigned int flag)
{
    char pad[0x10];
    return 0;
}
int WongWork::CGMAccounts::removeGM(unsigned int id, unsigned int flag)
{
    char pad[0x20];
    return 0;
}
void WongWork::CGMAccounts::clearGmList() { m_list.clear(); }
void WongWork::CGMAccounts::LoadGmList(unsigned int idx, int flag)
{
    stGMInfo_t info;
    info.m_field0 = (int)idx;
    info.m_field1 = flag;
    m_list.push_back(info);
}
void WongWork::CGMAccounts::AppendGM_Sys(unsigned int id, char flag)
{
    stGMInfo_t info = {};
    info.m_field0 = (int)id;
    info.m_field1 = (int)flag;
    m_list.push_back(info);
    char* mid = NumberToString(id, 0);
    CMyFileLog log(__FUNCTION__, 0xcd);
    log("./log/Init", "GM List Add mid:%s", mid);
}
