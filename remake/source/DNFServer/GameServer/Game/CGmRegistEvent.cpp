// df_game_r GM 注册事件 CGmRegistEvent 实现。
// 报告：docs/class_func_reports/CGmRegistEvent/ 全部 6 个方法
// （CGmRegistEvent / ~CGmRegistEvent / ~CGmRegistEvent_0811ccc4 /
//  StartEvent / StartEvent_0811cd2c / EndEvent）。
#include "CGmRegistEvent.h"

#include "GameTypes.h"             // cMyTrace
#include "W8LT_DBTask.h"           // DB_UpdateGMData::makeRequest

// ---------------------------------------------------------------------------
// ORIG 0x0811cc78：仅基类构造 + vptr 置位（无自有成员）。
CGmRegistEvent::CGmRegistEvent()
{
}

// ORIG 0x0811cc94 / 0x0811ccc4：D1 重置 vptr 后调 CEventBase::~CEventBase；
// D0 调 D1 后 operator delete。空体 + 虚析构由编译器生成同形。
CGmRegistEvent::~CGmRegistEvent()
{
}

// ---------------------------------------------------------------------------
// ORIG 0x0811cce2：DB_UpdateGMData::makeRequest(true) +
// cMyTrace(__PRETTY_FUNCTION__,0x16,0)
// ("CGmRegistEvent Start!")（无 SetEventFlag）。
void CGmRegistEvent::StartEvent()
{
    DB_UpdateGMData::makeRequest(true);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x16, 0);
    tr("CGmRegistEvent Start!");
}

// ---------------------------------------------------------------------------
// ORIG 0x0811cd2c：同 StartEvent()（file 串
// "virtual void CGmRegistEvent::StartEvent(Word_Param)"，行 0x1d）。
void CGmRegistEvent::StartEvent(Word_Param param)
{
    DB_UpdateGMData::makeRequest(true);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x1d, 0);
    tr("CGmRegistEvent Start!");
}

// ---------------------------------------------------------------------------
// ORIG 0x0811cd76：仅 cMyTrace(__PRETTY_FUNCTION__,0x24,0)
// ("CGmRegistEvent End!")（无 SetEventFlag）。
void CGmRegistEvent::EndEvent()
{
    cMyTrace tr(__PRETTY_FUNCTION__, 0x24, 0);
    tr("CGmRegistEvent End!");
}
