unsigned int CAchievement::_getSectionIdx(ENUM_TITLE_BOOK_CATEGORY category,
                                          int slot)
{
    // ORIG 0x828bcc0：while(i<size)；section 内层块使析构先于 ++i；
    // 每次循环直接调用 GetTitleSections(G_CDataManager())，不缓存局部引用
    unsigned int i = 0;
    while (i < GetTitleSections(G_CDataManager()).size())
    {
        {
            stTitleSection section = GetTitleSections(G_CDataManager())[i];
            if ((section.m_category == (int)category) &&
                (section.m_startIdx <= slot) && (slot <= section.m_endIdx))
            {
                return i;
            }
        }
        ++i;
    }
    return 0xffffffff;
