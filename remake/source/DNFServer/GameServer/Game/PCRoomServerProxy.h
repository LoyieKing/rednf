#ifndef DNF_PCROOMSERVERPROXY_H
#define DNF_PCROOMSERVERPROXY_H

// ============================================================================
// CPCRoomServerProxy 权威声明（PC 房服务器代理）
// 原为 GlobalData.cpp TU 局部类；为让 CUser::SendLoginToPCRoomServer /
// SendLogoutToPCRoomServer 按 ORIG 符号形态直调（替代硬编码 ORIG 地址桥），
// 收敛为独立头文件（2026-08-30 Fix14_I）。
// 反汇编基线：ORIG T 0x084717dc（ctor）/ 0x08471a32（SendTcpPacket）。
// ============================================================================

#include <string>
#include "CNetwork.h"
#include "BaseServerProxy.h"   // ENUM_SOCKET_STATE

class CPCRoomServerProxy
{
public:
    CPCRoomServerProxy(char* ip, int port);   // ORIG T 0x084717dc
    ~CPCRoomServerProxy();                    // ORIG T 0x08471874
    bool Init();                              // ORIG T 0x084718bc（!InitTcp() 取反）
    bool InitTcp();                           // ORIG T 0x084718e2
    void Disconnect();                        // ORIG T 0x084719de
    void SendTcpPacket(char* data, int len);  // ORIG T 0x08471a32
    bool OnTcpRecv();                         // ORIG T 0x08471a9a
    int OnTcpSend();                          // ORIG T 0x08471b32
    bool OnTcpError();                        // ORIG T 0x08471bd0
    bool Parsing(int recvLen);                // ORIG T 0x08471c5a（转发 Parsing_ServerPacket）
    // ORIG W 0x0857f444/0x0857f456：socketState 直写/直读（P119 外联定义发射 W 符号）。
    void SetSocketState(ENUM_SOCKET_STATE state);
    int GetSocketState();
    // ORIG W 0x08471e08：getter 直返 this（m_tcp @+0x00 → &m_tcp == this）（P119 外联）。
    CNetwork<100000, 100000>* GetTcpHandler();
private:
    // ORIG ctor 0x084717dc：CNetwork@+0x00 + string@+0x30df8 +
    // m_tcpPort@+0x30dfc + socketState@+0x30e00（sizeof 0x30e04）。
    CNetwork<100000, 100000> m_tcp;   // +0x00
    std::string m_ip;                 // +0x30df8
    int m_tcpPort;                    // +0x30dfc
    ENUM_SOCKET_STATE m_socketState;  // +0x30e00
};

#endif  // DNF_PCROOMSERVERPROXY_H
