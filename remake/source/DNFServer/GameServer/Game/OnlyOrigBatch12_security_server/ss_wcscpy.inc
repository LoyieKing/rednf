int ss_wcscpy(wchar_t* dest, unsigned int count, const wchar_t* src)
{
    // ORIG 0x08ad468b：else 块（清空 dest 返回 1）fall-through，
    // memcpy 块 jg 前跳；len 槽有符号比较（jg）⇒ (int)count > len。
    int len = wcslen(src);
    if ((int)count <= len)
    {
        dest[0] = L'\0';
        return 1;
    }
    memcpy(dest, src, (len + 1) * sizeof(wchar_t));
    return 0;
