// df_game_r 停止重叠经验加成事件 CStopOverlabExpEvent 实现。
// 报告缺失（§3.2 兜底）：nm 定址 + objdump 逐指令转写（0x081177a0..0x081178c7）。
#include "CStopOverlabExpEvent.h"

#include "GameTypes.h"             // cMyTrace

// ---------------------------------------------------------------------------
// ORIG 0x081177a0：仅基类构造 + vptr 置位（无自有成员初始化）。
CStopOverlabExpEvent::CStopOverlabExpEvent()
{
}

// ORIG 0x081177bc / 0x081177ec：D1 重置 vptr 后调 CEventBase::~CEventBase；
// D0 调 D1 后 operator delete。空体 + 虚析构由编译器生成同形。
CStopOverlabExpEvent::~CStopOverlabExpEvent()
{
}

// ---------------------------------------------------------------------------
// ORIG 0x0811780a：SetEventFlag(true) +
// cMyTrace(__PRETTY_FUNCTION__,0x15,0)
// ("CStopOverlabExpEvent Event Start!")。
void CStopOverlabExpEvent::StartEvent()
{
    SetEventFlag(true);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x15, 0);
    tr("CStopOverlabExpEvent Event Start!");
}

// ---------------------------------------------------------------------------
// ORIG 0x0811785c：thunk —— 经 vptr slot2 虚调 StartEvent()（mov (%eax),%eax;
// add $0x8; mov (%eax),%edx; call *%edx），不读 param。
void CStopOverlabExpEvent::StartEvent(Word_Param param)
{
    StartEvent();
}

// ---------------------------------------------------------------------------
// ORIG 0x08117876：SetEventFlag(false) +
// cMyTrace(__PRETTY_FUNCTION__,0x1f,0)
// ("CStopOverlabExpEvent Event End!")。
void CStopOverlabExpEvent::EndEvent()
{
    SetEventFlag(false);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x1f, 0);
    tr("CStopOverlabExpEvent Event End!");
}
