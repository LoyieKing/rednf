// CNetwork<6144,100000> 在本 TU 隐式实例化（ORIG 同：10 个 W 符号位于本 TU
// 0x0818df60-0x0818e837；CNetwork.h 的 extern template 已按 Main 批准移除该实例）；
// CNetwork.cpp 保留显式实例化（weak 合并）。TU 内还含 <iostream> 的
// std::ios_base::Init 静态初始化（ORIG _GLOBAL__I_ keyed to C2）。
// 实例化 CNetwork<6144,100000>（显式实例化在 CNetwork.cpp），TU 内还含
// <iostream> 的 std::ios_base::Init 静态初始化（ORIG _GLOBAL__I_ keyed to C2）。
#include <iostream>

// [b28d-28] DIFF | ORIG 0x0818e2c8 | CNetwork<6144,100000>::Parsing_ServerPacket(int)
// （定义在共享头 CNetwork.h，本 TU 隐式实例化）：残差仅 1 条 `jmp` 位置互换
// （P223 边界：4.4 对单源 goto/空 then 尾岛恒 jump-threading 合并，N 封顶）。

#include "Arad_DevMgr_Server_Proxy.h"
#include "LogManager.h"

Arad_DevMgr_Server_Proxy::Arad_DevMgr_Server_Proxy(std::basic_string<char, std::char_traits<char>, std::allocator<char> > ip, int port)
    : m_port(port), m_ip(ip), m_network()
{
}

int Arad_DevMgr_Server_Proxy::open()
{
    return 1;
}

int Arad_DevMgr_Server_Proxy::init()
{
    if (m_network.m_sock < 0)
    {
        m_ip = "127.0.0.1";
        m_port = 30000;
        if (CNetwork<6144, 100000>::CreateConnectionSocket(m_network.m_sock, m_ip.c_str(), m_port) == 0)
        {
            return 0;
        }
    }
    return 1;
}

void Arad_DevMgr_Server_Proxy::disconnect()
{
    m_network.disconnect();
}

int Arad_DevMgr_Server_Proxy::recv()
{
    int size = m_network.recv_packet();
    if (size > 0)
    {
        if (!m_network.Parsing_ServerPacket(size))
        {
            LogManager::logFormat(1, "localjapan/Arad_DevMgr_Server_Proxy.cpp", __FUNCTION__, 0x34, "Monitor Recv Parsing Err!!!");
            return 0;
        }
        return 1;
    }
    if (size <= 0)
    {
        disconnect();
        return 0;
    }
    return 1;
}

int Arad_DevMgr_Server_Proxy::send(char* data, int len)
{
    if (m_network.send_packet() < 0)
    {
        return -1;
    }
    return m_network.GetRemainSendSize();
}
