// 内存整体装载 @0x08ac4378
pack_ret_t IndexEntry::update_content_from_memory(unsigned char* mem, PackSet* owner)
{
    if (mem == 0)
    {
        fprintf(stdout, "%s(%d) - %s\n", k_srcFile, 0x1e0, __FUNCTION__);
        return PACK_ERROR;
    }
    if (type_ == ENTRY_FILE)
    {
        fprintf(stdout, "%s\n", "not implemented");
        fprintf(stdout, "%s(%d) - %s\n", k_srcFile, 0x1e6, __FUNCTION__);
        return PACK_ERROR;
    }
    if (type_ == ENTRY_PACK)
    {
        content_ = new unsigned char[aligned_size_];
        memcpy(content_, mem + owner->header_size() + offset_, aligned_size_);
    }
    return PACK_OK;
}