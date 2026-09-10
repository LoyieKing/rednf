bool CCreature::Evolute(CUser* user)
{
    if (m_script == 0)
    {
        // [b23S3C] ORIG 0x8338250：operator() 实参 0x8(%esp)=%esi(get_unique_id)、
        // 0xc(%esp)=%ebx(GetItemId)，即 vararg 序为 (uid, itemId)；交换实参只改
        // 求值次序、日志字段错位，属语义倒退 ⇒ 恢复 (uid, itemId)。
        cMyTrace(__PRETTY_FUNCTION__, 0x743, 5)(
            "cannot evolute because it haven't creature script uid(%d), "
            "creature id(%d) \n",
            user->get_unique_id() & 0xffff,
            reinterpret_cast<CCreatureItem*>(this)->GetItemId());
        return false;
    }
    int evolutionCreatureId = m_script->GetEvolutionCreatureId();
    if (evolutionCreatureId == 0)
        return false;
    CCreatureScript* nextScript =
        GetInstanceCreatureScriptMgr()->FindCreatureScript(evolutionCreatureId);
    if (nextScript == 0)
        return false;
    // [b23S3C] P0 律：ORIG -0x10(%ebp) 存 CCreatureScript::GetItemId 结果后回读
    // 入参 ⇒ 原码为具名中间局部（缺它整帧上移 4，全函数槽号错位）。
    int nextItemId = nextScript->GetItemId();
    SetItemId(nextItemId);
    // [b23S3C] P197③：ORIG 0x83382c7 先 `movl $0x0,-0xc(%ebp)` 再取 InvenRef
    // ⇒ 声明置 0 + 独立赋值两段式。
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
        cMyTrace(__PRETTY_FUNCTION__, 0x763, 5)(
            "cannot write db uid(%d)\n", user->get_unique_id() & 0xffff);
        return false;
    }
    sender.Send();
    CPacketRespondent pkt(user, 0, 0);       // 析构按 finalize 标志自动补发
    pkt.MakeNotipacketEvoluteCreature(evolutionCreatureId);
    pkt.SendAccordingToPlace();
    user->SendUpdateItemList(CUser::eSendTarget_1, (ENUM_ITEMSPACE)3, 0x16);
    return true;
