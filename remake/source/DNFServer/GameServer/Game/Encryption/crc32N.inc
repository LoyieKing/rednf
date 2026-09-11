unsigned int Encryption::crc32N(void* data, int len)
{
    unsigned int crc = crc32(0, 0, 0);
    crc = crc32(crc, (const char*)data, (unsigned int)len);
    _NS_PI_2ND_MakeChecksumTo1Byte((char*)&crc);
    return crc;
}