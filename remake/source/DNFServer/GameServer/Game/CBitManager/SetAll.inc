void CBitManager::SetAll(int value, unsigned char* buf)     // 0x0831c0ce
{
    // ORIG 0x0831c0d4/0x0831c0db：i 声明置 0 + for 头再置 0（两条 movl $0 同
    // 槽）——原码即 `int i = 0; for (i = 0; ...)` 形，忠实保留。
    int i = 0;
    for (i = 0; i < m_size; ++i)
    {
        if (value == 0)
        {
            buf[i] = 0x00;
        }
        else
        {
            buf[i] = 0xff;
        }
    }
