// ============================================================================
// addDungeonClear —— ORIG T 0x085bddfc。
// 按 param(slot) 成员的 uniqueId 匹配记录槽（this+0x1808+i*8：short uniqueId /
// int 计数）；kind==1 上限 30、kind==2 上限 50、kind==0 上限 10（param 与
// dm+0x512e/0x512c 当日门槛双判定），且成员 m_field14(+0x8c)==0 时
// AddDungeonClear(m_battleField.m_dungeonIndex=CParty+0xcb8, 3)。其余 kind
// 或未命中返回 false。
// ============================================================================
// [b28d-solo-CParty_KillChain 修法+P167] ORIG 0x085bddfc：本波修法——①kind 具名槽
// 删除、改 switch(getDungeonTypeKind(dungeon))（P251：ORIG 0x085bde5f 无槽 cmp 链）；
// ②记录槽/门槛改具名视图（KcClearRecView/KcDataMgrDailyLimitView）消 char* 的 shl+add
// 与 add $0x512e；③比较序改 get_unique_id 在左（ORIG 0x085bde2d call 在前）；
// ④三条件改 P226 语句表达式复现 mov $1/jmp/mov $0/test 物化岛。
// 残余（N 封顶）：①ORIG 0x085bde2d 后 4.4 多一条 `mov %eax,%edx` 保存 uid（4.4 恒用 %eax
// 作记录基址、uid 被挤到 %edx；两操作数换位则记录读提前到 call 前且落 %ebx，180=180 仍
// DIFF）⇒ N18a；②三处 `mov 0x8(%ebp),%eax` 先于 `mov -0xc(%ebp),%edx`（ORIG 相反）⇒
// N7/P75 装载序。P167 CLEAR（8=8）。
bool CParty::addDungeonClear(const CDungeon* dungeon, int param, int slot)
{
    for (int i = 0; i <= 3; ++i)
    {
        // ORIG 0x085bde2d 先 call get_unique_id、0x085bde3a 后读记录（cmp %dx,%ax）
        // ⇒ 左操作数写 get_unique_id（求值序对齐；记录在左会把记录读提前到 call 前）。
        if (static_cast<short>(m_member[slot].m_pUser->get_unique_id()) !=
            m_gameResultSet.m_shot[i].m_count)
            continue;

        // P251：ORIG 0x085bde5f `cmp $1,%eax; je; cmp $2,%eax; je;
        // test %eax,%eax; jne`（kind 无槽往返）⇒ 源形 switch(call())。
        switch (getDungeonTypeKind(dungeon))
        {
        case 0:
            // P226 扩展：ORIG 0x085bdebf `mov $1/jmp; mov $0; test; je` 短路
            // 物化岛 ⇒ 语句表达式包整个条件（内不含具名变量）。
            if (({ (param >= static_cast<int>(
                            G_CDataManager()->m_rankSystem.m_dungeonRank.m_rankLevel[0]) &&
                        m_gameResultSet.m_shot[i].m_field4 <= 10 &&
                        m_member[slot].m_field14 == 0); }))
            {
                return m_member[slot].m_pUser->AddDungeonClear(
                    m_battleField.m_dungeonIndex, 3);
            }
            continue;
        case 1:
            if (({ (param >= static_cast<int>(
                            G_CDataManager()->m_rankSystem.m_dungeonRank.m_rankLevel[1]) &&
                        m_gameResultSet.m_shot[i].m_field4 <= 0x1e &&
                        m_member[slot].m_field14 == 0); }))
            {
                return m_member[slot].m_pUser->AddDungeonClear(
                    m_battleField.m_dungeonIndex, 3);
            }
            continue;
        case 2:
            if (({ (param >= static_cast<int>(
                            G_CDataManager()->m_rankSystem.m_dungeonRank.m_rankLevel[1]) &&
                        m_gameResultSet.m_shot[i].m_field4 <= 0x32 &&
                        m_member[slot].m_field14 == 0); }))
            {
                return m_member[slot].m_pUser->AddDungeonClear(
                    m_battleField.m_dungeonIndex, 3);
            }
            continue;
        default:
            return false;
        }
    }
    return false;
}