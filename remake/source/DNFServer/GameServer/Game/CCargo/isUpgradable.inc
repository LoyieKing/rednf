bool CCargo::isUpgradable(int capacity) const   // b28d-Main：与 CUserCharacInfo.h:266 同步（P292 bool callee）
{
    if (m_capacity == 0 || m_capacity > capacity)
        return 0;
    if (m_capacity >= capacity) return 0;
    if (capacity > 0x98) return 0;
    return 1;
