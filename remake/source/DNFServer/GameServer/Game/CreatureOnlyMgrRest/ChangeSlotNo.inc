// ORIG _ZN13user_creature12CCreatureMgr12ChangeSlotNoEP10Inven_Itemii
// T 0x0833aedc（ChangeSlotNo.md）：FindCreatureItem(uid) 为空返 false；先本地
// SetSlotNo(newSlot)，再 CDBMsgSender(0x8a).ChangeSlotNo(item,characNo,...)；
// 成功 Send + WriteCreatureLog(3,item idx,addInfo2,1,type,param_3)；失败 trace 0xc44。
// [b28d-solo-CreatureOnlyMgrRest] verdict=NEAR：P212/P309 reason 全头部声明
// + P0 内联 characNo + P227 临时链 trace + P52 链式 receiver（reason 实参
// 经复核 = param_3 = ORIG 0x14(%ebp)，无 bug）+ P139（trace 串改
// __PRETTY_FUNCTION__）。残差 = N82附 聚合/标量槽分区（ORIG reason -0x3c、
// 我方 -0x1c）+ N115 帧 0x4c vs 0x5c（creatureItem/sender/trace 三槽统一浅 4）。
// [L2Free4to15.S3 复核] gap=0、ORIG-only/OURS-only 皆空 ⇒ 纯槽号漂移
// （-0x1c↔-0x20 / -0x34↔-0x38 / -0x2c↔-0x30），N82附+N115 封顶。
bool CCreatureMgr::ChangeSlotNo(Inven_Item* item, int newSlot, int param_3)
{
    // [SmallPoolD 2026-09-11 试形·落地] N115 帧 0x4c vs ORIG 0x5c：P232 补未使用
    // 局部数组撑帧 +0x10。实测 ndiff **13→11**（fresh routed：127/128 gap −1），
    // TU 三数不变（40/78/3/2）⇒ 保留；残余 = N82附 槽分区（-0x1c↔-0x20 等差 4 族）。
    char framePad[0x10];
    (void)framePad;
    if (!IsCreatureEquipmentScope(newSlot))
        return true;
    if (item->m_field1 != 5)
        return true;

    CCreatureItem* creatureItem =
        FindCreatureItem(static_cast<int>(item->m_addInfo2));
    if (creatureItem == 0)
        return false;

    creatureItem->SetSlotNo(newSlot);   // ORIG T 0x08337050（写 +0x10）

    // b28d-solo-CreatureOnlyMgrRest [P212/P309]：ORIG reason 占最深槽 -0x3c
    // （sender -0x34 / mapSize -0x1c）⇒ 全头部声明、先声明深。
    eItemAddReason reason;
    CDBMsgSender sender((ENUM_INTERNALPACKET)0x8a, m_user);
    // b28d-solo-CreatureOnlyMgrRest [P0/P227]：ORIG 0x0833af74 getCurCharacNo
    // 结果无槽（直接 0x8(%esp)）⇒ 内联；失败 trace uid 在 ctor 前
    // movzwl→%ebx 保活 ⇒ 全表达式临时链。
    if (!sender.ChangeSlotNo(item, m_user->getCurCharacNo(), newSlot, param_3))
    {
        // b28d-solo-CreatureOnlyMgrRest [P139]：函数名串用 __PRETTY_FUNCTION__
        // 复现 ORIG `_ZZ...E19__PRETTY_FUNCTION__` 局部静态符号。
        cMyTrace(__PRETTY_FUNCTION__, 0xc44, 5)(
            "cannot write db uid(%d)\n", m_user->get_unique_id() & 0xffff);
        return false;
    }
    sender.Send();

    // b28d-solo-CreatureOnlyMgrRest [P52]：ORIG 0x0833b00d reason 落槽 -0x3c、
    // field1/addInfo2/addInfo 先物化 edi/esi/ebx 再 call getCurCharacInvenR。
    reason = (eItemAddReason)param_3;
    m_user->getCurCharacInvenR()->WriteCreatureLog(
        (INVEN_TYPE)3, item->m_addInfo, item->m_addInfo2, 1,
        (int)item->m_field1, reason);
    return true;
}