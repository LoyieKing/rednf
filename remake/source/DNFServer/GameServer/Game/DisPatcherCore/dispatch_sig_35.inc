// ORIG 0x0820b802 T _ZN26DisPatcher_MultiBoxLottery12dispatch_sigEP5CUserR9PacketBuf
// 报告：docs/class_func_reports/DisPatcher_MultiBoxLottery/dispatch_sig.md（汇编+反编译 C 全读）
int DisPatcher_MultiBoxLottery::dispatch_sig(CUser* user, PacketBuf& packet)

{
    if (user->get_state() < 3)
    {
        return 0;
    }
    if (user->CheckInTrade())
    {
        return 0;
    }
    if (ARAD::Singleton<ServiceRestrictManager>::Get()->isRestricted(
            user, RestrictCategory::Enum_1, 0x12))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xd3, (unsigned char)0xd1);
        return 0;
    }
    int secu = GlobalData::s_pSecuProtectionField->Check(user, (SECURITY_PROTCTION)0x18);
    if (secu != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xd3, (unsigned char)secu);
        return 0;
    }
    STMultiBoxLotteryReq reqView;   // ORIG -0x21 局部消息视图（-0x14/-0x12 两 ushort）
    if (!packet.get_short(reqView.m_field0d))
    {
        return LineFunc(0xa838,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_short(reqView.m_field0f))
    {
        return LineFunc(0xa839,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    int result = user->multibox_lottery_item(
        (int)reqView.m_field0d,
        (int)reqView.m_field0f);
    if (result > 0)
    {
        // 技12：guard 置于显式 `{}` 块 → 早退 return 0 不携 guard dtor，
        // 各 `return 0` 恢复 ORIG `mov $0x0,%eax; jmp EPI` 形（4.4 无 %ebx phi）；
        // `LineFunc` 返回直落 %eax 无 `mov %eax,%ebx`（ORIG 0x0820b802）。
        PacketGuard guard;
        guard.put_header(1, 0xd3);
        guard.put_byte(0);
        guard.put_byte(result);
        guard.finalize(true);
        user->Send(guard);
    }
    return 0;
}
