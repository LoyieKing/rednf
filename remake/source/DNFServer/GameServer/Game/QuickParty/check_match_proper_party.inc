int CQuickPartySystemManager::check_match_proper_party(
    const STQuickPartyPoolMap_Key& key, CParty* party)
{
    if (party == 0)
        return 1;
    int members = party->get_member_count();
    if (members <= 0 || members > 3)
        return 1;

    // [b28d-solo-QuickParty] 修法（P0/P35）：ORIG 0x08269e6f 无 dungeonIdx 具名槽——
    // 每处 `movzwl (%eax),%eax` 重读 key.m_dungeonIndex（条件 test %ax、日志参 cwtl、
    // find_dungeon 实参 movswl %ax,%ebx）；具名 short 局部多 `mov %ax,-0x12` 槽往返。
    if (key.m_dungeonIndex < 0)
    {
        LogManager::logFormat(
            1, "QuickParty.cpp",
            __PRETTY_FUNCTION__,
            0x16e, "QUICK_PARTY_LOG : DUNGEON INDEX ERROR!! (%d)",
            static_cast<int>(key.m_dungeonIndex));
    }

    CDungeon* dungeon = G_CDataManager()->find_dungeon(key.m_dungeonIndex);
    if (dungeon == 0)
        return 1;

    // 血色副本按人数上限直接放行；否则走常规出入副本条件检查。
    // [b28d-solo-QuickParty] 修法（P226 语句表达式）：ORIG 0x08269ee6 的 bloodPass 物化在
    // %al（`mov $1/jmp`、`mov $0`、`test %al,%al`），无栈槽；具名 bool 局部使 4.4 落 -0x9
    // 并多发 `mov %al,-0x9; cmpb $0,-0x9`。
    if (({
            static_cast<int>(
                static_cast<signed char>(dungeon->get_blood_dungeon_type())) == 2 &&
            static_cast<int>(dungeon->get_limit_party_count()) <= members;
        }))
        return 1;
    return party->checkInoutConditionDungeon(dungeon,
                                             static_cast<ENUM_DUNGEON_MODE>(0),
                                             static_cast<ENUM_CMDPACKET>(0x1bc));
