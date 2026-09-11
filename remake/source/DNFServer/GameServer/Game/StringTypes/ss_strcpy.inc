int ss_strcpy(char* dest, size_t count, const char* src)
{
    // ORIG _Z9ss_strcpyPcjPKc @0x08ad463f：count 容纳 len+1 时 memcpy 并返回 0；
    // 否则 dest[0]=0 返回 1。
    int len = static_cast<int>(strlen(src));
    if (static_cast<int>(count) <= len)
    {
        *dest = '\0';
        return 1;
    }
    memcpy(dest, src, len + 1);
    return 0;
}