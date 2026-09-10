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
