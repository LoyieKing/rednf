int ss_wcscat(wchar_t* dest, unsigned int count, const wchar_t* src)
{
    // ORIG 0x08ad473e：count 槽有符号 jg ⇒ (int)count <= cur+len 为守卫，
    // return 1 fall-through、memcpy 块 jg 前跳；memcpy 首参按字节
    // (char*)dest+cur*4（shl $2+add），size (len+1)<<2（add+shl+mov edx）。
    int cur = wcslen(dest);
    int len = wcslen(src);
    if ((int)count <= cur + len)
    {
        return 1;
    }
    memcpy(&dest[cur], src, (len + 1) << 2);
    return 0;
