// ORIG T 0x081a28e4：遍历 m_data，命中 m_begin<=low 且 high<m_end（开区间）
// 的项：复制 m_begin/m_id/m_end；m_name 经 findString(4,...) 解析后
// strncpy 0x14；m_str 经 findString(4,...) 解析后 strncpy 0xff；返 1。
// 未命中返 0。
int UsedFatigueRewardItem::find(int low, int high, Data& out)
{
    // [b28c-30] ORIG 0x081a28e4：end() 在循环外求值一次落 -0xc（P13 end 缓存），
    // begin() 落 -0x10；原形每轮重调 m_data.end() 且槽序相反。
    std::vector<Data>::iterator itEnd = m_data.end();
    for (std::vector<Data>::iterator it = m_data.begin(); it != itEnd; ++it)
    {
        if (it->m_begin >= low && it->m_begin < high)   // ORIG 两次 movzwl (%eax)：low/high 均对 m_begin
        {
            out.m_begin = it->m_begin;
            out.m_id = it->m_id;
            out.m_end = it->m_end;
            strncpy(out.m_name,
                    g_scriptStringManager_.findString(4, it->m_name, 0), 0x14);
            strncpy(out.m_str,
                    g_scriptStringManager_.findString(4, it->m_str, 0), 0xff);
            return 1;
        }
    }
    return 0;
}