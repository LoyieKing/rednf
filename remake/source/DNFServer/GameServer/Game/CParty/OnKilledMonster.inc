void CParty::OnKilledMonster(CUser* user, int monsterIdx, map_monster monster,
                             MSG_MONSTER_DIE const& dieInfo)
{
    // b28d-49 判词：N 封顶 + 缺口。P167 MISS ActiveStaticInfo::getHpMax（1 次）。@0x85b5a4c
    IsCollectDungeonStatistic();
    // P183：ORIG 每个使用点各自 `mov 0x8(%ebp),%eax; add $0xb24,%eax` 重取址
    //（无引用缓存槽）；skip 布尔局部亦无槽 ⇒ 改短路表达式直用。
