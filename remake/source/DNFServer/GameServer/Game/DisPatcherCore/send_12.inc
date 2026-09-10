void DisPatcher_BuyItem::send(CUser* user, ParamBase& param)
{
    BuyItemParam& out = static_cast<BuyItemParam&>(param);
    PacketGuard guard;
    guard.put_header(1, 0x15);
    guard.put_byte(1);
    guard.put_int(user->getCurCharacInvenR()->get_money());
    guard.put_int(user->UseWinPoint(out.m_88, (eWPSubReason)0));
    guard.put_int(user->UseFinishPoint(out.m_8c));
    guard.put_int(user->UseCera(out.m_84));
    guard.put_short(out.m_a0);
    guard.put_int(out.m_item.m_addInfo);        // param+0x06（m_item+0x02）
    guard.put_int(out.m_item.m_addInfo2);       // param+0x0b（m_item+0x07）
    guard.put_short(out.m_item.m_fieldb);       // param+0x0f（m_item+0x0b）
    guard.put_byte((int)out.m_item.m_amp.getAbilityType() & 0xff);   // param+0x15
    guard.put_short((int)out.m_item.m_amp.getAbilityValue() & 0xffff);
    guard.put_packet(out.m_item);
    guard.put_int(out.m_pair.first);            // param+0x98
    const Inven_Item* ref =
        user->getCurCharacInvenR()->GetInvenRef(1, out.m_a4);
    if (ref == 0)
    {
        guard.put_int(0);
    }
    else
    {
        guard.put_int(ref->m_addInfo2);         // ref+0x07
    }
    guard.finalize(true);
    user->Send(guard);
