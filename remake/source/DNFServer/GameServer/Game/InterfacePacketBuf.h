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

    void AcquirePacketBuf(PacketBuf* packetBuf);
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
