void CMonsterDropTest::makeGoldItemDropList(std::list<map_item>& items, DropStat& out)
{
    for (std::list<map_item>::iterator it = items.begin(); it != items.end(); ++it)
    {
        if (it->m_item.m_addInfo == 0)
        {
            std::map<int, int>::iterator mit = out.find(it->m_item.m_addInfo2);
            // P222 正向包裹：ORIG 块序 = je→insert、increment fall-through。
            if (mit != out.end())
            {
                mit->second++;
            }
            else
            {
                out.insert(std::make_pair(it->m_item.m_addInfo2, 1));
            }
        }
    }
