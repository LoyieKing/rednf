void CParty::MemberRegisterKillMonster(map_monster& monster, unsigned char flag)
{
    if (monster.m_roleType == 3)
    {
        for (int i = 1; i <= 3; ++i)
        {
            if (!checkValidUser(i))
                continue;
            // P0：ORIG 0x085baec0 有 user(-0x28)/kmi(-0x24) 两具名槽，且
            // m_mobIndex 每处重读（0x085baee0，无缓存局部）。
            CUser* user = get_user(i);
            CKillMonsterInfo* kmi = reinterpret_cast<CKillMonsterInfo*>(
                user->GetCharacExpandData(
                    static_cast<ENUM_CHARAC_EXPAND_TYPE>(0xc)));
            if (!kmi->CheckBossKill(
                    static_cast<unsigned int>(monster.m_mobIndex), flag))
                kmi->RegisterBossKill(
                    static_cast<unsigned int>(monster.m_mobIndex), flag);
        }
    }
    else if (monster.m_roleType == 8)
    {
        for (int i = 1; i <= 3; ++i)
        {
            if (!checkValidUser(i))
                continue;
            CUser* user = get_user(i);
            CKillMonsterInfo* kmi = reinterpret_cast<CKillMonsterInfo*>(
                user->GetCharacExpandData(
                    static_cast<ENUM_CHARAC_EXPAND_TYPE>(0xc)));
            if (!kmi->CheckAPCBossKill(
                    static_cast<unsigned int>(monster.m_mobIndex), flag))
                kmi->RegisterAPCBossKill(
                    static_cast<unsigned int>(monster.m_mobIndex), flag);
        }
    }
    else if (monster.isNamedMonster())
    {
        for (int i = 1; i <= 3; ++i)
        {
            if (!checkValidUser(i))
                continue;
            CUser* user = get_user(i);
            CKillMonsterInfo* kmi = reinterpret_cast<CKillMonsterInfo*>(
                user->GetCharacExpandData(
                    static_cast<ENUM_CHARAC_EXPAND_TYPE>(0xc)));
            if (!kmi->CheckNamedKill(
                    static_cast<unsigned int>(monster.m_mobIndex), flag))
                kmi->RegisterNamedKill(
                    static_cast<unsigned int>(monster.m_mobIndex), flag);
        }
    }
