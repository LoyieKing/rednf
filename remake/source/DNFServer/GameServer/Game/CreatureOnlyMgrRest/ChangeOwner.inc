// ORIG _ZN13user_creature12CCreatureMgr11ChangeOwnerEP10Inven_Itemiiii
// T 0x0833ad34（ChangeOwner.md）：槽位/物品类型过滤后 CDBMsgSender(0x8a,m_user)，
// sender.ChangeOwner(item,characNo,slotFrom,mapSize,param_4,param_5)；
// 成功 → Send() + WriteCreatureLog(3,item idx,addInfo2,1,type,param_5)；
// 失败 → cMyTrace(...,0xc26,5) "cannot write db uid(%d)\n"。过滤分支均返 true。
// [b28d-solo-CreatureOnlyMgrRest] verdict=NEAR：**真语义 bug 修正**
// （WriteCreatureLog 末实参 ORIG 0x0833ae4e 读 0x14(%ebp)=mapSizeHint，
// 旧码误传 param_5）+ P212/P309 reason 全头部声明 + P0 内联 characNo
// + P227 临时链 trace + P52 链式 receiver + P139（trace 串改
// __PRETTY_FUNCTION__ 复现 `_ZZ...E19__PRETTY_FUNCTION__` 局部静态符号）。
// 残差 = N82附（4.4 聚合 sender/trace 占低槽、标量 reason 落 -0x1c，ORIG
// reason 最深 -0x3c）+ N115 帧 0x4c vs 0x5c；reason 声明位置/顺序、结构体/
// 数组聚合化、volatile、double framePad 四形实测不可翻（聚合化劣化已回退）。
// [L2Free4to15.S3 复核] reason 改「声明+初始化同位（Send 后）」实测
// ndiff 22→26 劣化，已回滚；残余 = 纯槽号漂移（gap=0、助记符序列相同）。
bool CCreatureMgr::ChangeOwner(Inven_Item* item, int slotFrom, int mapSizeHint,
                               int param_4, int param_5)
{
    // [SmallPoolD 2026-09-11 试形 A·落地] N115 帧 0x4c vs ORIG 0x5c：按 P232 补
    // 未使用局部数组撑帧 +0x10（帧达标 0x5c）。实测 ndiff **11→9**（fresh routed：
    // 121/121 gap 0），TU 三数不变（40/78/3/2）⇒ 保留。残余 sender -0x48 vs -0x34、
    // reason -0x20 vs -0x3c = N82附 聚合/标量分区（ORIG 标量 reason 比聚合 sender
    // 更深，与 4.4 聚合优先低槽相反）⇒ 判 N 封顶。
    char framePad[0x10];
    (void)framePad;
    if (!IsCreatureEquipmentScope(slotFrom))
        return true;
    if (item->m_field1 != 5)  // ITEM_TYPE_CREATURE_EQUIP
        return true;

    // b28d-solo-CreatureOnlyMgrRest [P212/P309]：ORIG reason 占最深槽 -0x3c
    // （sender -0x34 / mapSize -0x1c）⇒ 全头部声明、先声明深。
    eItemAddReason reason;
    CDBMsgSender sender((ENUM_INTERNALPACKET)0x8a, m_user);
    int mapSize = (int)m_creatureItems.size();
    // ORIG 实参序：(item, characNo, slotFrom, mapSizeHint, mapSize, p4, p5)；
    // sender 侧 +0x48=characNo、+0x2c=slotFrom、+0x51 低字节=mapSizeHint、
    // +0x68=mapSize、+0x6c=p4、+0x70=p5（汇编 ebp+0x10..0x24 对应）。
    // b28d-solo-CreatureOnlyMgrRest [P0]：ORIG 0x0833ada9 getCurCharacNo 结果
    // 无槽（直接 0x8(%esp)）⇒ 内联实参。
    if (!sender.ChangeOwner(item, m_user->getCurCharacNo(), slotFrom, mapSizeHint,
                            mapSize, param_4, param_5))
    {
        // b28d-solo-CreatureOnlyMgrRest [P227/P139]：ORIG uid 在 ctor call 前
        // movzwl→%ebx 保活 ⇒ 全表达式临时链；函数名串用 __PRETTY_FUNCTION__
        // 复现 ORIG `_ZZ...E19__PRETTY_FUNCTION__` 局部静态符号。
        cMyTrace(__PRETTY_FUNCTION__, 0xc26, 5)(
            "cannot write db uid(%d)\n", m_user->get_unique_id() & 0xffff);
        return false;
    }
    sender.Send();

    // b28d-solo-CreatureOnlyMgrRest [真语义 bug 修正 + P52]：ORIG 0x0833ae4e
    // 的 WriteCreatureLog 末实参读 0x14(%ebp)=mapSizeHint（旧码误传 param_5）；
    // 该值落具名槽 -0x3c，field1/addInfo2/addInfo 先物化 edi/esi/ebx 再 call
    // getCurCharacInvenR ⇒ receiver 单语句链式。
    reason = (eItemAddReason)mapSizeHint;
    m_user->getCurCharacInvenR()->WriteCreatureLog(
        (INVEN_TYPE)3, item->m_addInfo, item->m_addInfo2, 1,
        (int)item->m_field1, reason);
    return true;
}