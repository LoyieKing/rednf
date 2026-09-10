int Dispatcher_SeriaRoom_Ani_Deco_Event::check_error(CUser* user, MSG_BASE& msg)
{
    if (!GlobalData::s_event_manager->GetRepeatEvent(0x69)->IsEventing(0))
        return 1;
    return 0;
