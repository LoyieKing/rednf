int Inter_Monitor_Notify_New_Mail::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_Monitor_Notify_New_MailPktView* pPkt = reinterpret_cast<Inter_Monitor_Notify_New_MailPktView*>(data);
    if (user->getCurCharacR() == 0 ||
        user->getCurCharacNo() != pPkt->m_a)
        return 0;

    reinterpret_cast<WongWork::CMailBox*>(user->GetMailBox())
        ->IncNotLoadedMailCount();

    PacketGuard packet;
    packet.put_header(0, 99);
    packet.put_short(reinterpret_cast<WongWork::CMailBox*>(user->GetMailBox())
                         ->GetNotLoadedMailCount());
    packet.finalize(true);
    user->Send(packet);
    return 0;
