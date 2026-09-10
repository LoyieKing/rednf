bool convertFromUtf8(const char* src, int srcLength, WideString* out)
{
    int wideCount = 0;                        // -0x24
    int i = 0;                                // -0x18
    while (i < srcLength)
    {
        const unsigned char b = (unsigned char)src[i];
        // ORIG 块排布：ASCII 为 fall-through 首块（test;js→多字节区 @0xf8）。
        if ((char)b >= 0)
        {
            wideCount += 1;
            i += 1;
        }
        else
        {
            if (b <= 0xdf && i + 1 < srcLength)
    {
            wideCount += 1;
            i += 2;
        }
            else if (b <= 0xef && i + 2 < srcLength)
        {
            wideCount += 1;
            i += 3;
        }
        else
                return false;
        }
    }
    if (wideCount == 0)
    {
        *out = L"";                           // ORIG operator=(wchar_t const*)
        return true;
    }

    WideStringData* data = WideStringData::createTerminated(wideCount);
    wchar_t* dst = data->getBuffer();
    i = 0;
    while (i < srcLength)
    {
        const unsigned char b = (unsigned char)src[i];
        // 同上：ASCII 首块 + js 直跳多字节区；合成段间用加法（ORIG add
        // %eax,%edx，4.1 对无重叠位 or 发射 add；4.4 的 | 直出 or）。
        if ((char)b >= 0)
        {
            *dst++ = (wchar_t)b;
            i += 1;
        }
        else
        {
            if (b <= 0xdf && i + 1 < srcLength)
    {
                *dst++ = (wchar_t)(((b & 0x1f) << 6) +
                                   ((int)src[i + 1] & 0x3f));
            i += 2;
        }
            else if (b <= 0xef && i + 2 < srcLength)
        {
                *dst++ = (wchar_t)((unsigned short)(b << 12) +
                                   (((int)src[i + 1] & 0x3f) << 6) +
                                   ((int)src[i + 2] & 0x3f));
            i += 3;
        }
        else
                return false;
        }
    }
    WideString tmp(data);
    *out = tmp;
    return true;
