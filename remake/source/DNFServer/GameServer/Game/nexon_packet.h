#ifndef DNF_NEXON_PACKET_H
#define DNF_NEXON_PACKET_H

// ============================================================================
// nexon_packet 域权威声明（PC 房通信包封装）
// 反汇编基线：df_game_r ORIG 符号 _ZN12nexon_packet*
//   stHeader     ctor 0x086941cc / SetLength 0x086941e8 / GetLength 0x0869420a
//   stPacket     ctor 0x0869423e / GetLength 0x08694224
//   CPacketHelper ctor 0x086940d2 / get_offset 0x086940f2 /
//                padding 0x086940fc / scan 0x08694160
//   stLoginPacket ctor 0x08694252 (sz 0x15d) /
//                ReadPacket 0x086943af (sz 0x131)
// ============================================================================

namespace nexon_packet
{

// ORIG 域常量 _ZN12nexon_packetL11MaxBodySizeE（r 段 4 字节 = 0x1000，实测
// 0x08af2de4）；stLoginPacket body 缓冲长度。
static const int MaxBodySize = 4096;

// PC 房登录载荷。ORIG 中按原始偏移逐字段操作（CUser.cpp SendLoginToPCRoom-

// Server/Logout 亦按 +0/+1/+2/+0x101/+0x102/+0x201/+0x205/+0x206/+0x20a 填充），
// 因此定义为 packed，保证 sizeof==0x20e 且各偏移与 ORIG 一致。
#pragma pack(push, 1)
struct stLogin
{
    char         m_type;          // +0x000 (login=3 / logout=1)
    char         m_accLen;        // +0x001
    char         m_account[255];  // +0x002
    char         m_charLen;       // +0x101
    char         m_characName[255]; // +0x102
    unsigned int m_ip;            // +0x201
    char         m_canTry;        // +0x205
    unsigned int m_userIdx;       // +0x206
    unsigned int m_accId;         // +0x20a
};   // sizeof == 0x20e
#pragma pack(pop)

// ---- stHeader: magic@+0x00='AA', length(u16,网络序)@+0x01, cmd@+0x03 ----
struct stHeader
{
    stHeader();                                        // ORIG 0x086941cc

    void SetLength(unsigned short length);             // ORIG 0x086941e8
    unsigned short GetLength();                        // ORIG 0x0869420a

    unsigned char  m_magic;    // +0x00 写入 0xaa
    unsigned short m_length;   // +0x01 htons 存储
    unsigned char  m_cmd;      // +0x03（login 包构造中置 0x27）
};  // sizeof == 4

struct CPacketHelper
{
    CPacketHelper(unsigned char* buffer, int size, int startOffset); // ORIG

    int get_offset();                                  // ORIG 0x086940f2
    bool padding(unsigned char* src, int count);       // ORIG 0x086940fc
    bool scan(unsigned char* dst, int count);          // ORIG 0x08694160

    int             m_offset;  // +0x00 当前读写游标
    int             m_size;    // +0x04 缓冲区长度上限
    unsigned char*  m_buffer;  // +0x08 目标/源缓冲
};

// stPacket : stHeader。
// ORIG GetLength == stHeader::GetLength()+0x03 —— body 起始于 +0x04，
// “+3”即跨过 magic(u8)+length(u16) 三字节（cmd 不计入总长）。
struct stPacket : public stHeader
{
    stPacket();                                        // ORIG 0x0869423e
    int GetLength();                                   // ORIG 0x08694224
};

// stLoginPacket : stHeader + 0x1000 字节 body 缓冲。
// 构造时将 stLogin 各字段 padding 进 body，随后 SetLength(offset+1)、cmd=0x27。
class stLoginPacket : public stPacket
{
public:
    explicit stLoginPacket(stLogin& login);            // ORIG 0x08694252
    void ReadPacket(const char* packet, stLogin& out); // ORIG 0x086943af

private:
    // ORIG 无缺省构造/复制符号，删除之（保持唯一生命周期）
    stLoginPacket();
    stLoginPacket(const stLoginPacket&);
    stLoginPacket& operator=(const stLoginPacket&);

public:
    unsigned char m_body[MaxBodySize];  // +0x04
};

}  // namespace nexon_packet

#endif  // DNF_NEXON_PACKET_H
