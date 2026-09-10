void SecretShopScript::verifyNpc(std::multimap<int, stDungeonNpc>& npcMap)
{
    int total = 0;
    for (std::multimap<int, stDungeonNpc>::iterator it = npcMap.begin();
         it != npcMap.end(); ++it)
    {
        std::pair<std::multimap<int, stDungeonNpc>::iterator,
                  std::multimap<int, stDungeonNpc>::iterator> range =
            npcMap.equal_range(it->first);
        total = 0;
        for (std::multimap<int, stDungeonNpc>::iterator r = range.first;
             r != range.second; ++r)
        {
            total += r->second.m_rate;
        }
        if (total != 10000)
        {
            printf("[SecretShop Error] NPC ratio is wrong!(index:%d)\n",
                   range.first->second.m_field0);
        }
    }
