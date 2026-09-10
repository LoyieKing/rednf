void CUser::set_growth_power_exp_reward(int level)
{
    CEventBase* ev = GlobalData::s_event_manager->GetRepeatEvent(0x5d);
    if (ev->IsEventing(0))
    {
        stSeriaBlessingScript::Value value;               // +0x15 清零 ctor
        // ORIG +0x6d..+0x8f：`&&` 短路物化岛（mov $1/jmp/mov $0/test;je，值无槽往返）
        // ⇒ P226 扩展「语句表达式包住整个条件」（具名 bool 反产槽化+物化劣化）。
        if (({ G_CDataManager()->m_seriaBlessing.get_value(level, value) &&
               !is_growth_power_exp_reward_right(); }))
        {
            // ORIG +0x93..+0xbd：字节 store 序 +2(m_fieldC)→+3(m_field10)→+1(m_field8)，
            // 目标 char 与 RHS int 异型 ⇒ RHS-first+edx 中转（P211 档③）；用 STExpReward
            // 的 union 字节视图具名访问（§4.3，禁裸偏移指针视图）。
            m_reward.m_field5 = static_cast<char>(value.m_fieldC);    // +0x8d24f
            m_reward.m_powerUp = static_cast<char>(value.m_field10);  // +0x8d250
            m_reward.m_field4 = static_cast<char>(value.m_field8);    // +0x8d24e
        }
        else
        {
            memset(&m_reward, 0, 4);                       // +0xc2 分支
        }
    }
    else                                                      // +0xbb5 分支
    {
        // P48 no-CSE：ORIG 每处 use 各自 fresh G_CDataManager()（+0xca/+0xdf/+0x135
        // 共 3 处，加 if 分支 m_seriaBlessing 1 处 = 4 次调用）。
        const int idx = G_CDataManager()->m_growthPower.get_idx_break_away_section(level); // +0xca
        const int sz = G_CDataManager()->m_growthPower.m_vecC.size();  // +0xdf
        if (idx >= 0 && idx < sz &&                            // +0xeb cmpl/js、+0xf3 cmpl/jge（有符号）
            !is_growth_power_exp_reward_right())             // +0xfb
        {
            m_reward.m_reward = G_CDataManager()->m_growthPower.m_vecC[idx].m_reward;  // +0x135
        }
        else
        {
            memset(&m_reward, 0, 4);                      // +0x16f 分支
        }
    }
