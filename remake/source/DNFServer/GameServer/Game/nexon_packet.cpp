// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 nexon_packet/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// nexon_packet 域实现。逐符号按 ORIG 反汇编转写（df_game_r）。
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

// ORIG 0x086940d2 (sz 0x1f): m_offset=offset; m_size=size; m_buffer=buffer
#include "nexon_packet/CPacketHelper.inc"
}

// ORIG 0x086940f2 (sz 0xa): return this->m_offset;
#include "nexon_packet/get_offset.inc"
}

// ORIG 0x086940fc (sz 0x64):
//   if (m_offset + count >= m_size) return 0;
//   for (i=0; i<count; ++i) { m_buffer[m_offset++] = src[i]; }
//   return 1;
#include "nexon_packet/padding.inc"
}

// ORIG 0x08694160 (sz 0x6c):
//   if (m_offset + count >= m_size) return 0;
//   for (i=0; i<count; ++i) { dst[i] = m_buffer[m_offset++]; }
//   return 1;
// [b28d-D9] N18a/P200：ORIG 值先 eax 再中转 edx + m_buffer 缓存 %ebx；4.4 单段/不同寄存器（ORIG 0x08694160 sz 0x0000006c）
#include "nexon_packet/scan.inc"
}

// ---- stHeader --------------------------------------------------------------

// stHeader::stHeader 定义已迁 CUser.cpp 尾部（unit-at-a-time outgoing 塌缩
// 修复：本 TU 可见 callee 体使 stPacketC1 outgoing 塌缩 0x4，ORIG sub $0x18）。

// ORIG 0x086941e8 (sz 0x22): m_length = htons(length);
#include "nexon_packet/SetLength.inc"
}

// ORIG 0x0869420a (sz 0x1a): return ntohs(m_length);
#include "nexon_packet/GetLength.inc"
}

// ORIG 0x0869423e (sz 0x13): 只调用基类 stHeader ctor。
#include "nexon_packet/stPacket.inc"
}

// ORIG 0x08694224 (sz 0x19):
//   return static_cast<int>(stHeader::GetLength()) + 3;
#include "nexon_packet/GetLength_1.inc"
}

// ---- stLoginPacket ---------------------------------------------------------

// ORIG 0x08694252 (sz 0x15d)：
//   stPacket();
//   CPacketHelper helper(this->m_body, 0x1000, 0);
//   helper.padding((unsigned char*)&login.m_type,        1);           // +0x000 len 1
//   helper.padding((unsigned char*)&login.m_accLen,      1);           // +0x001 len 1
//   helper.padding((unsigned char*) login.m_account,
//                  (unsigned char)login.m_accLen);                     // +0x002 len accLen
//   helper.padding((unsigned char*)&login.m_charLen,     1);           // +0x101 len 1
//   helper.padding((unsigned char*) login.m_characName,
//                  (unsigned char)login.m_charLen);                    // +0x102 len charLen
//   helper.padding((unsigned char*)&login.m_ip,          4);           // +0x201 len 4
//   helper.padding((unsigned char*)&login.m_canTry,      1);           // +0x205 len 1
//   helper.padding((unsigned char*)&login.m_userIdx,     8);           // +0x206 len 8 (userIdx+accId)
//   SetLength(helper.get_offset()+1);
//   m_cmd = 0x27;
#include "nexon_packet/stLoginPacket.inc"
}

// ORIG 0x086943af (sz 0x131)：与 ctor 对称的读取路径。ORIG 将 m_body 内容
// scan 回 **packet 参数** 指向的缓冲（全部目标 = packet+偏移；out 参数未用，
// 见 CUser::SendLoginToPCRoomServer 桥接调用 (ReadPacketFn)(packet, reply)）。
//   CPacketHelper helper(this->m_body, 0x1000, 0);
//   helper.scan((unsigned char*)packet,      1);        // m_type
//   helper.scan((unsigned char*)packet+1,    1);        // m_accLen
//   helper.scan((unsigned char*)packet+2,    packet[1]); // m_account
//   helper.scan((unsigned char*)packet+0x101,1);        // m_charLen
//   helper.scan((unsigned char*)packet+0x102,packet[0x101]);
//   helper.scan((unsigned char*)packet+0x201,4);        // m_ip
//   helper.scan((unsigned char*)packet+0x205,1);        // m_canTry
//   helper.scan((unsigned char*)packet+0x206,8);        // m_userIdx+m_accId
// [b28d-D9] N95：CPacketHelper 对象槽基差 0xc（-0x18 vs -0xc）+ 帧 0x28 vs 0x20（ORIG 0x086943af sz 0x00000131）
#include "nexon_packet/ReadPacket.inc"
}

}  // namespace nexon_packet
