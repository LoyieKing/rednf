bool convertToUtf8(const wchar_t* src, int srcLength, CharString* out)
{
    int need = 0;
    for (int i = 0; i < srcLength; )
    {
        const int ch = static_cast<int>(src[i]);
        if (ch <= 0x7f)
        {
            need += 1;
            ++i;
        }
        else if (ch <= 0x7ff)
        {
            need += 2;
            ++i;
        }
        else
        {
            need += 3;
            ++i;
        }
    }
    if (need == 0)
    {
        *out = "";
        return true;
    }
    CharStringData* data = CharStringData::createTerminated(need);
    char* buf = data->getBuffer();
    for (int i = 0; i < srcLength; ++i)
    {
        const int ch = static_cast<int>(src[i]);   // ORIG signed（主循环 `cmpl $0x7f; jg`）
        if (ch <= 0x7f)
        {
            *buf++ = static_cast<char>(ch);
        }
        else if (ch <= 0x7ff)
        {
            *buf++ = static_cast<char>(0xc0 | (ch >> 6));
            *buf++ = static_cast<char>(0x80 | (ch & 0x3f));
        }
        else
        {
            *buf++ = static_cast<char>(0xe0 | (ch >> 12));
            *buf++ = static_cast<char>(0x80 | ((ch >> 6) & 0x3f));
            *buf++ = static_cast<char>(0x80 | (ch & 0x3f));
        }
    }
    // ORIG 0x8adf75a..0x8adf861：构造临时 CharString tmp(data)（lea -0x28）→
    // *out = tmp（aSERKS_）→ 析构 tmp（D1，含 EH 保护区）；非 attachData
    // 直挂（那样缺 C1EP14CharStringData / aSERKS_ / D1 三个调用）。
    // [b28d-solo-OnlyOrigBatch11] tmp 用**显式块**限定生存期（块尾析构），使
    // `return true` 的 `mov $1,%eax` 落在 D1 之后（ORIG +0x1a0 形），避免 4.4
    // 把返回值提前物化到 %ebx 跨 D1 call 保活（P78）。
    {
        CharString tmp(data);
        *out = tmp;
    }
    return true;
