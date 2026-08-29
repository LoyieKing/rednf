// df_game_r 升级支持二期事件 LevelupSupport2ndEvent 实现。
// ORIG 符号（报告缺失，nm 定址 + objdump 转写；地址见头文件注释）：
//   C1 0x0811b988 / D1 0x0811b9a4 / D0 0x0811b9d4 /
//   StartEvent() 0x0811b9f2 / StartEvent(Word_Param) 0x0811ba44 /
//   EndEvent 0x0811ba5e（均弱符号 W，独立 TU 定义）。
#include "LevelupSupport2ndEvent.h"

#include "GameTypes.h"             // cMyTrace

// ---------------------------------------------------------------------------
// ORIG 0x0811b988：仅基类构造 + vptr 置位（无自有成员初始化）。
LevelupSupport2ndEvent::LevelupSupport2ndEvent()
{
}

// ORIG 0x0811b9a4 / 0x0811b9d4：D1 重置 vptr 后调 CEventBase::~CEventBase；
// D0 调 D1 后 operator delete。空体 + 虚析构由编译器生成同形。
LevelupSupport2ndEvent::~LevelupSupport2ndEvent()
{
}

// ---------------------------------------------------------------------------
// ORIG 0x0811b9f2：SetEventFlag(true) + cMyTrace(__FUNCTION__,0x6b,0)
// ("LevelupSupport2ndEvent Event Start!")。
void LevelupSupport2ndEvent::StartEvent()
{
    SetEventFlag(true);
    cMyTrace tr(__FUNCTION__, 0x6b, 0);
    tr("LevelupSupport2ndEvent Event Start!");
}

// ---------------------------------------------------------------------------
// ORIG 0x0811ba44：thunk——经 vtable slot 2（StartEvent()）虚调用。
void LevelupSupport2ndEvent::StartEvent(Word_Param param)
{
    StartEvent();
}

// ---------------------------------------------------------------------------
// ORIG 0x0811ba5e：SetEventFlag(false) + cMyTrace(__FUNCTION__,0x75,0)
// ("LevelupSupport2ndEvent Event End!")。
void LevelupSupport2ndEvent::EndEvent()
{
    SetEventFlag(false);
    cMyTrace tr(__FUNCTION__, 0x75, 0);
    tr("LevelupSupport2ndEvent Event End!");
}
