// ---------------------------------------------------------------------------
// DispatchPacket                           ORIG 0x8493002
// data[0]=类型（当前仅识别 1），data[1]=子码 0x82..0x85；
// 返回类型 int（ORIG 尾 mov %ebx,%eax 无 bool 归一，0x315 行号直传；
// 调用点 ExchangeServer_Session.cpp 存 int moved 后 cmpl $0）。
// ---------------------------------------------------------------------------
// [b28e 2026-09-09] 129/129（p167 CLEAR）：唯一残差=外层 default 的 pktType
// 求值时机（ORIG 0x8493150 `mov -0x10(%ebp),%ebx` 在 ctor 前，4.4 恒 ctor 后读槽）。
// 已试无效：去 const、`(int)pktType`、语句表达式实参、纯 register（消掉 ORIG 的
// -0x10 槽 ⇒ 帧 0x50→0x40）⇒ 判 N80/P87（单实参 varargs 4.4 不提前求值）封顶。
int CExchangeServer::DispatchPacket(CSession* session, char* data,
                                    int len)
{
    // [b28d-solo-Exchange] 返回类型 bool→int（ORIG 尾 `mov %ebx,%eax` 无 bool
    // 归一、0x315 直传；调用点 Session:177 存 int moved 后 `cmpl $0` 佐证）。
    // case 叶块物理序 = ORIG 地址序 0x84930ab(0x82)/0x84930cb(0x83)/
    // 0x84930eb(0x84)/0x849310b(0x85)；default 与函数尾 return 0 共享
    // `mov $0x0,%ebx` 块（0x84931a8），非 pktCode。
    // [SemB04 2026-09-12] case 书写序须逐字按 ORIG 叶块地址升序（P324）——
    // 旧序 0x84/0x85/0x82/0x83 使我方叶块物理序反（链接口径 NEAR 残差根因）。
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
        // [SemB04 2026-09-12][真语义修复] ORIG 决策树（0x8493033 段）逐值落点：
        //   cmp$0x84->0x84930ab(call OnRecvPacketCharacInfo)
        //   cmp$0x85->0x84930cb(call OnRecvPacketCharacInfoRet)
        //   cmp$0x82->0x84930eb(call OnRecvPacketServerInfo)
        //   cmp$0x83->0x849310b(call OnRecvPacketServerInfoRet)
        // ⇒ 原重建把 0x82/0x83 与 0x84/0x85 的处理函数**互换**（协议分派错）；
        //   叶块物理序（书写序，P324）= CharacInfo/CharacInfoRet/ServerInfo/ServerInfoRet。
        case 0x84:
            return OnRecvPacketCharacInfo(session, guard);
        case 0x85:
            return OnRecvPacketCharacInfoRet(session, guard);
        case 0x82:
            return OnRecvPacketServerInfo(session, guard);
        case 0x83:
            return OnRecvPacketServerInfoRet(session, guard);
        default:
            // P221/P227 临时链：ORIG 0x849312b `movzbl -0x9(%ebp),%ebx` 保活
            // + default 尾 jmp 共享 return 0 块（0x84931a8）⇒ 用 break 出 switch。
            cMyTrace(__PRETTY_FUNCTION__,
                     0x328, 0)
                ("unknown packet code %d", pktCode);
            break;
        }
        break;
    default:
    {
        // [K2 2026-09-11] P231 纯 `register` 存储类提示（**非** asm 绑定，P0 合规）：
        // ORIG 0x0849316b `mov -0x10(%ebp),%ebx` 在 cMyTrace ctor **之前**载入
        // callee-saved 并跨 ctor 保活（对照 pktCode 的 `movzbl -0x9(%ebp),%ebx` 同形）。
        // 探针 /tmp/k2p/hoist.cpp fD 实证：`register` 复本令 4.4 复现该提前装载；
        // 对 pktType **本身**加 register 会消掉 ORIG 的 -0x10 槽（旧失败记录），
        // 故只在 default 块内加复本，pktType 保持普通具名局部（槽 -0x10 不变）。
        register int tracedType = pktType;
        cMyTrace(__PRETTY_FUNCTION__, 0x330, 0)
            ("unknown packet type %d", tracedType);
        break;
    }
    }
    return 0;
}