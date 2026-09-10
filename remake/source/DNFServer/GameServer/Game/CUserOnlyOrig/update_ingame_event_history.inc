void CUser::update_ingame_event_history(int eventKey)
{
    m_map8eb80[eventKey] = true;                         // map<int,bool>@+0x8eb80
