// df_game_r 收集图鉴事件日志 CCollectArchieveEventLog 实现。
// ORIG 符号（报告缺失，nm 定址 + objdump 转写，地址见头文件注释）。
#include "CCollectArchieveEventLog.h"

#include "GameTypes.h"             // cMyTrace

// ---------------------------------------------------------------------------
// ORIG 0x08116b48（弱 W）：仅基类构造 + vptr 置位。
CCollectArchieveEventLog::CCollectArchieveEventLog()
{
}

// ORIG 0x08116b64 / 0x08116b94（弱 W）：标准虚析构（空体编译器生成同形）。
CCollectArchieveEventLog::~CCollectArchieveEventLog()
{
}

// ---------------------------------------------------------------------------
// ORIG 0x08116bb2：SetEventFlag(true) + cMyTrace(__PRETTY_FUNCTION__,0x19,0)
// ("Collect Archieve Event Log Start!")。
void CCollectArchieveEventLog::StartEvent()
{
    SetEventFlag(true);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x19, 0);
    tr("Collect Archieve Event Log Start!");
}

// ---------------------------------------------------------------------------
// ORIG 0x08116c04：thunk——经 vtable slot 2（StartEvent()）虚调用。
void CCollectArchieveEventLog::StartEvent(Word_Param param)
{
    StartEvent();
}

// ---------------------------------------------------------------------------
// ORIG 0x08116c1e：SetEventFlag(false) + cMyTrace(__PRETTY_FUNCTION__,0x23,0)
// ("Collect Archieve Event Log End!")。
void CCollectArchieveEventLog::EndEvent()
{
    SetEventFlag(false);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x23, 0);
    tr("Collect Archieve Event Log End!");
}
