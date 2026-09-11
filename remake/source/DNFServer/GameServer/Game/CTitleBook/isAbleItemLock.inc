bool CTitleBook::isAbleItemLock(unsigned long itemIdx)
{
    // b28d-solo-CTitleBook 终判（AE，本波归零）：P74② 实参序旋钮——ORIG
    // 0x08642f5a 比较形为 `ne(end(-0x10), find-sret(-0x1c))`（end 在左、find 结果
    // 匿名）⇒ 源码写 `end() != std::find(...)`：匿名 find 消具名槽 -0x24、左操作数
    // 固定 ne 压参序，120=120 全等。旧「N93 槽序封顶」判词作废。
    for (std::map<int, std::vector<stTitleElement> >::iterator it =
             ((CTitleBookMgrRaw*)G_CDataManager())->m_titleBook.m_elements.begin();
         it != ((CTitleBookMgrRaw*)G_CDataManager())->m_titleBook.m_elements.end();
         ++it)
    {
        for (unsigned int i = 0; i < it->second.size(); ++i)
        {
            // ORIG 0x8641468 的 std::find 以 unsigned long（itemIdx 原类型）实例化，
            // 不做 (int) 转换。
            // b28d-solo-CTitleBook（P74②）：ORIG 0x08642f5a 比较端 end() 在左、
            // find 结果匿名（sret -0x1c）⇒ 保持 `end() != std::find(...)` 形。
            if (it->second[i].m_fitJobs.end() !=
                std::find(it->second[i].m_fitJobs.begin(),
                          it->second[i].m_fitJobs.end(),
                          itemIdx))
            {
                if (it->second[i].m_achievementIdx == -1)
                {
                    return 1;
                }
                return 0;
            }
        }
    }
    return 1;
}