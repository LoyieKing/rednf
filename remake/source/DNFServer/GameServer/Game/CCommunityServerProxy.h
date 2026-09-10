#ifndef GAME_CCOMMUNITYSERVERPROXY_H_
#define GAME_CCOMMUNITYSERVERPROXY_H_

#include <string>

#include "CNetwork.h"

#include "BaseServerProxy.h"   // ENUM_SOCKET_STATE（Get/SetSocketState 参数）

// df_game_r 32 位布局（sizeof 0x30e04）：
//   +0x00000 CNetwork<100000, 100000>
//   +0x30df8 int m_udpPort
//   +0x30dfc std::string m_ip
//   +0x30e00 int m_tcpPort
// ORIG 构造函数先初始化网络基类，再清零 udpPort 并构造 ip。
// （m_tcpPort @+0x30e00：ctor 0x0846c5c8 报告 `*(int*)(this+0x30e00)=param_2`；
//  原注释 +0x30e0c 有误，已按报告修正。）
class CUser;
struct SIG_LOAD_PVP_BUDDY;

class CCommunityServerProxy : public CNetwork<100000, 100000>
{
public:
    CCommunityServerProxy(char* ip, int port);   // ORIG T 0x0846c5c8
    void Init();                                 // ORIG T 0x0846c660（转发 InitTcp）
    bool InitTcp();                              // ORIG T 0x0846c674（P174 prett 串定：
                                                 // "bool CCommunityServerProxy::InitTcp()"）
    void Disconnect();                           // ORIG T 0x0846c770
    void SendTcpPacket(char* data, int len);     // ORIG T 0x0846c7c4
    bool OnTcpRecv();                            // ORIG T 0x0846c82c
    int OnTcpSend();                             // ORIG T 0x0846c8c4
    bool OnTcpError();                           // ORIG T 0x0846c962
    bool Parsing(int recvLen);                   // ORIG T 0x0846c9ec（转发 Parsing_ServerPacket）
    void SendLogin(CUser* user, SIG_LOAD_PVP_BUDDY* sig);          // ORIG T 0x0846ca06
    void SendLogout(unsigned int accId, unsigned int characNo);    // ORIG T 0x0846cb44
    // ORIG T 0x0846cb80 _ZN21CCommunityServerProxy16SendPvPBuddyListEjj
    //（Dispatcher_PvPBuddyConnList::dispatch_sig 0x08224e53 调用；DspD4 B6）
    void SendPvPBuddyList(unsigned int accId, unsigned int characNo);
    // ---- StubS3A_deps 批（PvP Buddy 包发送；定义 CCommunityServerProxy.cpp）----
    void SendReqAddPvPBuddy(unsigned int accId, unsigned int characNo, char flag, char* name, int nameLen);      // ORIG T 0x0846cbbc
    void SendResAddPvPBuddy(unsigned int accId, unsigned int characNo, char flag, char* name, int nameLen);      // ORIG T 0x0846cc22
    void SendRemovePvPBuddy(unsigned int accId, unsigned int characNo, char flag, char* name, int nameLen);      // ORIG T 0x0846cc88

    // ORIG W 0x0846d048：内联 getter 直返 this（OnTcpSend/OnTcpError 经此取 socket）
    inline CNetwork<100000, 100000>* GetTcpHandler() { return this; }

    // ORIG W 0x0857f476/0x0857f464：类自身 inline（weak）成员，读/写 +0x30df8
    //（即 m_udpPort = 连接态标志；MonitorTcpThread::dispatch 0x0857eb86..0x0857ed5f 调用）。
    // 非继承自 BaseServerProxy（本类基类是 CNetwork；BaseServerProxy 的版本操作 +0x30dfc）。
    inline int GetSocketState() { return m_udpPort; }
    inline void SetSocketState(ENUM_SOCKET_STATE state) { m_udpPort = state; }

private:
    // ORIG SetSocketState 0x0857f467：this 先载 + 全字直存 ⇒ 字段与 enum
    // 形参类型全等（技巧5）。GetSocketState 仍按 int 直返，全字 mov 不变形。
    ENUM_SOCKET_STATE m_udpPort;  // +0x30df8
    std::string m_ip;          // +0x30dfc
    int m_tcpPort;             // +0x30e00
};

#endif
