void Encryption::Decrypt(void* data, int len, unsigned int& seed)
{
    unsigned int seedVal = 0x04453eb5U;
    if (seed != 0)
        seedVal = SeedRand(seed);
    int i = 0;
    unsigned char rot;
    std::memcpy(&rot, ((char*)&seedVal) + 1, 1);
    rot &= 7;

    // [b28d-solo-Encryption] verdict=AE 83=83。ORIG 0x0848db5e：与 Encrypt 镜像，
    // 先旋转后 XOR；同样用 EncryptionByte 视图复现两段装载中转。
    for (i = 0; i < len; ++i)
    {
        ((EncryptionByte*)data)[i] =
            (EncryptionByte)((((unsigned char)((EncryptionByte*)data)[i]) << rot) |
                             (((unsigned char)((EncryptionByte*)data)[i]) >> (8 - rot)));
        ((EncryptionByte*)data)[i] = (EncryptionByte)(seedVal ^ ((EncryptionByte*)data)[i]);
    }
}