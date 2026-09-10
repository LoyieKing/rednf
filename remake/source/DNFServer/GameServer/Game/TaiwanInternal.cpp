// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__ZN6Taiwan15internal_streamER12CStreamGuardN18TaiwanInternalPack1TEi`
// （ORIG 0x8163a73）；internal_stream 已是首强符号。
#include <iostream>

#include "CStreamGuard.h"
#include "MoreTypes.h"
namespace Taiwan
{
void internal_stream(CStreamGuard& guard, TaiwanInternalPack::T packet, int uid)
{
    guard.operator*()->operator<<(0x34e);
    guard.operator*()->operator<<(uid);
    guard.operator*()->operator<<(static_cast<int>(packet));
}
void SigStayTimeEvent::init() { m_accId = 0; }
void SigStayTimeEvent::set(unsigned int accId) { m_accId = accId; }
}
