int Dispatcher_ReportBadP2PUser::dispatch_sig(CUser* user, PacketBuf& packet)
{
    char seat;
    if (!packet.get_byte(seat))
    {
        return LineFunc(0x8ddb,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    cMyTraceNoop log;
    log(" %s Lack votes slot %d as Lack.", user->getCurCharacName(), (int)seat);
    // ORIG 0x0820..：[46] 段先用 `$0x8de8`（_!pPVPRoom）⇒ CheckInPvp() 为**真**/PVPRoom
    //   分支在前；我方旧形 `if (!CheckInPvp())` 令两支序列整体互换（ndiff 38）。
    if (user->CheckInPvp())
    {
        PvP_Room* room = (PvP_Room*)user->GetPVPRoom();
        if (room == 0)
        {
            cMyTrace trace(__PRETTY_FUNCTION__, 0x8de8, 0x5);
            trace("[%s][%d]_!pPVPRoom", __PRETTY_FUNCTION__, 0x8de8);
        }
        else
        {
            room->VoteLackUser(user, (int)seat);
        }
    }
    else
    {
        if (user->IsState((ch_state)5))
        {
            CParty* party = (CParty*)user->GetParty();
            if (party == 0)
            {
                cMyTrace trace(__PRETTY_FUNCTION__, 0x8df8, 0x5);
                trace("[%s][%d]_!pParty", __PRETTY_FUNCTION__, 0x8df8);
            }
            else
            {
                party->VoteLackUser(user, (int)seat);
            }
        }
    }
    return 0;
