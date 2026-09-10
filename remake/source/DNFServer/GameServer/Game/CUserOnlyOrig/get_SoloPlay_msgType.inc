int CUser::get_SoloPlay_msgType(const MSG_REQ_TO_PEER& msg)
{
    // ORIG 0x0868dd7c：`movzbl 0xf(%eax),%eax; movsbl %al,%eax` 单条成员 disp
    // 装载（§4.3 例外视图，MSG_REQ_TO_PEER 无布局符号）+ 值留 %eax 二分下探
    // （4.1 switch 决策树 `cmp $1; je; cmp $1; jg; test; je;…`，技巧34）
    struct PeerTypeView { char m_pad[0xf]; unsigned char m_reqType; };
    // [b28d-solo-CUserOnlyOrig P279/P0] ORIG 无 v 槽（`movzbl 0xf(%eax),%eax;
    // movsbl %al,%eax` 后值留 %eax 直接二分）⇒ switch 直接吃表达式。
    switch (static_cast<signed char>(
        (reinterpret_cast<const PeerTypeView*>(&msg))->m_reqType)) {
    // ORIG 岛序 0x29 ret1 / 0x30 ret0 / 0x37 ret2 / 0x3e ret3 / 0x45 ret4
    // ⇒ 源 case 书写序 0,1,2,3（body 块按声明序发射）
    case 0: return 1;
    case 1: return 0;
    case 2: return 2;
    case 3: return 3;
    default: return 4;
    }
