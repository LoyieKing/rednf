pack_ret_t PackSet::get(char const* name, unsigned char** content,
                        unsigned int* content_size)                             // 0x08ac51ce
{
    // b28d-solo-PackSet 判词（AE，ORIG 0x08ac51ce size 0x163）：
    // ①ORIG 序言后**立即** `lea -0x10; call iterator::C1Ev`（默认构造）再做三参
    //   守卫 ⇒ 原码 `index_vector_it it;` 声明在参数检查之前（P28/P50 两段式）；
    //   旧形在守卫后才声明，it 槽漂到 -0x14、end 临时 -0x10（全帧错位）。
    // ②ORIG `mov (%eax),%ebx` + 尾 `mov -0x4(%ebp),%ebx`（push/pop %ebx）⇒ code
    //   无栈槽往返，跨第二次 operator-> 与 get_content 保活（P0/P231/P248 三条件：
    //   跨 callee call 保活 + ORIG 目标为 callee-saved %ebx）⇒ 裸 register
    //   存储类提示（非 asm 绑定，P0 合规）。
    index_vector_it it;
    if (name == 0 || content == 0 || content_size == 0)
    {
        fprintf(stdout, "%s(%d) - %s\n", k_srcFile, 0x32c, __FUNCTION__);
        return PACK_ERROR;
    }

    it = find(name);
    if (it == m_indexes.end())
    {
        fprintf(stdout, "%s(%d) - %s\n", k_srcFile, 0x333, __FUNCTION__);
        return PACK_ERROR;
    }

    register hash_t code = it->first;
    if (it->second.get_content(this, code, content, content_size) == PACK_ERROR)
    {
        fprintf(stdout, "%s(%d) - %s\n", k_srcFile, 0x339, __FUNCTION__);
        return PACK_ERROR;
    }
    return PACK_OK;
}