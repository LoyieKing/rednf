void CMissionList_Charac::Send_Cleared_Mission_List(
    CUser& user, const std::vector<int>& clearList)
{
    if (clearList.empty())
    {
        return;
    }
    PacketGuard packet;
    packet.put_header(0, 0x141);
    packet.put_byte((int)clearList.size());
    for (unsigned int i = 0; i < clearList.size(); ++i)
    {
        packet.put_short(clearList[i]);
    }
    packet.finalize(true);
    user.Send(packet);
