// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 AvatarItem/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r Game/ WongWork::Avatar_Item 实现（独立 TU，从 GameStubs.cpp 拆出）。
// 语义字段：m_emblemInfo 替代原 &m_ipgAgencyNo[0x19] 偏移访问。
#include "AvatarItem.h"
#include <cstddef>
#include <cstring>

static_assert(sizeof(stAvatarEmblemInfo_t) == 0x1e, "avatar emblem ABI layout changed");
static_assert(offsetof(stAvatarEmblemInfo_t, stEmblemSocket) == 0, "avatar emblem socket offset changed");
static_assert(sizeof(WongWork::Avatar_Item) == 0x44, "Avatar_Item ABI size changed (ORIG packed 0x44, DynamicPool MoreAlloc 步长 0x4c)");
static_assert(offsetof(WongWork::Avatar_Item, m_emblemInfo) == 0x21, "Avatar_Item emblem offset changed");
static_assert(offsetof(WongWork::Avatar_Item, m_color1) == 0x3f, "Avatar_Item color1 offset changed (ORIG 0x3f)");
static_assert(offsetof(WongWork::Avatar_Item, m_color2) == 0x41, "Avatar_Item color2 offset changed (ORIG 0x41)");

namespace WongWork
{

#include "AvatarItem/init.inc"


#include "AvatarItem/GetExpireDate.inc"

#include "AvatarItem/GetRegistTime.inc"

#include "AvatarItem/GetRemainDate.inc"


#include "AvatarItem/GetIPGAgencyNo.inc"

#include "AvatarItem/getJewelSocketData.inc"


#include "AvatarItem/getExpansionInfo.inc"

#include "AvatarItem/getExnansionInfoRef.inc"

// [b28d-Main·检查点18] 原 `setColorEss` 定义已删（发明名，ORIG 无此符号）；
//   权威定义在 WongWorkMiscB_A.cpp `Avatar_Item::setColor(short,short)` 0x082fa2fa。

#include "AvatarItem/setExpansionInfo.inc"
{
    // ORIG 0x82fa326：4 字节整读 info（m_color[0..1] 连续）整写 this+0x3f
    //（覆盖 m_color1/+0x3f 与 m_color2/+0x41）。4.4-O0 对两次 ushort 赋值
    // 恒拆 movzwl+mov %dx（14 条），整型写复现 ORIG 单 mov %edx,0x3f(%eax)（8 条）。
    // 复现 ORIG 单 `mov %edx,0x3f(%eax)`（无 add 折合）：P82 pack(1) 尾部视图
    // 具名成员访问保留 disp 寻址（探针实证；cast 指针 store 会折 add $0x3f）。
    struct __attribute__((packed)) V { char m_pad[0x3f]; unsigned int m_dw; };
    ((V*)this)->m_dw = *(const unsigned int*)&info.m_color[0];
}

#include "AvatarItem/SetData.inc"


#include "AvatarItem/setEmblemSocketData.inc"


// [b28d-Main·检查点18] 原 `updateEmblemSocket(int, unsigned long)` 定义已删（发明签名）；
//   权威定义在 WongWorkMiscB_A.cpp `updateEmblemSocket(unsigned int, unsigned int)` 0x082fa2a6。

}  // namespace WongWork
