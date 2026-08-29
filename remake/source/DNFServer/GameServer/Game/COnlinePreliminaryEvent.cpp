// df_game_r 在线预选赛事件 COnlinePreliminaryEvent 实现。
// 报告：docs/class_func_reports/COnlinePreliminaryEvent/ 全部 6 个方法。
#include "COnlinePreliminaryEvent.h"

#include "GameWorld.h"             // G_GameWorld / GetChannelType
#include "OnlinePreliminary.h"     // online_preliminary::COnlinePreliminary::SetMatchAble
#include "GameTypes.h"             // cMyTrace

// ---------------------------------------------------------------------------
// ORIG 0x081bb490：仅基类构造 + vptr 置位。
COnlinePreliminaryEvent::COnlinePreliminaryEvent()
{
}

// ORIG 0x081bb4ac / 0x081bb4dc：空体 + 虚析构由编译器生成同形。
COnlinePreliminaryEvent::~COnlinePreliminaryEvent()
{
}

// ---------------------------------------------------------------------------
// ORIG 0x081bb4fa：置事件标志、打日志；频道类型为 0xe 时开放匹配。
void COnlinePreliminaryEvent::StartEvent()
{
    SetEventFlag(true);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x15, 0);
    tr("COnlinePreliminaryEvent Start!");
    if (G_GameWorld()->GetChannelType() == 0xe)
    {
        online_preliminary::COnlinePreliminary::SetMatchAble(true);
    }
}

// ---------------------------------------------------------------------------
// ORIG 0x081bb56e：同 StartEvent()（忽略 param）。
void COnlinePreliminaryEvent::StartEvent(Word_Param param)
{
    SetEventFlag(true);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x1f, 0);
    tr("COnlinePreliminaryEvent Start!");
    if (G_GameWorld()->GetChannelType() == 0xe)
    {
        online_preliminary::COnlinePreliminary::SetMatchAble(true);
    }
}

// ---------------------------------------------------------------------------
// ORIG 0x081bb5e2：清事件标志；频道类型为 0xe 时关闭匹配。
void COnlinePreliminaryEvent::EndEvent()
{
    SetEventFlag(false);
    if (G_GameWorld()->GetChannelType() == 0xe)
    {
        online_preliminary::COnlinePreliminary::SetMatchAble(false);
    }
}
