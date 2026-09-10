void CParty::party_dungeon_fail()
{
    // b28d-49 判词：N 封顶 + 缺口。P167 EXTRA new/delete 2 项（P90/P83 landing 差）。@0x85b3670
    // [G7-sub] 语义等价形修正：ORIG 三条**独立早退**，各自跳不同尾标
    // （0x085b3685 m_tournamentVictory!=0 → +0x611；0x085b369b !check_allmember_die()
    // → +0x614；0x085b36ac m_pDungeon==0 → +0x617），无 bool 物化；我方原写复合
    // `!(a && b && c)` 被 4.4 折成三段 `mov $1/jmp/mov $0/test` 物化（+9 条来源）
    // ⇒ 拆回三条独立 if-return。
    if (m_tournamentVictory != 0)
        return;
    if (!check_allmember_die())
        return;
    if (m_battleField.m_pDungeon == 0)
        return;

    // P183：ORIG 本函数对 bf/bd 每个使用点各自重取址（无引用缓存槽）⇒ 宏直用。
