#ifndef GAME_CUDPHANDLER_H_
#define GAME_CUDPHANDLER_H_

// ============================================================================
// df_game_r 还原 —— CUdpHandler（UDP socket 句柄）。
// 参照 docs/class_func_reports/CUdpHandler.md。
// 布局：+0x00 m_serverSocket | +0x04 m_clientSocket（sizeof = 8）
//   —— 已在 ORIG 验证：CHadesServerProxy 总长 0x30e1c（BaseServerProxy 0x30e0c
//      + m_udpPort 4 + CUdpHandler 8 + m_connSec 4）；CMonitorServerProxy
//      总长 0x30e0c（m_udp @ +0x30e04，8 字节）。CUdpHandler.md ctor 只写
//      +0/+0x04 两个 fd。
// 方法定义属于独立 TU（ORIG T 符号 0x8483518 等），本头只做声明；
// 调用点按 ORIG 原样引用，链接时由对应 TU 提供。
// ============================================================================

class CUdpHandler
{
public:
    CUdpHandler();                                          // 0x08483518
    int InitServerSocket(int port);                         // 0x08483530
    int InitClientSocket();                                 // 0x0848369a
    bool RecvFromClient(char* buf, int* len, unsigned int* ip,
                        unsigned short* port) const;        // 0x08483732
    bool SendToClient(char* buf, int len, unsigned short port,
                      const char* ip, unsigned int addr) const;   // 0x08483846
    bool RecvFromServer(char* buf, int* len, unsigned int* ip,
                        unsigned short* port) const;        // 0x08483a52
    bool SendToServer(char* buf, int len, unsigned short port,
                      const char* ip) const;                // 0x08483b68
    unsigned int InetAddr(const char* ip) const;            // 0x08483e06

private:
    int m_serverSocket;   // +0x00
    int m_clientSocket;   // +0x04
};

#endif  // GAME_CUDPHANDLER_H_
