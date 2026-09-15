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
// ORIG 以 8 字节元素（2 个 int）寻址该表：索引形态 (3*k+c)*8 而非展平的 (6*k+c)*4。
// 二维声明才能让 gimplify 保留该索引分解（一维声明 + cast 视图无效）。
int village_attacked_scheduler[9][2];
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
    village_attacked_scheduler[0][0] = 2;
    village_attacked_scheduler[0][1] = 0x15;
    village_attacked_scheduler[1][0] = 0x1e;
    village_attacked_scheduler[1][1] = 2;
    village_attacked_scheduler[2][0] = 0x16;
    village_attacked_scheduler[2][1] = 0x1e;
    village_attacked_scheduler[3][0] = 6;
    village_attacked_scheduler[3][1] = 0x15;
    village_attacked_scheduler[4][0] = 0x1e;
    village_attacked_scheduler[4][1] = 6;
    village_attacked_scheduler[5][0] = 0x16;
    village_attacked_scheduler[5][1] = 0x1e;
    village_attacked_scheduler[6][0] = -1;
    village_attacked_scheduler[6][1] = -1;
    village_attacked_scheduler[7][0] = -1;
    village_attacked_scheduler[7][1] = -1;
    village_attacked_scheduler[8][0] = -1;
    village_attacked_scheduler[8][1] = -1;
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
    village_attacked_scheduler[6][0] = t1.tm_wday;
    village_attacked_scheduler[6][1] = t1.tm_hour;
    village_attacked_scheduler[7][0] = t1.tm_min;
    village_attacked_scheduler[7][1] = t2.tm_wday;
    village_attacked_scheduler[8][0] = t2.tm_hour;
    village_attacked_scheduler[8][1] = t2.tm_min;
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

// [DNF-IDENTICAL（2026-09-15 复核：已 identical ✓）] DNF-MON-DIFF-0046 | monitor | 与ORIG差异=DIFF | village_attacked::CVillageAttackedManager::InsertTimer | 详见 function_reports/monitor/_ZN16village_attacked23CVillageAttackedManager11InsertTimerEii.md
void CVillageAttackedManager::InsertTimer(int startTime, int endTime)
{
    m_startTime = startTime;
    m_endTime = endTime;
    m_app->GetTaskScheduler()->AddTask(
        new CVillageAttackedCountdownFirst(startTime - COUNTDOWN_FIRST_TIME, 0, this));
    m_app->GetTaskScheduler()->AddTask(
        new CVillageAttackedCountdownSecond(startTime - COUNTDOWN_SECOND_TIME, 0, this));
    m_app->GetTaskScheduler()->AddTask(
        new CVillageAttackedCountdownThird(startTime - COUNTDOWN_THIRD_TIME, 0, this));
    m_app->GetTaskScheduler()->AddTask(
        new CVillageAttackedStart(startTime, 0, this));
    m_app->GetTaskScheduler()->AddTask(
        new CVillageAttackedEnd(endTime, 0, this));
}

// [DNF-NONIDENTICAL] DNF-MON-DIFF-0045 | monitor | 与ORIG差异=DIFF | village_attacked::CVillageAttackedManager::OnSchedule | 详见 function_reports/monitor/_ZN16village_attacked23CVillageAttackedManager10OnScheduleEv.md
// 已试（ndiff 23→22，最优 22）：二维声明 [9][2] 已对齐 (3k+c)*8 / (6k+c)*4 两处；
// 余差含 `[i*3][0]` 的 ORIG「shl $3 + disp(base)」编码（GCC 对此恒选 index 形式）与尾部布局。
void CVillageAttackedManager::OnSchedule()
{
    time_t now = GetNowTime();
    tm* t = localtime(&now);
    int s = 0;
    int bestIdx = 0;
    int bestTime = ::GetNextSchedule(*t, village_attacked_scheduler[0][0],
                                     village_attacked_scheduler[0][1],
                                     village_attacked_scheduler[1][0]);
    for (int i = 1; i < MAX_SCHEDULER_COUNT; i++)
    {
        s = ::GetNextSchedule(*t, village_attacked_scheduler[i * 3][0],
                              village_attacked_scheduler[i * 3][1],
                              village_attacked_scheduler[i * 3 + 1][0]);
        if (bestTime > s)
        {
            bestTime = s;
            bestIdx = i;
        }
    }
    int end = ::GetNextSchedule(*t, village_attacked_scheduler[bestIdx * 3 + 1][1],
                                village_attacked_scheduler[bestIdx * 3 + 2][0],
                                village_attacked_scheduler[bestIdx * 3 + 2][1]);
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

// [DNF-IDENTICAL（2026-09-15 复核：已 identical ✓）] DNF-MON-DIFF-0049 | monitor | 与ORIG差异=DIFF | village_attacked::CVillageAttackedManager::OnEndVillageAttacked | 详见 function_reports/monitor/_ZN16village_attacked23CVillageAttackedManager20OnEndVillageAttackedEv.md
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
        m_app->GetTaskScheduler()->AddTask(
            new CVillageAttackedReward(REWARD_BUFF_TIME + now, 0, this));
    }
    else
    {
        m_rewardType = 2;
        m_app->GetTaskScheduler()->AddTask(
            new CVillageAttackedReward(REWARD_PENALTY_TIME + now, 0, this));
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
    sprintf(pkt.m_sql,
            "inSert into village_attacked_server_time_rank(server_info, occ_date, clear_time) values(%d,cast(from_unixtime(%d) as date),%u)",
            m_app->Get_ServerGroup(), now, elapse);
    m_app->Get_ServerHandler()->SendToDB(&pkt);
}

// [DNF-NONIDENTICAL] DNF-MON-DIFF-0051 | monitor | 与ORIG差异=DIFF | village_attacked::CVillageAttackedManager::OnServerGroupRewardVillageAttacked | 详见 function_reports/monitor/_ZN16village_attacked23CVillageAttackedManager34OnServerGroupRewardVillageAttackedEv.md
// 已试（ndiff 10→8，最优 8）：二维声明对齐两处访问；余 1 处同上 shl/base 编码差（试过 3*i、(i*3)、(i*3)+0、char* 算术、取址解引用视图，均生成 index 形式）。
void CVillageAttackedManager::OnServerGroupRewardVillageAttacked()
{
    Packet_DB_VillageAttackedRank pkt;
    pkt.m_group = (char)m_app->Get_ServerGroup();
    time_t now = GetNowTime();
    tm* pt = localtime(&now);
    int times[MAX_SCHEDULER_COUNT];
    for (int i = 0; i < MAX_SCHEDULER_COUNT; i++)
    {
        times[i] = GetPrevSchedule(*pt, village_attacked_scheduler[i * 3][0],
                                   village_attacked_scheduler[i * 3][1],
                                   village_attacked_scheduler[i * 3 + 1][0]);
    }
    std::sort(&times[0], &times[MAX_SCHEDULER_COUNT], compareTime);
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

// [DNF-NONIDENTICAL] DNF-MON-DIFF-0048 | monitor | 与ORIG差异=DIFF | village_attacked::CVillageAttackedManager::UpdateHuntingPoint | 详见 function_reports/monitor/_ZN16village_attacked23CVillageAttackedManager18UpdateHuntingPointEPP5CUserbPiPj.md
// 已试（ndiff 8→7→3，长度 158→161 vs ORIG 160）：
// ①末尾 return 补回 SendMinTime 后的 join jmp+nop（落点 A）；
// ②循环体改为「early-continue + 裸复合块」（落点 D，Main 广播的新可调落点）⇒ 复现了循环体末尾那条 join jmp+nop，为本族第 4 个正例；
// 余 3 条 = 尾部被 GCC 生成两条相同目标的 jmp（两级嵌套 join 各一条，ORIG 仅一条）；
// 已试无效：return 移到最外层 / `if(1) return;` / 去掉 return（变 0 条）/ goto end_loop 自跳（语义错、已弃）。
void CVillageAttackedManager::UpdateHuntingPoint(CUser** users, bool success, int* a,
                                                 unsigned int* charNos)
{
    if (m_state24 != 1)
    {
    }
    else
    {
        for (int i = 0; i < 4; i++)
        {
            // 落点 D：early-continue + 裸复合块，复现 ORIG 循环体末尾那条 join jmp+nop
            if (users[i] == 0)
            {
                continue;
            }
            {
                int total = 0;
                stHuntingPoint* hp = GetHuntingPoint(charNos[i]);
                if (hp != 0)
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
                else
                {
                    stHuntingPoint p;
                    if (success)
                    {
                        p.m_huntingPoint++;
                    }
                    else
                    {
                        p.m_bonusPoint++;
                    }
                    total = 1;  // 复刻 ORIG：hp==0 时 total 置 1（ORIG 死变量，保留）
                    m_huntingPoints.insert(std::make_pair(charNos[i], p));
                }
                if (success)
                {
                    stHuntingPoint* cur = GetHuntingPoint(charNos[i]);
                    SendVillageAttackedRewardJpn(users[i], cur->m_huntingPoint);
                    DNF_LOG_SCOPE_AT(__FUNCTION__, 0x3ae,"./log/village", "Send Success Count [charac:%u][count:%d]",
                        charNos[i], cur->m_huntingPoint);
                    goto next_loop;
                }
            next_loop:;
            }
        }
        if (success)
        {
            m_curHuntingPoint = m_curHuntingPoint + 1;
        }
        if (m_curHuntingPoint == m_maxHuntingPoint)
        {
            SendMinTime();
            goto end_loop;
        }
    end_loop:;
    }
    // ORIG 在 if/else 之后、函数块内还有一条 return（生成末尾 jmp+nop 的 join 残留）
    return;
}

// [DNF-NONIDENTICAL] DNF-MON-DIFF-0047 | monitor | 与ORIG差异=DIFF | village_attacked::CVillageAttackedManager::SendCharacRank | 详见 function_reports/monitor/_ZN16village_attacked23CVillageAttackedManager14SendCharacRankEv.md
// 已试（ndiff 135→14，长度 237→269 = ORIG）：①memset→`char sql[0xff] = {0}`（复现 rep stos + push edi）；
// ②sql 声明点移到 for 之后；③S5 占位 `int reservedSlot/reservedSlot2`（浅区容器槽由此对齐 -0x48/-0x58/-0x60）；
// ④`serverGroup` 改 unsigned int 且声明点移入块内；⑤`p.m_characNo`/`p.m_huntingPoint` 赋值次序对调；
// ⑥条件写成 `m_curHuntingPoint >= m_maxHuntingPoint`（复现先载 +0x1c 后载 +0x20、末尾 jb）。
// 余 14 条 = 深区（sql/pkt）槽仍深 8 字节（浅区已对齐，两组在单一占位下互斥，见 Main S5 规约）+ 日志段寄存器选择。
void CVillageAttackedManager::SendCharacRank()
{
    // S5：ORIG 含声明但从未引用的局部占位（反汇编中不可见），它决定后续变量的槽起点。
    // 实测补齐 8 字节后本函数与 ORIG 的槽偏移一致（不补则整体错位 8）。
    int reservedSlot;
    int reservedSlot2;
    if (m_huntingPoints.empty())
    {
        return;
    }
    {
        std::priority_queue<stUserHuntingPoint> pq;
        for (std::map<unsigned int, stHuntingPoint>::iterator it = m_huntingPoints.begin();
             it != m_huntingPoints.end(); ++it)
        {
            stUserHuntingPoint p;
            // ORIG 先写 m_characNo（pair +0）后写 m_huntingPoint（pair +4）
            p.m_characNo = it->first;
            p.m_huntingPoint = it->second.m_huntingPoint;
            pq.push(p);
        }
        // ORIG 内联零填（rep stos）且声明点在 for 之后 ⇒ 用初始化器写在此处
        char sql[0xff] = {0};
        std::string query;
        // ORIG 是 4 字节无符号（Get_ServerGroup() 返回值零扩展后存入 4 字节槽），且声明点在块内
        unsigned int serverGroup;
        serverGroup = m_app->Get_ServerGroup();
        unsigned int now = GetNowTime();
        // ORIG 左操作数是 m_curHuntingPoint（先载 +0x1c 再载 +0x20，末尾 jb）
        if ((unsigned int)m_curHuntingPoint >= (unsigned int)m_maxHuntingPoint)
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
                    CMyFileLog logUserNull(__FUNCTION__, 0x238);
                    logUserNull("./log/village", "User is null [charac_no:%u]", p.m_characNo);
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
                                                               CVillageAttackedManager* mgr) throw()
    : CTask(time, flag)
{
    m_mgr = mgr;
}

CVillageAttackedCountdownFirst::~CVillageAttackedCountdownFirst() {}

CVillageAttackedCountdownSecond::CVillageAttackedCountdownSecond(unsigned int time,
                                                                 unsigned int flag,
                                                                 CVillageAttackedManager* mgr) throw()
    : CTask(time, flag)
{
    m_mgr = mgr;
}

CVillageAttackedCountdownSecond::~CVillageAttackedCountdownSecond() {}

CVillageAttackedCountdownThird::CVillageAttackedCountdownThird(unsigned int time,
                                                               unsigned int flag,
                                                               CVillageAttackedManager* mgr) throw()
    : CTask(time, flag)
{
    m_mgr = mgr;
}

CVillageAttackedCountdownThird::~CVillageAttackedCountdownThird() {}

CVillageAttackedStart::CVillageAttackedStart(unsigned int time, unsigned int flag,
                                             CVillageAttackedManager* mgr) throw()
    : CTask(time, flag)
{
    m_mgr = mgr;
}

CVillageAttackedStart::~CVillageAttackedStart() {}

CVillageAttackedEnd::CVillageAttackedEnd(unsigned int time, unsigned int flag,
                                         CVillageAttackedManager* mgr) throw()
    : CTask(time, flag)
{
    m_mgr = mgr;
}

CVillageAttackedEnd::~CVillageAttackedEnd() {}

CVillageAttackedReward::CVillageAttackedReward(unsigned int time, unsigned int flag,
                                               CVillageAttackedManager* mgr) throw()
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

// [DNF-IDENTICAL（2026-09-15 复核：已 identical ✓）] DNF-MON-DIFF-0050 | monitor | 与ORIG差异=DIFF | village_attacked::CVillageAttackedManager::SendRequestRevengeDungeon | 详见 function_reports/monitor/_ZN16village_attacked23CVillageAttackedManager25SendRequestRevengeDungeonEPc.md
void CVillageAttackedManager::SendRequestRevengeDungeon(char* pkt)
{
    Packet_Request_Revenge_Dungeon* p = (Packet_Request_Revenge_Dungeon*)pkt;
    p->m_rewardType = m_rewardType;
    p->m_remainTime = GetDungeonRemainTime();
}

unsigned int CVillageAttackedManager::GetDungeonRemainTime()
{
    return m_rewardCloseTime;
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
