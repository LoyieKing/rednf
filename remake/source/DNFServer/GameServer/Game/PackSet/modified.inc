pack_ret_t PackSet::modified(char const* name, IPackFileProcess* process)   // 0x08ac54ee
{
    index_vector_it it;   // b28c-L8：ORIG 默认 ctor 前置（P28 两段式）
    if (m_type != PACK_LOAD_FULL)
    {
        fprintf(stdout, "%s\n", "PACK_LOAD_FULL needed");
        fprintf(stdout, "%s(%d) - %s\n", k_srcFile, 0x369, __FUNCTION__);
        return PACK_ERROR;
    }

    it = find(name);
    if (it == m_indexes.end())
    {
        fprintf(stdout, "find failed. %s", name);
        fprintf(stdout, "%s(%d) - %s\n", k_srcFile, 0x370, __FUNCTION__);
        return PACK_ERROR;
    }

    IndexEntry& entry = it->second;
    if (entry.content_ != 0)
    {
        delete[] entry.content_;
        // b28c-L8 真语义 bug 修正：ORIG 0x08ac54ee 此处 store 目标为 +0x18，
        // 而 IndexEntry::offset_ 经权威 getter _ZNK10IndexEntry6offsetEv
        // @0x08ac3e94 = `mov 0x14(%eax),%eax` 实证在 +0x14，+0x18 是 content_
        // （_ZNK10IndexEntry7contentEv @0x08ac3ea0 = `mov 0x18(%eax),%eax`）。
        // 旧形清 offset_ 会留下已释放的 content_ 野指针 ⇒ 二次 delete[] 风险。
        entry.content_ = 0;
    }

    if (entry.update_content_from_file(name, this, process) == PACK_ERROR)
    {
        fprintf(stdout, "%s(%d) - %s\n", k_srcFile, 0x37d, __FUNCTION__);
        return PACK_ERROR;
    }
    return PACK_OK;
