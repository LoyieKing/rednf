// ORIG _ZN13user_creature9CCreature12EvoluteMultyEP5CUseri T 0x8338950
//（EvoluteMulty.md 全转写）：进化目标由调用方给定 creatureId（isEvoluationCreature 校验）。
bool CCreature::EvoluteMulty(CUser* user, int creatureId)
{
    // [b28d-37 复核 2026-09-09] DIFF→AE：补 P0 具名局部 evolutionCreatureId(-0x18)/
    // nextItemId(-0x10)、P197③ slotItem 预清零、P227 全表达式临时链 trace；
    // 179/180（+1 尾部对齐 nop，§6.1 豁免）。
    if (m_script == 0)
    {
        // P227：ORIG 在 cMyTrace ctor 前右起求值 varargs 并跨 call 保活
        // %ebx/%esi（N80 反面），故用全表达式临时链而非具名 trace 局部。
        cMyTrace(__PRETTY_FUNCTION__, 0x843, 5)(
            "cannot evolute because it haven't creature script uid(%d), "
            "creature id(%d) \n", user->get_unique_id() & 0xffff,
            reinterpret_cast<CCreatureItem*>(this)->GetItemId());
        return false;
    }
    if (!m_script->isEvoluationCreature(creatureId))
        return false;
    // ORIG 83389eb：mov 0x10(%ebp),%eax; mov %eax,-0x18(%ebp)（P0 具名局部），
    // 后续 ==0 判定与 FindCreatureScript/包通知均读该槽。
    int evolutionCreatureId = creatureId;
    if (evolutionCreatureId == 0)
        return false;
    CCreatureScript* nextScript =
        GetInstanceCreatureScriptMgr()->FindCreatureScript(evolutionCreatureId);
    if (nextScript == 0)
        return false;
    // ORIG 8338a33：GetItemId() 结果先落 -0x10 槽再回读传 SetItemId（P0）。
    int nextItemId = nextScript->GetItemId();
    SetItemId(nextItemId);
    // P197③：ORIG 8338a48 `movl $0x0,-0xc(%ebp)` 预清 + 独立赋值两段式。
    Inven_Item* slotItem = 0;
    slotItem = user->getCurCharacInvenW()->GetInvenRef(0, 0x16);
    if (slotItem == 0)
        return false;
    slotItem->m_addInfo = reinterpret_cast<CCreatureItem*>(this)->GetItemId();
    CDBMsgSender sender((ENUM_INTERNALPACKET)0x8a, user);
    if (!sender.EvoluteCreature(reinterpret_cast<CCreatureItem*>(this)->GetUid(),
                                reinterpret_cast<CCreatureItem*>(this)->GetItemId()))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x866, 5)(
            "cannot write db uid(%d)\n", user->get_unique_id() & 0xffff);
        return false;
    }
    sender.Send();
    CPacketRespondent pkt(user, 0, 0);
    pkt.MakeNotipacketEvoluteCreature(evolutionCreatureId);
    pkt.SendAccordingToPlace();
    user->SendUpdateItemList(CUser::eSendTarget_1, (ENUM_ITEMSPACE)3, 0x16);
    return true;
}