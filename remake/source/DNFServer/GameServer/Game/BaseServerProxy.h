#ifndef GAME_BASESERVERPROXY_H_
#define GAME_BASESERVERPROXY_H_

// ============================================================================
// df_game_r 还原 —— BaseServerProxy（多态 TCP 代理基类）。
// 参照 docs/class_func_reports/BaseServerProxy.md。
// 布局：+0x00 vptr | +0x04 CNetwork<100000,100000>（0x30df8 字节）|
//       +0x30dfc m_socketState | +0x30e00 m_ip | +0x30e04 m_port |
//       +0x30e08 m_serverName（sizeof = 0x30e0c）
// vtable：[0]=OnDisconnect [1]=Disconnect [2]=Parsing（纯虚，__cxa_pure_virtual）
// 小访问器为 inline（ORIG 弱符号按使用 TU 落地：GetTcpHandler/GetServerName 在
// BaseServerProxy TU，GetServerIP/GetSocketState/SetSocketState 在 Hades TU）。
// ============================================================================

#include <string>

#include "CNetwork.h"

enum ENUM_SOCKET_STATE
{
    ENUM_SOCKET_STATE_DISCONNECTED = 0,
    ENUM_SOCKET_STATE_CONNECTING = 1,
    ENUM_SOCKET_STATE_CONNECTED = 2
};

class BaseServerProxy
{
public:
    BaseServerProxy(char* ip, int port, char* name);   // 0x082fd350
    ~BaseServerProxy();                                // 0x082aa92a

    bool Init();                                       // 0x082fd460
    bool InitTcp(bool bSetNonBlock);                   // 0x082fd47c
    bool SendTcpPacket(char* data, int len);           // 0x082fd830

    virtual void OnDisconnect();                       // vtable[0] 0x082fd82a
    virtual void Disconnect();                         // vtable[1] 0x082fd6e4
    virtual bool Parsing(int recvLen) = 0;             // vtable[2]（纯虚）

    bool OnTcpError();                                 // 0x082fdc2e
    bool OnTcpRecv();                                  // 0x082fd90e
    int OnTcpSend();                                   // 0x082fdaaa

    // ---- 小访问器（inline；ORIG 弱符号）----
    std::string GetServerName();                       // 0x082fdd6c
    std::string GetServerIP();                         // 0x0847093a
    CNetwork<100000,100000>* GetTcpHandler();          // 0x082f7ed6
    int GetSocketState();                              // 0x0847092c
    void SetSocketState(ENUM_SOCKET_STATE state);      // 0x0847091a

    // ---- 对象布局（ORIG 直访成员，全部 public）----
    CNetwork<100000,100000> m_tcpHandler;  // +0x04
    ENUM_SOCKET_STATE m_socketState;       // +0x30dfc
    std::string m_ip;                      // +0x30e00
    int m_port;                            // +0x30e04
    std::string m_serverName;              // +0x30e08
};

inline std::string BaseServerProxy::GetServerName() { return m_serverName; }
inline std::string BaseServerProxy::GetServerIP() { return m_ip; }
inline CNetwork<100000,100000>* BaseServerProxy::GetTcpHandler() { return &m_tcpHandler; }
inline int BaseServerProxy::GetSocketState() { return m_socketState; }
inline void BaseServerProxy::SetSocketState(ENUM_SOCKET_STATE state) { m_socketState = state; }

#endif  // GAME_BASESERVERPROXY_H_
