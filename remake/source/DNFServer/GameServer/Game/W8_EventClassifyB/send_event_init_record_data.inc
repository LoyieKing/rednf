void CEventScriptMng::send_event_init_record_data(CUser* user)
{
    if (user == 0)
        return;

    const std::map<int, std::pair<int, int> >* history =
        static_cast<const std::map<int, std::pair<int, int> >*>(
            user->get_ingame_event_history());

    PacketGuard packet;
    packet.put_header(0, 0x177);
    packet.put_byte((int)(signed char)history->size());
    std::map<int, std::pair<int, int> >::const_iterator it =
        history->begin();
    std::map<int, std::pair<int, int> >::const_iterator end =
        history->end();
    while (it != end)
    {
        packet.put_short((int)(short)it->first);
        packet.put_int(it->second.first);
        ++it;
    }
    packet.finalize(true);
    user->Send(packet);
