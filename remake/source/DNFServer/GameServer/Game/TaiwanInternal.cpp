#include "CStreamGuard.h"
#include "MoreTypes.h"
namespace Taiwan
{
void internal_stream(CStreamGuard& guard, TaiwanInternalPack::T packet, int uid)
{
    guard << 0x34e << uid << static_cast<int>(packet);
}
void SigStayTimeEvent::init() { m_accId = 0; }
void SigStayTimeEvent::set(unsigned int accId) { m_accId = accId; }
}
