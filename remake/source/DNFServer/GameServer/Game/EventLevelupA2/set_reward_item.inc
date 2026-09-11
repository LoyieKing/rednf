// ============ set_reward_item（报告转写） ============
// 查找/过滤与 check_recv_level 同形；命中 item.m_level == level 时对脚本
// +0x28 的 mail 执行 reset → set_item(itemIndex,itemCount) → 标题/正文 key
// 赋值（mail+0x30/+0x34 ← item+0xc/+0x10），返回 1；否则继续循环，耗尽返回 0。
bool CLevelUpReward::set_reward_item(int level, int job, int growType)   // [b28d-Main 代落·S366 P292] int→bool
{
    std::map<int, Condition_LevelUp>::iterator it =
        static_cast<LevelUpRewardScript*>(m_script1c)->m_conditions.find(job);
    if (it != static_cast<LevelUpRewardScript*>(m_script1c)->m_conditions.end())
    {
        if (it->second.m_filter1 == static_cast<unsigned char>(0xff)
            || static_cast<int>(static_cast<signed char>(it->second.m_filter1))
               == growType)
        {
            for (int i = 0; static_cast<int>(it->second.m_items.size()) > i; ++i)
            {
                if (it->second.m_items[i].m_level == level)       // sete 分支
                {
                    // ORIG 对 mail 的每步操作都重读 this->+0x1c 并加 +0x28。
                    static_cast<LevelUpRewardScript*>(m_script1c)->m_mail.reset();
                    static_cast<LevelUpRewardScript*>(m_script1c)->m_mail.set_item(
                        it->second.m_items[i].m_itemIndex,
                        it->second.m_items[i].m_itemCount);
                    static_cast<LevelUpRewardScript*>(m_script1c)->m_mail.m_field30 =
                        it->second.m_items[i].m_title;
                    static_cast<LevelUpRewardScript*>(m_script1c)->m_mail.m_field34 =
                        it->second.m_items[i].m_content;
                    return 1;
                }
            }
        }
    }
    return 0;
}