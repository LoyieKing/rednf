// df_game_r 还原 —— CCommunityServerProxy 构造函数。
// 依据 docs/class_func_reports/CCommunityServerProxy/CCommunityServerProxy.md。
#include "CCommunityServerProxy.h"

CCommunityServerProxy::CCommunityServerProxy(char* ip, int port)
    : CNetwork<100000, 100000>(), m_udpPort(0), m_ip(ip), m_tcpPort(port)
{
}
