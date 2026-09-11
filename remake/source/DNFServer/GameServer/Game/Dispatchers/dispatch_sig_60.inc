// ORIG 0x082252fe（报告 Dispatcher_CompleteLoadPvP/dispatch_sig.md）：
// state<=1 → LineFunc(0xe005)；matching 非空 → IMatch +0x54 vcall；
// room/matching 为空 → 直接 return 0（无 CAssaultMgr 回退分支）。
// [b28c-32] verdict=NEAR：P214① 正向包裹+双 return 0（共享 je 尾）；余 N7（42=42）。
int Dispatcher_CompleteLoadPvP::dispatch_sig(CUser* user, PacketBuf& packet)
{
    (void)packet;
    // ORIG 0x082252fe（报告 Dispatcher_CompleteLoadPvP/dispatch_sig.md 全文已读）：
    // state<=1（bool 物化）→ return LineFunc（返回值直出，无 mov $0 覆盖）；
    // room@-0xc 唯一局部槽；null 双检 || 短路共享 je 尾体；OnCompleteLoad 处
    // room->m_matching 成员链各子表达式独立回读（ORIG 4 次 0x6e4 载入）。
    if (user->get_state() <= 1)
    {
        return LineFunc(0xe005,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    PvP_Room* room = (PvP_Room*)user->GetPVPRoom();
    // [b28c-32] ORIG 0x0822534d/0x0822535e 两条件各自 `je <body 后共享出口>`（正极性
    // 短路）⇒ 源形为正向包裹 + 体内/体外双 return 0（P214①/P222）。
    if (room != 0 && room->m_matching != 0)
    {
        ((IMatch*)room->m_matching)->OnCompleteLoad(user);
        return 0;
    }
    return 0;
}