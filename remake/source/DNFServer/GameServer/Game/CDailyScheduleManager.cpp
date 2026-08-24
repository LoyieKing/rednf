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

// ORIG 0x81266e2：线性扫描 m_schedules，找 (m_idx==idx && m_type==type) 且
// m_state==1 的条目并返回其 state；未命中返回 0。
int CDailyScheduleManager::GetScheduleState(_DailyScheduleType type,
                                            unsigned int idx) const
{
    for (unsigned int i = 0; i < m_schedules.size(); ++i)
    {
        const _DailySchedule& entry = m_schedules[i];
        if (entry.m_idx == static_cast<int>(idx) &&
            entry.m_type == static_cast<int>(type) &&
            entry.m_state == 1)
            return entry.m_state;
    }
    return 0;
}

// ORIG 0x8126dc0：找到 (m_idx==idx && m_type==type) 条目，把其拷贝到 out（+0x20 清 0），
// 并置该条目 m_wday=7、起始时分秒(0,0,0)、结束时(23,50,50)；随后第二遍处理 m_type==1
// （进行中）的同类条目（GM 强制开启）。
void CDailyScheduleManager::GM_OpenSchedule(int idx, _DailyScheduleType type,
                                            _DailySchedule& out)
{
    for (unsigned int i = 0; i < m_schedules.size(); ++i)
    {
        _DailySchedule& entry = m_schedules[i];
        if (entry.m_idx == idx && entry.m_type == static_cast<int>(type))
        {
            if (entry.m_idx != out.m_idx)
            {
                out = entry;
                out.m_state = 0;
            }
            entry.m_wday = 7;                       // +0x14
            entry.m_startHour = 0;                  // +0x18
            entry.m_startMin = 0;                   // +0x19
            entry.m_startSec2 = 0;                  // +0x1a
            entry.m_endHour = 0x17;                 // +0x1b（23）
            entry.m_endMin = 0x32;                  // +0x1c（50）
            entry.m_endSec2 = 0x32;                 // +0x1d（50）
            break;
        }
    }
    for (unsigned int i = 0; i < m_schedules.size(); ++i)
    {
        _DailySchedule& entry = m_schedules[i];
        if (entry.m_idx == idx && entry.m_type == 1)
        {
            if (entry.m_idx != out.m_idx)
            {
                out = entry;
                out.m_state = 0;
            }
            entry.m_wday = 7;
            entry.m_startHour = 0;
            entry.m_startMin = 0;
            entry.m_startSec2 = 0;
            entry.m_endHour = 0x17;
            entry.m_endMin = 0x32;
            entry.m_endSec2 = 0x32;
            break;
        }
    }
}

// ORIG 0x812707c：找到 (m_idx==idx && m_type==type) 条目，按其当前 m_state 写 out.state
// （进行中则 1，否则 0），memcpy 回条目；第二遍对 m_idx==idx 条目做 memcpy。
void CDailyScheduleManager::GM_CloseSchedule(int idx, _DailyScheduleType type,
                                             _DailySchedule& out)
{
    for (unsigned int i = 0; i < m_schedules.size(); ++i)
    {
        _DailySchedule& entry = m_schedules[i];
        if (entry.m_idx == idx && entry.m_type == static_cast<int>(type)
            && entry.m_idx == out.m_idx)
        {
            out.m_state = (entry.m_state == 1) ? 1 : 0;   // +0x20
            entry = out;
            break;
        }
    }
    for (unsigned int i = 0; i < m_schedules.size(); ++i)
    {
        _DailySchedule& entry = m_schedules[i];
        if (entry.m_idx == idx && entry.m_type == idx && entry.m_idx == out.m_idx)
        {
            entry = out;
            break;
        }
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
// 打开：找 m_schedules 中 (m_idx==100 && m_type==0) 与 (m_idx==0x5045 && m_type==1) 两条，
// 各回填到 m_southernDale（+0x24），并把条目时间窗设为当前时刻 + 当前+600s；
// 关闭：把 m_southernDale 写回这两条（m_idx 匹配），并置 m_state。
void CDailyScheduleManager::GM_OpenSouthernDale()
{
    const std::time_t cur = GlobalData::s_systemTime_.getCurSec();
    std::tm tmStart;
    localtime_r(&cur, &tmStart);
    std::time_t end = cur + 600;
    std::tm tmEnd;
    localtime_r(&end, &tmEnd);

    int matched[2] = { 0, 0 };
    int target[2] = { 100, 0x5045 };
    int idx = 0;
    for (unsigned int i = 0; i < m_schedules.size(); ++i)
    {
        _DailySchedule& entry = m_schedules[i];
        if (entry.m_idx == target[idx] && (idx == 0 ? entry.m_type == 0 : entry.m_type == 1))
        {
            if (m_southernDale.m_idx != target[idx])
            {
                m_southernDale = entry;
                m_southernDale.m_state = 0;
            }
            entry.m_wday = 7;
            entry.m_startHour = (unsigned char)tmStart.tm_hour;
            entry.m_startMin = (unsigned char)tmStart.tm_min;
            entry.m_startSec2 = (unsigned char)tmStart.tm_sec;
            entry.m_endHour = (unsigned char)tmEnd.tm_hour;
            entry.m_endMin = (unsigned char)tmEnd.tm_min;
            entry.m_endSec2 = (unsigned char)tmEnd.tm_sec;
            matched[idx] = 1;
            if (idx == 0) { idx = 1; i = (unsigned int)-1; continue; }  // 重扫以找第二条
            break;
        }
    }
    (void)matched;
}

void CDailyScheduleManager::GM_CloseSouthernDale()
{
    const int target[2] = { 100, 0x5045 };
    for (unsigned int i = 0; i < m_schedules.size(); ++i)
    {
        _DailySchedule& entry = m_schedules[i];
        if (entry.m_idx == target[0] || entry.m_idx == target[1])
        {
            entry = m_southernDale;
        }
    }
}
void CDailyScheduleManager::CheckAndSendToAllUserSchedule()
{
    CheckAndSendToUserSchedule(0, false);
}

// ORIG 0x8125cb8：CheckAndSendToUserSchedule(user, false)。
void CDailyScheduleManager::CheckAndSendToOneUserSchedule(CUser* user)
{
    CheckAndSendToUserSchedule(user, false);
}

// ORIG 0x8125ff4：按 _DailySchedule 时间窗与 CEventManager::GetRepeatEvent
// 判定条目应处状态；返回 1=进入（或进行中），0=不进入。
int CDailyScheduleManager::CheckScheduleState(const _DailySchedule& entry,
                                              unsigned int& out, bool flag) const
{
    (void)flag;

    const std::time_t cur = GlobalData::s_systemTime_.getCurSec();

    std::tm tm;
    localtime_r(&cur, &tm);
    const int wday = tm.tm_wday;

    int result = 0;

    if (entry.m_eventId != 0)
    {
        CEventBase* ev = 0;
        if (GlobalData::s_event_manager != 0)
            ev = GlobalData::s_event_manager->GetRepeatEvent(entry.m_eventId);
        if (ev == 0)
        {
            out = 0;
            return 0;
        }
        if (!ev->IsEventing(0))
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
        (entry.m_wday == wday || entry.m_wday == 7) &&
        start <= cur && cur <= end)
    {
        result = 1;
    }

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
    if (prev)
        *prev = 0;
    if (next)
        *next = 0;

    const std::time_t cur = GlobalData::s_systemTime_.getCurSec();

    std::tm tmNow;
    localtime_r(&cur, &tmNow);
    const int wday = tmNow.tm_wday;

    // 输入条目当日起点/终点时刻，并分别退/进 1 秒得参考时/分/秒。
    std::tm tmStart = tmNow;
    tmStart.tm_hour = entry.m_startHour;
    tmStart.tm_min = entry.m_startMin;
    tmStart.tm_sec = entry.m_startSec2;
    const std::time_t start = mktime(&tmStart);
    const std::time_t startPrev = start - 1;
    std::tm tmStartPrev;
    localtime_r(&startPrev, &tmStartPrev);

    std::tm tmEnd = tmNow;
    tmEnd.tm_hour = entry.m_endHour;
    tmEnd.tm_min = entry.m_endMin;
    tmEnd.tm_sec = entry.m_endSec2;
    const std::time_t end = mktime(&tmEnd);
    const std::time_t endNext = end + 1;
    std::tm tmEndNext;
    localtime_r(&endNext, &tmEndNext);

    const unsigned int count = m_schedules.size();
    for (unsigned int i = 0; i < count; ++i)
    {
        _DailySchedule& other = m_schedules[i];
        if (other.m_idx == entry.m_idx &&
            other.m_type == entry.m_type && &other != &entry)
        {
            bool isNext = false;  // local_19
            bool isPrev = false;  // local_1a

            if (entry.m_state == 0)
            {
                // 本条起点 == 输入终点（退+1 秒参考）→ 后一条。
                if (tmEndNext.tm_hour == other.m_startHour &&
                    tmEndNext.tm_min == other.m_startMin &&
                    tmEndNext.tm_sec == other.m_startSec2)
                    isNext = true;
            }
            else if (entry.m_state == 1)
            {
                // 本条终点 == 输入起点（退-1 秒参考）→ 前一条。
                if (tmStartPrev.tm_hour == other.m_endHour &&
                    tmStartPrev.tm_min == other.m_endMin &&
                    tmStartPrev.tm_sec == other.m_endSec2)
                    isPrev = true;
            }

            if (!isNext)
            {
                if (isPrev && prev)
                    *prev = &other;
            }
            else
            {
                // 后一条：仅当其在当前时间窗内被活动。
                if (other.m_wday == wday || other.m_wday == 7)
                {
                    std::tm t = tmNow;
                    t.tm_hour = other.m_startHour;
                    t.tm_min = other.m_startMin;
                    t.tm_sec = other.m_startSec2;
                    const std::time_t s = mktime(&t);
                    t.tm_hour = other.m_endHour;
                    t.tm_min = other.m_endMin;
                    t.tm_sec = other.m_endSec2;
                    const std::time_t e = mktime(&t);
                    if (s <= cur && cur <= e && next)
                        *next = &other;
                }
            }
        }
    }

    return (*prev != 0 || *next != 0);
}

// ORIG 0x8125cda 主循环，逐指令转写：
void CDailyScheduleManager::CheckAndSendToUserSchedule(CUser* user, bool flag)
{
    const unsigned int count = m_schedules.size();
    for (unsigned int i = 0; i < count; ++i)
    {
        _DailySchedule* entry = &m_schedules[i];
        bool changed = false;

        unsigned int out = 0;   // ORIG 局部 out（CheckScheduleState 第二参）
        int state = CheckScheduleState(*entry, out, flag);

        if (state == 1)
        {
            if (!entry->m_state)
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

        if (changed)
        {
            _DailySchedule* prev = 0;
            _DailySchedule* next = 0;
            if (CheckContinueSchedule(*entry, &prev, &next))
            {
                if (!entry->m_state)
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
            // 广播 0x1ed：{type(+4), state(+0x20), idx(+8), 序号} 各 4 字节。
            PacketGuard packet;
            packet.put_header(0, 0x1ed);
            int body[4];
            body[0] = entry->m_type;
            body[1] = entry->m_state;
            body[2] = entry->m_idx;
            body[3] = (int)i;
            packet.put_binary((char*)body, 0x10);
            packet.finalize(true);
            G_GameWorld()->send_all(packet);
        }
        else if (entry->m_state == 1 && user != 0)
        {
            // 单播：state 字段固定 2（ORIG 0x8125f58）。
            PacketGuard packet;
            packet.put_header(0, 0x1ed);
            int body[4];
            body[0] = entry->m_type;
            body[1] = 2;
            body[2] = entry->m_idx;
            body[3] = (int)i;
            packet.put_binary((char*)body, 0x10);
            packet.finalize(true);
            user->Send(packet);
        }
    }
}
