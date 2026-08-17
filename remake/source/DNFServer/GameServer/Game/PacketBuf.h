#ifndef GAME_PACKETBUF_H
#define GAME_PACKETBUF_H

#include "ReverseEngineerLib.h"
#include "GameTypes.h"
#include "Cipher.h"
#include "Encryption.h"
#include "Inven_Item.h"

// ---------------------------------------------------------------------------
// 最小外部依赖声明（单 TU 编译用；实现由各自模块提供）。
// 加密/压缩/日志/锁均为通用算法或基础类，只要求算法正确、不逐指令对齐。
// ---------------------------------------------------------------------------

extern "C" int compress(unsigned char* dest, unsigned long* destLen,
                        const unsigned char* source, unsigned long sourceLen);

Cipher* G_Cipher();

class CUser;
class PacketGuard;

class CParty;
class GameWorld;    // 完整声明在 GameWorld.h（本头只提供前向 + 单例 extern）

GameWorld* G_GameWorld();

class CDataManager;
CDataManager* G_CDataManager();

// ---------------------------------------------------------------------------
// PacketBuf：原始包缓冲（无 vtable）。
//
// 布局（df_game_r ORIG）：
//   +0x00     int    m_byte_encoding      （0=小端 1=大端）
//   +0x04     int    m_use_size           （读写游标 / index）
//   +0x08     int    m_size               （当前包长度）
//   +0x0c     bool   m_finalized
//   +0x10     PACKETBUF_ERROR_TYPE m_last_error
//   +0x14     char*  m_ptr                （当前写指针，clear 后指向 m_buf）
//   +0x18     char   m_buf[95000]         （0x17318 字节）
//   +0x17330  bool   m_encrypt_required
//   sizeof    = 0x17334 = 95028
// ---------------------------------------------------------------------------

class PacketBuf
{
public:
    enum PACKETBUF_ERROR_TYPE
    {
        PACKETBUF_ERROR_NONE = 0,
        PACKETBUF_ERROR_PTR_END = 1
    };

    static const int MAX_PACKET_SIZE = 95000;

    PacketBuf();
    ~PacketBuf();

    bool bind_packet(char* packet, int size);
    void clear();
    void clear_ptr();
    bool compress_packet();
    void copy(const PacketBuf& other);
    void encFinalize();
    void encrypt_packet();
    void finalize(bool isEncryptRequired);

    bool get_binary(char* out, int size);
    char* get_buf_ptr(int size);
    bool get_byte(char& out);
    bool get_byte(unsigned char& out);
    bool get_int(int& out);
    bool get_int(unsigned int& out);
    bool get_int(unsigned long& out);
    int get_len() const;
    bool get_short(short& out);
    bool get_short(unsigned short& out);
    bool get_str(char* out, int maxLen, int len);
    bool isEncryptRequired() const;
    bool is_len_end(int len);
    bool is_ptr_end(int index, int len);

    bool put_binary(int& index, char* data, int len);
    bool put_binary(int& index, const char* data, int len);
    bool put_byte(int& index, int value);
    bool put_header(int packetId, int packetSize);
    bool put_int(int& index, int value);
    void put_packet(const Inven_Item& item);
    bool put_short(int& index, int value);
    bool put_str(int& index, char* str, int len);
    bool put_str(int& index, const char* str, int len);
    void set_packet(const char* data, int size);

    // 便捷重载 / 索引辅助（ORIG 中为弱符号，随 TU 实例化）
    unsigned char at(int index);
    PACKETBUF_ERROR_TYPE getLastError();
    int get_index();
    bool get_item_idx(unsigned long& value);
    char* get_packet(int size);
    bool isCompressRequired(char* packet) const;
    bool is_finallized() const;
    bool put_item_idx(unsigned long value);
    void setLastError(PACKETBUF_ERROR_TYPE error);
    void set_index(int index);
    bool put_str(const char* str, int len);
    bool put_str(char* str, int len);
    bool put_binary(const char* data, int len);
    bool put_binary(char* data, int len);
    bool put_byte(int value);
    bool put_int(int value);
    bool put_short(int value);

private:
    int m_byte_encoding;            // +0x00
    int m_use_size;                 // +0x04
    int m_size;                     // +0x08
    bool m_finalized;               // +0x0c
    PACKETBUF_ERROR_TYPE m_last_error;  // +0x10
    char* m_ptr;                    // +0x14
    char m_buf[MAX_PACKET_SIZE];    // +0x18
    bool m_encrypt_required;        // +0x17330
};

TEST_CLASS_SIZE(PacketBuf, 95028)

void _NS_PI_MakeHash_NOTI(PacketBuf* packet);

#endif  // GAME_PACKETBUF_H
