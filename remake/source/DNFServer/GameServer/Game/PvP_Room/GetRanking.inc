// [b28d-solo-PvP_Room] AE 归零（90=90，本轮）：ORIG 0x085dc5a3 内层循环只发
// 一条 `movl $0x0,-0x14` ⇒ 原码单处初始化；删 `int local_18 = 0;` 的声明
// 初始化（只留 for 初始化式）后 4.4 槽号集合与 ORIG 全等（旧 N95「槽基差
// 封顶」判词作废——真因是双初始化多 1 条 store 导致后续槽整体浅移 4）。
// 配套：临时 local_14 由函数头移入内层循环体（ORIG -0x10 深槽）。
int PvP_Room::GetRanking(int* out) const
{
    if ((m_pvpBattleMode == 4) || (m_pvpBattleMode == 5))
    {
        return m_deathMatch.getRanking(const_cast<CUser**>(m_users), out);
    }
    int local_24 = 0;
    int aiStack_44[8];
    for (int local_20 = 0; local_20 < 8; local_20 = local_20 + 1)
    {
        if (P_SEATS(local_20) != 0)
        {
            // ORIG 0x085dc58e：m_field78[seat]（PvP_Room 内嵌分数数组）
            int local_1c = m_field78[local_20];
            // [b28d-solo-PvP_Room] ORIG 0x085dc5a3 只发一条 `movl $0x0,-0x14`
            // ⇒ 原码单处初始化（for 初始化式），不写声明初始化（双写会多 1 条）。
            int local_18;
            // [b28d-solo-PvP_Room] ORIG 0x085dc5af 内层为独立 break 形
            // （local_10 先入 -0x10 槽，`cmp score; jl EXIT`），尾测 setle。
            for (local_18 = 0; local_18 < local_24; local_18 = local_18 + 1)
            {
                int local_14 = aiStack_44[local_18];
                if (local_14 < local_1c)
                    break;
            }
            for (int local_10 = local_24; local_18 < local_10; local_10 = local_10 - 1)
            {
                out[local_10] = out[local_10 - 1];
                aiStack_44[local_10] = aiStack_44[local_10 - 1];
            }
            out[local_18] = local_20;
            aiStack_44[local_18] = local_1c;
            local_24 = local_24 + 1;
        }
    }
    return local_24;
}