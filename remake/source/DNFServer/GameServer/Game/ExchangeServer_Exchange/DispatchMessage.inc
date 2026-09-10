bool CExchangeServer::DispatchMessage(Stream* stream)
{
    // [b28d-solo-Exchange] P46/P261：ORIG 0x849355e 起为 switch 二分决策树
    // （cmp/je/jg 无状态槽），非 if 链；映射与旧码相反——0xb8 → OnRecvMessage
    // ServerInfo（ORIG 0x849359e 调 0x84931de），{7..0xc,0x11,0x19,0x20,0xb3,
    // 0x282} → OnRecvMessageCharacInfo（ORIG 0x84935c0 调 0x849341c）。
    // 叶块物理序 = case 书写序（ServerInfo 块在前）。
    int msgid;                                        // -0x1c（ORIG 无零初始化）
    int param;                                        // -0x20
    *stream >> msgid;
    *stream >> param;

    switch (msgid)
    {
    case 0xb8:
        OnRecvMessageServerInfo(stream, msgid, param);
        break;
    case 0x07:
    case 0x08:
    case 0x09:
    case 0x0a:
    case 0x0b:
    case 0x0c:
    case 0x11:
    case 0x19:
    case 0x20:
    case 0xb3:
    case 0x282:
        OnRecvMessageCharacInfo(stream, msgid, param);
        break;
    default:
        // P221/P227 全表达式临时链：复现 ORIG 0x84935e2 `mov -0x1c(%ebp),%ebx`
        // （msgid 在 ctor 前物化并跨 ctor 保活）与 trace 槽 -0x18。
        cMyTrace("bool exchange_server::CExchangeServer::"
                 "DispatchMessage(Stream*)", 0x39d, 0)
            ("unknown message type %d", msgid);
        break;
    }
    return true;
