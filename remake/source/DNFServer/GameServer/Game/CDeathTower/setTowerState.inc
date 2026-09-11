int CDeathTower::setTowerState(ENUM_TOWER_STATE state)
{
    // ORIG 0x8469c34：state <= m_towerState 时返回 0；否则写入并返回 1。
    if (state <= (ENUM_TOWER_STATE)m_towerState)
    {
        return 0;
    }
    m_towerState = state;  // ORIG 0x8469c4b：同型直落 → 先载 this(%eax) 再载 state(%edx)
    return 1;
}