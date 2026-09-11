pack_ret_t PackSet::sort()      // 0x08ac58ee
{
    index_vector_it it;   // b28c-L8：ORIG 0x08ac58ee 函数头默认 ctor，循环内赋值
    if (m_type != PACK_LOAD_FULL)
    {
        fprintf(stdout, "%s\n", "PACK_LOAD_FULL needed");
        fprintf(stdout, "%s(%d) - %s\n", k_srcFile, 0x3b7, __FUNCTION__);
        return PACK_ERROR;
    }

    m_indexes.reserve(m_addeds.size());
    for (it = m_addeds.begin(); it != m_addeds.end(); ++it)
    {
        m_indexes.push_back(*it);
    }
    std::sort(m_indexes.begin(), m_indexes.end(), &_index_less);
    m_addeds.clear();
    return PACK_OK;
}