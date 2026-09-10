bool CEnvironment::DecryptValue(const char* value, char* dst)
{
    char buf1[0x40] = {0};
    unsigned char buf2[0x40] = {0};

    if (!DNFFLib::Hex2Binary(value, buf2, 0x18))
        return 0;
    m_cipher.Decrypt((const char*)buf2, buf1, 0x18);
    strncpy(dst, buf1, 0x14);
    return 1;
