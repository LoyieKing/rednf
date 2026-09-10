bool CNPCScript::isRewardLevel(unsigned short level) const
{
    // [b28d-solo-NPCScript AE] ORIG 0x08580fd0：`call deEv; mov %eax,-0xc(%ebp);
    // cmpl $0,-0xc; je <增量>; … cmp -0x1c(%ebp),%ax; jne <增量>` ⇒ 原码为
    // 具名指针局部 + 短路 `p != 0 && p->m_level == level`（P0 槽往返；
    // 两跳同落增量块 = 短路物化）。P28 增量侧：ORIG ppEi ⇒ 后置 `it++`。
    // 修正后与 ORIG 逐条全等（AE；p167 CLEAR 5=5）。
    for (std::vector<stFavorableRelationShip_t::stLevelRewardInfo>::const_iterator it =
             m_npcScript.m_favorableRelationShip.m_levelRewards.begin();
         it != m_npcScript.m_favorableRelationShip.m_levelRewards.end(); it++)
    {
        const stFavorableRelationShip_t::stLevelRewardInfo* p = &(*it);
        if (p != 0 && p->m_level == level)
        {
            return true;
        }
    }
    return false;
