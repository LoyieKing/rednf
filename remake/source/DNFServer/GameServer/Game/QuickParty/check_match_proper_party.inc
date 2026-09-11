// ---- check_match_proper_party @0x08269e34：匹配前置校验，返回 ENUM_ERROR ----
// [b28d-L1] N82 封顶：ORIG 0x08269e34；P167 CLEAR（7/7 调用点差集归零）⇒ 语义完成，残余为 4.1/4.4 槽位与寄存器分配差（real diff 58 全为 disp/reg 形态）。
// [b28d-solo-QuickParty] verdict=AE（73=73）：修法=P0/P35 删 dungeonIdx 具名 short 局部
// （三处重读 key.m_dungeonIndex：test %ax / cwtl / movswl %ax,%ebx）+ P226 匿名语句
// 表达式复现 bloodPass 零槽物化（旧 N82 判词作废）。P167 CLEAR（7/7）。
ENUM_ERROR CQuickPartySystemManager::check_match_proper_party(
    const STQuickPartyPoolMap_Key& key, CParty* party)
{
    if (party == 0)
        return static_cast<ENUM_ERROR>(1);
    int members = party->get_member_count();
    if (members <= 0 || members > 3)
        return static_cast<ENUM_ERROR>(1);

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
        return static_cast<ENUM_ERROR>(1);

    // 血色副本按人数上限直接放行；否则走常规出入副本条件检查。
    // [b28d-solo-QuickParty] 修法（P226 语句表达式）：ORIG 0x08269ee6 的 bloodPass 物化在
    // %al（`mov $1/jmp`、`mov $0`、`test %al,%al`），无栈槽；具名 bool 局部使 4.4 落 -0x9
    // 并多发 `mov %al,-0x9; cmpb $0,-0x9`。
    if (({
            static_cast<int>(
                static_cast<signed char>(dungeon->get_blood_dungeon_type())) == 2 &&
            static_cast<int>(dungeon->get_limit_party_count()) <= members;
        }))
        return static_cast<ENUM_ERROR>(1);
    return static_cast<ENUM_ERROR>(party->checkInoutConditionDungeon(
                                             dungeon,
                                             static_cast<ENUM_DUNGEON_MODE>(0),
                                             static_cast<ENUM_CMDPACKET>(0x1bc)));
}