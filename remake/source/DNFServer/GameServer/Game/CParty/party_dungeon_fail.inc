void CParty::party_dungeon_fail()
{
    // b28d-49 判词：N 封顶 + 缺口。P167 EXTRA new/delete 2 项（P90/P83 landing 差）。@0x85b3670
    if (!(m_tournamentVictory == 0 && check_allmember_die() && m_battleField.m_pDungeon != 0))
        return;

    // P183：ORIG 本函数对 bf/bd 每个使用点各自重取址（无引用缓存槽）⇒ 宏直用。
