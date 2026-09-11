// file_read @0x087ec7a0 (0xd0)：init&&fp&&out 检查 → fread(out,1,outl,fp)
// ferror 时按 ORIG 双 ERR 上报后 -1。
long file_read(bio_st_st* b, char* out, int outl)
{
    std::FILE* fp = static_cast<std::FILE*>(b->ptr);
    if (b->init == 0 || out == 0 || fp == 0)   // 87ec7b3/b8/b8 联合判定
        return 0;                              // xor edx,edx
    std::size_t r = std::fread(out, 1, static_cast<std::size_t>(outl), fp);
    if (std::ferror(fp)) {                     // 87ec7ed..f6
        ERR_put_error(2, 0x0b, *__errno_location(), "bss_file.c", 0xf5);
        ERR_put_error(0x20, 0x82, 2, "bss_file.c", 0xf6);
        return -1;                             // 87ec853
    }
    return static_cast<long>(r);
}