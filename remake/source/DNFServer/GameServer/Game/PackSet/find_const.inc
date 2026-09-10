PackSet::index_vector_const_it PackSet::find_const(char const* name)   // sret @0x08ac4b6c
{
    // b28d-solo-PackSet 判词（AE，ORIG 0x08ac4b6c size 0x291）：
    // 与 0x08ac4e00 同族（const_iterator 版）：①声明序 code,dummy,er,file 且 code
    // 无预清；②er 先 C1Ev 再 `aSEOS9_` 赋值；③`it->`（ptEv）非 `(*it)`；④it2
    // 两段式（C1Ev + 赋值）；⑤P283 goto 尾标签复现底部条件块。
    // ORIG 局部一律**非 const** iterator（er/it2），仅返回处隐式转换为
    // const_iterator（ORIG 0x08ac4d43/0x08ac4dbb 两处 call
    // `__normal_iterator<const pair*>::C1<__normal_iterator<pair*>>` 转换构造，
    // nm 实证 @0x08ac7ce4）⇒ P96/P270 非 const 形；旧形全 const 使 4.4 直接位拷贝。
    hash_t code;
    IndexEntry dummy;
    std::pair<index_vector_it, index_vector_it> er;
    char file[260];

    _correct_filename(name, file, 0x104, 0, 0);
    code = _hash(toMbcs(file));

    er = std::equal_range(m_indexes.begin(), m_indexes.end(),
                          index_pair_t(code, dummy), &_index_less);

    if (er.first == m_indexes.end() || er.first->first != code)
    {
        return m_indexes.end();
    }

    index_vector_it it2;
    it2 = er.first;
    goto find_const_cond;
find_const_body:
    if (strcmp(it2->second.name_.c_str(), file) == 0)
    {
        return it2;
    }
    ++it2;
find_const_cond:
    if (it2->first == code && it2 != er.second)
    {
        goto find_const_body;
    }
    return m_indexes.end();
