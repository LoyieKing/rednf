// ============================================================================
// 4. Dispatcher_EPLPCommand::excludeEplpState
//    报告 docs/class_func_reports/Dispatcher_EPLPCommand/excludeEplpState.md
// ============================================================================
bool Dispatcher_EPLPCommand::excludeEplpState(CUser* user)
{
    CParty* party = static_cast<CParty*>(user->GetParty());        // 0x0865514c
    if (party == 0)
        return false;

        // ORIG: TowerOfDespairMgr::IsTowerOfDespairDungeon_ByIndex(
    //           GlobalData::s_pTowerOfDespairMgr, bf(party).get_dungeon_index())
    // （TowerOfDespairMgr.cpp 按报告定义的 const 成员，0x08644272）
    if (static_cast<TowerOfDespairMgr*>(GlobalData::s_pTowerOfDespairMgr)
            ->IsTowerOfDespairDungeon_ByIndex(
                static_cast<int>(reinterpret_cast<CBattle_Field*>(
                    &party->m_battleField)->get_dungeon_index())))
        return false;

    if (user->getMoveSpace() == (UserSpace::T)1)   // 0x08690eca
        return false;

    // ORIG: GlobalScriptTutorial::IsGlobalTutorialDungeon(idx)（0x08ab976a，静态成员）
    if (GlobalScriptTutorial::IsGlobalTutorialDungeon(
            reinterpret_cast<CBattle_Field*>(&party->m_battleField)
                ->get_dungeon_index()))
        return false;
    return true;
}