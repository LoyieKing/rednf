// ORIG _ZN13user_creature12CCreatureMgr20SendCreatureItemInfoEP17SIG_CREATURE_ITEM
// T 0x0833b498（SendCreatureItemInfo.md）：SIG.m_slot(+0x2c) 槽物存在且非空 →
// 广播列表包后 SendUpdateItemList(1,ITEMSPACE 7,slot)；返回恒 1。
// [b28d-solo-CreatureOnlyMgrRest] verdict=IDENTICAL/AE：P197③ slotItem
// 两段式（ORIG `movl $0,-0xc`）+ P52 `sig->m_slot` 内联实参跨 receiver call
// 保活 + 去 inv/slot 具名局部（三处使用点各自重读 sig->m_slot）。
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
}