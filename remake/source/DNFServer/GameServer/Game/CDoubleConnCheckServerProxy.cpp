// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CDoubleConnCheckServerProxy/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r 还原 —— CDoubleConnCheckServerProxy（双开检测 UDP 代理，G2-4 批次）。
// 逐函数对照 docs/class_func_reports/CDoubleConnCheckServerProxy.md 与
// ORIG 反汇编实现。布局：+0x00 std::string m_ip | +0x04 int m_port |
// +0x08 CUdpHandler m_udpHandler（sizeof 0x10）。
// 与 BaseServerProxy/CHadesServerProxy 不同：本类不继承 BaseServerProxy，
// 直接内嵌 string + port + CUdpHandler（ORIG ctor 0x846d300 逐成员推导）。
// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__ZN27CDoubleConnCheckServerProxyC2ESsi`（ORIG 0x846d56e）。
#include <iostream>

#include <string>

#include "CUdpHandler.h"
#include "LogManager.h"
#include "CDoubleConnCheckServerProxy.h"

// ---- UDP 双开检测包（布局来自 shared/packet/include，ctor 为独立 TU
//      0x846d602/0x846d58a/0x846d5c6；本地声明避免与共享头内联 ctor 冲突） ----
class Packet_Double_Check_Server_UDP_HeartBeat
{
public:
    Packet_Double_Check_Server_UDP_HeartBeat();   // 0x0846d602

    unsigned short m_packetId;    // +0x00
    unsigned short m_packetSize;  // +0x02
    unsigned short m_reversed1;   // +0x04
    unsigned int m_connNo;        // +0x06
    unsigned char m_channel;      // +0x0a
    unsigned char m_group;        // +0x0b
} __attribute__((packed));

class Packet_DoubleCheck_UDP_Login
{
public:
    Packet_DoubleCheck_UDP_Login();   // 0x0846d58a

    unsigned short m_packetId;    // +0x00
    unsigned short m_packetSize;  // +0x02
    unsigned short m_reversed1;   // +0x04
    unsigned int m_connNo;        // +0x06
    unsigned int m_userID;        // +0x0a
    unsigned char m_channel;      // +0x0e
    unsigned char m_group;        // +0x0f
} __attribute__((packed));

class Packet_DoubleCheck_UDP_Logout
{
public:
    Packet_DoubleCheck_UDP_Logout();   // 0x0846d5c6

    unsigned short m_packetId;    // +0x00
    unsigned short m_packetSize;  // +0x02
    unsigned short m_reversed1;   // +0x04
    unsigned int m_connNo;        // +0x06
    unsigned int m_accId;         // +0x0a
    unsigned char m_channel;      // +0x0e
    unsigned char m_serverGroup;  // +0x0f
} __attribute__((packed));

// CDoubleConnCheckServerProxy 类声明见 CDoubleConnCheckServerProxy.h（唯一声明点）

// ============================================================================
// 实现
// ============================================================================

#include "CDoubleConnCheckServerProxy/CDoubleConnCheckServerProxy.inc"


#include "CDoubleConnCheckServerProxy/Init.inc"


#include "CDoubleConnCheckServerProxy/SendHeartBeat.inc"


#include "CDoubleConnCheckServerProxy/SendLogin.inc"


#include "CDoubleConnCheckServerProxy/SendLogout.inc"


#include "CDoubleConnCheckServerProxy/SendPacket.inc"

