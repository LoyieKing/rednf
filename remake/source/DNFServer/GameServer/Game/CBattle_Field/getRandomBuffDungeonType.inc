// ============================================================================
// CBattle_Field::getRandomBuffDungeonType —— ORIG 0x0830aaa4（0x1d5 字节）
//   报告：docs/class_func_reports/CBattle_Field/getRandomBuffDungeonType.md
// ============================================================================
// [b28d-solo-CBattle_Field] 已修 AE（138=138，call 目标/槽号/分支全等）：
// ①P48/P13 两次独立 G_CGameManager 链调用 + end 具名缓存；②const 引用视图
// 取 const begin/end（P96/P270，消转换构造）；③P0/P48 两次独立
// get_blood_dungeon_type（去具名 int blood）；④(signed char) cast 复现
// `setg`（ORIG 0x0830aaa4）。
int CBattle_Field::getRandomBuffDungeonType(int dungeonIdx)
{
    if (dungeonIdx == 0)
    {
        if (m_pDungeon == 0)
            return 7;
        dungeonIdx = m_pDungeon->get_index();
    }

    CDungeon* dungeon = G_CDataManager()->find_dungeon(dungeonIdx);
    if (dungeon == 0)
        return 7;

    // quick party 奖励副本表命中 → 类型 7
    // [b28d-solo-CBattle_Field] P48/P13：ORIG 0x0830ab02/0x0830ab29 两次独立
    // G_CGameManager 链调用（无 CSE），end 一次求值后具名缓存（循环条件只调
    // operator!=）；旧引用缓存 idxList 多一条槽 store 并使帧 0x38→0x28。
    // P96/P270：ORIG 调 `_ZNKSt6vectorIiSaIiEE5beginEv/endEv`（const 版）且
    // sret 直落 it/end 槽 ⇒ 对 const 引用视图取 begin/end（const_iterator 同型，
    // 无 __normal_iterator 转换 C1；非 const 版 begin 会多 2 次转换构造）。
    std::vector<int>::const_iterator it =
        ((const std::vector<int>&)
            G_CGameManager()->GetQuickPartyRewardManager()->get_dungeon_idx_list()).begin();
    std::vector<int>::const_iterator end =
        ((const std::vector<int>&)
            G_CGameManager()->GetQuickPartyRewardManager()->get_dungeon_idx_list()).end();
    for (; it != end; ++it)
    {
        if (*it == dungeonIdx)
            return 7;
    }

    // [b28d-solo-CBattle_Field] ORIG 0x0830ab9a `test %al,%al; setg %al`（有符号
    // 判 >0）⇒ 源码须写 (signed char) cast（探针 /tmp/s18/p3.cpp t4 逐条命中；
    // 裸 unsigned char > 0 在 4.4 恒折 setne）。
    if ((signed char)dungeon->get_dimension_possible() > 0)
        return 2;
    if (dungeon->m_89c != 0)                       // +0x89c 直读（ORIG movzbl 0x89c）
        return 1;

    // [b28d-solo-CBattle_Field] P0/P48：ORIG 0x0830abd0/0x0830abee 两次独立调用
    // get_blood_dungeon_type（无具名 int 槽，直接 cmp $1/$2,%al）。
    if (dungeon->get_blood_dungeon_type() == 1)
        return 4;
    if (dungeon->get_blood_dungeon_type() == 2)
        return 5;

    // kind==1 且塔内堆叠物品限制非 0 → 3；kind==1 且限制==0 → 6；否则 0。
    // （ORIG 分支顺序如此：!=0 → 3 先判，再判 !1 → 6。）
    if (dungeon->getDungeonKind() == 1 && dungeon->limitOfStackableItemInTower() != 0)
        return 3;
    if (dungeon->getDungeonKind() == 1 && dungeon->limitOfStackableItemInTower() != 1)
        return 6;
    return 0;
}