// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CDailyScheduleManager/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r Game/ CDailyScheduleManager 实现（批3：从 GameStubs.cpp 迁移）。
#include "CDailyScheduleManager.h"

#include "CDungeon.h"
#include "GameWorld.h"
#include "CSystemTime.h"
#include "GlobalData.h"
#include "CGuildServerProxy.h"

#include <ctime>

#include "CDailyScheduleManager/AddDailSchedule.inc"
{
    // ORIG 0x08125c3c：逐条 push_back 到 m_schedules(+0xd8)。
    const unsigned int count = schedule.size();
    for (unsigned int i = 0; i < count; ++i)
        m_schedules.push_back(schedule[i]);
}

// ORIG 0x81266e2：size() 缓存 + while 循环；命中条件 (m_idx==idx && type==m_type)
// 且 m_state==1 时返回其 state；未命中返回 0。
#include "CDailyScheduleManager/GetScheduleState.inc"
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
// [b28d-solo-CDailyScheduleManager] AE：m_type 经匿名 union 的 enum 视图 m_typeE 与形参比较，
// 复现 ORIG `mov 0x4(%eax),%eax; cmp 0x10(%ebp),%eax`（P254 enum 字段保留直载；int 字段 4.4
// 恒两段 `mov,%edx; mov,%eax; cmp`）；m_idx unsigned 化（P211 档③）令 idx 形参出 ebx 缓存形。
// p167 CLEAR。
#include "CDailyScheduleManager/GM_OpenSchedule.inc"
                                            _DailySchedule& out)
{
    unsigned char start[3] = { 0, 0, 0 };            // -0x17..-0x15
    unsigned char end[3] = { 0x17, 0x32, 0x32 };     // -0x1a..-0x18

    const unsigned int count = m_schedules.size();   // -0x14
    unsigned int i = 0;                              // -0x10
    while (i < count)
    {
        if (idx == m_schedules[i].m_idx && m_schedules[i].m_typeE == type)
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
// [b28d-solo-CDailyScheduleManager] AE：循环体内 `register long curIdx = idx;`（P231 纯
// register，每轮重载 %ebx，与 ORIG 装载点同层；long 与 unsigned m_idx 异型使 4.4 出
// `cmp %ebx,%eax`）+ m_typeE 视图 + 第三处比较回退形参 idx（复现 ORIG `mov 0xc(%ebp),%edx`）。
// p167 CLEAR。
#include "CDailyScheduleManager/GM_CloseSchedule.inc"
                                             _DailySchedule& out)
{
    const unsigned int count = m_schedules.size();   // -0x14
    unsigned int i = 0;                              // -0x10
    while (i < count)
    {
        register long curIdx = idx;
        if (curIdx == m_schedules[i].m_idx &&
            m_schedules[i].m_typeE == type &&
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
        register long curIdx = idx;
        if (curIdx == m_schedules[j].m_idx &&
            m_schedules[j].m_type == idx &&
            idx == out.m_idx)
        {
            memcpy(&m_schedules[j], &out, sizeof(_DailySchedule));
        }
        ++j;
    }
}

// ORIG 0x8126c5c / 0x8126c8a：KingHorn 打开/关闭（id=1, type=3）。
#include "CDailyScheduleManager/GM_OpenKingHorn.inc"
#include "CDailyScheduleManager/GM_CloseKingHorn.inc"

// ORIG 0x8126cb8 / 0x8126d0c：BossTower 打开/关闭（两组：0xdb1/type0 + 0x505a/type1）。
#include "CDailyScheduleManager/GM_OpenBossTower.inc"
{
    GM_OpenSchedule(0xdb1, (_DailyScheduleType)0, m_bossTowerA);
    GM_OpenSchedule(0x505a, (_DailyScheduleType)1, m_bossTowerB);
}
#include "CDailyScheduleManager/GM_CloseBossTower.inc"
{
    GM_CloseSchedule(0xdb1, (_DailyScheduleType)0, m_bossTowerA);
    GM_CloseSchedule(0x505a, (_DailyScheduleType)1, m_bossTowerB);
}

// ORIG 0x8126d60 / 0x8126d90：AdvanceAltar 打开/关闭（id=10, type=4）。
#include "CDailyScheduleManager/GM_OpenAdvanceAltar.inc"
#include "CDailyScheduleManager/GM_CloseAdvanceAltar.inc"

// ---- SouthernDale（GM_OpenSouthernDale 0x81267bc / GM_CloseSouthernDale 0x8126ad2）----
// 打开：第一目标 (m_idx==100 && m_type==0) 回填 this+0x0（m_southernDaleA），
// 第二目标 (m_idx==0x5045 && m_type==1) 回填 m_southernDale(+0x24)；
// 各自把条目 m_wday 置 7，起始 h/m/s = 当前时刻，结束 h/m/s = 当前+600s；
// 关闭：把两目标各自写回 m_schedules 中匹配条目。
#include "CDailyScheduleManager/GM_OpenSouthernDale.inc"
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

#include "CDailyScheduleManager/GM_CloseSouthernDale.inc"
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
#include "CDailyScheduleManager/CheckAndSendToAllUserSchedule.inc"
{
    CheckAndSendToUserSchedule(0, false);
}

// ORIG 0x8125cb8：CheckAndSendToUserSchedule(&user, false)。
#include "CDailyScheduleManager/CheckAndSendToOneUserSchedule.inc"
{
    CheckAndSendToUserSchedule(&user, false);
}

// ORIG 0x8125ff4：按 _DailySchedule 时间窗与 CEventManager::GetRepeatEvent
// 判定条目应处状态；返回 1=进入（或进行中），0=不进入。
// [b28d-solo-CDailyScheduleManager] 已收：两处 GetRepeatEvent 实参经 m_eventIdE（enum 视图）
// 复现 ORIG `mov (%eax),%eax; mov %eax,%edx` 两段（P254）。
// 残差 1 条 mov 判 N18b 封顶：ORIG `mov 0x14(%eax),%eax; mov %eax,%edx; mov -0x18(%ebp),%eax;
// cmp %eax,%edx`，4.4 对成员==局部比较恒折 `mov 0x14(%eax),%edx` 单段；字段 int/unsigned/
// enum 视图 × 局部 int/unsigned/long/enum 十二形实测均不翻（P248 三档试尽）。
// [N5Free211to230.N5D 2026-09-11] 残差唯一 = 4.1 独有 regmove，判 **N18a** 封顶：
//   ORIG 0x08126150..0x08126158（4 条）/ 4.4 现形 0x381（3 条）= gap +1。
//   本波新增证伪记录（/tmp/r5/n5/p_sched4.cpp、p_sched5.cpp，7 形矩阵 + enum 字段）：
//   F_uint|F_int × L_uint|L_int|L_long、反向写法、`(int)` cast、真实 enum 字段（SDE）
//   全部产出 `mov (%eax),%eax; cmp %eax,-N(%ebp)`（mem 直比，**比现形还少 1 条**）
//   ⇒ 把 m_wday 改 enum（P254 档）在本「成员==局部」比较位是**负收益**，勿再试；
//   P254 三例（JobKey/ExpertJobKey/InfoDaily）均为 map 键构造/实参位，不适用于比较位。
#include "CDailyScheduleManager/CheckScheduleState.inc"
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
            GlobalData::s_event_manager->GetRepeatEvent(entry.m_eventIdE) == 0)
        {
            out = 0;
            return 0;
        }
        if (!GlobalData::s_event_manager->GetRepeatEvent(entry.m_eventIdE)->IsEventing(0))
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
    // [N5D] 上述②为唯一残差（ORIG 136 / 现形 135）；7 形类型矩阵探针全败 ⇒ N18a 封顶，
    // 不再在本位引入 (int)/enum/unsigned 变换（T5：类型不是排序旋钮）。

    if (result == 0)
        out = 0;
    else
        out = static_cast<unsigned int>(end - cur);

    return result;
}

// ORIG 0x81261a8：在 m_schedules 中查找与 entry 同 m_idx+m_type 的相邻条目。
// 返回 true 并回填 prev（前一条）或 next（后一条）；两条均未命中返回 false。
// [b28d-solo-CDailyScheduleManager] AE：①5 个 tm 声明序倒置（ORIG 槽浅→深=声明序，P309）；
// ②time_t 序 cur/startT/endT（ORIG -0x28/-0x2c/-0x30）；③m_typeE 视图；
// ④tm 成员比较左置（`tmEnd.tm_hour == m_schedules[i].m_startHour` 复现 ORIG 先载 tm 成员
// 入 ebx 跨 operator[] call 保活）；⑤isNext/isPrev 块互换 + `cur >= s`（P222/P258）；
// ⑥wday2 unsigned（P211 档③）；⑦while→for 复现 ORIG 循环增量落在分支 jmp 之后的底部条件
// 布局（P283 同族）。p167 CLEAR。
#include "CDailyScheduleManager/CheckContinueSchedule.inc"
                                                  _DailySchedule** prev,
                                                  _DailySchedule** next)
{
    std::tm tm;        // -0x5c
    std::tm tmStart;   // -0x88（= tm 拷贝，写入 entry 起始 h/m/s 后 mktime-1s）
    std::tm tmEnd;     // -0xb4（= tm 拷贝，写入 entry 结束 h/m/s 后 mktime+1s）
    std::tm tmNow2;    // -0xe0（isNext 分支用）
    std::tm tmNow3;    // -0x10c（isNext 分支用）
    std::time_t cur;   // -0x28
    std::time_t startT;// -0x2c
    std::time_t endT;  // -0x30
    int wday;          // -0x24
    unsigned int count;// -0x20
    unsigned int i;    // -0x1c
    bool isPrev;       // -0x16
    bool isNext;       // -0x15
    unsigned int wday2;// -0x14
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
    for (; i < count; ++i)
    {
        if (entry.m_idx != m_schedules[i].m_idx ||
            entry.m_typeE != m_schedules[i].m_typeE) continue;
        if (&m_schedules[i] == &entry) continue;

        isPrev = false;
        isNext = false;

        if (entry.m_state == 0)
        {
            // 本条起点 == 输入终点+1s 的分解 → 后一条。
            if (tmEnd.tm_hour == m_schedules[i].m_startHour &&
                tmEnd.tm_min == m_schedules[i].m_startMin &&
                tmEnd.tm_sec == m_schedules[i].m_startSec2)
                isNext = true;
        }
        else if (entry.m_state == 1)
        {
            // 本条终点 == 输入起点-1s 的分解 → 前一条。
            if (tmStart.tm_hour == m_schedules[i].m_endHour &&
                tmStart.tm_min == m_schedules[i].m_endMin &&
                tmStart.tm_sec == m_schedules[i].m_endSec2)
                isPrev = true;
        }

        if (isNext)
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
                if (cur >= s && cur <= e)
                    *next = &m_schedules[i];
            }
        }
        else
        {
            if (isPrev)
                *prev = &m_schedules[i];
        }
    }

    return (*next != 0 || *prev != 0);
}

// ORIG 0x8125cda 主循环，逐指令转写：
//  - 仅 user==0（广播模式）时做 CheckScheduleState 状态翻转；
//  - 翻转后经 CheckContinueSchedule 联动 prev/next 条目的 m_state；
//  - changed → 广播 0x1ed {type,state,idx,out}；!changed 且条目进行中且 user!=0
//    → 单播 0x1ed {type,2,idx,out}。
// [b28d-solo-CDailyScheduleManager] AE：①body/bodyU 移入各自块内（packet 之后声明），
// 复现 ORIG 数组占块内深槽（4.4 聚合优先深槽，N82附）；②cur/out/prev/next 声明序倒置
// （P309 先声明浅）；③单播块改 `else if (!changed && ...)` 复现 ORIG 广播块尾 jmp +
// 冗余 !changed 检查。p167 CLEAR。
#include "CDailyScheduleManager/CheckAndSendToUserSchedule.inc"
{
    std::tm tm;                    // -0x98（localtime 用）
    std::time_t cur;               // -0x24
    unsigned int out;              // -0x28
    _DailySchedule* prev;          // -0x2c
    _DailySchedule* next;          // -0x30
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
            int body[4];               // -0x4c..-0x40
            packet.put_header(0, 0x1ed);
            body[0] = entry->m_type;
            body[1] = entry->m_state;
            body[2] = entry->m_idx;
            body[3] = (int)out;
            packet.put_binary((char*)body, 0x10);
            packet.finalize(true);
            G_GameWorld()->send_all(packet);
        }
        else if (!changed && entry->m_state == 1 && user != 0)
        {
            // 单播：state 字段固定 2（ORIG 0x8125f58）。
            PacketGuard packet;
            int bodyU[4];              // -0x68..-0x5c
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
