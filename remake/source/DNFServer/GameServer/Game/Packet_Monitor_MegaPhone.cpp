// ============================================================================
// Packet_Monitor_MegaPhone::set（ORIG W 0x086d19b0，
//   _ZN24Packet_Monitor_MegaPhone3setEhtPKchS1_h，
//   objdump 逐指令转写；消费方 GameWorld::send_chat_msg case 0xb/c/e 家族）。
//   参数序：channel/uniqueId/name/msgLen/msg/type。
//   写 m_channel(+b)/m_uniqueId(+c)/m_type(+e) → strncpy m_name 0x1e →
//   strncpy m_msg 0x100 → m_msgLength(+2d) → packetSize(+2) = msgLen + 0x2e。
//   返回 void（ORIG ret 前无 eax 赋值；同族 Hyper_Link set 同式）。
//   视图访问（P82）：ORIG gcc4.1 每处 store 先载 this 再载值，gcc4.4 对普通
//   成员赋值生成相反顺序；经 pack(1) 视图逐字段赋值可恢复先 this 形态。
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

void Packet_Monitor_MegaPhone::set(
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
