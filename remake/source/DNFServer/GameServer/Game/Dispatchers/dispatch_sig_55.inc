// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x082178c0 orig/ours=133/150 ndiff-blocks=20
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: setle  %al | setne  %al | test   %al,%al
int Dispatcher_PvPChannelInfo::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 段直证：入口**无** `cmpl $0x0,0xc(%ebp)` user 判空；`cmp $0x2,%eax; setle %al;
    // test %al,%al; je`（4.1 bool 物化，技巧9）⇒ 删 user==0 并改 (bool) 强制物化形。
    if ((bool)(user->get_state() <= 2)) return 0;
    // ORIG 段直证：`call getCurCharacR; test %eax,%eax; setne %al; test %al,%al; je`
    // （4.1 物化；4.4 裸 `!= 0` 折 jne）。
    if ((bool)(user->getCurCharacR() != 0) && user->isDisguiseCharac()) {
        user->setDisguiseCharac(false, '\0', 0);
    }
    if (user->isCompetitionMercenary()) {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xc6, (unsigned char)0x15);
        return 0;
    }
    if (user->GetSchoolNo() == 0) {
        PacketGuard response;
        response.put_header(1, 0xc6);
        response.put_byte(1);
        response.put_int(0);
        response.put_byte(0);
        response.finalize(true);
        user->Send(response);
        return 0;
    }
    Packet_PvPChannelInfo channelInfo;
    // ORIG 0x08217a3x 段：五个字段写全部直折叠（`mov %eax,-0x17fe(%ebp)` / `movb $0x9,-0x17f6(%ebp)`），
    //   size 取 PacketHeader::packetSize 具名成员（`movw $0x18,-0x180a; movzwl -0x180a,%eax`）。
    struct __attribute__((packed)) PvPChannelInfoView
    {
        char m_head[0xa];          // +0x00 PacketHeader
        unsigned int m_characNo;   // +0x0a
        unsigned int m_uid;        // +0x0e
        unsigned int m_schoolNo;   // +0x12
        unsigned char m_field16;   // +0x16
    };
    reinterpret_cast<PvPChannelInfoView*>(&channelInfo)->m_uid = user->GetUID();
    reinterpret_cast<PvPChannelInfoView*>(&channelInfo)->m_characNo = user->getCurCharacNo();
    reinterpret_cast<PvPChannelInfoView*>(&channelInfo)->m_field16 = 9;
    reinterpret_cast<PvPChannelInfoView*>(&channelInfo)->m_schoolNo = (unsigned int)user->GetSchoolNo();
    channelInfo.packetSize = 0x18;
    // [DispA2 P0/P183] ORIG 0x08217a8b..0x08217ab9：GetServerProxy 回值直作
    //   SendTcpPacket 的 receiver（%eax 直入 (%esp)），无 proxy 具名槽。
    GlobalData::s_monitor_proxy_mgr->GetServerProxy(user->GetServerGroup())
        ->SendTcpPacket((char*)&channelInfo, channelInfo.packetSize);
    return 0;

}