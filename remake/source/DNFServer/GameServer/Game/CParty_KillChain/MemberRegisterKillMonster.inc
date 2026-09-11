// ============================================================================
// MemberRegisterKillMonster —— ORIG T 0x085bae6e。
// 组队首杀同步：roleType==3（Boss）/8（APC Boss）/命名怪三分支，各遍历成员
// 1..3（跳过队长），对每成员 CKillMonsterInfo 先 Check 后 Register。
// ============================================================================
// [b28d-solo-CParty_KillChain AE] ORIG 0x085bae6e：本波修法——①删 mobIdx 缓存槽（P0：
// ORIG 0x085baee0 每处重读 monster.m_mobIndex）；②中间局部改 user(-0x28)+kmi(-0x24) 双
// 具名槽（ORIG 0x085baec0/0x085baed6 实证；原 void* data 槽语义错位）；③GetCharacExpandData
// 返回 CExpandEquipslot*，用 reinterpret_cast。160=160 AE，P167 CLEAR。
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
}