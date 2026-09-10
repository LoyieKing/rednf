void CUser::set_server_fatigue_count(short v)
{
    unsigned char todayDay =
        static_cast<EventClassify::CEventScriptMng*>(
            G_CDataManager()->get_event_script_mng())
            ->get_today_time_day();
    std::map<unsigned char, short>::iterator it = m_map8eb50.find(todayDay);
    if (it != m_map8eb50.end())
    {
        it->second = static_cast<short>(it->second + v);
    }
    else
    {
        // b28d-solo-CUser p167：ORIG 0x0868d32a 直接构造 value_type
        //   `pair<unsigned char const, short>::pair<unsigned char&, short const&>`
        //   （无 make_pair 调用）⇒ 改 value_type 直构（P123/P61）。
        m_map8eb50.insert(std::map<unsigned char, short>::value_type(todayDay, v));
    }
