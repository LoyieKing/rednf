// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P197④] ORIG 0x081d375e orig/ours=85/81 ndiff-blocks=15
//   ORIG setcc 物化（bool 归一岛）而 4.4 折直跳/槽化，源形不可择
//   首差: mov    %eax,-0x18(%ebp) | cmpl   $0x0,-0x18(%ebp) | mov    -0x18(%ebp),%eax
int Dispatcher_Enchant_3rdChronicleItem::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    const int err = check_error(user, msg);
    if (err > 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x173, (unsigned char)(err & 0xff));
        return -1;
    }

    stEnchantParam& out = static_cast<stEnchantParam&>(param);   // +0x04 结果槽
    MSG_ENCHANT& req = reinterpret_cast<MSG_ENCHANT&>(msg);

    // [SemA02 2026-09-12] ORIG 0x081d37bb `movl $0x0,-0xc(%ebp)`：位于 param/msg 两引用
    // 取址之后、PacketGuard 构造之前，**写后全函数无读**（死写，P171「原码死写入照抄」）。
    // 它决定其后全部槽位（ORIG out@-0x14 / req@-0x10 / PG@-0x24；我方若无此槽整体浅 4）。
    int deadInit = 0;
    (void)deadInit;

    PacketGuard pg;                                              // try 域 @081d37e3..081d3846
    pg.put_header(1, 0x173);

    out.m_4 = G_CDataManager()->m_itemUpgrade->ProcUpgrade(user, &req, &pg);

    if (out.m_4 == 0)
    {
        pg.finalize(true);
        user->Send(pg);
    }
    return 0;
}