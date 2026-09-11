void CItemList::getCeraShopBonusItem(int idx, unsigned long& outItem,
                                     unsigned int& outCount) const
{
    // [b28d-solo-CItemList] 复验 = AE（0 差异行）| 修：m_field0/m_field8
    //   int→unsigned int（P265/P211 同型化单段直存）+ P183 去 rates 引用缓存。
    // [b28c-21] N93 封顶：ORIG 双 const_iterator 槽 -0x1c/-0x14（外层先/内层后），
    // 4.4 恒反向（-0x14/-0x1c）；嵌套 for 声明序不可控。
    // [b28d-20] fresh 复验：迭代器槽已对齐（旧 N93 判词不再适用）；真实残差 = 唯一
    //   `sum = m_field8 + sum` 段（ORIG 0x8512xx 4 条：`mov 0x8(%eax),%edx;
    //   mov -0xc(%ebp),%eax; lea (%edx,%eax,1),%eax; mov %eax,-0xc(%ebp)`）被 4.4
    //   折叠为 2 条 `mov 0x8(%eax),%eax; add %eax,-0xc(%ebp)`（同型 int+int 恒折，
    //   P211 同型律）。异型化试形：sum 改 unsigned int + 比较处 (int)sum ⇒ 仍折叠
    //   （实测无变化）；volatile 致整帧槽漂移 111 vs 108 反劣。⇒ N86b/N18a 封顶。
    outItem = (unsigned long)-1;
    outCount = 0;
    int rand;
    std::vector<stCeraShopBonusInfo_t>::const_iterator it =
        m_bonusList.begin();
    std::vector<stCeraShopBonusInfo_t>::const_iterator end =
        m_bonusList.end();
    while (it != end)
    {
        if (it->m_field0 <= idx && idx <= it->m_field4)
        {
            rand = get_rand_int(1000);
            int sum = 0;
            std::vector<stCeraShopBonusItemInfo_t>::const_iterator it2 =
                it->m_items.begin();
            std::vector<stCeraShopBonusItemInfo_t>::const_iterator end2 =
                it->m_items.end();
            while (it2 != end2)
            {
                sum = sum + it2->m_field8;
                if (sum > rand)
                {
                    // ORIG 仅写 64 位出参低 32 位（int→ulong 不做零扩展高位写）。
                    (unsigned int&)outItem = it2->m_field0;
                    outCount = (unsigned short)it2->m_field4;
                    return;
                }
                ++it2;
            }
        }
        ++it;
    }
}