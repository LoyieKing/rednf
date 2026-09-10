void Dispatcher_CallPartyMemberRealtimeInfo::send(CUser* user, ParamBase& param)
{
    // [b28c-34] N104 封顶（EH 尾块相对序，75 vs 73）：ORIG 正常路径 guard D1 后
    // `jmp +0xe4` 跳过 unwind 块；4.4 把正常 D1 就地内联到 epilogue 前、unwind 压
    // 最末（少 1 jmp + 块位移）。块内容逐条全等（两 PacketGuard 析构段），仅相对序差。
    PacketGuard guard;
    PacketGuard unused;                            // -0x24（ORIG 第二个 PacketGuard，构造后不使用）
    (void)unused;
    CParty* party = (CParty*)user->GetParty();
    guard.put_header(1, 0xa9);
    if (party == 0)
    {
        guard.put_byte(0);
        guard.put_byte(0x13);
        guard.finalize(true);
        user->Send(guard);
    }
    else
    {
        party->send_party_realtime_info();
    }
