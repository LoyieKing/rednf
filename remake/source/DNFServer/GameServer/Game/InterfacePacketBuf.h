#ifndef GAME_INTERFACEPACKETBUF_H
#define GAME_INTERFACEPACKETBUF_H

#include "PacketBuf.h"

// ---------------------------------------------------------------------------
// InterfacePacketBuf：非虚适配层（仅持有 PacketBuf*，全部转发）。
// 布局：+0x00 PacketBuf* m_packetBuf（sizeof = 4）。
// ---------------------------------------------------------------------------

struct Inven_Item;

class InterfacePacketBuf
{
public:
    InterfacePacketBuf();
    ~InterfacePacketBuf();

    // 不抛异常（throw()）—— ORIG 实证：同 TU 内 PacketGuard::PacketGuard()
    // (0x0858dd4c) 带 EH landing pad（+0xdc call ~InterfacePacketBuf /
    // _Unwind_Resume），而 PacketGuard::PacketGuard(PacketBuf*) (0x0858de4a,
    // size 0x36) **无任何 cleanup**。基类 ~InterfacePacketBuf 非平凡
    // （ORIG W 0x858e29a），若本方法可抛则参数版 ctor 必生成 cleanup ⇒
    // ORIG 里本方法必然声明为不抛（否则两者不可同时成立）。
    void AcquirePacketBuf(PacketBuf* packetBuf) throw();
    bool bind_packet(char* packet, int size);
    void clear();
    void finalize(bool isEncryptRequired);
    PacketBuf* get() const;
    PacketBuf::PACKETBUF_ERROR_TYPE getLastError();
    bool get_binary(char* out, int size);
    int get_index();
    bool get_int(int& out);
    int get_len() const;
    char* get_packet(int size);
    bool get_short(short& out);
    bool is_finallized() const;
    bool put_binary(const char* data, int len);
    bool put_binary(char* data, int len);
    bool put_byte(int value);
    bool put_byte(int& index, int value);
    bool put_header(int packetId, int packetSize);
    bool put_int(int value);
    bool put_int(int& index, int value);
    bool put_item_idx(unsigned long value);
    void put_packet(const Inven_Item& item);
    bool put_short(int value);
    bool put_short(int& index, int value);
    bool put_str(const char* str, int len);
    bool put_str(char* str, int len);
    void set_index(int index);

private:
    PacketBuf* m_packetBuf;   // +0x00
};

TEST_CLASS_SIZE(InterfacePacketBuf, 4)

#endif  // GAME_INTERFACEPACKETBUF_H
