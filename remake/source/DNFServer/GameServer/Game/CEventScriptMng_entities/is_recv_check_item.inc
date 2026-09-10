bool CLevelUpReward::is_recv_check_item()   // [b28d-Main 代落·S366 P292] char→bool
{
    // ORIG 0x810e2f2：return m_flag08。
    return static_cast<LevelUpRewardScript*>(m_script1c)->m_flag08b;
