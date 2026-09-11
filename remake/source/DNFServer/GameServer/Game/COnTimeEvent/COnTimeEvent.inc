// ORIG C1 @0x081bb826：基类 ctor → CRewardUserList ctor(+0x14) →
// +0x0c=0、+0x10=1 → clear()。
COnTimeEvent::COnTimeEvent()
{
    m_ontimeId = 0;
    m_rewardState = 1;
    m_rewardUsers.clear();
}