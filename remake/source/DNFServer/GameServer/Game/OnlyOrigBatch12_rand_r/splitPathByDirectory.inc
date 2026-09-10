int splitPathByDirectory(const char* path)
{
    int last = -1;
    for (const char* p = path; *p != '\0'; ++p)
    {
        if (*p == '/' || *p == '\\')
            last = static_cast<int>(p - path);
    }
    return last;
