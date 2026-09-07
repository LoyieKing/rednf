// df_game_r 秘密商店事件 CSecretShopEvent 实现。
// 报告缺失（§3.2 兜底）：nm 定址 + objdump 逐指令转写（0x081172c2..0x081173e9）。
#include "CSecretShopEvent.h"

#include "GameTypes.h"             // cMyTrace

// ---------------------------------------------------------------------------
// ORIG 0x081172c2：仅基类构造 + vptr 置位（无自有成员初始化）。
CSecretShopEvent::CSecretShopEvent()
{
}

// ORIG 0x081172de / 0x0811730e：D1 重置 vptr 后调 CEventBase::~CEventBase；
// D0 调 D1 后 operator delete。空体 + 虚析构由编译器生成同形。
CSecretShopEvent::~CSecretShopEvent()
{
}

// ---------------------------------------------------------------------------
// ORIG 0x0811732c：SetEventFlag(true) +
// cMyTrace(__PRETTY_FUNCTION__,0x16,0)
// ("CSecretShopEvent Event Start!")。
void CSecretShopEvent::StartEvent()
{
    SetEventFlag(true);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x16, 0);
    tr("CSecretShopEvent Event Start!");
}

// ---------------------------------------------------------------------------
// ORIG 0x0811737e：thunk —— 经 vptr slot2 虚调 StartEvent()，不读 param。
void CSecretShopEvent::StartEvent(Word_Param param)
{
    // N7 封顶：唯一残差=虚调用 callee 寄存器互换（N7，探针 /tmp/l22p/vprobe*.cpp）。
    StartEvent();
}

// ---------------------------------------------------------------------------
// ORIG 0x08117398：SetEventFlag(false) +
// cMyTrace(__PRETTY_FUNCTION__,0x20,0)
// ("CSecretShopEvent Event End!")。
void CSecretShopEvent::EndEvent()
{
    SetEventFlag(false);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x20, 0);
    tr("CSecretShopEvent Event End!");
}
