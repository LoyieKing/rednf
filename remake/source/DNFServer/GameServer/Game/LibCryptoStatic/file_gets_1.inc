long file_gets(bio_st_st* b, char* buf, int size)
{
    buf[0] = 0;                                        // movb $0,(%ebx)
    std::FILE* fp = static_cast<std::FILE*>(b->ptr);
    if (std::fgets(buf, size, fp) == 0)
        return 0;
    if (buf[0] == 0)
        return 0;
    return static_cast<long>(std::strlen(buf));        // 87ec78e tail jmp strlen
