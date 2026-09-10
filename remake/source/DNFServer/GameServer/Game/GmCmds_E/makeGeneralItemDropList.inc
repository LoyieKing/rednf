void CMonsterDropTest::makeGeneralItemDropList(std::list<map_item>& items,
                                               DropStat& out)
{
    for (std::list<map_item>::iterator it = items.begin(); it != items.end(); ++it)
    {
        // P236：ORIG 无 key 具名局部槽（find 实参为内联成员表达式临时
        // `mov 0x12(%eax),%eax; mov %eax,-0x50` ⇒ 临时槽比 it 浅）；
        // 具名 key 会使 4.4 把 key 排到 it 之上（-0x54），ORIG 为 -0x50。
        std::map<int, int>::iterator mit = out.find(it->m_item.m_addInfo);
        // P0/P48：ORIG 条件与 insert 处均重新读成员（`call ptEv; mov 0x12(%eax)`），
        // 不用 key 槽；P222 正向包裹（mit!=end 直落、insert 走尾岛）。
        if (it->m_item.m_addInfo == 0)
        {
            if (mit != out.end())
            {
                mit->second += it->m_item.m_addInfo2;
            }
            else
            {
                out.insert(std::make_pair(
                    static_cast<int>(it->m_item.m_addInfo),
                    it->m_item.m_addInfo2));
            }
        }
        else
        {
            if (mit != out.end())
            {
                mit->second++;
            }
            else
            {
                out.insert(std::make_pair(
                    static_cast<int>(it->m_item.m_addInfo), 1));
            }
        }
    }
