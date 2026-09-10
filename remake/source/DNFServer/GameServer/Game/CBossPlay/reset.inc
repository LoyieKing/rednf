void CBossPlay::reset()
{
    m_party = 0;
    m_field8 = 0;
    m_allMemberReady = 0;
    memset(m_memberAlive, 0, sizeof(m_memberAlive));
    memset(m_memberReady, 0, sizeof(m_memberReady));
    memset(m_rewardExp, 0, sizeof(m_rewardExp));
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 10; ++j)
        {
            m_memberReward[i].m_count = 0;
            m_memberReward[i].m_items[j].reset();
            // ORIG 0x08149202 `movl $0xffffffff,0xe(%eax)`（eax=block+j*0x3d+0x20，
            // 即 &m_items[j]+0x02）⇒ 整字写 Inven_Item::m_addInfo(item idx) = -1。
            m_memberReward[i].m_items[j].m_addInfo = -1;
        }
    }
    memset(m_useItemCount, 0, sizeof(m_useItemCount));
    memset(m_memberField, 0, sizeof(m_memberField));
