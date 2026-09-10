long file_write(bio_st_st* b, const char* in, int inl)
{
    std::FILE* fp = static_cast<std::FILE*>(b->ptr);
    if (b->init == 0 || in == 0)
        return 0;
    std::size_t w = std::fwrite(in, static_cast<std::size_t>(inl), 1, fp);
    if (w != 0)
        return static_cast<long>(inl);         // ORIG: mov 0x28(%esp)（原参 larg/inl 回传）
    return 0;
