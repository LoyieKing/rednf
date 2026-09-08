#include "PacketGuard.h"
#include "GlobalData.h"
#include <pthread.h>

PacketGuard::PacketGuard()
    : InterfacePacketBuf(), m_pool(0), m_external(false)
{
    for (int i = 0; i < 9; ++i)
    {
        // [b24ab] P48：ORIG 无 pool 具名局部、无空判，逐点取 list[i]；线程比较
        // 走 pthread_equal（非 ==）。
        if (pthread_equal(GlobalData::s_packet_pool_list[i]->getThreadID(),
                          pthread_self()) != 0)
        {
            m_pool = GlobalData::s_packet_pool_list[i];
            break;
        }
    }
    if (m_pool)
    {
        AcquirePacketBuf(m_pool->Acquire());
    }
    else
    {
        LogManager::logFormat(1, "packet_buf.cpp", __PRETTY_FUNCTION__, 0x3e5,
                              "[PacketPool Acquire err - pool_type_err] "
                              "thread_id(%08x)",
                              (unsigned int)pthread_self());
    }
}

// ORIG T 0x0858de4a（size 0x36，17 insns）：本 ctor 在 ORIG 里**不带** EH landing
// pad（同 TU 的 PacketGuard() @0x0858dd4c 却带）——依据基类 ~InterfacePacketBuf
// 非平凡，唯一自洽解释是 InterfacePacketBuf::AcquirePacketBuf 声明为不抛
// （已在 InterfacePacketBuf.h 落 throw()，实测本函数 32→17 insns，逐条同构 AE）。
PacketGuard::PacketGuard(PacketBuf* packetBuf)
    : InterfacePacketBuf(), m_pool(0), m_external(true)
{
    AcquirePacketBuf(packetBuf);
}

PacketGuard::~PacketGuard()
{
    // ORIG 0x0858de70：m_pool==0 且 m_external!=1 → 打池类型错误日志（get()+
    // get_index+pthread_self）；否则 m_pool->Free(get())。基类 dtor 收尾。
    if (m_pool)   // ORIG 正条件：test %eax; je <log>，Free 落 fall-through
    {
        m_pool->Free(m_packetBuf);
    }
    else
    {
        if (m_external != 1)   // ORIG this[8]（bool 字节, xor$1/test 形）
        {
            LogManager::logFormat(
                1, "packet_buf.cpp", __PRETTY_FUNCTION__, 0x3f8,
                "[PacketPool Free err - pool_type_err] pool_type(%08x), packet_idx(%d)",
                pthread_self(), m_packetBuf->get_index());
        }
    }
    // [b24ab] 基类 ~InterfacePacketBuf 由编译器自动收尾（ORIG +0x9e call 0858e29a，
    // 非 AcquirePacketBuf(0)→ 后者多 this,0 两参数 + 多余 call）
}
