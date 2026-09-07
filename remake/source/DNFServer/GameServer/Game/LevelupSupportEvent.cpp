// df_game_r 升级支持事件 LevelupSupportEvent 实现。
// ORIG 符号（报告缺失，nm 定址 + objdump 转写；地址见头文件注释）：
//   C1 0x081169f8 / D1 0x08116a14 / D0 0x08116a44 /
//   StartEvent() 0x08116a62 / StartEvent(Word_Param) 0x08116ab4 /
//   EndEvent 0x08116ace（均弱符号 W，独立 TU 定义）。
#include "LevelupSupportEvent.h"

#include "GameTypes.h"             // cMyTrace

// ---------------------------------------------------------------------------
// ORIG 0x081169f8：仅基类构造 + vptr 置位（无自有成员初始化）。
LevelupSupportEvent::LevelupSupportEvent()
{
}

// ORIG 0x08116a14 / 0x08116a44：D1 重置 vptr 后调 CEventBase::~CEventBase；
// D0 调 D1 后 operator delete。空体 + 虚析构由编译器生成同形。
LevelupSupportEvent::~LevelupSupportEvent()
{
}

// ---------------------------------------------------------------------------
// ORIG 0x08116a62：SetEventFlag(true) + cMyTrace(__FUNCTION__,0x3f,0)
// ("LevelupSupportEvent Event Start!")。
void LevelupSupportEvent::StartEvent()
{
    SetEventFlag(true);
    cMyTrace tr(__FUNCTION__, 0x3f, 0);
    tr("LevelupSupportEvent Event Start!");
}

// ---------------------------------------------------------------------------
// ORIG 0x08116ab4：thunk——经 vtable slot 2（StartEvent()）虚调用。
void LevelupSupportEvent::StartEvent(Word_Param param)
{
    // N7 封顶：唯一残差=虚调用 callee 寄存器互换（N7，探针 /tmp/l22p/vprobe*.cpp）。
    StartEvent();
}

// ---------------------------------------------------------------------------
// ORIG 0x08116ace：SetEventFlag(false) + cMyTrace(__FUNCTION__,0x49,0)
// ("LevelupSupportEvent Event End!")。
void LevelupSupportEvent::EndEvent()
{
    SetEventFlag(false);
    cMyTrace tr(__FUNCTION__, 0x49, 0);
    tr("LevelupSupportEvent Event End!");
}
