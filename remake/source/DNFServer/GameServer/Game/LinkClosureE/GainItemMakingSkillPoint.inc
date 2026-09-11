// ORIG T _ZN16CItemMakingSkill24GainItemMakingSkillPointEPK14CStackableItemP5CUser
// @0x085453e0。报告 GainItemMakingSkillPoint.md：随机 gained = rand % m_bonusExp
// + m_exp（0x28/0x24）；等级/点数上限违规时 errCode/gained 清零（0xd/0x16/0x5f）；
// UpSkillLevel 后 gained=新增量；包 0x15f 应答 + ItemMakingSkillAdd 日志。
// [b28d-solo-LinkClosureE] AE 239/239（p167 CLEAR 28/28）：等级分支/循环改
//   rbegin()->first + `cur>=first` 正极性并拆嵌套 if（P225 推广，复现 setge+je 双物化）；
//   levelLimit 比较改 `skillLevel-needLevel >= m_levelLimit`（消 %ebx 中转）；
//   日志第 4 参改回 `(short)m_skillLevels[type]`（ORIG 重读成员）；
//   删发明 helper `userHistoryLogOf`（N114），改 `user->m_historyLog.ItemMakingSkillAdd`。
void CItemMakingSkill::GainItemMakingSkillPoint(const CStackableItem* item, CUser* user)
{
    const ENUM_ITEM_MAKING_TYPE type =
        static_cast<ENUM_ITEM_MAKING_TYPE>(item->GetUpSkillType());
    const int needLevel = item->GetNeedLevel();
    const stItemMakingSkill* table =
        reinterpret_cast<stItemMakingSkill*>(G_CDataManager()->GetItemMakingSkill());
    const int skillLevel = GetItemMakingSkillLevel(item, user);
    int gained = static_cast<unsigned int>(m_rand->randInt())
                     % static_cast<unsigned int>(table->m_bonusExp)
                 + table->m_exp;
    int errCode = 0;
    if (skillLevel - needLevel >= table->m_levelLimit)
    {
        errCode = 0xd;
        gained = 0;
    }
    if (table->m_vec18.rbegin() == table->m_vec18.rend())   // ORIG rbegin==rend 空判定（+0xe4 跳出）
        return;
    if (user->get_charac_level() == 0x46)
    {
        if (static_cast<short>(m_skillLevels[type])
            >= table->m_vec18.rbegin()->first)
        {
            errCode = 0x16;
            gained = 0;
        }
    }
    else
    {
        for (std::vector<std::pair<int, int> >::size_type i = 0;
             i < table->m_vec18.size(); ++i)
        {
            if (user->get_charac_level() < table->m_vec18[i].second)
            {
                if (static_cast<short>(m_skillLevels[type])
                    >= static_cast<int>(table->m_vec18[i].first))
                {
                    errCode = 0x16;
                    gained = 0;
                    break;
                }
            }
        }
    }
    if (0x3e7 < static_cast<short>(m_skillLevels[type]))
    {
        errCode = 0x5f;
        gained = 0;
    }
    const short before = static_cast<short>(m_skillLevels[type]);
    UpSkillLevel(type, gained, user);
    gained = static_cast<short>(m_skillLevels[type]) - before;
    PacketGuard packet;
    packet.put_header(0, 0x15f);
    packet.put_byte(errCode);
    packet.put_byte(static_cast<int>(type));
    packet.put_short(gained);
    packet.finalize(true);
    user->Send(packet);
    user->m_historyLog.ItemMakingSkillAdd(
        type, errCode, gained, static_cast<short>(m_skillLevels[type]));
}