int ss_strcat(char* dest, size_t count, const char* src)
{
    // ORIG _Z9ss_strcatPcjPKc @0x08ad46dd：容纳 dlen+slen+1 时 memcpy 并返回 0；
    // 否则直接返回 1（不写字节）。
    int dlen = static_cast<int>(strlen(dest));
    int slen = static_cast<int>(strlen(src));
    if (static_cast<int>(count) <= dlen + slen)
    {
        return 1;
    }
    memcpy(dest + dlen, src, slen + 1);
    return 0;
