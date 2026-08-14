// df_monitor_r — VillageAttackedManager（从 MonitorTypes/App/Table 拆分）
#include <stdio.h>
#include "RawAccess.h"
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

#include "VillageAttackedManager.h"
#include "DNFFileLog.h"
#include "DNFFunctionLib.h"
#include "Packet_Monitor_Member_Secede.h"
#include "Packet_GM_Request_Mid.h"
#include "Packet_PvPChannelInfo.h"
#include "Packet_PvPChannelUserCount.h"
#include "Packet_Item_Limit_Edition_Sell_Start.h"
#include "Packet_DBMW_Change_Char_Name.h"
#include "Packet_Monitor_Reply_Charac_Info.h"
#include "DNFApplication.h"
#include "DNFPacketTranslater.h"
#include "DNFServerHandler.h"
#include "SystemTimeHandler.h"
#include "DNFUser.h"

bool compareTime(int const& a, int const& b)
{
    return a > b;
}

namespace village_attacked
{
int village_attacked_scheduler[18];
int MAX_SCHEDULER_COUNT;
int HUNTING_POINT_WEIGTH_CONST;
static int HuntingPointMultiplier[0x12] = {0, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
static const int tRealConfig[7] = {2, 4, 3600, 600, 600, 300, 60};
static int tGMConfig[7] = {1, 600, 600, 30, 20, 10, 0};
int REWARD_BUFF_TIME;
int REWARD_PENALTY_TIME;
int COUNTDOWN_FIRST_TIME;
int COUNTDOWN_SECOND_TIME;
int COUNTDOWN_THIRD_TIME;

int GetNextSchedule(tm t, int wday, int hour, int min)
{
    int days = wday - t.tm_wday;
    if (days < 0)
    {
        days += 7;
    }
    else if (days == 0)
    {
        if (hour < t.tm_hour)
        {
            days = 7;
        }
        else if (t.tm_hour == hour && min <= t.tm_min)
        {
            days = 7;
        }
    }
    t.tm_hour = hour;
    t.tm_min = min;
    t.tm_sec = 0;
    time_t r = mktime(&t);
    return (int)(r + days * 86400);
}

void SetRealConfig()
{
    village_attacked_scheduler[0] = 2;
    village_attacked_scheduler[1] = 0x15;
    village_attacked_scheduler[2] = 0x1e;
    village_attacked_scheduler[3] = 2;
    village_attacked_scheduler[4] = 0x16;
    village_attacked_scheduler[5] = 0x1e;
    village_attacked_scheduler[6] = 6;
    village_attacked_scheduler[7] = 0x15;
    village_attacked_scheduler[8] = 0x1e;
    village_attacked_scheduler[9] = 6;
    village_attacked_scheduler[10] = 0x16;
    village_attacked_scheduler[11] = 0x1e;
    village_attacked_scheduler[12] = -1;
    village_attacked_scheduler[13] = -1;
    village_attacked_scheduler[14] = -1;
    village_attacked_scheduler[15] = -1;
    village_attacked_scheduler[16] = -1;
    village_attacked_scheduler[17] = -1;
    MAX_SCHEDULER_COUNT = tRealConfig[0];
    HUNTING_POINT_WEIGTH_CONST = tRealConfig[1];
    REWARD_BUFF_TIME = tRealConfig[2];
    REWARD_PENALTY_TIME = tRealConfig[3];
    COUNTDOWN_FIRST_TIME = tRealConfig[4];
    COUNTDOWN_SECOND_TIME = tRealConfig[5];
    COUNTDOWN_THIRD_TIME = tRealConfig[6];
}

void SetGMConfig(unsigned int a, unsigned int b, unsigned int c)
{
    if (a == 0)
    {
        a = 600;
    }
    else
    {
        a = a * 0x3c;
    }
    if (b == 0)
    {
        b = (unsigned int)tGMConfig[2];
    }
    else
    {
        b = b * 0x3c;
    }
    if (c == 0)
    {
        c = (unsigned int)tGMConfig[3];
    }
    else
    {
        c = c * 0x3c;
    }
    time_t now = time(0);
    time_t start = now + 0x3c;
    time_t end = start + a;
    tm t1;
    tm t2;
    localtime_r(&start, &t1);
    localtime_r(&end, &t2);
    village_attacked_scheduler[12] = t1.tm_wday;
    village_attacked_scheduler[13] = t1.tm_hour;
    village_attacked_scheduler[14] = t1.tm_min;
    village_attacked_scheduler[15] = t2.tm_wday;
    village_attacked_scheduler[16] = t2.tm_hour;
    village_attacked_scheduler[17] = t2.tm_min;
    MAX_SCHEDULER_COUNT = tGMConfig[0];
    HUNTING_POINT_WEIGTH_CONST = tGMConfig[1];
    REWARD_BUFF_TIME = b;
    REWARD_PENALTY_TIME = c;
    COUNTDOWN_FIRST_TIME = tGMConfig[4];
    COUNTDOWN_SECOND_TIME = tGMConfig[5];
    COUNTDOWN_THIRD_TIME = tGMConfig[6];
}

CVillageAttackedManager::CVillageAttackedManager(CApplication* app)
{
    SetRealConfig();
    m_app = app;
    m_rewardType = 0;
    Reset();
}

CVillageAttackedManager::~CVillageAttackedManager() {}

void CVillageAttackedManager::SendFirstRankerRewardJpn(CUser* user, int rank)
{
    Packet_VillageAttackedReward pkt;
    pkt.m_idByChannel = user->GetIdByChannel();
    pkt.m_uniqCharNo = user->GetUniqCharNo();
    pkt.m_count = 6;
    pkt.m_rewardType = rank;
    user->SendToGameserver((char*)&pkt, pkt.packetSize);
}

void CVillageAttackedManager::InsertTimer(int startTime, int endTime)
{
    m_startTime = startTime;
    m_endTime = endTime;
    CVillageAttackedCountdownFirst* t1 =
        new CVillageAttackedCountdownFirst(startTime - 600, 0, this);
    m_app->GetTaskScheduler()->AddTask(t1);
    CVillageAttackedCountdownSecond* t2 =
        new CVillageAttackedCountdownSecond(startTime - 300, 0, this);
    m_app->GetTaskScheduler()->AddTask(t2);
    CVillageAttackedCountdownThird* t3 =
        new CVillageAttackedCountdownThird(startTime - 60, 0, this);
    m_app->GetTaskScheduler()->AddTask(t3);
    CVillageAttackedStart* t4 = new CVillageAttackedStart(startTime, 0, this);
    m_app->GetTaskScheduler()->AddTask(t4);
    CVillageAttackedEnd* t5 = new CVillageAttackedEnd(endTime, 0, this);
    m_app->GetTaskScheduler()->AddTask(t5);
}

void CVillageAttackedManager::OnSchedule()
{
    time_t now = GetNowTime();
    tm* t = localtime(&now);
    int bestIdx = 0;
    int bestTime = ::GetNextSchedule(*t, village_attacked_scheduler[0],
                                     village_attacked_scheduler[1],
                                     village_attacked_scheduler[2]);
    for (int i = 1; i < MAX_SCHEDULER_COUNT; i++)
    {
        int s = ::GetNextSchedule(*t, village_attacked_scheduler[i * 6],
                                  village_attacked_scheduler[i * 6 + 1],
                                  village_attacked_scheduler[i * 6 + 2]);
        if (s < bestTime)
        {
            bestIdx = i;
            bestTime = s;
        }
    }
    int end = ::GetNextSchedule(*t, village_attacked_scheduler[bestIdx * 6 + 3],
                                village_attacked_scheduler[bestIdx * 6 + 4],
                                village_attacked_scheduler[bestIdx * 6 + 5]);
    InsertTimer(bestTime, end);
    tm* t2 = localtime((time_t*)&end);
    t2->tm_sec = 0;
    t2->tm_min = 0;
    t2->tm_hour = 6;
    t2->tm_mday = t2->tm_mday + 1;
    mktime(t2);
}

void CVillageAttackedManager::SetRewardCloseTime(ENUM_VILLAGE_ATTACKED_REWARD rewardType)
{
    switch (rewardType)
    {
    case ENUM_VILLAGE_ATTACKED_REWARD_BUFF:
        m_rewardCloseTime = GetNowTime() + REWARD_BUFF_TIME;
        break;
    case ENUM_VILLAGE_ATTACKED_REWARD_PENALTY:
        m_rewardCloseTime = GetNowTime() + REWARD_PENALTY_TIME;
        break;
    default:
        m_rewardCloseTime = 0;
        break;
    }
}

void CVillageAttackedManager::RequestEventEnd(bool flag)
{
    if (flag)
    {
        m_curHuntingPoint = (int)m_maxHuntingPoint;
    }
    else if ((unsigned int)m_curHuntingPoint >= (unsigned int)m_maxHuntingPoint)
    {
        m_curHuntingPoint = m_curHuntingPoint - 1;
    }
    OnEndVillageAttacked();
}

int CVillageAttackedManager::GetMaxHuntingPoint()
{
    int group = (unsigned char)m_app->Get_ServerGroup();
    if (0 < group && group <= 0x11)
    {
        return m_app->Get_UserManager()->Size() * HuntingPointMultiplier[group];
    }
    if (group > 0x61)
    {
        return m_app->Get_UserManager()->Size() * HUNTING_POINT_WEIGTH_CONST;
    }
    DNF_LOG_SCOPE_AT(__FUNCTION__, 0xfe, "./log/village", "ServerGroup is over REAL_GROUP_MAX : %d", group);
    return 0;
}

void CVillageAttackedManager::OnStartVillageAttacked()
{
    m_state24 = 1;
    ClearDungeonCloseTime();
    Packet_VillageAttackedStart pkt;
    pkt.m_remainTime = (int)GetRemainTime();
    pkt.m_curHuntingPoint = m_curHuntingPoint;
    pkt.m_maxHuntingPoint = m_maxHuntingPoint;
    m_app->Get_ServerHandler()->SendAllToGameServer((char*)&pkt, 0x16);
}

void CVillageAttackedManager::OnCountdownVillageAttacked(int time)
{
    if (time == 600)
    {
        m_maxHuntingPoint = GetMaxHuntingPoint();
    }
    Packet_VillageAttackedCountdown pkt;
    pkt.m_countdown = time;
    m_app->Get_ServerHandler()->SendAllToGameServer(
        (char*)&pkt, (unsigned int)pkt.packetSize);
}

void CVillageAttackedManager::SendFirstRankerReward(unsigned int charNo)
{
    Packet_DB_InsertMail pkt;
    pkt.m_characNo = charNo;
    pkt.m_fieldE = 0x1dfe;
    pkt.m_field12 = 1;
    memcpy(pkt.m_subject, "\xc1\xd6\xb9\xce \xb4\xeb\xc7\xa5", 10);
    memcpy(pkt.m_content,
           "\xbc\xd2\xb6\xf5\xc0\xbb \xc0\xe1\xc0\xe7\xbf\xec\xbd\xc3\xb4\xc0\xb6\xf3 \xbc\xf6\xb0\xed\xc7\xcf\xbd\xc5 \xb8\xf0\xc7\xe8\xb0\xa1\xb4\xd4\xb2\xb2 \xc1\xd6\xb9\xce\xb5\xe9\xc0\xc7 \xc1\xa4\xbc\xba\xc0\xbb \xb8\xf0\xbe\xc6 \xbc\xb1\xb9\xb0\xc0\xbb \xb5\xe5\xb8\xb3\xb4\xcf\xb4\xd9. \xb0\xa8\xbb\xe7\xc7\xd5\xb4\xcf\xb4\xd9 \xb8\xf0\xc7\xe8\xb0\xa1\xb4\xd4.(\xbc\xba\xc0\xe5\xc0\xc7 \xba\xf1\xbe\xe0 \xbb\xe7\xbf\xeb\xb1\xe2\xb0\xa3\xc0\xcc \xc1\xf6\xb3\xaa\xb8\xe9 \xbb\xe7\xb6\xf3\xc1\xfd\xb4\xcf\xb4\xd9.)",
           0x8e);
    pkt.m_delayHours = 3;
    m_app->Get_ServerHandler()->SendToDB(&pkt);
}

void CVillageAttackedManager::SendMaxHuntingPoint()
{
    Packet_DBMW_Query_Msg pkt;
    pkt.m_handleIdx = 6;
    pkt.m_queryId = 0x4ee2;
    register unsigned int hp = (unsigned int)m_curHuntingPoint;
    register unsigned int now = GetNowTime();
    sprintf(pkt.m_sql,
            "inSert into village_attacked_server_point_rank(server_info, occ_date, hunting_point) values(%d,cast(from_unixtime(%d) as date),%u)",
            (unsigned int)m_app->Get_ServerGroup() & 0xff, now, hp);
    m_app->Get_ServerHandler()->SendToDB(&pkt);
}

void CVillageAttackedManager::Reset()
{
    m_huntingPoints.clear();
    m_curHuntingPoint = 0;
    this->m_maxHuntingPoint = GetMaxHuntingPoint();
    m_state24 = 0;
    m_endTime = 0;
    m_startTime = 0;
}

void CVillageAttackedManager::OnEndVillageAttacked()
{
    if (!m_state24)
    {
        return;
    }
    int now = (int)GetNowTime();
    if ((unsigned int)m_curHuntingPoint >= (unsigned int)m_maxHuntingPoint)
    {
        m_rewardType = 1;
        CVillageAttackedReward* task =
            new CVillageAttackedReward(REWARD_BUFF_TIME + now, 0, this);
        m_app->GetTaskScheduler()->AddTask(task);
    }
    else
    {
        m_rewardType = 2;
        CVillageAttackedReward* task =
            new CVillageAttackedReward(REWARD_PENALTY_TIME + now, 0, this);
        m_app->GetTaskScheduler()->AddTask(task);
    }
    m_state24 = 0;
    SetRewardCloseTime((ENUM_VILLAGE_ATTACKED_REWARD)m_rewardType);
    SendVillageAttackedEnd();
    SendCharacRank();
    SendMaxHuntingPoint();
    Reset();
    OnSchedule();
}

void CVillageAttackedManager::OnRewardVillageAttacked()
{
    Packet_VillageAttackedRewardServer pkt;
    pkt.m_rewardType = 0;
    m_app->Get_ServerHandler()->SendAllToGameServer(
        (char*)&pkt, (unsigned int)pkt.packetSize);
    m_rewardType = 0;
}

unsigned int CVillageAttackedManager::GetDungeonRemainTime()
{
    return m_rewardCloseTime;
}

void CVillageAttackedManager::SendVillageAttackedEnd()
{
    Packet_VillageAttackedEnd pkt;
    pkt.m_dungeonRemain = GetDungeonRemainTime();
    pkt.m_huntingPoint = (unsigned int)m_curHuntingPoint;
    pkt.m_maxHuntingPoint = (unsigned int)m_maxHuntingPoint;
    m_app->Get_ServerHandler()->SendAllToGameServer((char*)&pkt, 0x16);
}

int CVillageAttackedManager::GetRemainTime()
{
    return m_endTime - (int)GetNowTime();
}

void CVillageAttackedManager::OnUpdateVillageAttacked()
{
    Packet_VillageAttackedUpdate pkt;
    pkt.m_remainTime = (unsigned int)GetRemainTime();
    pkt.m_huntingPoint = (unsigned int)m_curHuntingPoint;
    pkt.m_maxHuntingPoint = (unsigned int)m_maxHuntingPoint;
    m_app->Get_ServerHandler()->SendAllToGameServer((char*)&pkt, pkt.packetSize);
}

void CVillageAttackedManager::SendVillageAttackedScore(CUser* user)
{
    Packet_VillageAttackedScore pkt;
    pkt.m_idByChannel = (unsigned int)user->GetIdByChannel();
    pkt.m_uniqCharNo = (unsigned int)user->GetUniqCharNo();
    pkt.m_remainTime = (unsigned int)GetRemainTime();
    pkt.m_huntingPoint = (unsigned int)m_curHuntingPoint;
    pkt.m_maxHuntingPoint = (unsigned int)m_maxHuntingPoint;
    stHuntingPoint* hp = GetHuntingPoint(user->GetUniqCharNo());
    if (hp != 0)
    {
        pkt.m_cur = hp->m_huntingPoint;
        pkt.m_max = hp->m_huntingPoint + hp->m_bonusPoint;
    }
    else
    {
        pkt.m_cur = 0;
        pkt.m_max = 0;
    }
    user->SendToGameserver((char*)&pkt, pkt.packetSize);
}

void CVillageAttackedManager::SendVillageAttackedReward(CUser* user, int rewardType)
{
    Packet_VillageAttackedReward pkt;
    pkt.m_idByChannel = user->GetIdByChannel();
    pkt.m_uniqCharNo = user->GetUniqCharNo();
    pkt.m_count = rewardType;
    user->SendToGameserver((char*)&pkt, (unsigned int)pkt.packetSize);
}

void CVillageAttackedManager::OnCharacLogin(CUser* user)
{
    if (m_state24 != 0)
    {
        SendVillageAttackedScore(user);
    }
    if (m_rewardType != 0 && m_rewardType != 1)
    {
        SendVillageAttackedReward(user, m_rewardType);
    }
}

stHuntingPoint* CVillageAttackedManager::GetHuntingPoint(unsigned int charNo)
{
    std::map<unsigned int, stHuntingPoint>::iterator it = m_huntingPoints.find(charNo);
    if (it != m_huntingPoints.end())
    {
        stHuntingPoint* local_c = &it->second;
        return local_c;
    }
    return 0;
}

void CVillageAttackedManager::SendVillageAttackedRewardJpn(CUser* user, int count)
{
    Packet_VillageAttackedReward pkt;
    pkt.m_idByChannel = user->GetIdByChannel();
    pkt.m_uniqCharNo = user->GetUniqCharNo();
    pkt.m_count = 5;
    pkt.m_rewardType = count;
    user->SendToGameserver((char*)&pkt, (unsigned int)pkt.packetSize);
}

void CVillageAttackedManager::SendMinTime()
{
    Packet_DBMW_Query_Msg pkt;
    pkt.m_handleIdx = 6;
    pkt.m_queryId = 0x4ee3;
    register unsigned int elapse = GetElapseTime();
    register unsigned int now = GetNowTime();
    register unsigned int group = (unsigned int)m_app->Get_ServerGroup();
    sprintf(pkt.m_sql,
            "inSert into village_attacked_server_time_rank(server_info, occ_date, clear_time) values(%d,cast(from_unixtime(%d) as date),%u)",
            group & 0xff, now, elapse);
    m_app->Get_ServerHandler()->SendToDB(&pkt);
}

void CVillageAttackedManager::OnServerGroupRewardVillageAttacked()
{
    Packet_DB_VillageAttackedRank pkt;
    unsigned char group = m_app->Get_ServerGroup();
    time_t now = GetNowTime();
    tm* pt = localtime(&now);
    int times[18];
    for (int i = 0; i < MAX_SCHEDULER_COUNT; i++)
    {
        times[i] = GetPrevSchedule(*pt, village_attacked_scheduler[i * 6],
                                   village_attacked_scheduler[i * 6 + 1],
                                   village_attacked_scheduler[i * 6 + 2]);
    }
    std::sort(&times[0], &times[MAX_SCHEDULER_COUNT], compareTime);
    pkt.m_group = (char)group;
    pkt.m_time0a = times[0];
    pkt.m_time0b = times[0];
    pkt.m_time1a = times[1];
    pkt.m_time1b = times[1];
    m_app->Get_ServerHandler()->SendToDB(&pkt);
}

unsigned int CVillageAttackedManager::GetElapseTime()
{
    return (unsigned int)(GetNowTime() - m_startTime);
}

void CVillageAttackedManager::ClearDungeonCloseTime()
{
    m_rewardCloseTime = 0;
}

void CVillageAttackedManager::UpdateHuntingPoint(CUser** users, bool success, int* a,
                                                 unsigned int* charNos)
{
    if (m_state24 == 1)
    {
        for (int i = 0; i < 4; i++)
        {
            if (users[i] != 0)
            {
                int total = 0;
                stHuntingPoint* hp = GetHuntingPoint(charNos[i]);
                if (hp == 0)
                {
                    stHuntingPoint p;
                    p.m_huntingPoint = 0;
                    p.m_bonusPoint = 0;
                    if (success)
                    {
                        p.m_huntingPoint++;
                    }
                    else
                    {
                        p.m_bonusPoint++;
                    }
                    m_huntingPoints.insert(
                        std::pair<const unsigned int, stHuntingPoint>(charNos[i], p));
                }
                else
                {
                    if (success)
                    {
                        hp->m_huntingPoint++;
                    }
                    else
                    {
                        hp->m_bonusPoint++;
                    }
                    total = hp->m_huntingPoint + hp->m_bonusPoint;
                }
                if (success)
                {
                    stHuntingPoint* cur = GetHuntingPoint(charNos[i]);
                    SendVillageAttackedRewardJpn(users[i], cur->m_huntingPoint);
                    DNF_LOG_SCOPE_AT("UpdateHuntingPoint", 0x3ae,"./log/village", "Send Success Count [charac:%u][count:%d]",
                        charNos[i], cur->m_huntingPoint);
                }
            }
        }
        if (success)
        {
            m_curHuntingPoint = m_curHuntingPoint + 1;
        }
        if (m_curHuntingPoint == m_maxHuntingPoint)
        {
            SendMinTime();
        }
    }
}

void CVillageAttackedManager::SendCharacRank()
{
    unsigned char serverGroup = 0;
    if (!m_huntingPoints.empty())
    {
        std::priority_queue<stUserHuntingPoint> pq;
        for (std::map<unsigned int, stHuntingPoint>::iterator it = m_huntingPoints.begin();
             it != m_huntingPoints.end(); ++it)
        {
            stUserHuntingPoint p;
            p.m_huntingPoint = it->second.m_huntingPoint;
            p.m_characNo = it->second.m_bonusPoint;
            pq.push(p);
        }
        char sql[0x1001];
        memset(sql, 0, 0x1001);
        std::string query;
        serverGroup = m_app->Get_ServerGroup();
        unsigned int now = GetNowTime();
        if ((unsigned int)m_maxHuntingPoint <= (unsigned int)m_curHuntingPoint)
        {
            int rank = 0;
            int count = 0;
            while (!pq.empty())
            {
                stUserHuntingPoint p = pq.top();
                CUser* user = m_app->Get_UserManager()->FindUser_CharNo(p.m_characNo);
                if (user == 0)
                {
                    pq.pop();
                    DNF_LOG_SCOPE_AT(__FUNCTION__, 0x238, "./log/village", "User is null [charac_no:%u]", p.m_characNo);
                }
                else
                {
                    rank++;
                    SendFirstRankerRewardJpn(user, rank);
                    sprintf(sql, "(%d,cast(from_unixtime(%d) as date),%u,%u,%d)", serverGroup,
                            now, p.m_characNo, p.m_huntingPoint, rank);
                    if (query.length() != 0)
                    {
                        query += ",";
                    }
                    query += sql;
                    pq.pop();
                    if (2 < rank)
                    {
                        break;
                    }
                }
                count++;
            }
        }
        Packet_DBMW_Query_Msg pkt;
        pkt.m_handleIdx = 6;
        pkt.m_queryId = 0x4ee4;
        sprintf(pkt.m_sql,
                "inSert into village_attacked_charac_point_rank(server_info, occ_date, charac_no, hunting_point, rank) values%s",
                query.c_str());
        m_app->Get_ServerHandler()->SendToDB(&pkt);
    }
}

CVillageAttackedCountdownFirst::CVillageAttackedCountdownFirst(unsigned int time,
                                                               unsigned int flag,
                                                               CVillageAttackedManager* mgr)
    : CTask(time, flag)
{
    m_mgr = mgr;
}

CVillageAttackedCountdownFirst::~CVillageAttackedCountdownFirst() {}

CVillageAttackedCountdownSecond::CVillageAttackedCountdownSecond(unsigned int time,
                                                                 unsigned int flag,
                                                                 CVillageAttackedManager* mgr)
    : CTask(time, flag)
{
    m_mgr = mgr;
}

CVillageAttackedCountdownSecond::~CVillageAttackedCountdownSecond() {}

CVillageAttackedCountdownThird::CVillageAttackedCountdownThird(unsigned int time,
                                                               unsigned int flag,
                                                               CVillageAttackedManager* mgr)
    : CTask(time, flag)
{
    m_mgr = mgr;
}

CVillageAttackedCountdownThird::~CVillageAttackedCountdownThird() {}

CVillageAttackedStart::CVillageAttackedStart(unsigned int time, unsigned int flag,
                                             CVillageAttackedManager* mgr)
    : CTask(time, flag)
{
    m_mgr = mgr;
}

CVillageAttackedStart::~CVillageAttackedStart() {}

CVillageAttackedEnd::CVillageAttackedEnd(unsigned int time, unsigned int flag,
                                         CVillageAttackedManager* mgr)
    : CTask(time, flag)
{
    m_mgr = mgr;
}

CVillageAttackedEnd::~CVillageAttackedEnd() {}

CVillageAttackedReward::CVillageAttackedReward(unsigned int time, unsigned int flag,
                                               CVillageAttackedManager* mgr)
    : CTask(time, flag)
{
    m_mgr = mgr;
}

CVillageAttackedReward::~CVillageAttackedReward() {}

stHuntingPoint::stHuntingPoint()
{
    m_huntingPoint = 0;
    m_bonusPoint = 0;
}

void CVillageAttackedManager::RequestEventStart(int time)
{
    int t = (int)GetNowTime();
    t += 10;
    InsertTimer(t + 10, t + time);
}

void CVillageAttackedManager::RequestEventPenaltyEnd()
{
    OnRewardVillageAttacked();
}

void CVillageAttackedManager::SendRequestRevengeDungeon(char* pkt)
{
    Packet_Request_Revenge_Dungeon* req = (Packet_Request_Revenge_Dungeon*)pkt;
    req->m_rewardType = m_rewardType;
    req->m_remainTime = GetDungeonRemainTime();
}

void CVillageAttackedManager::ProcessByMinute()
{
    if (m_state24 != 0)
    {
        OnUpdateVillageAttacked();
    }
}

void CVillageAttackedCountdownFirst::_DoExecute()
{
    m_mgr->OnCountdownVillageAttacked(0x258);
}

void CVillageAttackedCountdownSecond::_DoExecute()
{
    m_mgr->OnCountdownVillageAttacked(0x12c);
}

void CVillageAttackedCountdownThird::_DoExecute()
{
    m_mgr->OnCountdownVillageAttacked(0x3c);
}

void CVillageAttackedStart::_DoExecute()
{
    m_mgr->OnStartVillageAttacked();
}

void CVillageAttackedEnd::_DoExecute()
{
    m_mgr->OnEndVillageAttacked();
}

void CVillageAttackedReward::_DoExecute()
{
    m_mgr->OnRewardVillageAttacked();
}

CVillageAttackedServerGroupReward::CVillageAttackedServerGroupReward(
    unsigned int time, unsigned int flag, CVillageAttackedManager* mgr)
    : CTask(time, flag)
{
    m_mgr = mgr;
}

CVillageAttackedServerGroupReward::~CVillageAttackedServerGroupReward() {}

void CVillageAttackedServerGroupReward::_DoExecute()
{
    m_mgr->OnServerGroupRewardVillageAttacked();
}
}
