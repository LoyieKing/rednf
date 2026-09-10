void CBitManager::ShiftLeft(int bits, unsigned char* buf)   // 0x0831c266（同 [N 类标注]）
{
    // P191 配套：ORIG 0x0831c26f/276 双 `movl $0x0,-0x10` = `int i=0;`+`for(i=0;..)`
    // 双初始化；移位掩码实测两侧均无 and（旧 N 标注掩码差已失效，仅剩此条）。
    int i = 0;
    for (i = 0; i < m_size; ++i)
    {
        buf[i] = static_cast<unsigned char>(buf[i] << bits);
        if (i != m_size - 1)
        {
            buf[i] = static_cast<unsigned char>(
                buf[i] | (buf[i + 1] >> (8 - bits)));
        }
    }
