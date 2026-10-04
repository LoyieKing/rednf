short CUserCharacInfo::GetFatigueBatteryCharging()
{
    return m_selected ? m_selected->m_fatigueBattery : 0;
}