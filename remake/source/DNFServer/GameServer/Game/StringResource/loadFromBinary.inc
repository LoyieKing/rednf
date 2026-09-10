bool StringResource::loadFromBinary(const unsigned char* data, int size, CharString* error)
{
    CharString content;                              // -0x48

    if (size > 4 && data[0] == 'S' && data[1] == 'C' &&
        data[2] == '0' && data[3] == '1')
    {
        int origSize = *reinterpret_cast<const int*>(data + 4);     // -0x2c
        int payloadBytes = (size + 3) / 4 * 4;                      // -0x28
        unsigned int k0 = 0x5819af17u;                              // -0x24
        char* buf = new char[payloadBytes];                         // -0x20
        memcpy(buf, data + 8, size);

        int slotPad;
        char* cur = buf;                                            // -0x18
        unsigned int key = k0;                                      // -0x14
        for (unsigned int i = 0; i < (unsigned int)payloadBytes / 4; ++i)
        {
            *reinterpret_cast<unsigned int*>(cur + i * 4) =
                *reinterpret_cast<unsigned int*>(cur + i * 4) ^ key;
            key = key ^ *reinterpret_cast<unsigned int*>(cur + i * 4);
        }

        WideString ws;
        bool converted = convertFromUtf8(buf, origSize, &ws);
        delete[] buf;                        // ORIG @0x12b 单对判空守卫为
                                             // 编译器隐式形（P141），勿再显式
        if (!converted)
            return false;
        content = convertToCharString(ws);
    }
    // BOM 常量用无符号立即数值（ORIG cmp $0xef/$0xbb/$0xbf,%al）：
    // '\xef' 等 char 字面量为负值，与 uchar 提升值恒不等，4.4 折叠死分支。
    else if (size > 3 && data[0] == 0xef &&
             data[1] == 0xbb && data[2] == 0xbf)
    {
        WideString ws;
        bool converted = convertFromUtf8(
            reinterpret_cast<const char*>(data + 3), size - 3, &ws);
        if (!converted)
            return false;
        content = convertToCharString(ws);
    }
    else if (size > 2 && data[0] == 0xff && data[1] == 0xfe)
    {
        WideString ws(reinterpret_cast<const wchar_t*>(data + 2),
                      (size - 2) / 2);
        content = convertToCharString(ws);
    }
    else
    {
        CharString tmp(reinterpret_cast<const char*>(data), size);
        content = tmp;
    }

    char* copyBuf = new char[content.length() + 1];                 // -0x34
    memcpy(copyBuf, content.c_str(), content.length() + 1);
    bool result = loadFromString(copyBuf, content.length(), error); // -0x2d
    delete[] copyBuf;                    // 同上 P141：隐式单守卫
    return result;
