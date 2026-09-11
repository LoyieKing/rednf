void Encryption::Encrypt(void* data, int len, unsigned int& seed)
{
    unsigned int seedVal = 0x04453eb5U;
    if (seed != 0)
        seedVal = SeedRand(seed);
    int i = 0;
    unsigned char rot;
    std::memcpy(&rot, ((char*)&seedVal) + 1, 1);
    rot &= 7;

    // [b28d-solo-Encryption] verdict=AE 83=83（.o 重定位标签伪差）。
    // ORIG 0x0848da70：无 p/xorByte/b 具名局部，逐次重算 data+i（三次 lea），
    // 首条用整字 seedVal 异或后 mov %al 存回；data[i] 经 EncryptionByte
    // 1 字节 enum 视图读，复现 ORIG `movzbl (%eax),%eax; mov %eax,%ecx`
    // 两段中转（unsigned char 直载恒折单段，P254 扩展）。
    for (i = 0; i < len; ++i)
    {
        ((EncryptionByte*)data)[i] = (EncryptionByte)(seedVal ^ ((EncryptionByte*)data)[i]);
        ((EncryptionByte*)data)[i] =
            (EncryptionByte)((((unsigned char)((EncryptionByte*)data)[i]) >> rot) |
                             (((unsigned char)((EncryptionByte*)data)[i]) << (8 - rot)));
    }
}