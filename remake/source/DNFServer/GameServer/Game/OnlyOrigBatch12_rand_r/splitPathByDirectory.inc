// ===========================================================================
// splitPathByDirectory  ORIG T 0x088bbc30：返回最后一个 '/' 或 '\\' 的下标，
// 无分隔符返回 -1。（ORIG TU = RDARScript.cpp）
// ===========================================================================
int splitPathByDirectory(const char* path)
{
    int last = -1;
    for (const char* p = path; *p != '\0'; ++p)
    {
        if (*p == '/' || *p == '\\')
            last = static_cast<int>(p - path);
    }
    return last;
}