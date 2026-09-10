pack_ret_t IndexEntry::get_content(PackSet* owner, hash_t code,
                                   unsigned char** dst, unsigned int* content_size)
{
    if (dst == 0)
    {
        return PACK_ERROR;
    }
    *dst = new unsigned char[aligned_size_];
    if (*dst == 0)
    {
        return PACK_ERROR;
    }
    return get_content(owner, code, *dst, aligned_size_, content_size);
