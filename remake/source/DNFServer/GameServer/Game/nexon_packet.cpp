// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 nexon_packet/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include "nexon_packet.h"

#include <arpa/inet.h>

#include <string.h>

namespace nexon_packet
{

// ---- ORIG 域常量 _ZN12nexon_packetL12domainStringE（r 段 0x08af2e20，32 字节）----
// 布局：4 组 { int, const char* } 对，指向 cain/diregie/first/release 字符串。
// gcc 4.4：namespace 内 static const 未使用也发射（内部链接，名含 L）。
struct DomainStringEntry
{
    int m_index;
    const char* m_string;
};
static const DomainStringEntry domainString[] = {
    { 0x00, "cain" },
    { 0x01, "diregie" },
    { 0x02, "first" },
    { 0x63, "release" },
};

// ---- CPacketHelper ---------------------------------------------------------

// ---- CPacketHelper ---------------------------------------------------------

#include "nexon_packet/CPacketHelper.inc"


#include "nexon_packet/get_offset.inc"


#include "nexon_packet/padding.inc"


#include "nexon_packet/scan.inc"


// ---- stHeader --------------------------------------------------------------

// stHeader::stHeader 定义已迁 CUser.cpp 尾部（unit-at-a-time outgoing 塌缩
// 修复：本 TU 可见 callee 体使 stPacketC1 outgoing 塌缩 0x4，ORIG sub $0x18）。

#include "nexon_packet/SetLength.inc"


#include "nexon_packet/GetLength.inc"


#include "nexon_packet/stPacket.inc"


#include "nexon_packet/GetLength_1.inc"


// ---- stLoginPacket ---------------------------------------------------------

#include "nexon_packet/stLoginPacket.inc"


#include "nexon_packet/ReadPacket.inc"


}  // namespace nexon_packet
