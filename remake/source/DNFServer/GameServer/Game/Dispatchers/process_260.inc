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

    PacketGuard pg;                                              // try 域 @081d37e3..081d3846
    pg.put_header(1, 0x173);

    out.m_4 = G_CDataManager()->m_itemUpgrade->ProcUpgrade(user, &req, &pg);

    if (out.m_4 == 0)
    {
        pg.finalize(true);
        user->Send(pg);
    }
    return 0;
