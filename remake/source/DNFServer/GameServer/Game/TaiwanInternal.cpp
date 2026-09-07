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
