pack_ret_t PackSet::get(char const* name, unsigned char* buffer,
                        unsigned int buffer_size, unsigned int* content_size)   // 0x08ac5060
{
    // b28d-solo-PackSet 判词（AE，ORIG 0x08ac5060 size 0x168）：
    // 同 0x08ac51ce 重载：①`index_vector_it it;` 必须在三参守卫之前（ORIG 序言后
    // 立即 call iterator::C1Ev，it@-0x10）；②code 无槽往返（ORIG `mov (%eax),%ebx`
    // + push/pop %ebx）⇒ 裸 register 存储类提示（P231/P248，非 asm 绑定）。
    index_vector_it it;
    if (name == 0 || buffer == 0 || content_size == 0)
    {
        fprintf(stdout, "%s(%d) - %s\n", k_srcFile, 0x312, __FUNCTION__);
        return PACK_ERROR;
    }

    it = find(name);
    if (it == m_indexes.end())
    {
        fprintf(stdout, "%s(%d) - %s\n", k_srcFile, 0x319, __FUNCTION__);
        return PACK_ERROR;
    }

    // P167：ORIG 调 `…ptEv`（it->），`(*it)` 形额外实例化 deEv。
    register hash_t code = it->first;
    if (it->second.get_content(this, code, buffer, buffer_size, content_size) == PACK_ERROR)
    {
        fprintf(stdout, "%s(%d) - %s\n", k_srcFile, 799, __FUNCTION__);
        return PACK_ERROR;
    }
    return PACK_OK;
}