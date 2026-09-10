int Dispatcher_CompleteLoadAssault::dispatch_sig(CUser* user, PacketBuf& packet)
{
    (void)packet;
    if (user->get_state() <= 1)
    {
        LineFunc(0xa199,
                 __PRETTY_FUNCTION__,
                 0, 0);
        return 0;
    }
    PvP_Room* room = (PvP_Room*)user->GetPVPRoom();
    if (room != 0 && room->m_matching != 0)
    {
        IMatch* matching = (IMatch*)room->m_matching;
        matching->OnCompleteLoad(user);
        return 0;
    }
    pvp_assault::CAssaultMgr* mgr = pvp_assault::GetInstanceAssaultMgr();
    if (!mgr->OnCompleteLoad(user))
    {
        LineFunc(0xa1a8,
                 __PRETTY_FUNCTION__,
                 0, 0);
    }
    return 0;
