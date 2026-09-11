short CUserCharacInfo::GetFatigueBatteryCharging()
{
    return m_selected ? CUR->m_fatigueBattery : 0;
}