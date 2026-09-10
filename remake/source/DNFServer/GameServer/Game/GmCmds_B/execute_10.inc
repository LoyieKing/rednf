void CClearRewardTest::execute()
{
    CUser* user = GetUser();
    if (user == 0 || user->getCurCharacR() == 0 || user->get_state() != 5)
    {
        return;
    }
    CParty* party = static_cast<CParty*>(user->GetParty());
    if (party == 0 || party->m_battleField.m_pDungeon == 0)
    {
        return;
    }
    CDungeon* dungeon = party->m_battleField.m_pDungeon;

    // CBattle_Field 子对象位于 CParty+0xb24（占位期经 m_battleField 视图访问）。
    CBattle_Field* battleField =
        &party->m_battleField;
    int diff = battleField->get_dungeon_diff();

    int standardLevel = 0;
    if (dungeon->get_dimension_possible() < 1)
    {
        standardLevel = dungeon->get_standard_level();
    }
    else
    {
        standardLevel = party->getStandardDimensionLevel();
    }

    WongWork::stGenerateRefData_t ref;                    // ORIG 0814a706 ctor
    ref.m_field34 = (char)standardLevel;                  // +0x34（基准值）
    ref.m_field35 = (char)diff;                           // +0x35（难度差【推断】）
    ref.m_rankBonusIndex = 0x3f800000;                    // +0x38（float 位型常量）
    ref.m_goldBase = 0;                                   // +0x3c 4 字节写 0
    ref.m_goldFactor = 0;
    ref.m_dungeon = dungeon;                              // +0x40

    WongWork::CItemGeneratorMgr* mgr = G_CDataManager()->getItemGenerator();
    WongWork::CItemGenerator* generator =
        mgr->getGenerator((WongWork::CItemGeneratorMgr::eGenerateType_t)2);

    // ---- 第一轮：默认掉率 → cltr_normal.txt ----
    {
        WongWork::stGenerateResult_t result;
        for (int i = 0; i < 100000; ++i)
        {
            generator->generateRandomItem(ref, result);
        }

        std::map<int, int> statMap;
        for (std::vector<Inven_Item>::iterator it = result.m_dropItems.begin();
             it != result.m_dropItems.end(); ++it)
        {
            int itemIdx = it->m_addInfo;
            std::map<int, int>::iterator mit = statMap.find(itemIdx);
            if (mit != statMap.end())
            {
                ++mit->second;
            }
            else
            {
                statMap.insert(std::make_pair(itemIdx, 1));
            }
        }

        std::string out;
        char line[1024];
        for (std::map<int, int>::iterator it = statMap.begin();
             it != statMap.end(); ++it)
        {
            memset(line, 0, sizeof(line));
            if (it == statMap.begin())
            {
                memcpy(line,
                       "    Item Index\t\t                     Item Name\t\tRarity\t\tCount\n",
                       0x3f);
                out += line;
                memset(line, 0, 0x400);
            }
            CItem* item = G_CDataManager()->find_item(it->first);
            if (item != 0)
            {
                sprintf(line, "%14d\t\t%30s\t\t%6d\t\t%5d\n", item->get_index(),
                        item->GetItemName(), item->get_rarity(), it->second);
                out += line;
            }
        }
        printClient(GetUser(), "cltr_normal.txt", out.c_str());
    }

    // ---- 第二轮：ref+0x3c 置 1（金币基数）→ 清空结果容器后重采样 ----
    ref.m_goldBase = 1;                                   // ORIG movl $0x1,-0x120(%ebp)
    ref.m_goldFactor = 0;
    {
        WongWork::stGenerateResult_t result;
        for (int i = 0; i < 100000; ++i)
        {
            generator->generateRandomItem(ref, result);
        }

        std::map<int, int> statMap;
        for (std::vector<Inven_Item>::iterator it = result.m_dropItems.begin();
             it != result.m_dropItems.end(); ++it)
        {
            int itemIdx = it->m_addInfo;
            std::map<int, int>::iterator mit = statMap.find(itemIdx);
            if (mit != statMap.end())
            {
                ++mit->second;
            }
            else
            {
                statMap.insert(std::make_pair(itemIdx, 1));
            }
        }

        std::string out;
        char line[1024];
        for (std::map<int, int>::iterator it = statMap.begin();
             it != statMap.end(); ++it)
        {
            memset(line, 0, sizeof(line));
            if (it == statMap.begin())
            {
                memcpy(line,
                       "    Item Index\t\t                     Item Name\t\tRarity\t\tCount\n",
                       0x3f);
                out += line;
                memset(line, 0, 0x400);
            }
            CItem* item = G_CDataManager()->find_item(it->first);
            if (item != 0)
            {
                sprintf(line, "%14d\t\t%30s\t\t%6d\t\t%5d\n", item->get_index(),
                        item->GetItemName(), item->get_rarity(), it->second);
                out += line;
            }
        }
        printClient(GetUser(), "cltr_gold.txt", out.c_str());
    }
