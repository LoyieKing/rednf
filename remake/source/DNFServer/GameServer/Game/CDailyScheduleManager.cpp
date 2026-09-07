// df_game_r Game/ CDailyScheduleManager 实现（批3：从 GameStubs.cpp 迁移）。
#include "CDailyScheduleManager.h"

#include "CDungeon.h"
#include "GameWorld.h"
#include "CSystemTime.h"
#include "GlobalData.h"
#include "CGuildServerProxy.h"

#include <ctime>

void CDailyScheduleManager::AddDailSchedule(const std::vector<_DailySchedule>& schedule)
{
    // ORIG 0x08125c3c：逐条 push_back 到 m_schedules(+0xd8)。
    const unsigned int count = schedule.size();
    for (unsigned int i = 0; i < count; ++i)
        m_schedules.push_back(schedule[i]);
}

// ORIG 0x81266e2：size() 缓存 + while 循环；命中条件 (m_idx==idx && type==m_type)
// 且 m_state==1 时返回其 state；未命中返回 0。
int CDailyScheduleManager::GetScheduleState(_DailyScheduleType type,
                                            unsigned int idx) const
{
    const unsigned int count = m_schedules.size();
    unsigned int i = 0;
    while (i < count)
    {
        if (m_schedules[i].m_idx == idx && type == m_schedules[i].m_type)
        {
            if (m_schedules[i].m_state == 1)
                return m_schedules[i].m_state;
        }
        ++i;
    }
    return 0;
}

// ORIG 0x8126dc0：找到 (m_idx==idx && m_type==type) 条目，把其拷贝到 out（+0x20 清 0），
// 并置该条目 m_wday=7、起始时分秒(0,0,0)、结束时(23,50,50)；随后第二遍处理 m_type==1
// （进行中）的同类条目（GM 强制开启）。
void CDailyScheduleManager::GM_OpenSchedule(int idx, _DailyScheduleType type,
                                            _DailySchedule& out)
{
    unsigned char start[3] = { 0, 0, 0 };            // -0x17..-0x15
    unsigned char end[3] = { 0x17, 0x32, 0x32 };     // -0x1a..-0x18

    const unsigned int count = m_schedules.size();   // -0x14
    unsigned int i = 0;                              // -0x10
    while (i < count)
    {
        if (idx == m_schedules[i].m_idx && m_schedules[i].m_type == static_cast<int>(type))
        {
            if (idx != out.m_idx)
            {
                memcpy(&out, &m_schedules[i], sizeof(_DailySchedule));  // 0x24
                out.m_state = 0;
            }
            m_schedules[i].m_wday = 7;               // +0x14
            memcpy(&m_schedules[i].m_startHour, start, 3);   // +0x18（0,0,0）
            memcpy(&m_schedules[i].m_endHour, end, 3);       // +0x1b（23,50,50）
            break;
        }
        ++i;
    }

    unsigned int j = 0;                              // -0xc
    while (j < count)
    {
        if (idx == m_schedules[j].m_idx && m_schedules[j].m_type == 1)
        {
            if (idx != out.m_idx)
            {
                memcpy(&out, &m_schedules[j], sizeof(_DailySchedule));
                out.m_state = 0;
            }
            m_schedules[j].m_wday = 7;
            memcpy(&m_schedules[j].m_startHour, start, 3);
            memcpy(&m_schedules[j].m_endHour, end, 3);
            break;
        }
        ++j;
    }
}

// ORIG 0x812707c：找到 (m_idx==idx && m_type==type && idx==out.m_idx) 条目，按其当前
// m_state 写 out.state（进行中则 1，否则 0），memcpy 回条目；第二遍对
// (m_idx==idx && m_type==idx && idx==out.m_idx) 条目做 memcpy（ORIG 第二遍用 idx 比较
// m_type，历史行为照抄）。
void CDailyScheduleManager::GM_CloseSchedule(int idx, _DailyScheduleType type,
                                             _DailySchedule& out)
{
    const unsigned int count = m_schedules.size();   // -0x14
    unsigned int i = 0;                              // -0x10
    while (i < count)
    {
        if (idx == m_schedules[i].m_idx &&
            m_schedules[i].m_type == static_cast<int>(type) &&
            idx == out.m_idx)
        {
            if (m_schedules[i].m_state == 1)
                out.m_state = 1;                     // +0x20
            else
                out.m_state = 0;
            memcpy(&m_schedules[i], &out, sizeof(_DailySchedule));  // 0x24
            break;
        }
        ++i;
    }

    unsigned int j = 0;                              // -0xc
    while (j < count)
    {
        if (idx == m_schedules[j].m_idx &&
            m_schedules[j].m_type == idx &&
            idx == out.m_idx)
        {
            memcpy(&m_schedules[j], &out, sizeof(_DailySchedule));
        }
        ++j;
    }
}

// ORIG 0x8126c5c / 0x8126c8a：KingHorn 打开/关闭（id=1, type=3）。
void CDailyScheduleManager::GM_OpenKingHorn() { GM_OpenSchedule(1, (_DailyScheduleType)3, m_kingHorn); }
void CDailyScheduleManager::GM_CloseKingHorn() { GM_CloseSchedule(1, (_DailyScheduleType)3, m_kingHorn); }

// ORIG 0x8126cb8 / 0x8126d0c：BossTower 打开/关闭（两组：0xdb1/type0 + 0x505a/type1）。
void CDailyScheduleManager::GM_OpenBossTower()
{
    GM_OpenSchedule(0xdb1, (_DailyScheduleType)0, m_bossTowerA);
    GM_OpenSchedule(0x505a, (_DailyScheduleType)1, m_bossTowerB);
}
void CDailyScheduleManager::GM_CloseBossTower()
{
    GM_CloseSchedule(0xdb1, (_DailyScheduleType)0, m_bossTowerA);
    GM_CloseSchedule(0x505a, (_DailyScheduleType)1, m_bossTowerB);
}

// ORIG 0x8126d60 / 0x8126d90：AdvanceAltar 打开/关闭（id=10, type=4）。
void CDailyScheduleManager::GM_OpenAdvanceAltar() { GM_OpenSchedule(10, (_DailyScheduleType)4, m_advanceAltar); }
void CDailyScheduleManager::GM_CloseAdvanceAltar() { GM_CloseSchedule(10, (_DailyScheduleType)4, m_advanceAltar); }

// ---- SouthernDale（GM_OpenSouthernDale 0x81267bc / GM_CloseSouthernDale 0x8126ad2）----
// 打开：第一目标 (m_idx==100 && m_type==0) 回填 this+0x0（m_southernDaleA），
// 第二目标 (m_idx==0x5045 && m_type==1) 回填 m_southernDale(+0x24)；
// 各自把条目 m_wday 置 7，起始 h/m/s = 当前时刻，结束 h/m/s = 当前+600s；
// 关闭：把两目标各自写回 m_schedules 中匹配条目。
void CDailyScheduleManager::GM_OpenSouthernDale()
{
    unsigned int count;                              // -0x14
    unsigned int i;                                  // -0x10
    unsigned int j;                                  // -0xc
    int target[2];                                   // -0x1c/-0x18（死局部，帧对齐）
    std::time_t cur;                                 // -0x20
    std::time_t endT;                                // -0x24
    unsigned char start[3];                          // -0x27..-0x25
    unsigned char end[3];                            // -0x2a..-0x28
    std::tm tmStart;                                 // -0x58
    std::tm tmEnd;                                   // -0x84

    target[0] = 100;
    target[1] = 0x5045;
    cur = GlobalData::s_systemTime_.getCurSec();
    localtime_r(&cur, &tmStart);
    endT = cur + 600;
    localtime_r(&endT, &tmEnd);

    start[0] = (unsigned char)tmStart.tm_hour;
    start[1] = (unsigned char)tmStart.tm_min;
    start[2] = (unsigned char)tmStart.tm_sec;
    end[0] = (unsigned char)tmEnd.tm_hour;
    end[1] = (unsigned char)tmEnd.tm_min;
    end[2] = (unsigned char)tmEnd.tm_sec;

    count = m_schedules.size();
    i = 0;
    while (i < count)
    {
        if (m_schedules[i].m_idx == 100 && m_schedules[i].m_type == 0)
        {
            if (m_southernDaleA.m_idx != 100)
            {
                memcpy(&m_southernDaleA, &m_schedules[i], sizeof(_DailySchedule));  // 0x24
                m_southernDaleA.m_state = 0;
            }
            m_schedules[i].m_wday = 7;               // +0x14
            memcpy(&m_schedules[i].m_startHour, start, 3);
            memcpy(&m_schedules[i].m_endHour, end, 3);
            break;
        }
        ++i;
    }

    j = 0;
    while (j < count)
    {
        if (m_schedules[j].m_idx == 0x5045 && m_schedules[j].m_type == 1)
        {
            if (m_southernDale.m_idx != 0x5045)
            {
                memcpy(&m_southernDale, &m_schedules[j], sizeof(_DailySchedule));
                m_southernDale.m_state = 0;
            }
            m_schedules[j].m_wday = 7;
            memcpy(&m_schedules[j].m_startHour, start, 3);
            memcpy(&m_schedules[j].m_endHour, end, 3);
            break;
        }
        ++j;
    }
}

void CDailyScheduleManager::GM_CloseSouthernDale()
{
    int target[2];                                   // -0x1c/-0x18（死局部，帧对齐）
    unsigned int count;                              // -0x14
    unsigned int i;                                  // -0x10
    unsigned int j;                                  // -0xc

    target[0] = 100;
    target[1] = 0x5045;
    count = m_schedules.size();
    i = 0;
    while (i < count)
    {
        if (m_schedules[i].m_idx == 100 && m_schedules[i].m_type == 0
            && m_southernDaleA.m_idx == 100)
        {
            memcpy(&m_schedules[i], &m_southernDaleA, sizeof(_DailySchedule));
        }
        ++i;
    }

    j = 0;
    while (j < count)
    {
        if (m_schedules[j].m_idx == 0x5045 && m_schedules[j].m_type == 1
            && m_southernDale.m_idx == 0x5045)
        {
            memcpy(&m_schedules[j], &m_southernDale, sizeof(_DailySchedule));
        }
        ++j;
    }
}
void CDailyScheduleManager::CheckAndSendToAllUserSchedule()
{
    CheckAndSendToUserSchedule(0, false);
}

// ORIG 0x8125cb8：CheckAndSendToUserSchedule(&user, false)。
void CDailyScheduleManager::CheckAndSendToOneUserSchedule(CUser& user)
{
    CheckAndSendToUserSchedule(&user, false);
}

// ORIG 0x8125ff4：按 _DailySchedule 时间窗与 CEventManager::GetRepeatEvent
// 判定条目应处状态；返回 1=进入（或进行中），0=不进入。
int CDailyScheduleManager::CheckScheduleState(const _DailySchedule& entry,
                                              unsigned int& out, bool flag) const
{
    (void)flag;

    std::tm tm;
    const std::time_t cur = GlobalData::s_systemTime_.getCurSec();
    localtime_r(&cur, &tm);
    const int wday = tm.tm_wday;

    int result = 0;

    if (entry.m_eventId != 0)
    {
        // ORIG 0x812603b：事件管理器为空或 GetRepeatEvent 为空 → 物化布尔后
        // out=0 返回 0；否则再调 GetRepeatEvent 经 vptr+0x34 判活（IsEventing）。
        if (GlobalData::s_event_manager == 0 ||
            GlobalData::s_event_manager->GetRepeatEvent(entry.m_eventId) == 0)
        {
            out = 0;
            return 0;
        }
        if (!GlobalData::s_event_manager->GetRepeatEvent(entry.m_eventId)->IsEventing(0))
        {
            out = 0;
            return 0;
        }
    }

    // 起止时刻（当日 h/m/s）基于当前日期求纪元秒，复用同一 tm（ORIG 行为）。
    tm.tm_hour = entry.m_startHour;
    tm.tm_min = entry.m_startMin;
    tm.tm_sec = entry.m_startSec2;
    const std::time_t start = mktime(&tm);

    tm.tm_hour = entry.m_endHour;
    tm.tm_min = entry.m_endMin;
    tm.tm_sec = entry.m_endSec2;
    const std::time_t end = mktime(&tm);

    if (entry.m_startSec <= cur && cur <= entry.m_endSec &&
        (wday == entry.m_wday || entry.m_wday == 7) &&
        cur >= start && cur <= end)
    {
        result = 1;
    }
    // 残差 ndiff 10（136→132）定性：
    // ①N18a×2：ORIG GetRepeatEvent 实参 `mov (%eax),%eax; mov %eax,%edx`
    // 双段 regmove，4.4 恒折 `mov (%eax),%edx` 单段（两处 call 前同形）；
    // ②N18b：`wday == entry.m_wday` ORIG 出 m_wday→edx、wday→eax 双物化
    // `cmp %eax,%edx`，4.4 恒折 `cmp -0x18(%ebp),%eax`（正反向写法均试）。
    // `cur >= start` 方向翻转已收（cmp start,cur+jg → cmp cur/start+jl 对齐）。

    if (result == 0)
        out = 0;
    else
        out = static_cast<unsigned int>(end - cur);

    return result;
}

// ORIG 0x81261a8：在 m_schedules 中查找与 entry 同 m_idx+m_type 的相邻条目。
// 返回 true 并回填 prev（前一条）或 next（后一条）；两条均未命中返回 false。
bool CDailyScheduleManager::CheckContinueSchedule(const _DailySchedule& entry,
                                                  _DailySchedule** prev,
                                                  _DailySchedule** next)
{
    std::tm tmNow3;    // -0x10c（isNext 分支用）
    std::tm tmNow2;    // -0xe0（isNext 分支用）
    std::tm tmEnd;     // -0xb4（= tm 拷贝，写入 entry 结束 h/m/s 后 mktime+1s）
    std::tm tmStart;   // -0x88（= tm 拷贝，写入 entry 起始 h/m/s 后 mktime-1s）
    std::tm tm;        // -0x5c
    std::time_t endT;  // -0x30
    std::time_t startT;// -0x2c
    std::time_t cur;   // -0x28
    int wday;          // -0x24
    unsigned int count;// -0x20
    unsigned int i;    // -0x1c
    bool isPrev;       // -0x16
    bool isNext;       // -0x15
    int wday2;         // -0x14
    std::time_t s;     // -0x10
    std::time_t e;     // -0xc

    cur = GlobalData::s_systemTime_.getCurSec();
    localtime_r(&cur, &tm);
    wday = tm.tm_wday;
    memcpy(&tmStart, &tm, sizeof(std::tm));   // 0x2c
    memcpy(&tmEnd, &tm, sizeof(std::tm));     // 0x2c

    tmStart.tm_hour = entry.m_startHour;
    tmStart.tm_min = entry.m_startMin;
    tmStart.tm_sec = entry.m_startSec2;
    startT = mktime(&tmStart);
    startT -= 1;
    localtime_r(&startT, &tmStart);

    tmEnd.tm_hour = entry.m_endHour;
    tmEnd.tm_min = entry.m_endMin;
    tmEnd.tm_sec = entry.m_endSec2;
    endT = mktime(&tmEnd);
    endT += 1;
    localtime_r(&endT, &tmEnd);

    count = m_schedules.size();
    i = 0;
    while (i < count)
    {
        if (entry.m_idx != m_schedules[i].m_idx ||
            entry.m_type != m_schedules[i].m_type) continue;
        if (&m_schedules[i] == &entry) continue;

        isPrev = false;
        isNext = false;

        if (entry.m_state == 0)
        {
            // 本条起点 == 输入终点+1s 的分解 → 后一条。
            if (m_schedules[i].m_startHour == tmEnd.tm_hour &&
                m_schedules[i].m_startMin == tmEnd.tm_min &&
                m_schedules[i].m_startSec2 == tmEnd.tm_sec)
                isNext = true;
        }
        else if (entry.m_state == 1)
        {
            // 本条终点 == 输入起点-1s 的分解 → 前一条。
            if (m_schedules[i].m_endHour == tmStart.tm_hour &&
                m_schedules[i].m_endMin == tmStart.tm_min &&
                m_schedules[i].m_endSec2 == tmStart.tm_sec)
                isPrev = true;
        }

        if (!isNext)
        {
            if (isPrev)
                *prev = &m_schedules[i];
        }
        else
        {
            // 后一条：仅当其在当前时间窗内被活动。
            localtime_r(&cur, &tmNow2);
            localtime_r(&cur, &tmNow3);
            wday2 = tmNow2.tm_wday;
            if (m_schedules[i].m_wday == wday2 || m_schedules[i].m_wday == 7)
            {
                tmNow3.tm_hour = m_schedules[i].m_startHour;
                tmNow3.tm_min = m_schedules[i].m_startMin;
                tmNow3.tm_sec = m_schedules[i].m_startSec2;
                s = mktime(&tmNow3);
                tmNow3.tm_hour = m_schedules[i].m_endHour;
                tmNow3.tm_min = m_schedules[i].m_endMin;
                tmNow3.tm_sec = m_schedules[i].m_endSec2;
                e = mktime(&tmNow3);
                if (s <= cur && cur <= e)
                    *next = &m_schedules[i];
            }
        }
        ++i;
    }

    return (*next != 0 || *prev != 0);
}

// ORIG 0x8125cda 主循环，逐指令转写：
//  - 仅 user==0（广播模式）时做 CheckScheduleState 状态翻转；
//  - 翻转后经 CheckContinueSchedule 联动 prev/next 条目的 m_state；
//  - changed → 广播 0x1ed {type,state,idx,out}；!changed 且条目进行中且 user!=0
//    → 单播 0x1ed {type,2,idx,out}。
void CDailyScheduleManager::CheckAndSendToUserSchedule(CUser* user, bool flag)
{
    std::tm tm;                    // -0x98（localtime 用）
    int bodyU[4];                  // -0x68..-0x5c（单播 body）
    int body[4];                   // -0x4c..-0x40（广播 body）
    _DailySchedule* next;          // -0x30
    _DailySchedule* prev;          // -0x2c
    unsigned int out;              // -0x28
    std::time_t cur;               // -0x24
    unsigned int count;            // -0x20
    int wday;                      // -0x1c
    unsigned int i;                // -0x18
    _DailySchedule* entry;         // -0x14
    bool changed;                  // -0xd
    int state;                     // -0xc

    count = m_schedules.size();
    cur = GlobalData::s_systemTime_.getCurSec();
    localtime_r(&cur, &tm);
    wday = tm.tm_wday;
    i = 0;
    while (i < count)
    {
        entry = &m_schedules[i];
        out = 0;
        changed = false;
        if (user == 0)
        {
            state = CheckScheduleState(*entry, out, flag);
            if (state == 1)
            {
                if (entry->m_state == 0)
                {
                    entry->m_state = 1;
                    changed = true;
                }
            }
            else if (state == 0)
            {
                if (entry->m_state == 1)
                {
                    entry->m_state = 0;
                    changed = true;
                }
            }
        }

        if (changed)
        {
            prev = 0;
            next = 0;
            if (CheckContinueSchedule(*entry, &prev, &next))
            {
                if (entry->m_state == 0)
                {
                    if (next)
                        next->m_state = 1;   // ORIG 0x8125e15
                    changed = false;
                }
                else if (entry->m_state == 1)
                {
                    if (prev)
                        prev->m_state = 0;   // ORIG 0x8125e37
                    changed = false;
                }
            }
        }

        if (changed)
        {
            // 广播 0x1ed：{type(+4), state(+0x20), idx(+8), out} 各 4 字节。
            PacketGuard packet;
            packet.put_header(0, 0x1ed);
            body[0] = entry->m_type;
            body[1] = entry->m_state;
            body[2] = entry->m_idx;
            body[3] = (int)out;
            packet.put_binary((char*)body, 0x10);
            packet.finalize(true);
            G_GameWorld()->send_all(packet);
        }
        if (!changed && entry->m_state == 1 && user != 0)
        {
            // 单播：state 字段固定 2（ORIG 0x8125f58）。
            PacketGuard packet;
            packet.put_header(0, 0x1ed);
            bodyU[0] = entry->m_type;
            bodyU[1] = 2;
            bodyU[2] = entry->m_idx;
            bodyU[3] = (int)out;
            packet.put_binary((char*)bodyU, 0x10);
            packet.finalize(true);
            user->Send(packet);
        }
        ++i;
    }
}
