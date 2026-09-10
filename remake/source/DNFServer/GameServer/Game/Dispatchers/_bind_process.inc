bool Dispatcher_MonstercardBind::_bind_process(
    ENUM_MONSTER_CARD_BIND_TYPE type, ENUM_RARITY rarity,
    std::vector<std::pair<int, int> >& results)
{
    STExpertJobScript* script = static_cast<STExpertJobScript*>(
        G_CDataManager()->GetExpertJobScript(1));
    STExpertJobScript::stMonsterCardBindInfo* info;
    if (type == BIND_TYPE_SPECIAL)
    {
        info = script->GetSpecialBindInfo();
    }
    else if (type == BIND_TYPE_COMMON)
    {
        info = script->GetCommonBindInfo();
    }
    else
    {
        results.clear();
        return false;
    }
    const std::vector<stMonsterCardBindResult>& table = info->m_vec[rarity];
    const int total = info->m_count[rarity];
    // ORIG：table.size() 各次独立调用（不缓存 entryCount）
    if (table.size() == 0)
    {
        cMyTrace trace(
            "bool Dispatcher_MonstercardBind::_bind_process("
            "Dispatcher_MonstercardBind::ENUM_MONSTER_CARD_BIND_TYPE, "
            "ENUM_RARITY, IntPairVector&)",
            0x44b1, 5);
        trace("MonstercardBind Error : size(%d)", (int)table.size());
        return false;
    }
    if (table.size() != 0 && total != 0)
    {
        unsigned long range = (unsigned long)(total - 1);
        const int pick = m_rand->randInt(range) + 1;
        int accWeight = 0;
        for (size_t i = 0; i < table.size(); ++i)
        {
            if (accWeight < pick && pick <= accWeight + (int)table[i].m_field4)
            {
                results.push_back(std::make_pair(table[i].m_field0, 1));
                break;
            }
            accWeight += (int)table[i].m_field4;
        }
        if (results.empty())
        {
            cMyTrace trace(
                "bool Dispatcher_MonstercardBind::_bind_process("
                "Dispatcher_MonstercardBind::ENUM_MONSTER_CARD_BIND_TYPE, "
                "ENUM_RARITY, IntPairVector&)",
                0x44ca, 5);
            trace("Dispatcher_MonstercardBind::_bind_process ERROR : result "
                  "is empty");
            return false;
        }
        return true;
    }
    cMyTrace trace(
        "bool Dispatcher_MonstercardBind::_bind_process("
        "Dispatcher_MonstercardBind::ENUM_MONSTER_CARD_BIND_TYPE, "
        "ENUM_RARITY, IntPairVector&)",
        0x44b6, 5);
    trace("MonstercardBind Error : size(%d), sum(%d)", (int)table.size(),
          total);
    return false;
