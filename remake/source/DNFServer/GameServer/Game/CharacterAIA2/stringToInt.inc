int stringToInt(const char* s, const char** table, int maxCount)
{
    if (maxCount == -1)
        maxCount = 999;
    // ORIG 0x08a38aa7..0x08a38ae8：loop 头只判 i<max（setl）；体内 table[i]
    // 两次独立装载（null 守卫 + strcmp 实参，O0 无 CSE，P48），空则跳出。
    for (int i = 0; i < maxCount; ++i)
    {
        if (table[i] == 0)
            break;
        if (strcmp(table[i], s) == 0)
            return i;
    }
    return -1;
