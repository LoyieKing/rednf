bool CBattle_Field::DestroyPassiveObject(int idx)
{
    // b28d-60：补 ORIG 的 empty() 调用（原缺 = 语义缺口 N-88b）+ P222 正向包裹
    //（ORIG `test;je <尾 mov$0>`，非提前返回）+ P183 去 list 引用缓存（ORIG 每处
    // 重算 `mov 0x108(%eax),%eax; add $0xa8,%eax`，无 -0x10 槽）。
    if (m_pMap)                                        // this+0x108
    {
        if (m_pMap->m_passiveObjectList.empty())       // CMap+0xa8
            return false;
        for (std::size_t i = 0; i < m_pMap->m_passiveObjectList.size(); ++i)
        {
            if (m_pMap->m_passiveObjectList[i] == idx)
                return true;
        }
    }
    return false;
