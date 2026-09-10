int GrowthEquipEvent::SendInfinityGrowthWeapon(CUser* user, const Inven_Item* item,
                                               int space, int slot) const
{
    // [D04 配方⑬ P322① 姊妹形] 判词翻案（原记 N 封顶）：ORIG 的 rewardIdx 槽 -0x4b 不是
    // 独立局部，而是 **newItem 的成员 m_addInfo**（newItem 槽基 -0x4d + 0x02）：
    //   0x080f7503 `mov %eax,-0x4b(%ebp)`（get_GrowthWeaponEventReward 返回值直存成员）、
    //   0x080f7508 `mov -0x4b(%ebp),%eax; test`（成员读）、
    //   0x080f76bd `mov -0x4b(%ebp),%eax; mov %eax,%esi`（成员读入 callee-saved）、
    //   0x080f76fa `mov -0x4b(%ebp),%edx; mov %edx,0xc(%eax)`（写回取成员）。
    // ⇒ 源码以 newItem.m_addInfo 承载 reward id（make_item 亦按该 id 构造，语义等价）。
    Inven_Item newItem;
    newItem.m_addInfo = G_CDataManager()->get_GrowthWeaponEventReward(item->m_addInfo);
    int invenType = GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)space);
    if (newItem.m_addInfo == 0)
    {
        return 0x17;
    }

    CItem* pItem = G_CDataManager()->find_item(newItem.m_addInfo);
    if (pItem != 0)
    {
        pItem->make_item(newItem);                       // vtable 槽 +0x08
        newItem.SetUpgrade(item->GetUpgrade());
        newItem.set_add_info(item->get_add_info());
        // ORIG 0x080f7572..0x080f759c：仅从源 item 覆盖
        //   +0x33 低 5 位（UpgradeSeparate）、+0xd（m_fieldd）、+0x11（m_amp 前 4 字节）。
        //
        // [D04] ORIG 0x080f7572 七条形 `movzbl 0x33(%eax),%eax; and $0x1f,%eax;
        // mov %eax,%edx; and $0x1f,%edx; movzbl -0x1a(%ebp),%eax; and $0xffffffe0,%eax;
        // or %edx,%eax; mov %al,-0x1a(%ebp)` = **5 位位域赋值**（源提取 + 目标位清 + 合并），
        // 非手工 & 掩码（手工形恒少 `mov %eax,%edx` 前的 `and $0x1f,%eax`）。
        newItem.m_upgradeSep.m_bits.m_lo = item->m_upgradeSep.m_bits.m_lo;
        // ORIG 0x080f758d 先 m_amp(+0x11) 后 m_fieldd(+0xd)；P235 匿名 union
        // 的 m_amp0Int 具名视角产 `mov 0x11(%eax),%eax; mov %eax,-0x3c(%ebp)` 直拷。
        newItem.m_amp.m_amp0Int = item->m_amp.m_amp0Int;
        newItem.m_fieldd = item->m_fieldd;
    }

    if (!user->getCurCharacInvenW()->delete_item(
            (INVEN_TYPE)invenType, slot, 1, (eItemDelReason)0x36, true))
    {
        return 0x11;
    }

    user->SendUpdateItem((CUser::eSendTarget)1, (ENUM_ITEMSPACE)space, slot);
    user->getCurCharacInvenW()->update_item((INVEN_TYPE)invenType, slot, newItem);
    user->SendUpdateItem((CUser::eSendTarget)1, (ENUM_ITEMSPACE)space, slot);

    // [D04] P200：short 表达式作普通实参直传（movswl %ax,%ebx），禁具名 timepiece 局部；
    // P52/P48：newItem.m_addInfo 先入 %esi 跨 get_acc_id 保活（ORIG 0x080f76bd
    // `mov -0x4b(%ebp),%eax; mov %eax,%esi`，语句表达式形亦已验证有效）。
    DB_EventUpdateGrowthWeaponTimepiece::makeRequest(
        user->get_acc_id(),
        static_cast<GrowthWeaponEvent*>(user->getGrowthWeaponEventdata())->m_field10,
        ({ newItem.m_addInfo; }));
    static_cast<GrowthWeaponEvent*>(user->getGrowthWeaponEventdata())->m_fieldc =
        (unsigned int)newItem.m_addInfo;
    return 0;
