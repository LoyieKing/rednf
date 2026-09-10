void village_attacked::CVillageMonsterMgr::SetRewardType(ENUM_VILLAGE_ATTACKED_REWARD reward)
{
    // P95：ORIG 先载 this 后载 reward；具名视图复现（m_field20 = +0x20）。
    struct CVMgrRewardView
    {
        char m_head[0x20];
        int  m_field20;
    };
    ((CVMgrRewardView*)this)->m_field20 = reward;
