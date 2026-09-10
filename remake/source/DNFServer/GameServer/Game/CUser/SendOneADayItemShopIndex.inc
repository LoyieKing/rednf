void CUser::SendOneADayItemShopIndex(
    const std::vector<std::pair<int, int> >& list)
{
    // ORIG 0x868aaf2：GetRepeatEvent(0x6d) → vtable+0x34(0)；返回真才发包
    CEventBase* event = GlobalData::s_event_manager->GetRepeatEvent(0x6d);
    if (event == 0)
        return;
    if (!event->IsEventing(0))
        return;

    PacketGuard packet;                                          // ORIG local_20
    InterfacePacketBuf& pkt = *(InterfacePacketBuf*)&packet;
    pkt.put_header(0, 0x155);
    int count = (int)list.size();
    pkt.put_byte(count);
    for (int i = 0; i < count; ++i)
    {
        pkt.put_int(list[i].first);                              // ORIG operator[] → *piVar2
        pkt.put_int(list[i].second);                             // ORIG put_int(second)
    }
    pkt.finalize(true);
    Send(packet);
