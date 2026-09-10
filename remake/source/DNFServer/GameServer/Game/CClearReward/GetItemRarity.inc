int CLuckPoint::GetItemRarity(int* table, int roll, int dropType)
{
    int acc[6] = {0};

    // [b28d-solo-CClearReward] ORIG 0x08550c0e 两守卫**直跳**（`jne` + `call empty;
    // xor $1; test; je`，无 && 整体物化）⇒ 拆嵌套 if（P225 推广反向；`&&` 单表达式
    // 在 4.4 会多一段 mov$1/jmp/mov$0 常量岛）。luck 无槽（ORIG `mov %eax,%ebx` 内联
    // 实参）、sp 无槽（`lea 0x68(%eax),%edx` 内联）⇒ 均按 P0 内联。
    if (dropType == 2)
    {
        if (!m_users.empty())
        {
        int lp = 0;
        G_CDataManager()->m_serverParameter.getLuckPoint(GetLuckPoint(), lp);
        // [b28e-solo-CClearReward] ORIG 0x08550c73：`table[2]-table[1]` 结果落具名槽
        // -0x24 后按槽读取传参（P0 槽往返律）⇒ 原码有具名 diff 局部。
        int diff = table[2] - table[1];
        int neg = -GetTenThousandPercentage(diff, lp);
        int unique = 0;
        int rare = 0;
        for (size_t i = 0; i < m_users.size(); ++i)
        {
            if (m_users[i] != 0)
            {
                CParty* party = reinterpret_cast<CParty*>(m_users[i]->GetParty());
                if (party != 0)
                {
                    unique = party->Get_party_overlapped_drop_ratio_unique();
                    rare = party->Get_party_overlapped_drop_ratio_rare();
                }
                // [b28e-solo-CClearReward] 真 bug 修正：ORIG 0x08550ce0/0x08550d02
                // party==0 与 party!=0 两路都汇到 0x08550d20（出循环）⇒ break 在
                // if(m_users[i]!=0) 块内、if(party!=0) 块**外**（Ghidra C 同证）。
                break;
            }
        }
        if (unique != 0)
        {
            // ORIG 0x08550dxx：`fildl A; fildl B; flds 0.01f; fmulp; fmulp`
            // ⇒ A*(B*0.01f) 右结合（P243 书写序）。
            // [b28e-solo-CClearReward] 真 bug 修正：ORIG 0x08550d26/0x08550d71/0x08550d83
            // 读-写-清零均为 -0x34 = acc[3]（acc 基址 -0x40）⇒ 原码是 acc[3] 非 acc[2]。
            // 另：ORIG 无具名 v 局部（fistpl 直落 -0x4c 临时槽后立即读），故 ROUND 结果
            // 就地内联进 `-=` 右值（具名 v 会多占两槽使全帧 -8 漂移、200→197）。
            acc[3] -= static_cast<int>(
                static_cast<float>(table[3] - table[2]) *
                           (static_cast<float>(unique) * 0.01f));
            if (table[3] > 999999)
                acc[3] = 0;
        }
        if (rare != 0)
        {
            neg -= static_cast<int>(
                static_cast<float>(table[2] - table[1]) *
                           (static_cast<float>(rare) * 0.01f));
        }
        if (table[1] != 0)
            acc[0] = table[0] * neg / table[1];
        acc[1] = neg;
        }
    }
    int ret = 0;
    for (int i = 0; i <= 5; ++i)
    {
        if (roll <= table[i] + acc[i])
        {
            ret = i;
            break;
        }
    }
    return ret;
