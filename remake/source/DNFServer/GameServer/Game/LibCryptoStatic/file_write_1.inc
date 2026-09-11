// file_write @0x087ec870 (0x48)：init&&in && fwrite(in,inl→1,inl? ORIG
// fwrite(in, larg, 1, fp)? 参数序 (in, inl, 1, fp)（87ec88c movl $1,+8 =第3参 size, +4=inl 第2参）。
long file_write(bio_st_st* b, const char* in, int inl)
{
    std::FILE* fp = static_cast<std::FILE*>(b->ptr);
    if (b->init == 0 || in == 0)
        return 0;
    std::size_t w = std::fwrite(in, static_cast<std::size_t>(inl), 1, fp);
    if (w != 0)
        return static_cast<long>(inl);         // ORIG: mov 0x28(%esp)（原参 larg/inl 回传）
    return 0;
}