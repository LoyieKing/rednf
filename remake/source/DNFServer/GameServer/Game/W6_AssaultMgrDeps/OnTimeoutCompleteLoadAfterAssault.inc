// ===========================================================================
// OnTimeoutCompleteLoadAfterAssault —— ORIG T 0x082ef86e (0x169)
// TimerCompleteLoadAfterAssaultTimeout 回调主体（LfAssaultPlace 只交付了 registNextTimer）。
// [b28d-solo-W6_AssaultMgrDeps] AE（95=95）：catch(...) 改 catch(int) 补 `cmp $0x1,%edx` landing；
// 返回类型 void→int（P159：ORIG 尾 `mov $0x1,%eax` 0x082ef9cb）；旧 N93 判词作废。
// ===========================================================================
int CAssaultMgr::OnTimeoutCompleteLoadAfterAssault(unsigned int partyIndex, int timerKey)
{
    try
    {
        CParty* party = G_CGameManager()->GetParty(static_cast<int>(partyIndex));
        if (party == 0)
        {
            throw static_cast<ENUM_ERROR>(0x1);
        }
        if (party->check_timer_key(static_cast<TIMER_MESSAGE>(0x2a),
                                   timerKey) == false)
        {
            throw static_cast<ENUM_ERROR>(0x17);
        }
        if (party->TimeoutCompleteAfterAssault())       // ORIG 返回值为包触发条件（test %al; je 跳过）
        {
            PacketGuard packet;
            packet.clear();                             // InterfacePacketBuf::clear @080cb8e6
            packet.put_header(1, 0xf8);
            packet.put_byte(1);
            packet.finalize(true);
            party->send_to_party(packet);
        }
    }
    catch (int)                             // [b28d-solo] ORIG landing pad
                                            // `cmp $0x1,%edx; je` + 存值槽 -0xc
                                            // ⇒ 定类型 catch(int)，非 catch-all。
    {
    }
    return 1;                               // [b28d-solo] P159：ORIG 尾 `mov $0x1,%eax`
                                            // （0x082ef9cb）⇒ 返回类型非 void。
}