#include "LogManager.h"
#include "PacketBuf.h"

#include <cstring>

#include "CDataManager.h"
#include "GameWorld.h"
#include "md5.h"

// ===========================================================================
// 生命周期
// ===========================================================================

PacketBuf::PacketBuf()
{
    m_byte_encoding = 0;
    clear();
}

PacketBuf::~PacketBuf()
{
}

// ===========================================================================
// 绑定 / 清空
// ===========================================================================

bool PacketBuf::bind_packet(char* packet, int size)
{
    if (packet == NULL || size == 0)
    {
        LogManager::logFormat(1, "packet_buf.cpp",
                              __PRETTY_FUNCTION__, 0x27c,
                              "packet is null or size is zero");
        return false;
    }

    clear();
    m_ptr = packet;
    m_size = size;
    return true;
}

void PacketBuf::clear()
{
    m_use_size = 0;
    m_size = 0;
    m_finalized = false;
    m_ptr = m_buf;
    setLastError(PACKETBUF_ERROR_NONE);
    m_encrypt_required = true;
    clear_ptr();
}

void PacketBuf::clear_ptr()
{
    m_use_size = 13;
}

void PacketBuf::set_packet(const char* data, int size)
{
    if (data == NULL || size < 1 || size > MAX_PACKET_SIZE)
    {
        LogManager::logFormat(1, "packet_buf.cpp",
                              __PRETTY_FUNCTION__, 0x270,
                              "!packet || size<1 || size> MAX_PACKET_SIZE");
    }
    else
    {
        clear();
        memcpy(m_ptr, data, size);
        m_size = size;
    }
}

void PacketBuf::copy(const PacketBuf& other)
{
    m_use_size = other.m_use_size;
    m_size = other.m_size;
    m_finalized = other.m_finalized;
    m_ptr = m_buf;
    memset(m_buf, 0, MAX_PACKET_SIZE);
    memcpy(m_buf, other.m_buf, m_size);
}

// ===========================================================================
// 写入
// ===========================================================================

bool PacketBuf::put_header(int packetId, int packetSize)
{
    *m_ptr = (char)packetId;
    *(unsigned short*)(m_ptr + 1) = (unsigned short)packetSize;
    m_use_size = 15;
    return true;
}

bool PacketBuf::put_byte(int& index, int value)
{
    if (is_ptr_end(index, 1))
    {
        return false;
    }

    *(char*)(m_ptr + index++) = (char)value;
    return true;
}

bool PacketBuf::put_short(int& index, int value)
{
    if (is_ptr_end(index, 2))
    {
        return false;
    }

    switch (m_byte_encoding)
    {
    case 0:
        *(char*)(m_ptr + index++) = (char)value;
        *(char*)(m_ptr + index++) = (char)((value & 0xff00) >> 8);
        break;
    case 1:
        *(char*)(m_ptr + index++) = (char)((value & 0xff00) >> 8);
        *(char*)(m_ptr + index++) = (char)value;
        break;
    default:
        LogManager::logFormat(1, "packet_buf.cpp",
                              __PRETTY_FUNCTION__, 0xa6,
                              "m_byte_encoding error %d", m_byte_encoding);
        return false;
    }
    return true;
}

bool PacketBuf::put_int(int& index, int value)
{
    if (is_ptr_end(index, 4))
    {
        return false;
    }

    switch (m_byte_encoding)
    {
    case 0:
        *(char*)(m_ptr + index++) = (char)value;
        *(char*)(m_ptr + index++) = (char)((value & 0xff00) >> 8);
        *(char*)(m_ptr + index++) = (char)((value & 0xff0000) >> 16);
        *(char*)(m_ptr + index++) = (char)((unsigned int)value >> 24);
        break;
    case 1:
        *(char*)(m_ptr + index++) = (char)((unsigned int)value >> 24);
        *(char*)(m_ptr + index++) = (char)((value & 0xff0000) >> 16);
        *(char*)(m_ptr + index++) = (char)((value & 0xff00) >> 8);
        *(char*)(m_ptr + index++) = (char)value;
        break;
    default:
        LogManager::logFormat(1, "packet_buf.cpp",
                              __PRETTY_FUNCTION__, 0xc4,
                              "m_byte_encoding error %d", m_byte_encoding);
        break;
    }
    return true;
}

bool PacketBuf::put_binary(int& index, char* data, int len)
{
    if (data == NULL || len == 0)
    {
        LogManager::logFormat(1, "packet_buf.cpp",
                              __PRETTY_FUNCTION__, 0x123,
                              "bin is null or size is zero");
        return false;
    }

    if (is_ptr_end(index, len))
    {
        return false;
    }

    for (int i = 0; i < len; i = i + 1)
    {
        *(char*)(m_ptr + index++) = data[i];
    }
    return true;
}

bool PacketBuf::put_binary(int& index, const char* data, int len)
{
    if (data == NULL || len == 0)
    {
        LogManager::logFormat(1, "packet_buf.cpp",
                              __PRETTY_FUNCTION__, 0x138,
                              "bin is null or size is zero");
        return false;
    }

    if (is_ptr_end(index, len))
    {
        return false;
    }

    for (int i = 0; i < len; i = i + 1)
    {
        *(char*)(m_ptr + index++) = data[i];
    }
    return true;
}

bool PacketBuf::put_str(int& index, char* str, int len)
{
    if (is_ptr_end(index, len))
    {
        return false;
    }

    for (int i = 0; i < len; i = i + 1)
    {
        if (str[i] == 0)
        {
            break;
        }
        *(char*)(m_ptr + index++) = str[i];
    }
    return true;
}

bool PacketBuf::put_str(int& index, const char* str, int len)
{
    if (is_ptr_end(index, len))
    {
        return false;
    }

    for (int i = 0; i < len; i = i + 1)
    {
        if (str[i] == 0)
        {
            break;
        }
        *(char*)(m_ptr + index++) = str[i];
    }
    return true;
}

// ===========================================================================
// 读取
// ===========================================================================

bool PacketBuf::is_ptr_end(int index, int len)
{
    if (index + len > PKT_PTR_END_LIMIT)
    {
        setLastError(PACKETBUF_ERROR_PTR_END);
        return true;
    }
    return false;
}

bool PacketBuf::is_len_end(int len)
{
    if (m_use_size + len > m_size)
    {
        return true;
    }
    return false;
}

bool PacketBuf::get_byte(char& out)
{
    if (is_len_end(1))
    {
        return false;
    }

    out = m_ptr[m_use_size++];
    return true;
}

bool PacketBuf::get_byte(unsigned char& out)
{
    if (is_len_end(1))
    {
        return false;
    }

    out = m_ptr[m_use_size++];
    return true;
}

bool PacketBuf::get_short(short& out)
{
    if (is_len_end(2))
    {
        return false;
    }

    unsigned int b0 = (unsigned char)m_ptr[m_use_size++];
    unsigned int b1 = (unsigned char)m_ptr[m_use_size++];

    switch (m_byte_encoding)
    {
    case 0:
        out = (short)(b1 * 0x100 + b0);
        break;
    case 1:
        out = (short)(b0 * 0x100 + b1);
        break;
    default:
        LogManager::logFormat(1, "packet_buf.cpp",
                              __PRETTY_FUNCTION__, 0x191,
                              "m_byte_encoding error %d", m_byte_encoding);
        return false;
    }
    return true;
}

bool PacketBuf::get_short(unsigned short& out)
{
    short value = 0;
    if (get_short(value) == false)
    {
        return false;
    }
    out = (unsigned short)value;
    return true;
}

bool PacketBuf::get_int(int& out)
{
    if (is_len_end(4))
    {
        return false;
    }

    int b0 = (unsigned char)m_ptr[m_use_size++];
    int b1 = (unsigned char)m_ptr[m_use_size++];
    int b2 = (unsigned char)m_ptr[m_use_size++];
    int b3 = (unsigned char)m_ptr[m_use_size++];

    switch (m_byte_encoding)
    {
    case 0:
        out = (b3 << 24) + (b2 << 16) + (b1 << 8) + b0;
        break;
    case 1:
        out = (b0 << 24) + (b1 << 16) + (b2 << 8) + b3;
        break;
    default:
        LogManager::logFormat(1, "packet_buf.cpp",
                              __PRETTY_FUNCTION__, 0x1b7,
                              "m_byte_encoding error %d", m_byte_encoding);
        return false;
    }
    return true;
}

bool PacketBuf::get_int(unsigned int& out)
{
    int value = 0;
    if (get_int(value) == false)
    {
        return false;
    }
    out = (unsigned int)value;
    return true;
}

bool PacketBuf::get_int(unsigned long& out)
{
    int value = 0;
    if (get_int(value) == false)
    {
        return false;
    }
    out = (unsigned long)value;
    return true;
}

bool PacketBuf::get_binary(char* out, int size)
{
    if (out == NULL || size == 0)
    {
        LogManager::logFormat(1, "packet_buf.cpp",
                              __PRETTY_FUNCTION__, 0x21e,
                              "o_buf == NULL || size == 0");
        return false;
    }

    if (is_len_end(size))
    {
        return false;
    }

    for (int i = 0; i < size; i = i + 1)
    {
        out[i] = m_ptr[m_use_size++];
    }
    return true;
}

bool PacketBuf::get_str(char* out, int maxLen, int len)
{
    if (out == NULL)
    {
        LogManager::logFormat(1, "packet_buf.cpp",
                              __PRETTY_FUNCTION__, 0x1f8,
                              "o_buf is null");
        return false;
    }
    else if (len < 0 || maxLen < 0)
    {
        return false;
    }
    else if (len >= maxLen)
    {
        return false;
    }
    else
    {
        if (is_len_end(len))
        {
            return false;
        }
        if (len == 0)
        {
            return true;
        }

        int i;
        int c;
        for (i = 0; i < len; i = i + 1)
        {
            c = m_ptr[m_use_size++];
            if (c == 0)
            {
                break;
            }
            out[i] = (char)c;
        }
        out[i] = 0;
        return true;
    }
}

// ===========================================================================
// 收尾 / 加密 / 压缩
// ===========================================================================

void PacketBuf::finalize(bool isEncryptRequired)
{
    m_encrypt_required = isEncryptRequired;
    _NS_PI_MakeHash_NOTI(this);
    m_size = m_use_size;

    unsigned int crc1 = Encryption::crc32N(m_ptr + PKT_BODY_OFF, m_size - PKT_BODY_OFF);
    switch (m_byte_encoding)
    {
    case 0:
        *(char*)(m_ptr + PKT_LEN_OFF) = (char)m_size;
        *(char*)(m_ptr + PKT_LEN_OFF + 1) = (char)((m_size & 0xff00) >> 8);
        *(char*)(m_ptr + PKT_LEN_OFF + 2) = (char)((m_size & 0xff0000) >> 16);
        *(char*)(m_ptr + PKT_LEN_OFF + 3) = (char)((unsigned int)m_size >> 24);
        break;
    case 1:
        *(char*)(m_ptr + PKT_LEN_OFF) = (char)((unsigned int)m_size >> 24);
        *(char*)(m_ptr + PKT_LEN_OFF + 1) = (char)((m_size & 0xff0000) >> 16);
        *(char*)(m_ptr + PKT_LEN_OFF + 2) = (char)((m_size & 0xff00) >> 8);
        *(char*)(m_ptr + PKT_LEN_OFF + 3) = (char)m_size;
        break;
    default:
        LogManager::logFormat(1, "packet_buf.cpp",
                              __PRETTY_FUNCTION__, 0x2c4,
                              "m_byte_encoding error %d", m_byte_encoding);
        return;
    }

    if (isCompressRequired(m_ptr + PKT_SIZE_OFF))
    {
        if (compress_packet() != true)
        {
            return;
        }
    }

    unsigned int crc2 = Encryption::crc32N(m_ptr + PKT_BODY_OFF, m_size - PKT_BODY_OFF);
    switch (m_byte_encoding)
    {
    case 0:
        *(char*)(m_ptr + PKT_CRC1_OFF) = (char)crc1;
        *(char*)(m_ptr + PKT_CRC1_OFF + 1) = (char)((crc1 & 0xff00) >> 8);
        *(char*)(m_ptr + PKT_CRC1_OFF + 2) = (char)((crc1 & 0xff0000) >> 16);
        *(char*)(m_ptr + PKT_CRC1_OFF + 3) = (char)(crc1 >> 24);
        *(char*)(m_ptr + PKT_CRC2_OFF) = (char)crc2;
        *(char*)(m_ptr + PKT_CRC2_OFF + 1) = (char)((crc2 & 0xff00) >> 8);
        *(char*)(m_ptr + PKT_CRC2_OFF + 2) = (char)((crc2 & 0xff0000) >> 16);
        *(char*)(m_ptr + PKT_CRC2_OFF + 3) = (char)(crc2 >> 24);
        break;
    case 1:
        *(char*)(m_ptr + PKT_CRC1_OFF) = (char)(crc1 >> 24);
        *(char*)(m_ptr + PKT_CRC1_OFF + 1) = (char)((crc1 & 0xff0000) >> 16);
        *(char*)(m_ptr + PKT_CRC1_OFF + 2) = (char)((crc1 & 0xff00) >> 8);
        *(char*)(m_ptr + PKT_CRC1_OFF + 3) = (char)crc1;
        *(char*)(m_ptr + PKT_CRC2_OFF) = 0;
        *(char*)(m_ptr + PKT_CRC2_OFF + 1) = 0;
        *(char*)(m_ptr + PKT_CRC2_OFF + 2) = (char)((crc2 & 0xff0000) >> 8);
        *(char*)(m_ptr + PKT_CRC2_OFF + 3) = 0;
        break;
    default:
        LogManager::logFormat(1, "packet_buf.cpp",
                              __PRETTY_FUNCTION__, 0x2ec,
                              "m_byte_encoding error %d", m_byte_encoding);
        return;
    }

    m_finalized = true;
}

void PacketBuf::encFinalize()
{
    unsigned int crc1 = Encryption::crc32N(m_ptr + PKT_BODY_OFF, m_size - PKT_BODY_OFF);
    switch (m_byte_encoding)
    {
    case 0:
        *(char*)(m_ptr + PKT_LEN_OFF) = (char)m_size;
        *(char*)(m_ptr + PKT_LEN_OFF + 1) = (char)((m_size & 0xff00) >> 8);
        *(char*)(m_ptr + PKT_LEN_OFF + 2) = (char)((m_size & 0xff0000) >> 16);
        *(char*)(m_ptr + PKT_LEN_OFF + 3) = (char)((unsigned int)m_size >> 24);
        break;
    case 1:
        *(char*)(m_ptr + PKT_LEN_OFF) = (char)((unsigned int)m_size >> 24);
        *(char*)(m_ptr + PKT_LEN_OFF + 1) = (char)((m_size & 0xff0000) >> 16);
        *(char*)(m_ptr + PKT_LEN_OFF + 2) = (char)((m_size & 0xff00) >> 8);
        *(char*)(m_ptr + PKT_LEN_OFF + 3) = (char)m_size;
        break;
    default:
        LogManager::logFormat(1, "packet_buf.cpp",
                              __PRETTY_FUNCTION__, 0x419,
                              "m_byte_encoding error %d", m_byte_encoding);
        return;
    }

    unsigned int crc2 = Encryption::crc32N(m_ptr + PKT_BODY_OFF, m_size - PKT_BODY_OFF);
    switch (m_byte_encoding)
    {
    case 0:
        *(char*)(m_ptr + PKT_CRC1_OFF) = (char)crc1;
        *(char*)(m_ptr + PKT_CRC1_OFF + 1) = (char)((crc1 & 0xff00) >> 8);
        *(char*)(m_ptr + PKT_CRC1_OFF + 2) = (char)((crc1 & 0xff0000) >> 16);
        *(char*)(m_ptr + PKT_CRC1_OFF + 3) = (char)(crc1 >> 24);
        *(char*)(m_ptr + PKT_CRC2_OFF) = (char)crc2;
        *(char*)(m_ptr + PKT_CRC2_OFF + 1) = (char)((crc2 & 0xff00) >> 8);
        *(char*)(m_ptr + PKT_CRC2_OFF + 2) = (char)((crc2 & 0xff0000) >> 16);
        *(char*)(m_ptr + PKT_CRC2_OFF + 3) = (char)(crc2 >> 24);
        break;
    case 1:
        *(char*)(m_ptr + PKT_CRC1_OFF) = (char)(crc1 >> 24);
        *(char*)(m_ptr + PKT_CRC1_OFF + 1) = (char)((crc1 & 0xff0000) >> 16);
        *(char*)(m_ptr + PKT_CRC1_OFF + 2) = (char)((crc1 & 0xff00) >> 8);
        *(char*)(m_ptr + PKT_CRC1_OFF + 3) = (char)crc1;
        *(char*)(m_ptr + PKT_CRC2_OFF) = 0;
        *(char*)(m_ptr + PKT_CRC2_OFF + 1) = 0;
        *(char*)(m_ptr + PKT_CRC2_OFF + 2) = (char)((crc2 & 0xff0000) >> 8);
        *(char*)(m_ptr + PKT_CRC2_OFF + 3) = 0;
        break;
    default:
        LogManager::logFormat(1, "packet_buf.cpp",
                              __PRETTY_FUNCTION__, 0x439,
                              "m_byte_encoding error %d", m_byte_encoding);
        return;
    }
}

void PacketBuf::encrypt_packet()
{
    if (*m_ptr != 0 || *(unsigned short*)(m_ptr + PKT_SIZE_OFF) != 1)
    {
        char localBuffer[MAX_PACKET_SIZE];
        unsigned int bufferSize;
        int dataSize;
        char allocated;
        char* buffer;

        allocated = 0;
        buffer = localBuffer;
        bufferSize = MAX_PACKET_SIZE;
        dataSize = m_size - PKT_BODY_OFF;

        G_Cipher()->AdjustDataSize(*(unsigned short*)(m_ptr + PKT_SIZE_OFF), &dataSize);

        if (dataSize > MAX_PACKET_SIZE)
        {
            LogManager::logFormat(1, "packet_buf.cpp",
                                  __PRETTY_FUNCTION__, 0x302,
                                  "Buffer overflow[Cipher] : EncryptDataSize[%d]>[%d]",
                                  dataSize, MAX_PACKET_SIZE);
            buffer = new char[dataSize];
            allocated = 1;
        }

        G_Cipher()->Encrypt(*(unsigned short*)(m_ptr + PKT_SIZE_OFF),
                            (unsigned char*)(m_ptr + PKT_BODY_OFF), m_size - PKT_BODY_OFF,
                            (unsigned char*)buffer, (int*)&bufferSize);
        m_size = m_size + (PKT_BODY_OFF - m_size) + bufferSize;

        if ((int)bufferSize > 0)   // ORIG test;jle 形（signed 比较）
        {
            memcpy(m_ptr + PKT_BODY_OFF, buffer, bufferSize);
        }

        if (allocated && buffer != NULL)
        {
            delete[] buffer;
        }
    }
    else
    {
        unsigned int size = 0;
        Encryption::Encrypt(m_ptr + PKT_BODY_OFF, m_size - PKT_BODY_OFF, size);
    }
}

bool PacketBuf::compress_packet()
{
    static Mutex compressed_buffer_mutex;
    static char compressed_buffer[MAX_PACKET_SIZE];

    int ret;
    unsigned long compressedSize;
    int uncompressedSize = m_size - PKT_BODY_OFF;
    if (uncompressedSize <= 0)
    {
        LogManager::logFormat(1, "packet_buf.cpp",
                              __PRETTY_FUNCTION__, 0x36e,
                              "error; compress failed:unexpected message size");
        return false;
    }

    compressedSize = uncompressedSize + 13;
    compressed_buffer_mutex.lock();
    ret = compress((unsigned char*)compressed_buffer, &compressedSize,
                   (unsigned char*)(m_ptr + PKT_BODY_OFF), uncompressedSize);

    if (ret != 0 || compressedSize > MAX_PACKET_SIZE - PKT_BODY_OFF)
    {
        LogManager::logFormat(1, "packet_buf.cpp",
                              __PRETTY_FUNCTION__, 0x380,
                              "error; compress failed:%d", ret);
        compressed_buffer_mutex.unlock();
        return false;
    }

    memcpy(m_ptr + PKT_BODY_OFF, compressed_buffer, compressedSize);
    compressed_buffer_mutex.unlock();
    m_size = (int)(compressedSize + PKT_BODY_OFF);
    *(int*)(m_ptr + PKT_LEN_OFF) = m_size;
    return true;
}

// ===========================================================================
// 便捷重载 / 索引
// ===========================================================================

unsigned char PacketBuf::at(int index)
{
    return m_ptr[index];
}

PacketBuf::PACKETBUF_ERROR_TYPE PacketBuf::getLastError()
{
    return m_last_error;
}

void PacketBuf::setLastError(PACKETBUF_ERROR_TYPE error)
{
    m_last_error = error;
}

int PacketBuf::get_index()
{
    return m_use_size;
}

void PacketBuf::set_index(int index)
{
    m_use_size = index;
}

bool PacketBuf::get_item_idx(unsigned long& value)
{
    return get_int(value);
}

bool PacketBuf::put_item_idx(unsigned long value)
{
    return put_int(m_use_size, (int)value);
}

bool PacketBuf::is_finallized() const
{
    return m_finalized;
}

bool PacketBuf::isEncryptRequired() const
{
    return m_encrypt_required;
}

bool PacketBuf::isCompressRequired(char* packet) const
{
    char* q = packet;
    if (*(unsigned short*)q == 2 || *(unsigned short*)q == 9 ||
        *(unsigned short*)q == 0x57)
    {
        return true;
    }
    return false;
}

int PacketBuf::get_len() const
{
    if (m_finalized == false)
    {
        LogManager::logFormat(1, "packet_buf.cpp",
                              __PRETTY_FUNCTION__, 0x338,
                              "this packet is not finalized");
        return 0;
    }
    return m_size;
}

char* PacketBuf::get_packet(int size)
{
    if (size > MAX_PACKET_SIZE - 1)
    {
        cMyTrace ct(__FUNCTION__, 0x170, 5);
        ct("[%s][%d]", __FUNCTION__, 0x170);
    }
    return m_ptr + size;
}

char* PacketBuf::get_buf_ptr(int size)
{
    return get_packet(size);
}

bool PacketBuf::put_str(const char* str, int len)
{
    return put_str(m_use_size, str, len);
}

bool PacketBuf::put_str(char* str, int len)
{
    return put_str(m_use_size, str, len);
}

bool PacketBuf::put_binary(const char* data, int len)
{
    return put_binary(m_use_size, data, len);
}

bool PacketBuf::put_binary(char* data, int len)
{
    return put_binary(m_use_size, data, len);
}

bool PacketBuf::put_byte(int value)
{
    return put_byte(m_use_size, value);
}

bool PacketBuf::put_int(int value)
{
    return put_int(m_use_size, value);
}

bool PacketBuf::put_short(int value)
{
    return put_short(m_use_size, value);
}

// ===========================================================================
// put_packet（Inven_Item 序列化）
// ===========================================================================

void PacketBuf::put_packet(const Inven_Item& item)
{
    item.m_random.m_random.put_packet_random_option(*this);

    if (G_GameWorld()->IsIntegratedPvPBaseChannel() &&
        G_CDataManager()->GetUpgradeRevisionPvPChannel() == 0)
    {
        put_byte(0);
    }
    else
    {
        put_byte(item.m_upgradeSep.GetUpgradeSeparate());
    }

    put_byte(item.m_upgradeSep.IsTradeRestriction());
}

// ===========================================================================
// _NS_PI_MakeHash_NOTI（ORIG 0x82746f3 T）
// 包尾哈希：对包体（m_ptr + 0xf 起，长度 = get_index() - 0xf）经
// _NS_MakeHash_（ORIG 0x808c8b5，真实定义 GameSecurity.cpp）折叠为 4 字节，
// 以小端 put_int 写包尾；len 越界（<=0 或 >0x190）时不写。
// （ORIG 直接 call _Z13_NS_MakeHash_PcsRi——本 TU 不再本地复制其折叠规则。）
// ORIG 实证（docs/class_func_reports/_lt_global_gt_/_NS_PI_MakeHash_NOTI.md）：
//   -18 base(char*) / -12 未使用的 ushort / -10 region(char*) /
//   -0a len(**short** 槽：`mov %ax,-0xa` + `movswl` 读回)/ -1c hash(int，
//    **无初始化 store**)；调用点为 `_NS_MakeHash_(region, len, hash) == 1`
//    （`xor $0x1,%eax; test %al,%al` ⇒ 返回类型为窄类型 + 显式 ==1 比较）。
// ===========================================================================

// 权威定义 GameSecurity.cpp（ORIG T 0x0808c8b5）；返回类型 char 与其同步：
// ORIG 本调用点为 `xor $0x1,%eax; test %al,%al; jne`（只测低字节 ⇒ char 值域
// 的显式 ==1 判据），ORIG 体内两条返回 `mov $0x0/$0x1,%eax` 严格 0/1。
char _NS_MakeHash_(char* data, short len, int& out);

// [b22判词·N] 残余唯一 diff：ORIG 为 `xor $0x1,%eax; test %al,%al; jne`（==1 被
//   融合成 xor），gcc 4.4.7 -O0 恒出 `cmp $0x1,%al; sete %al; test %al,%al; je`
//   —— xor 融合需 -O1 combine 阶段，实测 13 种源码写法（==1/!=1/char 具名局部/
//   unsigned char 化/(x^1)==0/!((x^1))/signed char/int 返回/== '\x01' 等）无一命中。
//   返回类型已按 Main 授权改 char（GameSecurity.cpp 定义同步；实测该 TU 逐函数
//   verdict 零倒退），值域 0/1 下与 ORIG 语义完全等价；ORIG 尾部多一对
//   `jmp/nop` 跳板是该分支融合的布局副产物（§6.1 nop 豁免同类）。
//   ⇒ 指令选择层 N 族，不可源码复现，语义等价。
void _NS_PI_MakeHash_NOTI(PacketBuf* packet)
{
    if (packet == 0)
    {
        return;
    }
    char* base = packet->get_packet(0);
    // ORIG：读取包首 +1 处的 ushort 字段（本函数内不使用，仅为对齐 ORIG 读取行为）
    unsigned short headerField = *(unsigned short*)(base + PKT_SIZE_OFF);
    char* region = base + PKT_BODY_OFF;
    short idx = packet->get_index() - PKT_BODY_OFF;
    int hash;
    if (_NS_MakeHash_(region, idx, hash) == 1)
    {
        packet->put_int(hash);
    }
}
