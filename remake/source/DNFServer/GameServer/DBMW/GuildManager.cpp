// df_dbmw_r - GuildManager (ORIG GuildManager.cpp)
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
STGuildWarRankInfo::STGuildWarRankInfo()
{
    m_field0 = 0;
    m_field4 = 0;
    m_field8 = 0;
    memset(m_name, 0, 0x17);
}
STGuildRankInfo::STGuildRankInfo()
{
    m_field0 = 0;
    m_field4 = 0;
    m_field8 = 0;
}
CGuildManager::CGuildManager() {}
CGuildManager::~CGuildManager() { clear(); }
void CGuildManager::clear()
{
    if (m_rankList.empty())
        return;
    for (std::vector<std::pair<unsigned int, STGuildRankInfo*> >::iterator it =
             m_rankList.begin();
         it != m_rankList.end(); ++it)
    {
        std::pair<unsigned int, STGuildRankInfo*> p = *it;
        delete p.second;
    }
    m_rankList.clear();
}
void CGuildManager::clearGuildWar()
{
    if (m_warRankList.empty())
        return;
    for (std::vector<std::pair<unsigned int, STGuildWarRankInfo*> >::iterator it =
             m_warRankList.begin();
         it != m_warRankList.end(); ++it)
    {
        std::pair<unsigned int, STGuildWarRankInfo*> p = *it;
        delete p.second;
    }
    m_warRankList.clear();
}
std::vector<std::pair<unsigned int, STGuildWarRankInfo*> >*
CGuildManager::GetVtGuildWarRankInfo()
{
    return &m_warRankList;
}
STGuildMemberProxy* CGuildManager::GetArrayTempGuildMemberList()
{
    return m_members;
}
bool GuildWarPairDataCompare(const std::pair<unsigned int, STGuildWarRankInfo*>& a,
                             const std::pair<unsigned int, STGuildWarRankInfo*>& b)
{
    return a.second->m_field4 < b.second->m_field4;
}
bool CPairDataCompare::operator()(
    const std::pair<unsigned int, STGuildRankInfo*>& a,
    const std::pair<unsigned int, STGuildRankInfo*>& b) const
{
    return keyLess(a.first, b.first);
}
bool CPairDataCompare::keyLess(const unsigned int& a, const unsigned int& b) const
{
    return a > b;
}
bool CPairDataGuildWarCompare::operator()(
    const std::pair<unsigned int, STGuildWarRankInfo*>& a,
    const std::pair<unsigned int, STGuildWarRankInfo*>& b) const
{
    return keyLess(a.first, b.first);
}
bool CPairDataGuildWarCompare::keyLess(const unsigned int& a,
                                       const unsigned int& b) const
{
    return a > b;
}
STGuildWarRankInfo* CGuildManager::GetFirstRankGuild()
{
    if (m_warRankList.empty())
        return 0;
    return m_warRankList.begin()->second;
}
void CGuildManager::insertGuildWar(STGuildWarRankInfo* info)
{
    if (info)
        m_warRankList.push_back(std::make_pair(info->m_field4, info));
}
unsigned int CGuildManager::getFirstGuildOfGuildWar()
{
    if (m_warRankList.empty())
        return 0;
    return (*std::max_element(m_warRankList.begin(), m_warRankList.end(),
                              GuildWarPairDataCompare))
        .second->m_field0;
}
char CGuildManager::InitGuildWarPointList()
{
    if (m_warRankList.empty())
        return 0;
    int count = 0;
    for (std::vector<std::pair<unsigned int, STGuildWarRankInfo*> >::iterator it =
             m_warRankList.begin();
         it != m_warRankList.end(); ++it)
    {
        it->second->m_field4 = 0x3e8;
        if (++count > 0xa)
            break;
    }
    return 1;
}
char CGuildManager::rank()
{
    if (m_rankList.empty())
        return 0;
    std::sort(m_rankList.begin(), m_rankList.end(), CPairDataCompare());
    int r = 0;
    for (std::vector<std::pair<unsigned int, STGuildRankInfo*> >::iterator it =
             m_rankList.begin();
         it != m_rankList.end(); ++it)
    {
        STGuildRankInfo* p = (*it).second;
        p->m_field8 = ++r;
    }
    return 1;
}
char CGuildManager::rankGuildWar()
{
    if (m_warRankList.empty())
        return 0;
    std::sort(m_warRankList.begin(), m_warRankList.end(),
              CPairDataGuildWarCompare());
    int r = 0;
    for (std::vector<std::pair<unsigned int, STGuildWarRankInfo*> >::iterator it =
             m_warRankList.begin();
         it != m_warRankList.end(); ++it)
    {
        STGuildWarRankInfo* p = (*it).second;
        p->m_field8 = ++r;
    }
    return 1;
}
void CGuildManager::insert(STGuildRankInfo* info)
{
    if (info)
        m_rankList.push_back(std::make_pair(info->m_field4, info));
}
std::vector<std::pair<unsigned int, STGuildRankInfo*> >*
CGuildManager::GetVtGuildRankInfo()
{
    return &m_rankList;
}
void CGuildManager::printGuildWarRank()
{
    if (m_warRankList.empty())
        return;
    for (std::vector<std::pair<unsigned int, STGuildWarRankInfo*> >::iterator it =
             m_warRankList.begin();
         it != m_warRankList.end(); ++it)
    {
        std::pair<unsigned int, STGuildWarRankInfo*> p = *it;
        CMyFileLog(__FUNCTION__, 0x10a)("./log/GuildWar",
            "GuildKey : %d,  GuildWarPoint : %d, Guild Rank : %d",
            p.second->m_field0, p.second->m_field4, p.second->m_field8);
    }
}
// ORIG 将 info 视为 0x23 字节记录的数组（字段 +0/+4/+8[0x16]/+0x1f，packed）。
struct ST_Guild_War_Info {
    unsigned int m_field0;
    unsigned int m_field4;
    char m_field8[0x16];
    char m_pad1e;
    unsigned int m_field1f;
} __attribute__((packed));

void CGuildManager::GetGuildWarEnterableRank(ST_Guild_War_Info* info)
{
    if (m_warRankList.empty())
        return;
    int i = 0;
    for (std::vector<std::pair<unsigned int, STGuildWarRankInfo*> >::iterator it =
             m_warRankList.begin();
         it != m_warRankList.end(); ++it)
    {
        info[i].m_field0 = it->second->m_field0;
        info[i].m_field4 = it->second->m_field4;
        memcpy(info[i].m_field8, (char*)it->second + 0xc, 0x16);
        info[i].m_field1f = it->second->m_field24;
        if (++i > 9)
            break;
    }
    printGuildWarRank();
}
STGuildMemberProxy::STGuildMemberProxy()
{
    m_no = 0;
    m_field22 = 0xff;
    m_field23 = 0xff;
    m_field24 = 0xffff;
    m_field26 = 0;
    m_field27 = 0;
    m_field28 = 0;
    memset(m_name, 0, 0x1e);
    memset(m_data2c, 0, 0x15);
}
