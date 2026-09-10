long file_puts(bio_st_st* b, const char* str)
{
    if (b->init == 0 || str == 0)
        return 0;
    unsigned long len = std::strlen(str);
    std::FILE* fp = static_cast<std::FILE*>(b->ptr);
    if (std::fwrite(str, 1, len, fp) != 0)
        return static_cast<long>(len);
    return 0;
