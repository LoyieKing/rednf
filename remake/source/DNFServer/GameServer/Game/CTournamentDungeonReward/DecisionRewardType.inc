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
