// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 Packet_Monitor_MegaPhone/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
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

#include "Packet_Monitor_MegaPhone/set.inc"
}
