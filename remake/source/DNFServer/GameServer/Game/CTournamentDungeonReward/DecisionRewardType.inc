// ============================================================================
// ORIG 0x8284a24：两张卡槽各按 rate 掷点决定奖励（未过首档则 m_id=0）。
// [b28d-N82] fresh 复验 118/130：残差 = 4.4 多 push %edi + 帧 0x30→0x3c
// （callee-saved 需求/帧策略差）+ 二维索引寄存器（N18a）⇒ 语义等价封顶。
// [b28d-solo-CTournamentDungeonReward] DIFF 118/121：P272 德摩根块序（else 体直落、
// 双跳同落 then）+ `pick+1` 初始化物化 + operator[] 引用缓存（ORIG 槽 -0xc）三项修后
// 130→121；残 = 寄存器角色互换（card↔slotIdx，N7/N18a 族）；p167 4=4 CLEAR。
// [SweepCTournament 2026-09-09] 复核 118/121：残 = N18a/N7 索引中转×3；p167 4=4
// CLEAR；jcc（je/jl/jg/seta/setle）与 ORIG 逐位全等 ⇒ N 封顶。
// ============================================================================
void CTournamentDungeonReward::DecisionRewardType(
    ENUM_TOURNAMENT_REWARD_CARD_TYPE card, TournamentRewardRate& rate,
    const TournamentRewardItemRate* itemRate)
{
    for (int slotIdx = 0; slotIdx < 2; ++slotIdx)
    {
        int roll = get_rand_int(rate.m_c + 1);
        // ORIG 双跳同落 then（je/jl 0x8284a92）、else 体直落（P272 德摩根形）。
        if (rate.m_0 != 0 && rate.m_0 >= roll)
        {
            m_rewards[(int)card][slotIdx].m_id = 0;
        }
        else if (roll <= rate.m_0 + rate.m_4)
        {
            // ORIG `add $1,%eax; mov %eax,-0x18` ⇒ pick 在初始化处 +1 物化
            int pick = get_rand_int(itemRate->m_c) + 1;
            int accum = 0;
            for (size_t i = 0; i < itemRate->m_items.size(); ++i)
            {
                // ORIG 缓存 operator[] 返回的引用（槽 -0xc），两处赋值复用
                const TournamentRewardItem& item = itemRate->m_items[i];
                accum += item.m_data[1];
                if (pick <= accum)
                {
                    m_rewards[(int)card][slotIdx].m_id = item.m_data[0];
                    m_rewards[(int)card][slotIdx].m_field04 = item.m_data[2];
                    break;
                }
            }
        }
    }
}