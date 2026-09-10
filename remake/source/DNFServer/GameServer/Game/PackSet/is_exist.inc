bool PackSet::is_exist(char const* name)     // @0x08ac498c
{
    // b28c-L8：ORIG 0x08ac498c 先 find(sret -0x10) 后 end(临时 -0xc)，
    // 即左操作数具名结果再比较；直接写二进制表达式 4.4 反序求值。
    index_vector_it rval = find(name);
    return rval != m_indexes.end();
