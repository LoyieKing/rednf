bool CCreature::SelectEvolute(CUser* user)
{
    if (m_script == 0)
    {
        // [b23S3C] ORIG 0x8337e60/0x6d：vararg（GetItemId→%ebx、uid→%esi）在 ctor
        // call 之前求值、trace 对象占 -0x38 匿名槽 ⇒ 临时对象直调形（同 Evolute，
        // 具名 trace 局部会挪槽位并改求值次序）。
        cMyTrace(__PRETTY_FUNCTION__, 0x6a2, 5)(
            "cannot selectevolute because it haven't creature script uid(%d), "
            "creature id(%d) \n",
            user->get_unique_id() & 0xffff,
            reinterpret_cast<CCreatureItem*>(this)->GetItemId());
        return false;
    }
    // [b23S3C] P197③：ORIG 0x8337ebd 分支前先 `movl $0x0,-0x18(%ebp)` ⇒ 声明置 0
    // + 各分支独立赋值（合并初始化式会丢预清行）。
    int targetCreatureId = 0;
    // [b23S3C] P191 极性：ORIG 0x8337ecd `cmp $0x4,%al; jbe +0x9a`(BType 为跳转目标)，
    // AType 走 fall-through ⇒ 先写 `> 4` 分支（等价 A/B 选择，块序对齐）。
    if (m_field4d > 4)
        targetCreatureId = m_script->GET_ATypeCreature();
    else
        targetCreatureId = m_script->GET_BTypeCreature();
    if (targetCreatureId == -1)
        return false;
    CCreatureScript* nextScript =
        GetInstanceCreatureScriptMgr()->FindCreatureScript(targetCreatureId);
    if (nextScript == 0)
        return false;
    // [b23S3C] 同 Evolute：P0 律 -0x10 具名中间局部 + P197③ -0xc 预清两句式。
    int nextItemId = nextScript->GetItemId();
    SetItemId(nextItemId);
    Inven_Item* slotItem = 0;
    slotItem = user->getCurCharacInvenW()->GetInvenRef(0, 0x16);
    if (slotItem == 0)
        return false;
    slotItem->m_addInfo = reinterpret_cast<CCreatureItem*>(this)->GetItemId();
    CDBMsgSender sender((ENUM_INTERNALPACKET)0x8a, user);
    if (!sender.EvoluteCreature(
            reinterpret_cast<CCreatureItem*>(this)->GetUid(),
            reinterpret_cast<CCreatureItem*>(this)->GetItemId()))
    {
        // [b23S3C] 同上：ORIG 0x8338012 ctor 前求 uid→%ebx，trace 落 -0x28 匿名槽。
        cMyTrace(__PRETTY_FUNCTION__, 0x6c9, 5)(
            "cannot write db uid(%d)\n", user->get_unique_id() & 0xffff);
        return false;
    }
    sender.Send();
    CPacketRespondent pkt(user, 0, 0);
    pkt.MakeNotipacketEvoluteCreature(targetCreatureId);
    pkt.SendAccordingToPlace();
    user->SendUpdateItemList(CUser::eSendTarget_1, (ENUM_ITEMSPACE)3, 0x16);
    return true;
}
