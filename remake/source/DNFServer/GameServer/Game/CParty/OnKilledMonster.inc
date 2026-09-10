    // [G7-sub 2026-09-10 复核·帧差归属] ORIG 0x085b5a4c `sub $0x44` vs 我方 `sub $0x64`
    // （+0x20）。逐槽核对：差量全部来自 **.cpp 段（非本 .inc 写域）** 的具名量
    //   · `WongWork::CHackAnalyzer* hack`（ORIG 0x085b5b0a `call getHackAnalyzer` 后
    //     直用，无槽；我方落 -0x34）—— 应改链式 `…->getHackAnalyzer()->addServerHackCnt(…)`
    //   · `member/memberParty/userParty/mpDungeon/upDungeon`（ORIG 0x085b5c30 段各为
    //     表达式重读；我方 -0x28/-0x24/-0x20 三个槽）
    //   · `MapInfo* mapInfo`（ORIG 直用返回值）
    //   本 .inc 仅到 `IsCollectDungeonStatistic();`（ORIG 0x085b5a51 首条 call ✓ 一致），
    //   故本轮在此无可改项；上述具名量请由 CParty.cpp 属主按 P0/P183 处理。
    // p167：CALLSITE CLEAR（ORIG/我方各 34 call）。
void CParty::OnKilledMonster(CUser* user, int monsterIdx, map_monster monster,
                             MSG_MONSTER_DIE const& dieInfo)
{
    // b28d-49 判词：N 封顶 + 缺口。P167 MISS ActiveStaticInfo::getHpMax（1 次）。@0x85b5a4c
    IsCollectDungeonStatistic();
    // P183：ORIG 每个使用点各自 `mov 0x8(%ebp),%eax; add $0xb24,%eax` 重取址
    //（无引用缓存槽）；skip 布尔局部亦无槽 ⇒ 改短路表达式直用。
