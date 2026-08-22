#ifndef DNF_GAME_CDOUBLE_CONN_CHECK_SERVER_PROXY_H_
#define DNF_GAME_CDOUBLE_CONN_CHECK_SERVER_PROXY_H_

// df_game_r 还原 —— CDoubleConnCheckServerProxy（双开检测 UDP 代理，G2-4 批次）。
// 原类定义位于 CDoubleConnCheckServerProxy.cpp 内部；因 CGameManager.cpp 需真实
// 调用 SendPacket（原 asm 桥 _ZN27CDoubleConnCheckServerProxy10SendPacketEPci），
// 拆到本头作为唯一声明点（CDoubleConnCheckServerProxy.cpp 亦 include）。
// 布局：+0x00 std::string m_ip | +0x04 int m_port |
// +0x08 CUdpHandler m_udpHandler（sizeof 0x10）。

#include <string>

#include "CUdpHandler.h"

class CDoubleConnCheckServerProxy
{
public:
    CDoubleConnCheckServerProxy(std::string ip, int port);  // 0x0846d300 T

    bool Init();                                 // 0x0846d356 T
    void SendHeartBeat(int channel, int group);  // 0x0846d3ae T
    void SendLogin(unsigned int userID, int channel, int group);  // 0x0846d412 T
    void SendLogout(unsigned int userID, int channel, int group); // 0x0846d47c T
    void SendPacket(char* packet, int size);     // 0x0846d4ee T

    std::string m_ip;          // +0x00
    int m_port;                // +0x04
    CUdpHandler m_udpHandler;  // +0x08
};

#endif  // DNF_GAME_CDOUBLE_CONN_CHECK_SERVER_PROXY_H_
