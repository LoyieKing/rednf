// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x081fd0a8 orig/ours=106/104 ndiff-blocks=16
//   帧差 sub $40 vs $44 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: push   %esi | sub    $0x40,%esp | lea    -0x32(%ebp),%eax
int Dispatcher_ReportBadP2PUser::dispatch_sig(CUser* user, PacketBuf& packet)
{
    char seat;
    if (!packet.get_byte(seat))
    {
        return LineFunc(0x8ddb,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    // [J1Dispatchers 2026-09-11] §6.5 文本契约：ORIG .rodata 0x0b7a6ec =
    //   "user %s Lack votes slot %d as Lack"（无前导空格、无结尾句点）——
    //   旧形 " %s ... as Lack." 为发明文本，AE 口径不可见（P-6.5）。
    // [R4BigTUDispatchers 2026-09-11 · 调用序轴（callseq3 定位，与 Back2Village 同型）]
    //   ORIG 0x081fd0ff `call getCurCharacName`（结果入 %ebx）**先于** 0x081fd10c
    //   `call cMyTraceNoopC1` ⇒ 原码先在 ctor 前求实参 ⇒ 具名局部前移。
    register const char* characName = user->getCurCharacName();
    cMyTraceNoop log;
    log("user %s Lack votes slot %d as Lack", characName, (int)seat);
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
}