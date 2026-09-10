int BlueMarble::getRandomDungeon(int seat, BlueMarbleTileScript::Type type) const
{
    std::vector<int> local;
    int level = m_users[seat].getUser()->get_charac_level();
    // ORIG 0x080d741d-0x080d748a：getBlueMarbleMap()+getMap 链在判空与
    // getTile 两处**各取一次**（无 map 局部缓存，P160/P172）；判空为
    // 4.1 sete 双物化（test;je 直连不可复现，残差记录）。
    if (m_pScript->getMap(getBlueMarbleMap()) == 0)
    {
        return 0;
    }
    BlueMarbleTileScript* tile = m_pScript->getMap(getBlueMarbleMap())->getTile(
        (unsigned int)m_users[seat].getBoardZone());
    if (tile == 0)
    {
        return 0;
    }
    // ORIG 0x080d74a5 `movzbl 0x4(%eax),%eax; xor $0x1,%eax; test %al,%al; je`
    // ⇒ m_char4 已 bool 化（BlueMarbleTileScript.h）+ 裸 `if(!field)`（P292/P300）。
    if (!tile->m_char4)
    {
        return 0;
    }
    // ORIG 0x080d74ba-：type 三分支各自 sret 临时槽（-0x38/-0x2c/-0x20）
    // 直写 `local = getXXXIndex(level)`（每分支一对 op=+D1），无跨分支
    // 共享 tmp——旧 tmp 块形与 move 中转已废。
    // ORIG 0x080d74ba `mov 0x10(%ebp),%eax` 单次载入 + cmp$7/cmp$8/cmp$2 链
    // ⇒ switch（P251/P261）；Type 是窄枚举（仅 Type_0）故必须 (int) 化（P305）。
    switch ((int)type)
    {
    case 7:
        local = m_pScript->m_info.getUniqueDungeonIndex(level);
        break;
    case 8:
    case 2:
        local = m_pScript->m_info.getBossDungeonIndex(level);
        break;
    default:
        local = m_pScript->m_info.getRandomDungeonIndex(level);
        break;
    }
    if (local.size() == 0)
    {
        return 0;
    }
    // ORIG 0x080d7611 `mov %eax,%ebx`（randInt 结果直入 callee-saved、无 r 槽，
    // P0：原码无具名 r 局部）⇒ 调用内联进取模表达式。
    unsigned int idx =
        (unsigned int)m_pRands[2].randInt() % (unsigned int)local.size();
    if (idx >= local.size())
    {
        return 0;
    }
    return local[idx];
