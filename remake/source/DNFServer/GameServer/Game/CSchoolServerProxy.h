#ifndef GAME_CSCHOOLSERVERPROXY_H_
#define GAME_CSCHOOLSERVERPROXY_H_

#include <string>

#include "CUdpHandler.h"

// df_game_r 还原：DF_SchoolServerProxy.cpp。
// 32 位布局：+0x00 std::string m_ip（sizeof 0x10） |
//       +0x10 int m_port | +0x14 CUdpHandler m_udpHandler（sizeof 0x10）。
// 成员顺序必须与 ORIG 构造函数对 this+4/this+8 的访问一致；
// this+4 是 std::string 的内部字段，不是 m_port。
class CSchoolServerProxy
{
public:
    CSchoolServerProxy(std::string ip, int port);

    bool Init();
    void SendPacket(char* packet, int length);

private:
    std::string m_ip;          // +0x00
    int m_port;                // +0x10
    CUdpHandler m_udpHandler;  // +0x14
};

#endif  // GAME_CSCHOOLSERVERPROXY_H_
