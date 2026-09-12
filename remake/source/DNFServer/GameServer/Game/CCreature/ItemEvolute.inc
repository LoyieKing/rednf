// ORIG _ZN13user_creature9CCreature11ItemEvoluteEP5CUseri T 0x8338ca0
//（ItemEvolute.md 全转写）：目标取 GetItemEvolutionCreatureId(itemIdx)（0x833cad8）。
// [R11fixE7 2026-09-12] 返回类型 int→bool：ORIG prett 串 `bool user_creature::
// CCreature::ItemEvolute(CUser*, int)`（raw @0x8c2f320）为直证（P174）；体内
// `return true/false`。caller CCreatureMgr::ItemEvolute 0x833d6c2 的 ORIG 形
// `call; jmp leave;ret`（4.1 bool→int 不归一）保持。
bool CCreature::ItemEvolute(CUser* user, int itemIdx)
{
    // [b28d-37 复核 2026-09-09] DIFF→AE：补 nextItemId(-0x10) 具名局部 + P197③
    // slotItem 预清零 + P227 trace；173/174（+1 尾部对齐 nop）。
    if (m_script == 0)
    {
        // P227 全表达式临时链（同 EvoluteMulty 失败路径）。
        cMyTrace(__PRETTY_FUNCTION__, 0x89b, 5)(
            "cannot evolute because it haven't creature script uid(%d), "
            "creature id(%d) \n", user->get_unique_id() & 0xffff,
            reinterpret_cast<CCreatureItem*>(this)->GetItemId());
        return false;
    }
    int evolutionCreatureId = m_script->GetItemEvolutionCreatureId(itemIdx);
    if (evolutionCreatureId == 0)
        return false;
    CCreatureScript* nextScript =
        GetInstanceCreatureScriptMgr()->FindCreatureScript(evolutionCreatureId);
    if (nextScript == 0)
        return false;
    // ORIG 8338d6f：GetItemId() 结果先落 -0x10 槽再回读传 SetItemId（P0）。
    int nextItemId = nextScript->GetItemId();
    SetItemId(nextItemId);
    // P197③：ORIG 8338d84 `movl $0x0,-0xc(%ebp)` 预清 + 独立赋值两段式。
    Inven_Item* slotItem = 0;
    slotItem = user->getCurCharacInvenW()->GetInvenRef(0, 0x16);
    if (slotItem == 0)
        return false;
    slotItem->m_addInfo = reinterpret_cast<CCreatureItem*>(this)->GetItemId();
    CDBMsgSender sender((ENUM_INTERNALPACKET)0x8a, user);
    if (!sender.EvoluteCreature(reinterpret_cast<CCreatureItem*>(this)->GetUid(),
                                reinterpret_cast<CCreatureItem*>(this)->GetItemId()))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x8bb, 5)(
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