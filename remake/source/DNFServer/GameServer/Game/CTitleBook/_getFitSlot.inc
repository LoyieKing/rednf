bool CTitleBook::_getFitSlot(int itemIdx, ENUM_TITLE_BOOK_CATEGORY& category,
                             int& slot)
{
    // [b28c-6] N82/P75：同 isAbleItemLock——std::find 实参装载序 + 迭代器槽基
    // 跨版本差，115=115 全等仅槽/序差。ndiff 24 封顶。
    std::map<int, std::vector<stTitleElement> >::iterator it =
        ((CTitleBookMgrRaw*)G_CDataManager())->m_titleBook.m_elements.begin();
    for (; it != ((CTitleBookMgrRaw*)G_CDataManager())->m_titleBook.m_elements.end();
         ++it)
    {
        for (unsigned int i = 0; i < it->second.size(); ++i)
        {
            // b28d-solo-CTitleBook 终判（AE，本波归零）：P74② 实参序旋钮——ORIG
            // 0x08642baa 比较形为 `ne(end(-0x10), find-sret(-0x1c))` ⇒ 匿名
            // `end() != std::find(...)` 形，115=115 全等；旧「N93/P278 封顶」作废。
            if (it->second[i].m_fitJobs.end() !=
                std::find(it->second[i].m_fitJobs.begin(),
                          it->second[i].m_fitJobs.end(),
                          itemIdx))
            {
                category = (ENUM_TITLE_BOOK_CATEGORY)it->first;
                slot = (int)i;
                return 1;
            }
        }
    }
    return 0;
}