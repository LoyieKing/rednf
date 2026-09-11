// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P63] ORIG 0x081de0c6 orig/ours=67/65 ndiff-blocks=1
//   ORIG 正常路径尾 jmp+nop 中继岛（4.1 EH 尾块布局），4.4 fall-through/jump-thread 恒消除（P223 尾空标签试形实测无效）
//   首差: jmp    081de1a3 <+0xdd> | nop
void Dispatcher_Register_Quick_Party::send(CUser* user, ParamBase& param)
{
    // ORIG 0x081de0c6：p4==0 → SendCmdOkPacket(0x1bc)；p4==1 → header(1,0x1bc)+byte 0+int p4
    stQuickPartyParam& p = static_cast<stQuickPartyParam&>(param);
    if (p.m_4 == 0)
    {
        user->SendCmdOkPacket((ENUM_CMDPACKET)0x1bc);
    }
    else if (p.m_4 == 1)
    {
        PacketGuard pkt;
        pkt.put_header(1, 0x1bc);
        pkt.put_byte(0);
        pkt.put_int(p.m_4);
        pkt.finalize(true);
        user->Send(pkt);
    }
    else
    {
        // b28d-solo-Dispatchers: P285 —— ORIG 0x081de1a2 `jmp; nop` 体尾中继岛；
        //   空 else{return;} 使 4.4 保留体尾 jmp+nop（纯正向 if 恒被 jump-thread）。
        return;
    }
}