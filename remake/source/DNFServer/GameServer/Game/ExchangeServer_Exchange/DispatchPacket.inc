int CExchangeServer::DispatchPacket(CSession* session, char* data,
                                    int len)
{
    // [b28d-solo-Exchange] 返回类型 bool→int（ORIG 尾 `mov %ebx,%eax` 无 bool
    // 归一、0x315 直传；调用点 Session:177 存 int moved 后 `cmpl $0` 佐证）。
    // case 叶块物理序 = ORIG 地址序 0x84930ab(0x84)/0x84930cb(0x85)/
    // 0x84930eb(0x82)/0x849310b(0x83)；default 与函数尾 return 0 共享
    // `mov $0x0,%ebx` 块（0x84931a8），非 pktCode。
    PacketGuard guard;
    // [b28e] ORIG 0x8493150 `mov -0x10(%ebp),%ebx` 在 cMyTrace ctor 之前读槽并
    // 跨 ctor 保活（P87/P227）；对照 pktCode 的 `(int)pktCode` 实参形使 4.4
    // 提前求值 ⇒ pktType 实参同样显式 cast（裸 register 会消掉 ORIG 的 -0x10 槽）。
    // [b28e-L21 已试无效] P231 纯 register 提示：ORIG 0x8493150 `mov -0x10(%ebp),%ebx`
    // 在 cMyTrace ctor **之前**载入 callee-saved 并跨 ctor 保活；4.4 裸形恒 ctor 后
    // 读槽；加 register 后 4.4 直接**消掉 -0x10 槽**（帧 0x50→0x40）⇒ 更偏离，已回退。
    // 判 N80/P87（单实参 varargs 4.4 不提前求值）。
    int pktType = (signed char)data[0];
    const unsigned char pktCode = (unsigned char)data[1];

    if (!guard.bind_packet(data, len))
    {
        return 0x315;                                 // ORIG ebx=0x315
    }
    guard.set_index(0xf);

    // P286：ORIG 0x849306a `mov -0x10(%ebp),%eax; cmp $0x1,%eax`（载寄存器比较）
    // ⇒ 外层单 case switch（if 形在 4.4 恒折 `cmpl $1,-0x10(%ebp)` 内存直比）。
    switch (pktType)
    {
    case 1:
        switch (pktCode)
        {
        case 0x84:
            return OnRecvPacketServerInfo(session, guard);
        case 0x85:
            return OnRecvPacketServerInfoRet(session, guard);
        case 0x82:
            return OnRecvPacketCharacInfo(session, guard);
        case 0x83:
            return OnRecvPacketCharacInfoRet(session, guard);
        default:
            // P221/P227 临时链：ORIG 0x849312b `movzbl -0x9(%ebp),%ebx` 保活
            // + default 尾 jmp 共享 return 0 块（0x84931a8）⇒ 用 break 出 switch。
            cMyTrace("virtual int exchange_server::CExchangeServer::"
                     "DispatchPacket(exchange_server::CSession*, char*, int)",
                     0x328, 0)
                ("unknown packet code %d", pktCode);
            break;
        }
        break;
    default:
        // P226 裸语句表达式实测无效（4.4 仍 ctor 后读槽）⇒ 判 N7/N18a。
        cMyTrace("virtual int exchange_server::CExchangeServer::"
                 "DispatchPacket(exchange_server::CSession*, "
                 "char*, int)", 0x330, 0)
            ("unknown packet type %d", pktType);
        break;
    }
    return 0;
