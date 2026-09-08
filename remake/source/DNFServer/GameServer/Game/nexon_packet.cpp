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
CPacketHelper::CPacketHelper(unsigned char* buffer, int size, int startOffset)
    : m_offset(startOffset), m_size(size), m_buffer(buffer)
{
}

// ORIG 0x086940f2 (sz 0xa): return this->m_offset;
int CPacketHelper::get_offset()
{
    return m_offset;
}

// ORIG 0x086940fc (sz 0x64):
//   if (m_offset + count >= m_size) return 0;
//   for (i=0; i<count; ++i) { m_buffer[m_offset++] = src[i]; }
//   return 1;
bool CPacketHelper::padding(unsigned char* src, int count)
{
    if (static_cast<int>(static_cast<unsigned>(m_offset)) + count >= m_size)
        return false;
    // ORIG 0x08694126：`m_buffer[m_offset++] = src[i]` 后缀自增形——m_buffer
    // 落 %ecx、src+i 落 %edx、m_offset 留 %eax 跨 store（拆两句使 4.4 把
    // src+i 占 %eax 并重载 m_offset，+1 条；b28c-lane25 探针 /tmp/l25probe/pad.cpp fB）。
    for (int i = 0; i < count; ++i)
    {
        m_buffer[m_offset++] = static_cast<char>(src[i]);
    }
    return true;
}

// ORIG 0x08694160 (sz 0x6c):
//   if (m_offset + count >= m_size) return 0;
//   for (i=0; i<count; ++i) { dst[i] = m_buffer[m_offset++]; }
//   return 1;
bool CPacketHelper::scan(unsigned char* dst, int count)
{
    if (static_cast<int>(static_cast<unsigned>(m_offset)) + count >= m_size)
        return false;
    for (int i = 0; i < count; ++i)
    {
        dst[i] = static_cast<char>(m_buffer[m_offset]);
        ++m_offset;
    }
    return true;
}

// ---- stHeader --------------------------------------------------------------

// stHeader::stHeader 定义已迁 CUser.cpp 尾部（unit-at-a-time outgoing 塌缩
// 修复：本 TU 可见 callee 体使 stPacketC1 outgoing 塌缩 0x4，ORIG sub $0x18）。

// ORIG 0x086941e8 (sz 0x22): m_length = htons(length);
void stHeader::SetLength(unsigned short length)
{
    m_length = htons(length);
}

// ORIG 0x0869420a (sz 0x1a): return ntohs(m_length);
unsigned short stHeader::GetLength()
{
    return ntohs(m_length);
}

// ORIG 0x0869423e (sz 0x13): 只调用基类 stHeader ctor。
stPacket::stPacket()
    : stHeader()
{
}

// ORIG 0x08694224 (sz 0x19):
//   return static_cast<int>(stHeader::GetLength()) + 3;
int stPacket::GetLength()
{
    return static_cast<int>(stHeader::GetLength()) + 3;
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
stLoginPacket::stLoginPacket(stLogin& login)
{
    CPacketHelper helper(m_body, MaxBodySize, 0);

    helper.padding(reinterpret_cast<unsigned char*>(&login.m_type), 1);
    helper.padding(reinterpret_cast<unsigned char*>(&login.m_accLen), 1);
    helper.padding(
        reinterpret_cast<unsigned char*>(login.m_account),
        static_cast<int>(static_cast<unsigned char>(login.m_accLen)));
    helper.padding(reinterpret_cast<unsigned char*>(&login.m_charLen), 1);
    helper.padding(
        reinterpret_cast<unsigned char*>(login.m_characName),
        static_cast<int>(static_cast<unsigned char>(login.m_charLen)));
    helper.padding(reinterpret_cast<unsigned char*>(&login.m_ip), 4);
    helper.padding(reinterpret_cast<unsigned char*>(&login.m_canTry), 1);
    // ORIG 一条 padding 长度 8：连续覆盖 userIdx(+0x206..+0x209)+accId(+0x20a..+0x20d)
    helper.padding(reinterpret_cast<unsigned char*>(&login.m_userIdx), 8);

    SetLength(static_cast<unsigned short>(helper.get_offset() + 1));
    m_cmd = 0x27;
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
void stLoginPacket::ReadPacket(const char* packet, stLogin& out)
{
    (void)out;  // ORIG 未使用该参数（解码目标为 packet 缓冲）

    CPacketHelper helper(m_body, MaxBodySize, 0);

    helper.scan(reinterpret_cast<unsigned char*>(const_cast<char*>(packet)), 1);
    helper.scan(reinterpret_cast<unsigned char*>(const_cast<char*>(packet)) + 1, 1);
    helper.scan(reinterpret_cast<unsigned char*>(const_cast<char*>(packet)) + 2,
                static_cast<int>(static_cast<unsigned char>(packet[1])));
    helper.scan(reinterpret_cast<unsigned char*>(const_cast<char*>(packet)) + 0x101, 1);
    helper.scan(reinterpret_cast<unsigned char*>(const_cast<char*>(packet)) + 0x102,
                static_cast<int>(static_cast<unsigned char>(packet[0x101])));
    helper.scan(reinterpret_cast<unsigned char*>(const_cast<char*>(packet)) + 0x201, 4);
    helper.scan(reinterpret_cast<unsigned char*>(const_cast<char*>(packet)) + 0x205, 1);
    helper.scan(reinterpret_cast<unsigned char*>(const_cast<char*>(packet)) + 0x206, 8);
}

}  // namespace nexon_packet
