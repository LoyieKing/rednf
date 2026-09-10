bool CCreatureMgr::SendCreatureItemInfo(SIG_CREATURE_ITEM* sig)
{
    // b28d-solo-CreatureOnlyMgrRest [P52/P197③/P0]：ORIG 0x0833b4a0
    // `movl $0x0,-0xc` 两段式初始化；0x0833b4aa `mov 0x2c(%eax),%ebx` 把
    // sig->m_slot 落 callee-saved 跨 getCurCharacInvenW call 保活（无具名局部），
    // 后续两处使用点各自 `mov 0x2c(%eax),%edx` 重读 ⇒ 全部内联 sig->m_slot。
    Inven_Item* slotItem = 0;
    slotItem = m_user->getCurCharacInvenW()->GetInvenRef(3, sig->m_slot);
    if (slotItem != 0 && slotItem->m_addInfo != 0)   // +0x02
    {
        CPacketRespondent pkt(m_user, 0, 0);
        pkt.MakeNotipacketCreatureItemList(&m_creatureItems, sig->m_slot);
        pkt.Send();
        m_user->SendUpdateItemList((CUser::eSendTarget)1, (ENUM_ITEMSPACE)7,
                                   sig->m_slot);
    }
    return true;
