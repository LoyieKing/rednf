// ORIG _ZN13user_creature9CCreature12EventEvoluteEP5CUserii T 0x833db2e
//（EventEvolute.md 全转写）：活动进化。成长门（xor 链实证）：仅 next 脚本
// IsItemGrowCreature()==0 且 自身 m_isGrow!=1（即成长宠）进入经验折算；
// newExp = get_creature_level_exp((short)(oldExp>>16)/step + 1)，step 取
// GetGrowItemCountforLevelUp()（0→0x19）。包通知携带 param_3(slot)。
bool CCreature::EventEvolute(CUser* user, int creatureId, int slot)
{
    // [b28d-37 复核 2026-09-09] DIFF→AE，含一处真 bug：GetGrowItemCountforLevelUp()
    // 的 receiver ORIG 为 this->m_script(+0x44)（Ghidra C 与 0x833dc30 实证），原实现
    // 误用 nextScript；另补 isGrow(-0x21)/商(-0xc) 具名局部、P197③ 预清零、P227 trace；
    // 246/247（+1 尾部对齐 nop）。
    if (m_script == 0)
    {
        // P227 全表达式临时链（ORIG ctor 前求值实参并保活 %ebx/%esi）。
        cMyTrace(__PRETTY_FUNCTION__, 0x10cc, 5)(
            "cannot event evolute because it haven't creature script uid(%d), "
            "creature id(%d) \n", user->get_unique_id() & 0xffff,
            reinterpret_cast<CCreatureItem*>(this)->GetItemId());
        return false;
    }
    int baseCreatureId = GetCreatureId(creatureId);   // +0x85 call
    CCreatureScript* nextScript =
        GetInstanceCreatureScriptMgr()->FindCreatureScript(baseCreatureId);
    if (nextScript == 0)
        return false;
    int nextItemId = nextScript->GetItemId();
    // ORIG 833dbf0：m_isGrow 先落字节槽 -0x21（P0 具名 bool 局部），
    // 守卫读槽（movzbl -0x21; xor $1; test）而非直读字段。
    bool isGrow = m_isGrow;

    // ORIG：连用两段 xor$1/test 守卫链物化 bool 后 if（P198 ①）。
    // exp 高 16 位→(int)(short)：先用全宽槽保存 m_exp，再读槽+2 高半字
    // movzwl+cwtl（P197 ① union 拆半）。
    if (!nextScript->IsItemGrowCreature() && !isGrow)
    {
        union { int all; unsigned short w[2]; } expSplit;
        expSplit.all = m_exp;
        // ORIG 833dc30：receiver 是 this->m_script(+0x44)，**非** nextScript
        // （Ghidra C 亦为 GetGrowItemCountforLevelUp(*(CCreatureScript**)(this+0x44))）。
        int step = m_script->GetGrowItemCountforLevelUp();     // thunk 0x833f356
        if (step == 0)
            step = 0x19;
        int level = (int)(short)expSplit.w[1] / step;          // ORIG -0xc 商槽
        m_exp = G_CDataManager()->get_creature_level_exp(level + 1);
    }
    SetItemId(nextItemId);
    CItem* item = G_CDataManager()->find_item(nextItemId);
    if (item == 0)
        return false;
    int usablePeriod = item->getUsablePeriod();       // thunk 0x08110c60
    int expirationDate = item->getExpirationDate();   // thunk 0x080f1306
    if (usablePeriod == 0 && expirationDate != 0)
        m_expireTime = expirationDate;                // +0x18 ← 过期日
    else if (usablePeriod == 0 && expirationDate == 0)
        m_expireTime = 0;

    // P197③：ORIG 833dcfd `movl $0x0,-0x14(%ebp)` 预清 + 独立赋值两段式。
    Inven_Item* slotItem = 0;
    slotItem = user->getCurCharacInvenW()->GetInvenRef(0, 0x16);
    if (slotItem == 0)
        return false;
    slotItem->m_addInfo = reinterpret_cast<CCreatureItem*>(this)->GetItemId();
    CDBMsgSender sender((ENUM_INTERNALPACKET)0x8a, user);
    if (!sender.EventEvoluteCreature(
            reinterpret_cast<CCreatureItem*>(this)->GetUid(),
            reinterpret_cast<CCreatureItem*>(this)->GetItemId(), m_exp))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1104, 5)(
            "cannot write db uid(%d)\n", user->get_unique_id() & 0xffff);
        return false;
    }
    sender.Send();
    CPacketRespondent pkt(user, 0, 0);
    pkt.MakeNotipacketEvoluteCreature(slot);
    pkt.SendAccordingToPlace();
    user->SendUpdateItemList(CUser::eSendTarget_1, (ENUM_ITEMSPACE)3, 0x16);
    return true;
}