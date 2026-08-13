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
    m_guildId = 0;
    m_guildWarPoint = 0;
    m_rank = 0;
    memset(m_name, 0, 0x17);
}
STGuildRankInfo::STGuildRankInfo()
{
    m_guildId = 0;
    m_guildPoint = 0;
    m_rank = 0;
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
    return a.second->m_guildWarPoint < b.second->m_guildWarPoint;
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
        m_warRankList.push_back(std::make_pair(info->m_guildWarPoint, info));
}
unsigned int CGuildManager::getFirstGuildOfGuildWar()
{
    if (m_warRankList.empty())
        return 0;
    return (*std::max_element(m_warRankList.begin(), m_warRankList.end(),
                              GuildWarPairDataCompare))
        .second->m_guildId;
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
        it->second->m_guildWarPoint = 0x3e8;
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
        p->m_rank = ++r;
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
        p->m_rank = ++r;
    }
    return 1;
}
void CGuildManager::insert(STGuildRankInfo* info)
{
    if (info)
        m_rankList.push_back(std::make_pair(info->m_guildPoint, info));
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
            p.second->m_guildId, p.second->m_guildWarPoint, p.second->m_rank);
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
        info[i].m_field0 = it->second->m_guildId;
        info[i].m_field4 = it->second->m_guildWarPoint;
        memcpy(info[i].m_field8, (char*)it->second + 0xc, 0x16);
        info[i].m_field1f = it->second->m_guildPointPrev;
        if (++i > 9)
            break;
    }
    printGuildWarRank();
}
STGuildMemberProxy::STGuildMemberProxy()
{
    m_no = 0;
    m_job = 0xff;
    m_growType = 0xff;
    m_lev = 0xffff;
    m_sex = 0;
    m_grade = 0;
    m_lastPlayTime = 0;
    memset(m_name, 0, 0x1e);
    memset(m_memo, 0, 0x15);
}
