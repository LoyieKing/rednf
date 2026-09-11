// ============================================================================
// 5. Dispatcher_EPLPCommand::isReTurnToVillage
//    报告 docs/class_func_reports/Dispatcher_EPLPCommand/isReTurnToVillage.md
// ============================================================================
bool Dispatcher_EPLPCommand::isReTurnToVillage(CUser* user)
{
    if (user == 0)
        return false;
    CParty* party = static_cast<CParty*>(user->GetParty());
    if (party == 0)
        return false;
    if (user->getMoveSpace() == (UserSpace::T)1)
        return true;
    // [b28c-30] ORIG 0x0825f7f4 段：mov -0xc(%ebp),%eax; add $0xb24,%eax;
    // mov %eax,(%esp); call CBattle_Field::get_dungeon_index ⇒ 直成员调用形
    // （原 A2_dungeonIndexOf 局部 helper 生 call _ZL17A2_dungeonIndexOfP6CParty）。
    if (GlobalScriptTutorial::IsGlobalTutorialDungeon(
            reinterpret_cast<CBattle_Field*>(&party->m_battleField)
                ->get_dungeon_index()))
        return true;
    return false;
}