pack_ret_t PackSet::deleted(char const* name)   // 0x08ac5332
{
    index_vector_it it;   // b28c-L8：ORIG 默认 ctor 前置（P28 两段式）
    if (m_type != PACK_LOAD_FULL)
    {
        fprintf(stdout, "%s\n", "PACK_LOAD_FULL needed");
        fprintf(stdout, "%s(%d) - %s\n", k_srcFile, 0x346, __FUNCTION__);
        return PACK_ERROR;
    }

    it = find(name);
    if (it != m_indexes.end())
    {
        if (it->second.content_ != 0)
        {
            delete[] it->second.content_;
        }
        m_indexes.erase(it);
    }
    return PACK_OK;
}