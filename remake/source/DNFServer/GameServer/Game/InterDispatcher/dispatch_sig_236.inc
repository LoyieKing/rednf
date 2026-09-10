int Inter_Antibot_PunishClient::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_Antibot_PunishClientPktView* pPkt = reinterpret_cast<Inter_Antibot_PunishClientPktView*>(data);
    // §P0：ORIG 084e4782/084e479c 两次独立重读 msg+0x0（uid 无槽），
    //   084e4792/084e47f8 两次独立重读 msg+0x4（punishMode 留 %esi / 重载）⇒ 不设具名缓存。
    CUser* target = getUser(pPkt->m_0);

    // ORIG 0x84e4792..0x84e47b1：punish_mode→%esi、NumberToString→%ebx（均无槽往返）
    // ⇒ 全表达式临时链，无 uidStr 具名局部（P0/P328⑩/P227）。
    cMyTrace(__PRETTY_FUNCTION__, 0x5eb3, 0)(
        "requested uid(%s), punish_mode(%d), user(%x)",
        NumberToString(pPkt->m_0, 0), (int)pPkt->m_4, target);

    // ORIG 0x84e4802..0x84e480f 决策链 cmp $2/cmp $3/cmp $1 ⇒ 原码 switch（P17/P251）。
    switch (pPkt->m_4)
    {
    case 1:
        break;
    case 2:
        if (target != 0 && target->get_state() != 0)
            target->DisConnSig((DISCONN_SIG)0x23, true, 0);
        break;
    case 3:
        if (target != 0 && target->get_state() != 0 &&
            0 < pPkt->m_c)
        {
            target->DisConnSig((DISCONN_SIG)0x23, true, 0);
        }
        break;
    }
    return 0;
