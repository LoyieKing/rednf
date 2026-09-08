#ifndef GAME_DBMWCLIENT_H_
#define GAME_DBMWCLIENT_H_

// ============================================================================
// df_game_r 还原 —— DbmwClient（AccountDBMW 中间件 socket 客户端）。
// ORIG 权威证据（df_game_r，报告 docs/class_func_reports/DbmwClient/，以
// objdump 字节偏移复核为准）：
//   C1/C2 T @0x08120454   D1/D2 T @0x081204be
//   Init 0x081205e6  set_query 0x081204fa  Close 0x08120ab0
//   SendHeader 0x08120654  RegistToMiddleware 0x081206c6
//   ReceiveRegistAck 0x08120702  Regist 0x081209ba  Connect 0x081209fa
//   SetServerAddress 0x08120af6  ReadData 0x08120b32
//   DoQuery 0x08120cd0  Error 0x08120c18  DoReceive 0x08120dbc
//   Reconnect 0x08121156  exec 0x081211c4  getAffectedRowCount 0x08120dac
//   IsConnected W 0x08121428 / IsInitialized W 0x0812141c（读 +0x02/+0x01）
// 布局：
//   +0x00 bool m_isSelect       （set_query 按前缀 "select" 判定）
//   +0x01 bool m_initialized    （Init 成功置 1；Init 先测 movzbl 0x1）
//   +0x02 bool m_connected      （Connect 测 movzbl 0x2；Close 清 0）
//   +0x04 int  m_reconnectCount （ctor 0；Reconnect++）
//   +0x08 int  m_sock           （ctor -1）
//   +0x0c unsigned m_field0c[2] （Init 成功清零，无其他访问语义→保留命名区）
//   +0x14 timeval m_timeout     （select 前 tv_sec=2/tv_usec=0）
//   +0x1c sockaddr_in m_addr    （family=AF_INET/inet_addr/htons）
//   +0x2c char* m_recvBuf       （operator new(0x200)，dtor delete）
//   +0x30 unsigned m_affectedRow（DoReceive: buf+0x16 处 uint）
//   +0x34 char m_query[0x1001]
//   +0x1038 unsigned long m_queryLen
// 协议（18B 头）：{byte0=0x15, byte1=msgType, int=len+0x12, 其余 12B 0}；
// 应答头：{buf+1=msg_no, uint(buf+2)=payloadLen(需 <0x1b), +0x12 payload}。
// ============================================================================

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>

class DbmwClient
{
public:
    DbmwClient();
    ~DbmwClient();

    unsigned int Init();                    // _ZN10DbmwClient4InitEv（-100/-0/-0）
    void SetServerAddress(const char* ip, unsigned short port);
    unsigned int Connect();                 // 0 ok / -98 init / -96 connected / -97 conn / -95 regist
    bool Reconnect();
    int  Close();                           // ORIG 返回 close 结果或 errno

    bool set_query(const char* fmt, ...);   // _ZN10DbmwClient9set_queryEPKcz
    bool DoQuery();
    int  DoReceive();                       // 0 err / 1 断开 ack / 2 数据到
    bool ReceiveRegistAck();
    char* getBuffer() const { return m_recvBuf; }

    int  ReadData(char* dst, int len);
    bool SendHeader(char type, int bodyLen);

    bool RegistToMiddleware();
    bool Regist();

    bool IsConnected() { return m_connected; }       // 头内联 → W（ORIG W @0x08121428，读 +0x02）
    bool IsInitialized() { return m_initialized; }   // 头内联 → W（ORIG W @0x0812141c，读 +0x01）

    int  exec();
    long long getAffectedRowCount();        // cpp 定义 → T（ORIG T @0x08120dac：读 +0x30，sar 符号扩展返 64 位）

    void Error(int level, int useErrno, char* fmt, ...);

private:
    bool          m_isSelect;        // +0x00
    bool          m_initialized;     // +0x01
    bool          m_connected;       // +0x02
    char          m_pad03;           // +0x03
    int           m_reconnectCount;  // +0x04
    int           m_sock;            // +0x08
    unsigned int  m_field0c[2];      // +0x0c
    timeval       m_timeout;         // +0x14
    sockaddr_in   m_addr;            // +0x1c
    char*         m_recvBuf;         // +0x2c
    int  m_affectedRow;              // +0x30（ORIG getAffectedRowCount 以 sar 符号扩展返回 → signed）
    char          m_query[0x1001];   // +0x34
    long          m_queryLen;        // +0x1038（ORIG set_query 0x08120572 用有符号
                                     // jg 比较、0x08120534 store 为 RHS-先+中转
                                     // ⇒ 字段 signed（P211 异型化：size_t RHS 异型
                                     // 触发 RHS-first）；宽度/mangled 不变）
};

#endif  // GAME_DBMWCLIENT_H_
