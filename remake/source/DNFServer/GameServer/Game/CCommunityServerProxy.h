#ifndef GAME_CCOMMUNITYSERVERPROXY_H_
#define GAME_CCOMMUNITYSERVERPROXY_H_

#include <string>

#include "CNetwork.h"

// df_game_r 32 位布局（sizeof 0x30e04）：
//   +0x00000 CNetwork<100000, 100000>
//   +0x30df8 int m_udpPort
//   +0x30dfc std::string m_ip
//   +0x30e0c int m_tcpPort
// ORIG 构造函数先初始化网络基类，再清零 udpPort 并构造 ip。
class CCommunityServerProxy : public CNetwork<100000, 100000>
{
public:
    CCommunityServerProxy(char* ip, int port);

private:
    int m_udpPort;             // +0x30df8
    std::string m_ip;          // +0x30dfc
    int m_tcpPort;             // +0x30e0c
};

#endif
