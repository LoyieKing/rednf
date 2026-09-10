// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 Packet_Monitor_MegaPhone/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include "Packet_Monitor_MegaPhone.h"
#include <string.h>

// 载荷视图（+0xa 起，m_sg 之后；P80 pack(1)）。仅用于 set 内逐字段访问，
// 不伪造对象生命周期；布局与 PacketHeader 基类 0xa 字节头一致。
#pragma pack(push, 1)
struct Packet_Monitor_MegaPhone_View
{
    char m_pad0[0xa];
    char m_sg;              // +a（set 不写）
    char m_channel;         // +b
    unsigned short m_uniqueId;  // +c
    unsigned char m_type;   // +e
    char m_name[0x1e];      // +f..+2c
    char m_msgLength;       // +2d
    char m_msg[0x100];      // +2e..+12d
};
#pragma pack(pop)

#include "Packet_Monitor_MegaPhone/set.inc"
    unsigned char channel, unsigned short uniqueId, char const* name,
    unsigned char msgLen, char const* msg, unsigned char type)
{
    ((Packet_Monitor_MegaPhone_View*)this)->m_channel = channel;
    ((Packet_Monitor_MegaPhone_View*)this)->m_uniqueId = uniqueId;
    ((Packet_Monitor_MegaPhone_View*)this)->m_type = type;
    strncpy(((Packet_Monitor_MegaPhone_View*)this)->m_name, name, 0x1e);
    strncpy(((Packet_Monitor_MegaPhone_View*)this)->m_msg, msg, 0x100);
    ((Packet_Monitor_MegaPhone_View*)this)->m_msgLength = msgLen;
    packetSize = 0x2e + msgLen;
}
