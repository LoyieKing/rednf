long file_read(bio_st_st* b, char* out, int outl)
{
    std::FILE* fp = static_cast<std::FILE*>(b->ptr);
    if (b->init == 0 || out == 0 || fp == 0)   // 87ec7b3/b8/b8 联合判定
        return 0;                              // xor edx,edx
    std::size_t r = std::fread(out, 1, static_cast<std::size_t>(outl), fp);
    if (std::ferror(fp)) {                     // 87ec7ed..f6
        static const char kFile[] = "bss_file.c";
        ERR_put_error(2, 0x0b, *__errno_location(), kFile, 0xf5);
        ERR_put_error(0x20, 0x82, 2, kFile, 0xf6);
        return -1;                             // 87ec853
    }
    return static_cast<long>(r);
