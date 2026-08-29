#ifndef DNF_GAME_STATISTIC_SERVER_PROXY_H_
#define DNF_GAME_STATISTIC_SERVER_PROXY_H_

#include <string>

#include "CUdpHandler.h"

class CUser;

// ============================================================================
// df_game_r 还原 —— CStatisticServerProxy（统计上报 UDP 代理）。
// 布局（C1ESsi 0x08471f84 / C1Ev 0x082a6b72 / InitEv 0x08471fda / SendPacket
// 0x08472044 交叉验证，非 BaseServerProxy 派生、无 vptr）：
//   +0x00 std::string  m_ip     （C1ESsi 直 string 拷贝构造；SendPacket c_str）
//   +0x04 int          m_port   （C1 清 0；Init 判 0；SendPacket 低 16 位端口）
//   +0x08 CUdpHandler  m_udp    （Init 调 InitClientSocket；SendPacket SendToServer）
// sizeof = 0x10（string 4 + int 4 + CUdpHandler 8；CServerProxyMgr 元素步长 0）。
// C1Ev/InitESsii/D1Ev 为 ORIG 弱符号（0x082a6b72/0x082a6bc0/0x082b3282），
// 头内 inline 由使用 TU（GlobalData/GameMain 的 CServerProxyMgr 模板实例化）
// 落地；本 TU 只定义 ORIG T 强符号：C1ESsi/InitEv/SendPacket/SendFileStatistic×2。
// ============================================================================
class CStatisticServerProxy
{
public:
    CStatisticServerProxy();                          // ORIG W 0x082a6b72
    CStatisticServerProxy(std::string ip, int port);  // ORIG T 0x08471f84
    ~CStatisticServerProxy();                         // ORIG W 0x082b3282
    void Init(std::string ip, int tcpPort, int udpPort); // ORIG W 0x082a6bc0
    bool Init();                                      // ORIG T 0x08471fda
    void SendPacket(char* packet, int size);          // ORIG T 0x08472044
    void SendFileStatistic(const char* file);         // ORIG T 0x08472092
    void SendFileStatistic(const char* file, const char* key); // ORIG T 0x084720b4

    // ---- 对象布局（ORIG 直访成员，全部 public）----
    std::string m_ip;     // +0x00
    int m_port;           // +0x04
    CUdpHandler m_udp;    // +0x08
};

// ORIG W 弱符号（0x082a6b72）：string + CUdpHandler 构造后 m_port 清零
// （ORIG 汇编序：string ctor → CUdpHandler ctor → movl $0,this+4）。
inline CStatisticServerProxy::CStatisticServerProxy()
    : m_ip(), m_udp()
{
    m_port = 0;
}

// ORIG W 弱符号（0x082a6bc0）：记录 IP/端口后转调无参 Init()（udpPort 未用）。
inline void CStatisticServerProxy::Init(std::string ip, int tcpPort, int udpPort)
{
    m_ip = ip;
    m_port = tcpPort;
    Init();
}

// ORIG W 弱符号（0x082b3282）：仅 string 析构（CUdpHandler 无用户析构）。
inline CStatisticServerProxy::~CStatisticServerProxy()
{
}

// statistc_proxy::add（ORIG T 0x0860fdd2 _ZN14statistc_proxy3addEP5CUserPKcS3_jS3_z）：
// 变参统计上报（vsprintf 格式化后经统计代理发送）。定义属统计域批次，
// 此处仅声明供调用方（CParty::get_item 等）编译。
namespace statistc_proxy
{
void add(CUser* user, char const* table, char const* key, unsigned int value,
         char const* fmt, ...);
}

#endif
