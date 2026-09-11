// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x082178c0 orig/ours=133/150 ndiff-blocks=20
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: setle  %al | setne  %al | test   %al,%al
int Dispatcher_PvPChannelInfo::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 段直证：入口**无** `cmpl $0x0,0xc(%ebp)` user 判空；`cmp $0x2,%eax; setle %al;
    // test %al,%al; je`（4.1 bool 物化，技巧9）⇒ 删 user==0 并改 (bool) 强制物化形。
    if ((bool)(user->get_state() <= 2)) return 0;
    // [G00 2026-09-11 P-G7a 试形] ORIG 段直证：`call getCurCharacR; test %eax,%eax;
    //   setne %al; test %al,%al; je` 与 `call isDisguiseCharac; test %al,%al; je`
    //   ⇒ 两个子条件**各自物化** bool。4.4 只在 while/for 条件位物化（`if`/`&&`/`!!`
    //   直折；旧 `(bool)(…)&&…` 形实测产 `mov $1/%eax` 岛）⇒ 改逐层 `while{…;break;}`。
    // [G00 试形 2] ORIG 的外层条件物化 `setne %al; test %al,%al`（⇒ 必须处 while
    //   条件位，P351），内层仅 `test %al,%al; je`（无 setne ⇒ 4.4 对 `if` 直折），
    //   且 setDisguiseCharac 之后**无 jmp**（fall through 到循环出口）⇒ 内层用 `if`，
    //   外层 `while` 末尾仅一条 `break`（末语句 ⇒ 4.4 折掉其 jmp）。
    // [G00 试形 2 = 当前最佳 nd=1] 外层 while（条件位物化 `setne %al; test %al,%al`）
    //   + 内层 if（4.4 对 if 直折 `test %al,%al; je`，与 ORIG 821790b 同形）+
    //   末语句 `break`。**负结论**：试形 3（`goto` 落点重合）与试形 4（内层并入同一
    //   `&&` 链、单 while）均不消 `break` 的 `jmp`：3 仍留 `jmp 13737` 越过 `nop`，
    //   4 反劣 nd 1→21（`&&` 产 `mov $0x1/%eax` 岛，复现历史结论）。
    //   残差 = 我方 13734 `jmp 13737` + 13736 `nop` 两条，属「4.1 在 if 位物化、
    //   4.4 仅 while 位物化」的 P351b 跨版本残差（ORIG 是嵌套 if 形，0 jmp）。
    while (user->getCurCharacR() != 0)
    {
        if (user->isDisguiseCharac())
            user->setDisguiseCharac(false, '\0', 0);
        break;
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
    // [G00 2026-09-11 P-g03-b 反向试形] ORIG 0x08217a3x 段：四个载荷写全部直折叠
    //   （`mov %eax,-0x17fe(%ebp)` / `movb $0x9,-0x17f6(%ebp)`），size 取
    //   PacketHeader::packetSize 具名成员（`movw $0x18,-0x180a; movzwl -0x180a,%eax`）。
    //   旧 cast 视图指针形恒生 `lea -0x180c(%ebp),%ebx; mov %eax,0xe(%ebx)` 两步
    //   ⇒ 改用头内新建的真实具名成员（偏移/尺寸逐字节不变）。
    channelInfo.m_uid = user->GetUID();
    channelInfo.m_characNo = user->getCurCharacNo();
    channelInfo.m_field16 = 9;
    channelInfo.m_schoolNo = (unsigned int)user->GetSchoolNo();
    channelInfo.packetSize = 0x18;
    // [DispA2 P0/P183] ORIG 0x08217a8b..0x08217ab9：GetServerProxy 回值直作
    //   SendTcpPacket 的 receiver（%eax 直入 (%esp)），无 proxy 具名槽。
    GlobalData::s_monitor_proxy_mgr->GetServerProxy(user->GetServerGroup())
        ->SendTcpPacket((char*)&channelInfo, channelInfo.packetSize);
    return 0;

}