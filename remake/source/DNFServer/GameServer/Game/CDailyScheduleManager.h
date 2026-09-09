// df_game_r Game/ CDailyScheduleManager（批3：从 GameStubs.cpp 迁移）。
// ORIG 布局：+0xd8 vector<_DailySchedule>（GetScheduleState 0x81266e2 证据）。
#ifndef DNF_CDAILYSCHEDULEMANAGER_H
#define DNF_CDAILYSCHEDULEMANAGER_H

#include <vector>
#include "PacketGuard.h"

#include "GameEnums.h"

// 权威定义（原在 CDungeon.h；上移至此以解除前向声明循环，布局不变）。
// ORIG 布局（CheckScheduleState 0x8125ff4 / CheckContinueSchedule 0x81261a8 /
// GetScheduleState 0x81266e2 证据）：
//   +0x00 int m_eventId（重复事件 id，传给 CEventManager::GetRepeatEvent）
//   +0x04 int m_type（_DailyScheduleType）
//   +0x08 unsigned int m_idx
//   +0x0c int m_startSec / +0x10 int m_endSec（时间窗，秒）
//   +0x14 unsigned int m_wday（7 = 每天）
//   +0x18..0x1a byte 起始时/分/秒，+0x1b..0x1d byte 结束时/分/秒
//   +0x20 int m_state（1 = 进行中；GetScheduleState ORIG 0x81266e2）
enum _DailyScheduleEventIdEnum { _DSEI_NONE = 0 };

struct _DailySchedule
{
    union {
        int m_eventId;              // +0x00
        _DailyScheduleEventIdEnum m_eventIdE;
    };
    union {
        int m_type;                 // +0x04（_DailyScheduleType）
        _DailyScheduleType m_typeE;
    };
    unsigned int m_idx;         // +0x08（P211 档③：与 idx 形参异型触发 ORIG 形）
    int m_startSec;             // +0x0c
    int m_endSec;               // +0x10
    unsigned int m_wday;        // +0x14（7 = 每天）
    unsigned char m_startHour;  // +0x18
    unsigned char m_startMin;   // +0x19
    unsigned char m_startSec2;  // +0x1a
    unsigned char m_endHour;    // +0x1b
    unsigned char m_endMin;     // +0x1c
    unsigned char m_endSec2;    // +0x1d
    short m_pad1e;              // +0x1e
    int m_state;                // +0x20（1 = 进行中；GetScheduleState ORIG 0x81266e2）
};

class CUser;

class CDailyScheduleManager
{
public:
    CDailyScheduleManager();   // ORIG 0x08125c0c T（定义在 GlobalData.cpp）
    ~CDailyScheduleManager();  // ORIG 0x08125c24 T（定义在 GlobalData.cpp）
    void AddDailSchedule(const std::vector<_DailySchedule>& schedule);   // ORIG 0x08125c3c T
    int GetScheduleState(_DailyScheduleType type, unsigned int idx) const;
    void CheckAndSendToAllUserSchedule();  // ORIG _ZN21CDailyScheduleManager29CheckAndSendToAllUserScheduleEv @0x8125c94
    void CheckAndSendToOneUserSchedule(CUser& user); // ORIG 0x8125cb8（_ZN21CDailyScheduleManager29CheckAndSendToOneUserScheduleER5CUser）

    // GM 开关（自包含，遍历 m_schedules）：
    void GM_OpenSchedule(int idx, _DailyScheduleType type, _DailySchedule& out);   // ORIG 0x8126dc0
    void GM_CloseSchedule(int idx, _DailyScheduleType type, _DailySchedule& out);  // ORIG 0x812707c
    void GM_OpenKingHorn();     // ORIG 0x8126c5c：GM_OpenSchedule(1,3,&m_kingHorn)
    void GM_CloseKingHorn();    // ORIG 0x8126c8a：GM_CloseSchedule(1,3,&m_kingHorn)
    void GM_OpenBossTower();    // ORIG 0x8126cb8：Open(0xdb1,0,&m_bossA) + Open(0x505a,1,&m_bossB)
    void GM_CloseBossTower();   // ORIG 0x8126d0c：Close(0xdb1,0,&m_bossA) + Close(0x505a,1,&m_bossB)
    void GM_OpenAdvanceAltar(); // ORIG 0x8126d60：Open(10,4,&m_advanceAltar)
    void GM_CloseAdvanceAltar();// ORIG 0x8126d90：Close(10,4,&m_advanceAltar)
    void GM_OpenSouthernDale(); // ORIG 0x81267bc：打开 100/0x5045 两条并将时间窗设为当前+600s
    void GM_CloseSouthernDale();// ORIG 0x8126ad2：关闭写回 m_southernDale

private:
    // ORIG 0x8125cda：遍历 m_schedules，状态翻转后广播 0x1ed 包；
    // user!=0 且条目进行中时另发 state=2 单播。
    void CheckAndSendToUserSchedule(CUser* user, bool flag);
    int  CheckScheduleState(const _DailySchedule& entry, unsigned int& out, bool flag) const; // ORIG 0x8125ff4
    bool CheckContinueSchedule(const _DailySchedule& entry, _DailySchedule** prev, _DailySchedule** next); // ORIG 0x81261a8

public:

private:
    // ORIG GM_Open/CloseSouthernDale（0x81267bc/0x8126ad2 实证）：第一目标
    // (m_idx==100 && m_type==0) 的条目存于 this+0x0 的 _DailySchedule（GM_Close
    // 读 this+0x8 判 m_idx、memcpy src=this）；第二目标存 m_southernDale(+0x24)。
    _DailySchedule m_southernDaleA;          // +0x000（idx=100 / type 0）
    _DailySchedule m_southernDale;           // +0x024（idx=0x5045 / type 1）
    _DailySchedule m_kingHorn;               // +0x048（GM_Open/CloseKingHorn 参数）
    _DailySchedule m_bossTowerA;             // +0x06c（0xdb1 / type 0）
    _DailySchedule m_bossTowerB;             // +0x090（0x505a / type 1）
    _DailySchedule m_advanceAltar;           // +0x0b4（10 / type 4）
    std::vector<_DailySchedule> m_schedules; // +0xd8
};

#endif  // DNF_CDAILYSCHEDULEMANAGER_H