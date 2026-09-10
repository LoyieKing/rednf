void CViewTutorialFlagCmd::execute()
{
    std::string text("tutorial flag : ");
    char buf[255] = { 0 };
    for (int i = 0; i <= 0x7f; ++i)
    {
        if ((char)GetUser()->getCurCharacEscaladeTutorialFlag((unsigned int)i))
        {
            sprintf(buf, "(%d) ", i);
            text += buf;
        }
    }

    PacketGuard packet;
    packet.put_header(0, 0xc);
    packet.put_byte(0);
    packet.put_short(0);
    packet.put_byte(0);
    packet.put_int((int)text.size());
    packet.put_str(text.c_str(), (int)text.size());
    packet.finalize(true);
    GetUser()->Send(packet);
