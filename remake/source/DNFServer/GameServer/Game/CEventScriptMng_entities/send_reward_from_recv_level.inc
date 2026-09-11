// ORIG 0x810e51a：不缓存脚本/items/item/mail 局部——条件与循环每次内联
// 解引用 it 与 m_script1c；循环计数为 signed int（setg 比较）。
// [b28d-solo-CEventScriptMng_entities] AE（169/169，compare_tu 口径；报告工具
//   对 .o 未解析重定位不归一化，故其 verdict 显示 DIFF，属 P264 伪差）（ORIG 0x0810e51a sz 0x00000241）
int CLevelUpReward::send_reward_from_recv_level(CUser* user, int level,
// [P8S6 2026-09-11] 新口径复核 n1_v=AE（reloc 归一）；旧判词 NEAR 系 objdump 无 -r 的假残差
                                                int growType)
{
    int job = user->get_charac_job();
    int curGrowType = static_cast<int>(user->getCurCharFirstGrowType());

    std::map<int, Condition_LevelUp>::iterator it =
        static_cast<LevelUpRewardScript*>(m_script1c)->m_conditions.find(job);
    if (it != static_cast<LevelUpRewardScript*>(m_script1c)->m_conditions.end() &&
        !(it->second.m_filter1 != 0xff &&
          static_cast<int>(static_cast<signed char>(it->second.m_filter1)) !=
              curGrowType))
    {
        for (int i = 0; i < (int)it->second.m_items.size(); ++i)
        {
            if (growType < it->second.m_items[i].m_level &&
                it->second.m_items[i].m_level <= level)
            {
                static_cast<LevelUpRewardScript*>(m_script1c)->m_mail.reset();
                static_cast<LevelUpRewardScript*>(m_script1c)->m_mail.set_item(
                    it->second.m_items[i].m_itemIndex,
                    it->second.m_items[i].m_itemCount);
                static_cast<LevelUpRewardScript*>(m_script1c)->m_mail.m_field30 =
                    it->second.m_items[i].m_title;
                static_cast<LevelUpRewardScript*>(m_script1c)->m_mail.m_field34 =
                    it->second.m_items[i].m_content;
                do_action(user);
            }
        }
    }
    return 1;
}