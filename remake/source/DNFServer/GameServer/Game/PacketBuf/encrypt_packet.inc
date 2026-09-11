// [b28d-solo-PacketBuf] verdict=AE（check_tu_routed clean；p167 9/9 CLEAR，129=129）。
// 修法：①P207 显式 `operator delete[](buffer)` 消 4.4 -O0 delete[] 自带的
// test/je 判空（ORIG 4.1 只有源码 if 的一次判空，原 delete[] 多一对 cmpl/je+jmp/nop）；
// ②`int bufferSize` + 显式括号 `m_size + ((PKT_BODY_OFF - m_size) + bufferSize)`
// 阻止 4.4 把 `m + (C - m)` 代数折成 `C`（探针 /tmp/probe_enc.cpp f1 折成
// `buf+0xf` 4 条；/tmp/probe_enc4.cpp h6 保留 ORIG 的 `(0xf-m)+buf` 两段链）。
void PacketBuf::encrypt_packet()
{
    if (*m_ptr != 0 || *(unsigned short*)(m_ptr + PKT_SIZE_OFF) != 1)
    {
        char localBuffer[MAX_PACKET_SIZE];
        int bufferSize;
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
                            (unsigned char*)buffer, &bufferSize);
        // [b28d-solo-PacketBuf] 显式括号 + int 局部：4.4 对
        // `m + (0xf - m) + buf` 恒代数折成 `buf + 0xf`（探针 /tmp/probe_enc.cpp
        // f1 4 条）；加括号后保留 ORIG 的 `(0xf-m)+buf` 两段计算链
        // （探针 /tmp/probe_enc4.cpp h6）。
        m_size = m_size + ((PKT_BODY_OFF - m_size) + bufferSize);

        if (bufferSize > 0)   // ORIG test;jle 形（signed 比较）
        {
            memcpy(m_ptr + PKT_BODY_OFF, buffer, bufferSize);
        }

        if (allocated && buffer != NULL)
        {
            // [b28d-solo-PacketBuf P207] 4.4 -O0 对 delete[] 自带 test/je 判空；
            // ORIG 4.1 无该守卫 ⇒ 显式 operator delete[] 调用复现单次判空直落。
            operator delete[](buffer);
        }
    }
    else
    {
        unsigned int size = 0;
        Encryption::Encrypt(m_ptr + PKT_BODY_OFF, m_size - PKT_BODY_OFF, size);
    }
}