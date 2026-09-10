bool CAchievement::_getTabComplete(stTitleSection* section,
                                   ENUM_TITLE_BOOK_CATEGORY category)
{
    // ORIG 0x828bdd4：不缓存 elements 引用，find/end 各重新取一次
    std::map<int, std::vector<stTitleElement> >::iterator it =
        GetTitleElements(G_CDataManager()).find((int)category);
    if (it != GetTitleElements(G_CDataManager()).end())
    {
        // ORIG 0x828be47：vector 复制构造到局部（非引用），
        // 且每次成员访问都重新 operator[]（不缓存 elem 引用）
        std::vector<stTitleElement> list = it->second;
        for (unsigned int i = (unsigned int)section->m_startIdx;
             i < (unsigned int)section->m_endIdx; ++i)
        {
            // ORIG 0x828be6e/0x828be8d 两守卫各自 `cmp $0xffffffff; sete %al;
            // test %al,%al; jne <独立 nop 岛>`（P133/P225：否定式独立守卫 +
            // continue 才产 sete 物化与两枚独立岛；&& 短路被 4.4 折成直跳）。
            if (list[i].m_index == -1)
            {
                continue;
            }
            if (list[i].m_achievementIdx == -1)
            {
                continue;
            }
            if (!isClearedAchievement(
                    (unsigned int)list[i].m_achievementIdx))
            {
                return false;
            }
        }
    }
    return true;
