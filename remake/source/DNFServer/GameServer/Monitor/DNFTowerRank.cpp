// df_monitor_r — DNFTowerRank（从 MonitorTypes/App/Table 拆分）
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <cerrno>
#include <signal.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <algorithm>

#include "DNFTowerRank.h"
#include "DNFFileLog.h"
#include "DNFFunctionLib.h"
#include "Packet_Monitor_Member_Secede.h"
#include "Packet_GM_Request_Mid.h"
#include "Packet_PvPChannelInfo.h"
#include "Packet_PvPChannelUserCount.h"
#include "Packet_Item_Limit_Edition_Sell_Start.h"
#include "Packet_DBMW_Change_Char_Name.h"
#include "Packet_Monitor_Reply_Charac_Info.h"
#include "DNFServerHandler.h"

CTowerRank::CTowerRank() {}

CTowerRank::~CTowerRank() {}

void CTowerRank::processReloadRanking(CServerHandler* handler, bool flag, unsigned int tick)
{
    time_t t = time(0);
    struct tm* lt = localtime(&t);
    if (flag || (lt->tm_hour == 4 && lt->tm_min == 30))
    {
        handler->queryReloadTowerRank(tick);
    }
}

stTowerRankElement_t::stTowerRankElement_t()
{
    m_job = 0;
    m_score = 0;
}

stTowerRankElement_t::stTowerRankElement_t(unsigned char job, unsigned short score)
{
    m_job = job;
    m_score = score;
}

void CTowerRank::registCharacRank(unsigned int floor, const char* name, unsigned int job,
                                  unsigned int score)
{
    std::multimap<std::string, stTowerRankElement_t>& mm = m_ranks[floor - 1];
    std::multimap<std::string, stTowerRankElement_t>::iterator lo = mm.lower_bound(name);
    std::multimap<std::string, stTowerRankElement_t>::iterator up = mm.upper_bound(name);
    do
    {
        if (lo == up)
        {
            goto INSERT;
        }
        if (lo->second.m_job == (unsigned char)job)
        {
            if (score <= (unsigned int)lo->second.m_score)
            {
                return;
            }
            mm.erase(lo);
            goto INSERT;
        }
        ++lo;
    } while (true);
INSERT:
    stTowerRankElement_t elem((unsigned char)job, (unsigned short)score);
    mm.insert(std::pair<const std::string, stTowerRankElement_t>(name, elem));
}

unsigned int CTowerRank::getRankData(unsigned int floor, const char* name, unsigned int maxCount,
                                     stTowerRankElement_t* out)
{
    unsigned int count = 0;
    std::multimap<std::string, stTowerRankElement_t>& mm = m_ranks[floor - 1];
    std::multimap<std::string, stTowerRankElement_t>::const_iterator lo = mm.lower_bound(name);
    std::multimap<std::string, stTowerRankElement_t>::const_iterator up = mm.upper_bound(name);
    while (lo != up)
    {
        out[count] = lo->second;
        count++;
        if (count == maxCount)
        {
            break;
        }
        ++lo;
    }
    return count;
}

void CTowerRank::reset()
{
    for (int i = 0; i < 4; i++)
    {
        m_ranks[i].clear();
    }
}

void CTowerRank::registRank(unsigned int floor, unsigned int job, unsigned int score,
                            const stDeathTowerRecordMemberInfo_t* records)
{
    for (unsigned int i = 0; i < floor; i++)
    {
        registCharacRank(floor, records[i].m_name, job, score);
    }
}

