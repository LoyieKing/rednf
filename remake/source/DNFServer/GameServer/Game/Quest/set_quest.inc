void Quest::set_quest(QuestScript script)
{
    m_name = script.m_name;
    m_index = script.m_index;
    m_type = script.m_questType;
    m_npcIdx = script.m_npcIdx;
    m_targetCharacters = script.m_targetCharacters;
    m_jobChangeQuest = script.m_jobChangeQuest;
    m_questCountLimit = script.m_questCountLimit;
    m_levelMin = script.m_levelMin[0];
    m_levelMax = script.m_levelMin[1];
    m_intDataList = script.m_intDataList;
    m_antiQuestList = script.m_antiQuestList;
    m_clearSendFlag = script.m_field130;
    m_clearItems = script.m_clearItems;
    m_killItems = script.m_killItems;
    m_appearMap = script.m_appearMap;
    m_rewardSelectList = script.m_rewardSelectList;
    m_triggerType = script.m_triggerType;
    m_triggerSubType = script.m_triggerSubType;
    m_triggerData = script.m_triggerData;
    m_field90 = script.m_field1b0;
    m_postalReward = script.m_postalReward;
    m_field101 = script.m_field22e;
    m_field130 = script.m_field108;

    if (m_triggerType == 0)
    {
        // ORIG 0x835256c：pair p(-0x48) 具名 + `p.first = *it; it++;` 形；
        // 奇数尾部走 cMyTrace（0x8352602 "(it == intDatas_.end())"）后跳出。
        // b28d-solo-Quest：ORIG 槽 it=-0x40（浅）/pair=-0x48（深）+ 发射序 begin→pair C1
        // ⇒ 4.4「先声明占浅槽」下 iterator 必须声明在 pair 之前（P28）。
        std::vector<int>::iterator it = m_triggerData.begin();
        std::pair<int, int> p;
        while (it != m_triggerData.end())
        {
            p.first = *it;
            it++;
            if (it == m_triggerData.end())
            {
                // b28d-solo-Quest [P221]：ORIG trace 槽 -0x30 浅于 it(-0x40)/pair(-0x48)
                // ⇒ 全表达式临时链（构造即调用）复现块内临时槽方向。
                cMyTrace(__PRETTY_FUNCTION__, 0x109d, 5)(
                    "[%s][%d]_(it == intDatas_.end())",
                    __PRETTY_FUNCTION__, 0x109d);
                break;
            }
            p.second = *it;
            it++;
            m_triggerDataPairs.push_back(p);
        }
    }
    else if (m_triggerType == 0x19)
    {
        m_triggerDataPairs.clear();
        // ORIG 0x83526bf：sibling 分支 pair p(-0x50)；ORIG 形 `*it++` 后置
        // 递增（临时槽 -0x18/-0x14）且循环条件 = `i < n && it != end`。
        std::pair<int, int> p;
        // b28d-solo-Quest：ORIG 尾测 `cmp -0x10(%ebp),%eax; jge`（有符号）⇒ i/n 为 int
        // （unsigned 形在 4.4 恒 jae，P2/P209）。
        int n = (int)(m_triggerData.size() >> 1);
        std::vector<int>::iterator it = m_triggerData.begin();
        for (int i = 0; (i < n) && (it != m_triggerData.end()); ++i)
        {
            p.first = *it++;
            p.second = *it++;
            m_triggerDataPairs.push_back(p);
        }
    }

    m_fieldDC.clear();
    m_fieldE8 = 0;
    m_rewardItems.clear();
    m_fieldB8 = script.m_rewardType;
    m_fieldC0 = script.m_field1e4;
    m_fieldBC = script.m_field228;
    if ((m_fieldB8 == 0) || (m_fieldB8 == 0x19))
    {
        m_titleReward = script.m_titleReward;
        m_selectReward = script.m_selectReward;
        m_fieldDC = script.m_field20c;
        m_fieldE8 = script.m_field218;
        makeRewardItemInfo(script.m_rewardData, m_rewardItems);
    }
    m_fieldF8 = script.m_field128;
    m_fieldFC = script.m_field12c;
    m_exposeQuest = script.m_field22d;
    m_field102 = script.m_field23d;
    m_field104 = script.m_field240;
    m_mailFlag = script.m_mailQuestFlag;
    m_mailTitle = script.m_mailTitle;
    m_mailContents = script.m_mailContents;
    m_mailItems = script.m_mailItems;
    m_field120 = script.m_field270;
    m_eventDialogs = script.m_eventDialogs;
    m_powerSide = script.m_powerSide;
    m_field134 = 0;
    m_field134 = script.m_fieldD8;
    m_field138 = script.m_field280;
    m_field13a = script.m_field282;
    m_field13c = script.m_field2a8;
    m_field140 = script.m_field2ac;
    m_exceptionList = script.m_exceptionList;
