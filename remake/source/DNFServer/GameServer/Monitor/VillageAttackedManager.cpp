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
int HuntingPointMultiplier[0x12] = {0, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
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
    MAX_SCHEDULER_COUNT = 2;
    HUNTING_POINT_WEIGTH_CONST = 4;
    REWARD_BUFF_TIME = 3600;
    REWARD_PENALTY_TIME = 600;
    COUNTDOWN_FIRST_TIME = 600;
    COUNTDOWN_SECOND_TIME = 300;
    COUNTDOWN_THIRD_TIME = 60;
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
        b = 600;
    }
    else
    {
        b = b * 0x3c;
    }
    if (c == 0)
    {
        c = 600;
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
    MAX_SCHEDULER_COUNT = 1;
    HUNTING_POINT_WEIGTH_CONST = 1;
    REWARD_BUFF_TIME = b;
    REWARD_PENALTY_TIME = c;
    COUNTDOWN_FIRST_TIME = 30;
    COUNTDOWN_SECOND_TIME = 20;
    COUNTDOWN_THIRD_TIME = 10;
}

CVillageAttackedManager::CVillageAttackedManager(CApplication* app)
{
    SetRealConfig();
    m_app = app;
    m_field30 = 0;
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
    m_field2c = startTime;
    m_field28 = endTime;
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
    if (rewardType == ENUM_VILLAGE_ATTACKED_REWARD_BUFF)
    {
        m_field34 = (int)GetNowTime() + REWARD_BUFF_TIME;
    }
    else if (rewardType == ENUM_VILLAGE_ATTACKED_REWARD_PENALTY)
    {
        m_field34 = (int)GetNowTime() + REWARD_PENALTY_TIME;
    }
    else
    {
        m_field34 = 0;
    }
}

void CVillageAttackedManager::RequestEventEnd(bool flag)
{
    if (flag)
    {
        m_field1c = m_field20;
    }
    else if ((unsigned int)m_field20 <= (unsigned int)m_field1c)
    {
        m_field1c = m_field1c - 1;
    }
    OnEndVillageAttacked();
}

int CVillageAttackedManager::GetMaxHuntingPoint()
{
    unsigned int group = (unsigned int)m_app->Get_ServerGroup() & 0xff;
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
    ((RA_UINT<10>*)&pkt)->v = (unsigned int)GetRemainTime();
    ((RA_UINT<14>*)&pkt)->v = (unsigned int)m_field1c;
    ((RA_UINT<18>*)&pkt)->v = (unsigned int)m_field20;
    m_app->Get_ServerHandler()->SendAllToGameServer((char*)&pkt, 0x16);
}

void CVillageAttackedManager::OnCountdownVillageAttacked(int time)
{
    if (time == 600)
    {
        m_field20 = GetMaxHuntingPoint();
    }
    Packet_VillageAttackedCountdown pkt;
    ((RA_INT<10>*)&pkt)->v = time;
    m_app->Get_ServerHandler()->SendAllToGameServer(
        (char*)&pkt, (unsigned int)((RA_U16<2>*)&pkt)->v);
}

void CVillageAttackedManager::SendFirstRankerReward(unsigned int charNo)
{
    static const char kTitle[10] = {'\xc1', '\xd6', '\xb9', '\xce', ' ',
                                    '\xb4', '\xeb', '\xc7', '\xa5', '\0'};
    static const char kBody[0x8e] = {
        '\xbc', '\xd2', '\xb6', '\xf5', '\xc0', '\xbb', '\x20', '\xc0', '\xe1', '\xc0', '\xe7', '\xbf',
        '\xec', '\xbd', '\xc3', '\xb4', '\xc0', '\xb6', '\xf3', '\x20', '\xbc', '\xf6', '\xb0', '\xed',
        '\xc7', '\xcf', '\xbd', '\xc5', '\x20', '\xb8', '\xf0', '\xc7', '\xe8', '\xb0', '\xa1', '\xb4',
        '\xd4', '\xb2', '\xb2', '\x20', '\xc1', '\xd6', '\xb9', '\xce', '\xb5', '\xe9', '\xc0', '\xc7',
        '\x20', '\xc1', '\xa4', '\xbc', '\xba', '\xc0', '\xbb', '\x20', '\xb8', '\xf0', '\xbe', '\xc6',
        '\x20', '\xbc', '\xb1', '\xb9', '\xb0', '\xc0', '\xbb', '\x20', '\xb5', '\xe5', '\xb8', '\xb3',
        '\xb4', '\xcf', '\xb4', '\xd9', '\x2e', '\x20', '\xb0', '\xa8', '\xbb', '\xe7', '\xc7', '\xd5',
        '\xb4', '\xcf', '\xb4', '\xd9', '\x20', '\xb8', '\xf0', '\xc7', '\xe8', '\xb0', '\xa1', '\xb4',
        '\xd4', '\x2e', '\x28', '\xbc', '\xba', '\xc0', '\xe5', '\xc0', '\xc7', '\x20', '\xba', '\xf1',
        '\xbe', '\xe0', '\x20', '\xbb', '\xe7', '\xbf', '\xeb', '\xb1', '\xe2', '\xb0', '\xa3', '\xc0',
        '\xcc', '\x20', '\xc1', '\xf6', '\xb3', '\xaa', '\xb8', '\xe9', '\x20', '\xbb', '\xe7', '\xb6',
        '\xf3', '\xc1', '\xfd', '\xb4', '\xcf', '\xb4', '\xd9', '\x2e', '\x29', '\x00'};
    Packet_DB_InsertMail pkt;
    ((RA_UINT<10>*)&pkt)->v = charNo;
    ((RA_UINT<14>*)&pkt)->v = 0x1dfe;
    ((RA_UINT<18>*)&pkt)->v = 1;
    memcpy((char*)&pkt + 0x1a, kTitle, 10);
    memcpy((char*)&pkt + 0x2f, kBody, 0x8e);
    ((RA_INT<303>*)&pkt)->v = 3;
    m_app->Get_ServerHandler()->SendToDB(&pkt);
}

void CVillageAttackedManager::SendMaxHuntingPoint()
{
    Packet_DBMW_Query_Msg pkt;
    char sql[0x1001];
    pkt.m_fieldB = 6;
    pkt.m_fieldA = 0x4ee2;
    register unsigned int hp = (unsigned int)m_field1c;
    register unsigned int now = GetNowTime();
    sprintf(sql,
            "inSert into village_attacked_server_point_rank(server_info, occ_date, hunting_point) values(%d,cast(from_unixtime(%d) as date),%u)",
            (unsigned int)m_app->Get_ServerGroup() & 0xff, now, hp);
    m_app->Get_ServerHandler()->SendToDB(&pkt);
}

void CVillageAttackedManager::Reset()
{
    m_huntingPoints.clear();
    m_field1c = 0;
    m_field20 = GetMaxHuntingPoint();
    m_state24 = 0;
    m_field28 = 0;
    m_field2c = 0;
}

void CVillageAttackedManager::OnEndVillageAttacked()
{
    if (!m_state24)
    {
        return;
    }
    int now = (int)GetNowTime();
    if ((unsigned int)m_field1c >= (unsigned int)m_field20)
    {
        m_field30 = 1;
        CVillageAttackedReward* task =
            new CVillageAttackedReward(REWARD_BUFF_TIME + now, 0, this);
        m_app->GetTaskScheduler()->AddTask(task);
    }
    else
    {
        m_field30 = 2;
        CVillageAttackedReward* task =
            new CVillageAttackedReward(REWARD_PENALTY_TIME + now, 0, this);
        m_app->GetTaskScheduler()->AddTask(task);
    }
    m_state24 = 0;
    SetRewardCloseTime((ENUM_VILLAGE_ATTACKED_REWARD)m_field30);
    SendVillageAttackedEnd();
    SendCharacRank();
    SendMaxHuntingPoint();
    Reset();
    OnSchedule();
}

void CVillageAttackedManager::OnRewardVillageAttacked()
{
    Packet_VillageAttackedRewardServer pkt;
    m_app->Get_ServerHandler()->SendAllToGameServer((char*)&pkt, 0xe);
    m_field30 = 0;
}

unsigned int CVillageAttackedManager::GetDungeonRemainTime()
{
    return m_field34;
}

void CVillageAttackedManager::SendVillageAttackedEnd()
{
    Packet_VillageAttackedEnd pkt;
    pkt.m_dungeonRemain = GetDungeonRemainTime();
    pkt.m_fieldE = (unsigned int)m_field1c;
    pkt.m_field12 = (unsigned int)m_field20;
    m_app->Get_ServerHandler()->SendAllToGameServer((char*)&pkt, 0x16);
}

int CVillageAttackedManager::GetRemainTime()
{
    return m_field28 - (int)GetNowTime();
}

void CVillageAttackedManager::OnUpdateVillageAttacked()
{
    Packet_VillageAttackedUpdate pkt;
    pkt.m_remainTime = (unsigned int)GetRemainTime();
    pkt.m_fieldE = (unsigned int)m_field1c;
    pkt.m_field12 = (unsigned int)m_field20;
    m_app->Get_ServerHandler()->SendAllToGameServer((char*)&pkt, 0x16);
}

void CVillageAttackedManager::SendVillageAttackedScore(CUser* user)
{
    Packet_VillageAttackedScore pkt;
    pkt.m_idByChannel = user->GetIdByChannel();
    pkt.m_uniqCharNo = user->GetUniqCharNo();
    pkt.m_remainTime = (unsigned int)GetRemainTime();
    pkt.m_field16 = (unsigned int)m_field1c;
    pkt.m_field1a = (unsigned int)m_field20;
    user->GetUniqCharNo();
    int* hp = GetHuntingPoint(user->GetUniqCharNo());
    int cur = 0;
    int max = 0;
    if (hp != 0)
    {
        cur = *hp;
        max = *hp + hp[1];
    }
    pkt.m_cur = cur;
    pkt.m_max = max;
    user->SendToGameserver((char*)&pkt, 0x26);
}

void CVillageAttackedManager::SendVillageAttackedReward(CUser* user, int rewardType)
{
    Packet_VillageAttackedReward pkt;
    pkt.m_idByChannel = user->GetIdByChannel();
    pkt.m_uniqCharNo = user->GetUniqCharNo();
    pkt.m_rewardType = rewardType;
    user->SendToGameserver((char*)&pkt, 0x1a);
}

void CVillageAttackedManager::OnCharacLogin(CUser* user)
{
    if (m_state24 != 0)
    {
        SendVillageAttackedScore(user);
    }
    if (m_field30 != 0 && m_field30 != 1)
    {
        SendVillageAttackedReward(user, m_field30);
    }
}

int* CVillageAttackedManager::GetHuntingPoint(unsigned int charNo)
{
    std::map<unsigned int, stHuntingPoint>::iterator it = m_huntingPoints.find(charNo);
    if (it != m_huntingPoints.end())
    {
        return (int*)&it->second;
    }
    return 0;
}

void CVillageAttackedManager::SendVillageAttackedRewardJpn(CUser* user, int count)
{
    Packet_VillageAttackedReward pkt;
    pkt.m_idByChannel = user->GetIdByChannel();
    pkt.m_uniqCharNo = user->GetUniqCharNo();
    pkt.m_rewardType = 5;
    pkt.m_count = count;
    user->SendToGameserver((char*)&pkt, 0x1a);
}

void CVillageAttackedManager::SendMinTime()
{
    Packet_DBMW_Query_Msg pkt;
    pkt.m_fieldB = 6;
    pkt.m_fieldA = 0x4ee3;
    register unsigned int elapse = GetElapseTime();
    register unsigned int now = GetNowTime();
    register unsigned int group = (unsigned int)m_app->Get_ServerGroup();
    char sql[0x1001];
    sprintf(sql,
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
    ((RA_S8<10>*)&pkt)->v = (char)group;
    ((RA_INT<11>*)&pkt)->v = times[0];
    ((RA_INT<15>*)&pkt)->v = times[0];
    ((RA_INT<19>*)&pkt)->v = times[1];
    ((RA_INT<23>*)&pkt)->v = times[1];
    m_app->Get_ServerHandler()->SendToDB(&pkt);
}

unsigned int CVillageAttackedManager::GetElapseTime()
{
    return (unsigned int)(GetNowTime() - m_field2c);
}

void CVillageAttackedManager::ClearDungeonCloseTime()
{
    m_field34 = 0;
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
                int* hp = GetHuntingPoint(charNos[i]);
                if (hp == 0)
                {
                    stHuntingPoint p;
                    p.m_huntingPoint = 0;
                    p.m_field4 = 0;
                    if (success)
                    {
                        p.m_huntingPoint++;
                    }
                    else
                    {
                        p.m_field4++;
                    }
                    m_huntingPoints.insert(
                        std::pair<const unsigned int, stHuntingPoint>(charNos[i], p));
                }
                else
                {
                    if (success)
                    {
                        hp[0]++;
                    }
                    else
                    {
                        hp[1]++;
                    }
                    total = hp[0] + hp[1];
                }
                if (success)
                {
                    int* cur = GetHuntingPoint(charNos[i]);
                    SendVillageAttackedRewardJpn(users[i], *cur);
                    DNF_LOG_SCOPE_AT("UpdateHuntingPoint", 0x3ae,"./log/village", "Send Success Count [charac:%u][count:%d]",
                        charNos[i], *cur);
                }
            }
        }
        if (success)
        {
            m_field1c = m_field1c + 1;
        }
        if (m_field1c == m_field20)
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
            p.m_characNo = it->second.m_field4;
            pq.push(p);
        }
        char sql[0x1001];
        memset(sql, 0, 0x1001);
        std::string query;
        serverGroup = m_app->Get_ServerGroup();
        unsigned int now = GetNowTime();
        if ((unsigned int)m_field20 <= (unsigned int)m_field1c)
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
        pkt.m_fieldB = 6;
        pkt.m_fieldA = 0x4ee4;
        sprintf(sql,
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
    m_field4 = 0;
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
    char* p = pkt;
    ((RA_UINT<10>*)p)->v = m_field30;
    ((RA_UINT<14>*)p)->v = GetDungeonRemainTime();
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
