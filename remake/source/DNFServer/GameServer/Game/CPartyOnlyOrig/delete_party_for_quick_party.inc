int CParty::delete_party_for_quick_party(CUser* user,
                                         std::list<CUser*>& list)
{
    if (user == 0)
    {
        LogManager::logFormat(
            1, "party.cpp",
            __PRETTY_FUNCTION__,
            0x465, "QUICK_PARTY_LOG : NO EXIST USER!!");
        return 1;
    }
    if (get_member_count() == 1)
    {
        // P87/P52：ORIG 以 %edi/%esi/%ebx 跨 NumberToString/cMyTrace ctor
        // 缓存三实参（+0x27/+0x31/+0x38），4.4 普通局部恒 spill；register
        // 形态贴近 callee-saved 驻留。
        register int partyIdx = static_cast<short>(user->GetPartyIndex());
        register int userState = user->get_state();
        register char* idStr = NumberToString(user->get_acc_id(), 0);
        cMyTrace trace(__PRETTY_FUNCTION__,
                       0x46c, 0);
        trace("<IN_DUNGEON_NULL_PARTY> m_id(%s), user_state(%d), PartyIndex(%d)",
              idStr, userState, static_cast<int>(partyIdx));
        user->SetPartyIndex(-1);
        list.push_back(user);
        destroy();
        return 0;
    }
    if (m_manager == user)
    {
        change_manager_for_quick_party();
        list.push_back(user);
        return 0;
    }
    for (int i = 0; i <= 3; i++)
    {
        if (!_checkValidUser(i))
        {
            continue;
        }
        if (m_member[i].m_pUser == user)
        {
            m_member[i].m_pUser = 0;
            m_member[i].m_field8 = 0;
            user->SetPartyIndex(-1);
            list.push_back(user);
            return 0;
        }
    }
    return 1;
