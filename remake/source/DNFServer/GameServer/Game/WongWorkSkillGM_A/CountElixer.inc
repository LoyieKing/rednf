// ============================================================================
// ORIG T 0x08609d9c CountElixer(CUser*)：重置四组数据（8/组首）、置就绪标志，
// 统计用户物品并应用双树 kind 数据。
// ============================================================================
void CSkillChanger::CountElixer(CUser* user)
{
    memset(m_data.m_elixerCount, 0, 0x24);
    memset(m_data.m_skillBookCount, 0, 0x24);
    memset(m_data.m_needElixerCount, 0, 0x24);
    memset(m_data.m_needElixerAbility, 0, 0x24);
    m_state.m_sec0.m_kindState = 8;   // 组首 +0x00/0x24/0x48/0x6c（SkillB 将 CSkillChanger 4 节结构化为 STKindSection）
    m_state.m_sec1.m_kindState = 8;
    m_state.m_sec2.m_kindState = 8;
    m_state.m_sec3.m_kindState = 8;
    m_state.m_reserved91 = 1;                      // ORIG 0x08609e41 movb $0x1,0x91(%eax)
    _GetUserItemCount(user);
    _ApplyUserData(user, (ENUM_SKILL_TREE_KIND)0);
    _ApplyUserData(user, (ENUM_SKILL_TREE_KIND)1);
}