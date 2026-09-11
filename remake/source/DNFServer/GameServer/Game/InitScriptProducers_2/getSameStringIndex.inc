// ============================================================================
// getSameStringIndex —— ORIG T 0x08990ed5
// strcmp 线性查表，命中返回下标，未命中返回 -1。
// ============================================================================
int getSameStringIndex(const char* s, const char** table, int count)
{
    for (int i = 0; i < count; ++i)
    {
        if (std::strcmp(s, table[i]) == 0)
            return i;
    }
    return -1;
}