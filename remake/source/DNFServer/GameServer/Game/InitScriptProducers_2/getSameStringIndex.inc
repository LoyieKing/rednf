int getSameStringIndex(const char* s, const char** table, int count)
{
    for (int i = 0; i < count; ++i)
    {
        if (std::strcmp(s, table[i]) == 0)
            return i;
    }
    return -1;
