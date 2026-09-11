pack_ret_t IndexEntry::write_info(FILE* fp)   // @0x08ac4284
{
    if (fp == 0)
    {
        fprintf(stdout, "%s\n", "fp is NULL");
        fprintf(stdout, "%s(%d) - %s\n", k_srcFile, 0x1d0, __FUNCTION__);
        return PACK_ERROR;
    }
    _fwrite_str(name_.c_str(), fp);
    fwrite(&size_, 4, 1, fp);
    fwrite(&crc_, 4, 1, fp);
    fwrite(&offset_, 4, 1, fp);
    return PACK_OK;
}