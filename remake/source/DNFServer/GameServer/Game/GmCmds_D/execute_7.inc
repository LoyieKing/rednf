void CIndependentDropByForce::execute()
{
    CUser* user = GetUser();
    if (user == 0)
    {
        return;
    }
    // b28d-solo-GmCmds_D：ORIG 无 flag 槽（`movzbl 0x8(%eax),%eax; test %al,%al;
    // je <off>`）⇒ 表达式直读（P0）；块序 fall-through=on（P47/P191）。
    std::string msg;                                          // local_14
    if (((const GmDFld<bool>*)this)->f08)
    {
        msg = "independent drop item 100% on";
    }
    else
    {
        msg = "independent drop item 100% off";
    }
    PacketGuard packet;                                       // -0x1c
    G_GameWorld()->make_packet_chat_msg(packet, user,
                                        const_cast<char*>(msg.c_str()),
                                        static_cast<int>(msg.length()), 2,
                                        static_cast<ENUM_NOTIPACKET>(0xc));
    user->Send(packet);
    user->setForceDropFlag(((const GmDFld<bool>*)this)->f08);
