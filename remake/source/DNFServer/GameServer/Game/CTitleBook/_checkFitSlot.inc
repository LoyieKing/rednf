bool CTitleBook::_checkFitSlot(int itemIdx, ENUM_TITLE_BOOK_CATEGORY category,
                               int slot)
{
    // b28d-solo-CTitleBook（P96/P270）：ORIG 0x08642d42 map::find 为非 const 版
    // （`findERS7_` 无 K），it 即 `iterator`；写 const_iterator 会多一次转换构造
    // （4.4 额外 call + 临时槽 -0x24，帧 0x38→0x48）。
    // b28d-solo-CTitleBook 终判（AE，本波归零）：P74② 实参序旋钮——ORIG 0x08642e2d
    // 比较形为 `ne(end(-0x10), find-sret(-0x1c))` ⇒ 匿名 `end() != std::find(...)`
    // 形，111=111 全等。旧「N93 槽序封顶」判词作废。
    std::map<int, std::vector<stTitleElement> >::iterator it =
        ((CTitleBookMgrRaw*)G_CDataManager())->m_titleBook.m_elements.find(
            (int)category);
    if (it == ((CTitleBookMgrRaw*)G_CDataManager())
                  ->m_titleBook.m_elements.end())
    {
        return 0;
    }
    // b28d-solo-CTitleBook（P157 反向：ORIG 缓存）：ORIG 0x08642daf 把
    // `it->second` 的地址缓存到具名局部 -0xc（ptEv + `add $0x4`），后续
    // size()/三次 operator[] 全部复用该局部；逐次展开会多发 ptEv。
    // b28d-solo-CTitleBook 本波：P167 差集实证 ORIG 调 ptEv、旧写 `(*it).second`
    // 调 deEv（MISSING ptEv / EXTRA deEv）⇒ 改 `it->second`。
    const std::vector<stTitleElement>* pVec = &it->second;
    if ((int)slot >= (int)pVec->size())
    {
        return 0;
    }
    // b28d-solo-CTitleBook（P74②）：ORIG 0x08642e2d 比较端 end() 在左、find 结果
    // 匿名（sret -0x1c）⇒ 保持 `end() != std::find(...)` 形。
    if ((*pVec)[slot].m_fitJobs.end() !=
        std::find((*pVec)[slot].m_fitJobs.begin(),
                  (*pVec)[slot].m_fitJobs.end(),
                  itemIdx))
    {
        return 1;
    }
    if ((*pVec)[slot].m_achievementIdx == -1)
    {
        return 1;
    }
    return 0;
