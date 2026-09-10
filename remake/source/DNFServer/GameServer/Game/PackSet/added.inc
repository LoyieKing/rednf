pack_ret_t PackSet::added(char const* name, IPackFileProcess* process)   // 0x08ac56a2
{
    // b28c-L8：ORIG 函数头唯一的前置 ctor call 是 index_pair_t entry（sret
    // -0x30），迭代器 it 无默认 ctor（find 的 sret 直落其槽 -0x10）⇒ entry
    // 声明提到函数头、it 保持直接初始化（P28/P212 声明序=槽深序）。
    index_pair_t entry;
    if (m_type != PACK_LOAD_FULL)
    {
        fprintf(stdout, "%s\n", "PACK_LOAD_FULL needed");
        fprintf(stdout, "%s(%d) - %s\n", k_srcFile, 0x38a, __FUNCTION__);
        return PACK_ERROR;
    }

    index_vector_it it = find(name);
    // b28d-36（P74② 实参序旋钮）：ORIG 0x08ac57a0 段 ne() 首参=&end(-0xc)、
    // 次参=&it(-0x10) ⇒ 原码 end() 在比较式**左侧**；写 `it != end()` 时 4.4
    // 反压。槽位不变，仅 0x4(%esp)/(%esp) 互换。
    if (m_indexes.end() != it)
    {
        fprintf(stdout, "find %s", name);
        fprintf(stdout, "%s(%d) - %s\n", k_srcFile, 0x390, __FUNCTION__);
        return PACK_ERROR;
    }

    // b28c-L8：ORIG 0x08ac56a2 无该 store —— pair<int,int> 的默认 ctor（函数头
    // 唯一前置 call，_ZNSt4pairIiiEC1Ev）已把 first 值初始化为 0，原源码不重复写。
    if (entry.second.update_content_from_file(name, this, process) == PACK_ERROR)
    {
        fprintf(stdout, "%s(%d) - %s\n", k_srcFile, 0x396, __FUNCTION__);
        return PACK_ERROR;
    }
    entry.first = _hash(toMbcs(entry.second.name_.c_str()));
    m_addeds.push_back(entry);
    return PACK_OK;
