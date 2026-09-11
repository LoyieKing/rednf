// ============================================================================
// CCreatureMgr::Hatch @ 0x08339fa0（Hatch.md）
//   蛋孵化：m_creatureItems 按 addInfo 查蛋（不存在 → SetError(0x15)）；
//   蛋类型非 0 → SetError(0x17)；CEgg::Hatch 失败 → SetError(0x11)；
//   工厂取新宠物项失败 → SetError(0x16)；Set(蛋副本) + SetItemId(孵化结果) +
//   SetCreatureItemType(1) + SetStomach(100)；
//   计算到期（getUsablePeriod==0 且 getExpirationDate!=0 → 过期日-当前秒，
//   否则 当前秒+usablePeriod*0x15180；两者皆 0 → 0）；
//   Unregister 旧蛋 + Register 新宠；槽位写 m_addInfo/m_itemId；
//   CDBMsgSender::HatchCreature 落库 → 成功 LogHistory+Send+SetOk+列表包+Send
//   +SendUpdateItemList；失败 trace+SetError(2)。
// ============================================================================
// [b28d-solo-CCreatureMgr N] ndiff=63（原 245）：已修**真语义缺口**（p167 由 6 MISSING
// +1 EXTRA → CLEAR）——①去掉臆造的 `itemScript != 0` 判空（ORIG 0x833a153 直用
// find_item 结果，P171 照抄）；②getUsablePeriod/getExpirationDate 各独立调用 3 次
// （P48 -O0 无 CSE，删具名缓存）；③槽位写回改 `m_field0=0` + `m_addInfo=itemId`
// （ORIG 0x833a27a `movb $0,(%eax)` + 0x833a28d `mov %edx,0x2(%eax)`；原写的
// m_addInfo=0/m_addInfo2 是错字段）；④newItem 函数头预清（P198①）；⑤`register bool ok`
// （P231，ORIG 无槽 ebx 汇聚）；⑥Set(egg) 隐式派生→基类转换；⑦Set 实参复用 egg
//   （消第二次 operator->）。326 vs 325。ORIG 0x08339fa0
// [b28d-solo-CCreatureMgr §6.2 封顶] p167 CLEAR(60=60) + gap 1 + 逐块比对。
// [L23 本轮重核 2026-09-10] 旧判词「槽布局族 it/end -0x34/-0x30 vs -0x24/-0x20 +
//   sender 与 trace 槽互换」**作废**：fresh .o 槽表与 ORIG **逐值全等**
//   （respondent -0x60 / sender -0x40 / it-sret -0x34 / itEnd-sret -0x30 /
//    trace -0x2c / creatureItemId -0x38 / unwind -0x8）⇒ 判据⑩不适用、无槽问题。
//   本轮已修：ok 由「赋值即初始化」改 `register bool ok; if(!call()){…ok=false;}
//   else{…ok=true;}` ⇒ 复现 ORIG 0x0833a2e0 `call; xor $0x1,%eax; test %al,%al; je`
//   直判形 + %ebx 汇聚（ORIG 0x0833a345 `mov $0,%ebx` / 0x0833a3ff `mov $1,%ebx`），
//   原形多 `mov %eax,%ebx; mov %ebx,%eax` 两条。
//   残余（唯一族）：Set/SetItemId/SetExpireTime/RegisterCreatureItem 四处实参装载序
//   （ORIG 0x833a0eb `mov -0x18,%edx; mov -0x1c,%eax; mov %edx,0x4(%esp)` 双载 vs
//   我方逐条压栈；SetItemId 处两侧恰好相反）+ LogHistory 寄存器角色（N7）+ sender
//   析构块序（N104）= P75/N-G 跨版本装载序族。P277 不适用：ORIG 槽表无具名派生
//   指针槽（⇒ 原码无该局部）。
// [2026-09-11 收口判词 · N104 + P75/N-G] ORIG 0x08339fa0（range 0x8339fa0..0x833a459，
//   326 条）；我方 325 条（gap +1）。p167 调用点差集：MISSING/EXTRA 全为 C1/C2·D1/D2
//   同址别名（P93 豁免：CDBMsgSender/CPacketRespondent/cMyTrace 各 1~2 对），无真实缺口。
//   残余两类（均结构性）：
//   ① 正常路径尾部析构块布局（N104）：ORIG 成功路径 `mov $0x1,%ebx; jmp 0x833a41b`
//      后置独立 cleanup 块 0x833a428（`lea -0x40(%ebp); call CDBMsgSenderD1Ev;
//      jmp 0x833a443`），4.4 把同一段 epilogue 就地内联 ⇒ 恰少 1 条 jmp（= gap +1 全部）。
//   ② 多实参装载序（P75/N-G 跨版本调度）：ORIG 0x833a0eb `mov -0x18(%ebp),%edx;
//      mov -0x1c(%ebp),%eax; mov %edx,0x4(%esp); mov %eax,(%esp)`（先双载再压栈）
//      vs 4.4 逐个 load-store；同族另有 SetItemId / SetExpireTime /
//      RegisterCreatureItem 各 1 处方向互反。
//   §6.2 三件套验收：call 多重集 CLEAR + gap ≤ 1 + 槽位集一致 ⇒ 语义完成、byte 层 N 封顶。
bool user_creature::CCreatureMgr::Hatch(int addInfo)
{
    // ORIG 0x8339fa4 `movl $0x0,-0x1c(%ebp)`：newItem 先预清再赋值
    // （P198① 声明=0 + 独立赋值两句式）。
    CCreatureItem* newItem = 0;
    CPacketRespondent respondent(m_user, 1, 0x69);
    std::map<int, CCreatureItem*>::iterator it = m_creatureItems.find(addInfo);
    if (it == m_creatureItems.end())
    {
        respondent.SetError(0x15);
        return false;
    }
    CEgg* egg = reinterpret_cast<CEgg*>(it->second);
    if (reinterpret_cast<CCreatureItem*>(egg)->GetCreatureItemType() != 0)
    {
        respondent.SetError(0x17);
        return false;
    }
    int creatureItemId;
    if (!egg->Hatch(&creatureItemId))
    {
        respondent.SetError(0x11);
        return false;
    }
    newItem = GetInstanceCreatureItemFactory()->AcquireCreatureItem(
        user_creature::CREATURE_ITME_TYPE_CREATURE);
    if (newItem == 0)
    {
        respondent.SetError(0x16);
        return false;
    }
    // ORIG 仅一次 _Rb_tree_iterator::operator->（P167 差集：EXTRA ptEv 清零）——
    // Set 的实参复用已取出的 egg 指针，勿二次 it->second。
    // ORIG 0x833a043 段实参经 %edx 中转 ⇒ 原码是派生指针隐式转基类（P211）；
    // 显式 reinterpret_cast 会同型化掉该中转。
    newItem->Set(egg);
    reinterpret_cast<CCreature*>(newItem)->SetItemId(creatureItemId);
    newItem->SetCreatureItemType(user_creature::CREATURE_ITME_TYPE_CREATURE);
    reinterpret_cast<CCreature*>(newItem)->SetStomach(100);

    CItem* itemScript = G_CDataManager()->find_item(creatureItemId);
    int expireTime = 0;
    // ORIG 0x833a153 起无 itemScript 判空（find_item 结果直用；P171 照抄原码遗留）。
    // 三次 getUsablePeriod / 三次 getExpirationDate 为独立调用（P48 -O0 无 CSE），
    // 禁具名缓存局部（P183）。
    if (itemScript->getUsablePeriod() == 0 && itemScript->getExpirationDate() != 0)
        expireTime = itemScript->getExpirationDate() - OS_API::GetDateTimeTick();
    else
        expireTime = OS_API::GetDateTimeTick() + itemScript->getUsablePeriod() * 0x15180;
    if (itemScript->getUsablePeriod() == 0 && itemScript->getExpirationDate() == 0)
        expireTime = 0;
    newItem->SetExpireTime(expireTime);

    UnregisterCreatureItem(reinterpret_cast<CCreatureItem*>(egg)->GetUid());
    RegisterCreatureItem(newItem, newItem->GetUid());

    // ORIG 0x833a23e 预清 -0xc + 独立赋值（P197③/P198①）；0x833a27a/0x833a28d
    // 写 `movb $0,(%eax)`（+0 = m_field0）+ `mov %edx,0x2(%eax)`（+2 = m_addInfo
    // 存新宠 itemIdx），无判空（P171 照抄）。
    Inven_Item* slotItem = 0;
    slotItem = m_user->getCurCharacInvenW()->GetInvenRef(3, newItem->GetSlotNo());
    slotItem->m_field0 = 0;
    slotItem->m_addInfo = newItem->GetItemId();

    CDBMsgSender sender((ENUM_INTERNALPACKET)0x8a, m_user);
    // ORIG 0x833a2db..a2e5：`call HatchCreature; xor $0x1,%eax; test %al,%al; je
    //   <成功块>` —— call 结果**直判**（无 eax→ebx 中转），ok 的值只在两条分支
    //   尾部写 callee-saved %ebx（0x833a345 `mov $0,%ebx` / 0x833a3ff `mov $1,%ebx`）
    //   ⇒ 原码 = `register bool ok;`（无初始化）+ `if (!call()) {失败; ok=false;} else
    //   {成功; ok=true;}`（P231/P248 裸 register 四条件全满足；禁 asm 绑定）。
    register bool ok;
    if (!sender.HatchCreature(newItem->GetUid(), 1, newItem->GetItemId()))
    {
        // P227/P221：ORIG ctor 前求值 vararg 并跨 ctor 保活 %ebx ⇒ 全表达式临时链。
        cMyTrace(__PRETTY_FUNCTION__, 0xb1a, 5)(
            "cannot write db uid(%d)\n", m_user->get_unique_id() & 0xffff);
        respondent.SetError(2);
        ok = false;
    }
    else
    {
        m_user->LogHistory("Hatch,%d,%d", newItem->GetItemId(), newItem->GetUid());
        sender.Send();
        respondent.SetOk();
        respondent.Send();
        respondent.MakeNotipacketCreatureItemList(&m_creatureItems, newItem->GetSlotNo());
        respondent.Send();
        m_user->SendUpdateItemList(CUser::eSendTarget_1, (ENUM_ITEMSPACE)7, newItem->GetSlotNo());
        ok = true;
    }
    return ok;
}
