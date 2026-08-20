#include "PacketGuard.h"
#include "GlobalData.h"

PacketGuard::PacketGuard()
    : InterfacePacketBuf(), m_pool(0), m_external(false)
{
    for (int i = 0; i < 9; ++i)
    {
        PacketPool* pool = GlobalData::s_packet_pool_list[i];
        if (pool)
        {
            m_pool = pool;
            break;
        }
    }
    if (m_pool)
        AcquirePacketBuf(m_pool->Acquire());
}

PacketGuard::PacketGuard(PacketBuf* packetBuf)
    : InterfacePacketBuf(), m_pool(0), m_external(true)
{
    AcquirePacketBuf(packetBuf);
}

PacketGuard::~PacketGuard()
{
    PacketBuf* packet = get();
    if (packet && !m_external && m_pool)
        m_pool->Free(packet);
    AcquirePacketBuf(0);
}
