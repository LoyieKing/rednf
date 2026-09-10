// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CHadesServerProxy/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__ZN17CHadesServerProxyC2EPcii`（ORIG 0x84708c7）；ctor 已是首强符号。
#include <iostream>

#include "LogManager.h"
// df_game_r 还原 —— CHadesServerProxy（E1 批次）。
// 参照 docs/class_func_reports/CHadesServerProxy.md + docs/hades_protocol.md。
#include "CHadesServerProxy.h"

#include "GlobalData.h"
#include "StreamPool.h"
#include "CStreamGuard.h"
#include "MsgQueueMgr.h"
#include "PacketHeader.h"
#include "Packet_Hades_Udp_HeartBeat.h"

#include "CHadesServerProxy/CHadesServerProxy.inc"
    : BaseServerProxy(ip, tcpPort, "HadesSvr"), m_udpPort(udpPort), m_connSec(0)
{
    HadesSvrInit();
}

#include "CHadesServerProxy/HadesSvrInit.inc"
{
    ConnTcpHadesSvr();
    InitUdp();
    return 1;
}

#include "CHadesServerProxy/ConnTcpHadesSvr.inc"
{
    BaseServerProxy::Init();
    BaseServerProxy::SetSocketState(ENUM_SOCKET_STATE_CONNECTED);
    m_connSec = GlobalData::s_systemTime_.getCurSec();
    return 1;
}

#include "CHadesServerProxy/InitUdp.inc"
{
    if (m_udpHandler.InitClientSocket() == -1)
    {
        LogManager::logFormat(1, "DF_HadesServerProxy.cpp",
                              __PRETTY_FUNCTION__, 0xd5,
                              "[CHadesServerProxy::Init] Can't init udp client socket");
        return false;
    }
    return true;
}

#include "CHadesServerProxy/IsConnect.inc"
{
    if (BaseServerProxy::GetSocketState() == ENUM_SOCKET_STATE_CONNECTED)
    {
        return 1;
    }
    return 0;
}

#include "CHadesServerProxy/OnDisconnect.inc"
{
    RunConnTimer();
}

#include "CHadesServerProxy/Parsing.inc"
{
    return GetTcpHandler()->Parsing_ServerPacket(recvLen);
}

#include "CHadesServerProxy/RunConnTimer.inc"
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("DF_HadesServerProxy.cpp", 0x119),
                       true);
    (**guard) << 0x23a;
    (**guard) << -1;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
}

#include "CHadesServerProxy/SendHadesTcpPacket.inc"
{
    if (BaseServerProxy::GetSocketState() == ENUM_SOCKET_STATE_CONNECTED)
    {
        BaseServerProxy::SendTcpPacket(packet, ((PacketHeader*)packet)->packetSize);
    }
    return 1;
}

#include "CHadesServerProxy/SendHadesUdpPacket.inc"
{
    std::string serverIP = GetServerIP();
    m_udpHandler.SendToServer(packet, ((PacketHeader*)packet)->packetSize,
                              (unsigned short)m_udpPort, serverIP.c_str());
}

#include "CHadesServerProxy/SendHeartBeatPacket.inc"
{
    Packet_Hades_Udp_HeartBeat packet;
    packet.m_channelNo = (unsigned short)channelNo;
    packet.m_udpPort = udpPort;
    SendHadesUdpPacket((char*)&packet);
}
