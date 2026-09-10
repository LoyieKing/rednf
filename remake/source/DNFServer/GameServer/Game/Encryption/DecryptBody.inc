void Encryption::DecryptBody(unsigned short, void* data, int len, unsigned int& seed)
{
    // [b28d-solo-Encryption] verdict=NEAR 293=293（助记符序列全等；残余为
    // N7 间接 call 目标寄存器 %ebx(ORIG) vs %edx(4.4.7) + 参数装载寄存器）。
    // ORIG 0x0848dd0c 逐块还原：
    // ① len==0 直返；② mode 先 1 后 5（原码遗留双写，P171 照抄）；
    // ③ switch(mode) 每 case 独立 for 循环（非循环内 switch）；i 槽 -0x1c 复用；
    // ④ 无 p/xorByte/b 具名局部，全部 data+i 重算；⑤ tmp 0x578（基址 -0x59d）。
    if (len == 0)
        return;
    int mode = 1;
    mode = 5;
    if (mode == 4)
    {
        getOriginalBuf((char*)data, len, seed);
        return;
    }
    if (mode == 5)
    {
        // ORIG 0x848dd5e：G_EncryptTool()->m_rijndael(+0x204) 虚调用
        // Decrypt(data, tmp, len) 后 memcpy 拷回（加密类，算法正确即可）。
        extern EncryptTool* G_EncryptTool();
        char tmp[0x578];
        G_EncryptTool()->m_rijndael->Decrypt((const char*)data, tmp, len);
        std::memcpy(data, tmp, len);
        return;
    }

    unsigned int seedVal = 0x04453eb5U;
    if (seed != 0)
        seedVal = SeedRand(seed);
    int i = 0;
    unsigned char rot;
    std::memcpy(&rot, ((char*)&seedVal) + 1, 1);
    rot &= 7;

    switch (mode)
    {
    case 0:
        for (i = 0; i < len; ++i)
        {
            ((EncryptionByte*)data)[i] = (EncryptionByte)(seedVal ^ ((EncryptionByte*)data)[i]);
            ((EncryptionByte*)data)[i] =
                (EncryptionByte)((((unsigned char)((EncryptionByte*)data)[i]) << rot) |
                                 (((unsigned char)((EncryptionByte*)data)[i]) >> (8 - rot)));
        }
        return;
    case 1:
        for (i = 0; i < len; ++i)
        {
            ((EncryptionByte*)data)[i] =
                (EncryptionByte)((((unsigned char)((EncryptionByte*)data)[i]) >> rot) |
                                 (((unsigned char)((EncryptionByte*)data)[i]) << (8 - rot)));
            ((EncryptionByte*)data)[i] = (EncryptionByte)(seedVal ^ ((EncryptionByte*)data)[i]);
        }
        return;
    case 2:
        for (i = 0; i < len; ++i)
        {
            ((EncryptionByte*)data)[i] = (EncryptionByte)(seedVal ^ ((EncryptionByte*)data)[i]);
            ((EncryptionByte*)data)[i] =
                (EncryptionByte)((((unsigned char)((EncryptionByte*)data)[i]) >> rot) |
                                 (((unsigned char)((EncryptionByte*)data)[i]) << (8 - rot)));
        }
        return;
    case 3:
        for (i = 0; i < len; ++i)
        {
            ((EncryptionByte*)data)[i] =
                (EncryptionByte)((((unsigned char)((EncryptionByte*)data)[i]) << rot) |
                                 (((unsigned char)((EncryptionByte*)data)[i]) >> (8 - rot)));
            ((EncryptionByte*)data)[i] = (EncryptionByte)(seedVal ^ ((EncryptionByte*)data)[i]);
        }
        return;
    default:
        return;
    }
