bool CTitleBook::_checkMovetoInven(ENUM_TITLE_BOOK_CATEGORY category,
                                   int slot)
{
    // b28d-solo-CTitleBook（P96/P270 + P157 反向）：ORIG 0x08641c52 用非 const
    // map::find（`findERS7_` 无 K）⇒ it 为 iterator；且 `it->second` 缓存到具名
    // 局部 -0xc（ptEv + `add $0x4`）后复用，operator[] 为 const 版（ixEj 的 K 版）。
    // b28d-solo-CTitleBook 本波：P167 差集实证 ORIG 调 ptEv、旧写 `(*it).second`
    // 调 deEv ⇒ 改 `it->second`（verdict 保持 AE）。
    std::map<int, std::vector<stTitleElement> >::iterator it =
        ((CTitleBookMgrRaw*)G_CDataManager())->m_titleBook.m_elements.find(
            (int)category);
    if (it == ((CTitleBookMgrRaw*)G_CDataManager())
                  ->m_titleBook.m_elements.end())
    {
        return 0;
    }
    const std::vector<stTitleElement>* pVec = &it->second;
    if ((int)slot >= (int)pVec->size())
    {
        return 0;
    }
    // P56：ORIG 0x08641d01 为 sete/test/je + mov$1/jmp/mov$0 分支形 ⇒ 写 if-return。
    if ((*pVec)[slot].m_achievementIdx == -1)
    {
        return 1;
    }
    return 0;
}