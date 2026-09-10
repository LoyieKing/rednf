bool AvatarConvertServer::Convert(CUser* user,
                                  arad::SigAradAvatarConvert* sig)
{
    CItem* item = G_CDataManager()->find_item(sig->m_itemId);
    if (item == 0 || !item->IsAvatarItem())
    {
        ARAD::PRINT_ERROR::find_item("AVATAR CONVERT", user, sig->m_itemId);
        return false;
    }

    int grade = item->get_grade();
    if (item->GetItemType() == 9)
        grade = 3;

    unsigned int selectIdx = sig->m_selectNo;
    ARAD::SCRIPT::AvatarConvert::ConvertResult* result =
        getConvertResult(grade);
    if (result->m_field20 != true)
    {
        // upper_bound(cumWeights, rand(totalWeight)) - begin
        // P0/P183：ORIG 无 cw 引用局部——每处使用点内联 result->m_cumWeights。
        // ORIG 亦无 count 局部：`mov %eax,-0x40(%ebp)` 直接写回 selectIdx。
        int randVal = get_rand_int(result->m_totalWeight);
        std::vector<int>::iterator up =
            std::upper_bound(result->m_cumWeights.begin(),
                             result->m_cumWeights.end(), randVal);
        selectIdx = (unsigned int)(up - result->m_cumWeights.begin());
    }
    // P157：ORIG 0x08192888/0x081928a5 每字段重调 operator[]（无 info 引用缓存），
    // 且后续统一复用已写回的 sig->m_resultItemId。
    sig->m_resultItemId = result->m_infos[selectIdx].m_key;
    sig->m_resultCount = result->m_infos[selectIdx].m_count;

    CItem* resultItem = G_CDataManager()->find_item(sig->m_resultItemId);
    if (resultItem == 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24f, 0x11);
        ARAD::PRINT_ERROR::find_item("AVATAR CONVERT", user, sig->m_itemId);
        return false;
    }

    // ORIG 0x08192919-0x0819294e：newItem 先构造并写 m_addInfo（+0x2），
    // stackable 校验通过后才写 m_addInfo2（+0x7，真语义 bug：原实现漏写两字段）。
    Inven_Item newItem;
    newItem.m_addInfo = sig->m_resultItemId;
    // 结果物品入包（stackable 校验 → make_item → tryInsertItemIntoInventory）
    if (!resultItem->is_stackable())
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24f, 0x11);
        LogManager::logFormat(
            1, "localjapan/Arad_EmblemReformServer.cpp",
            __PRETTY_FUNCTION__,
            0x322,
            "AVATAR CONVERT : is_stackable() fail [item_id:%d] [charac_no:%d]",
            sig->m_resultItemId, (unsigned int)user->getCurCharacNo());
        return false;
    }
    newItem.m_addInfo2 = sig->m_resultCount;
    resultItem->make_item(newItem);          // vtable slot2
    // P0/P160：ORIG 无 invW 具名局部——5 处使用点各自重新取
    // user->getCurCharacInvenW()（p167 差集 MISSING×4）。
    int insertSlot =
        user->getCurCharacInvenW()->tryInsertItemIntoInventory(newItem);
    if (insertSlot < 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24f, 4);
        LogManager::logFormat(
            1, "localjapan/Arad_EmblemReformServer.cpp",
            __PRETTY_FUNCTION__,
            0x329,
            "AVATAR CONVERT : tryInsertItemIntoInventory() fail [item_id:%d] [charac_no:%d]",
            sig->m_resultItemId, (unsigned int)user->getCurCharacNo());
        return false;
    }
    // 释放原 avatar（type 2 槽删除 → UnRegistItem → SendAvatarEvent → DB history）
    // ORIG 0x08192b14/0x08192b28：读路径走 const 版
    // getCurCharacInvenR()->GetInvenRef（p167 MISSING const 版两符号）。
    // b29（判据⑫/clr_l，ORIG 0x08192bc7-0x08192c92 三处 `movzwl %ax,%ebx`）：
    // m_slot 三处零扩展收窄（原来写 (int) 生 movswl ⇒ ext_movswl 3 vs 0）。
    const Inven_Item* oldRef =
        user->getCurCharacInvenR()->GetInvenRef(
            2, (int)(unsigned short)sig->m_slot);
    unsigned int uiId = oldRef->m_addInfo2;
    if (user->getCurCharacInvenW()->delete_item(INVEN_TYPE(2),
                                                (int)(unsigned short)sig->m_slot, 1,
                                                eItemDelReason(6), true) != 1)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24f, 0x16);
        ARAD::PRINT_ERROR::delete_item("AVATAR CONVERT", user,
                                       (unsigned short)sig->m_slot);
        return false;
    }
    if (!user->getCurCharacInvenW()->GetAvatarItemMgrW()
             ->UnRegistItem((int)uiId))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24f, 0x16);
        LogManager::logFormat(
            1, "localjapan/Arad_EmblemReformServer.cpp",
            __PRETTY_FUNCTION__,
            0x33c,
            "AVATAR CONVERT : UnRegistItem() fail [ui_id:%d] [charac_no:%d]",
            uiId, (unsigned int)user->getCurCharacNo());
        return false;
    }
    user->getCurCharacInvenW()->SendAvatarEvent(0x36, (int)uiId,
                                                (int)(unsigned short)sig->m_slot + 10,
                                                5, 0);
    DB_UpdateAvatarHistory::makeRequest(user->getCurCharacNo(), (int)uiId,
                                        (char*)"AVATAR CONVERT");

    // 结果物品经事件发放 + 通知包
    std::vector<std::pair<int, int> > items;
    std::vector<int> outSlots;
    // ORIG 0x08192cf2：make_pair<unsigned long&, short&>（两成员均为左值，
    // 字段 m_resultItemId 为 unsigned long）→ pair<int,int> 转换构造。
    items.push_back(std::make_pair(sig->m_resultItemId, sig->m_resultCount));
    int retSlots = user->getCurCharacInvenW()->insert_event_items(
        items, outSlots, eMoneyAddReason(0), eItemAddReason(10), false,
        "AVATAR CONVERT", "AVATAR CONVERT");

    PacketGuard packet;
    packet.clear();
    packet.put_header(1, 0x24f);
    packet.put_byte(1);
    packet.put_short((unsigned short)sig->m_slot);
    packet.put_short(retSlots);
    // ORIG 用 const_iterator（p167 MISSING ppEi/deEv 的 const 族）+
    // 语句位后置 ++（4.4 保留 ppEi）。
    // b29（判据⑫，ORIG 0x08192e21/0x08192e28/0x08192e2f 三条 `movl $0x0,-0x28/-0x24/-0x20`）：
    // 三个具名 int 局部（=0 声明）在循环前，循环内赋值（原来在循环内声明 ⇒ clr_l 0 vs 3、
    // 帧少 0xc）。
    int slotIdx = 0;
    int slotOut = 0;
    int extra = 0;
    for (std::vector<int>::const_iterator s = outSlots.begin();
         s != outSlots.end(); s++)
    {
        slotIdx = *s;
        packet.put_short(slotIdx);
        s++;
        slotOut = *s;
        packet.put_int(slotOut);
        s++;
        extra = *s;
        packet.put_int(extra);
        cMyTrace trace(
            __PRETTY_FUNCTION__,
            0x368, 0);
        trace("AVATAR CONVERT : success [slot:%d] [selectNo:%d] [resultIdx:%d] "
              "[charac_no:%d]",
              (int)(unsigned short)sig->m_slot,
              (int)(unsigned short)sig->m_selectNo, slotOut,
              (unsigned int)user->getCurCharacNo());
    }
    packet.finalize(true);
    user->Send(packet);
    // b28c-9 已把原成员 m_script 改为首个非主基类（ARAD::SCRIPT::AvatarRecharge
    // 落 +0x04），此处改走基类引用（ORIG 0x08190200 布局：+0x00 vptr / +0x04 脚本体）。
    bool isRechargeStone =
        static_cast<ARAD::SCRIPT::AvatarRecharge&>(
            *ARAD::Singleton<AvatarRechargeServer>::Get())
            .IsEmblemRechargeStone(sig->m_resultItemId);
    StatisticsAvatarDisjoint(item, isRechargeStone ? 1 : 0);

    // ORIG 0x0819301d：统计实参复用 items（无独立 created vector——
    // p167 EXTRA 的 vector<pair> C1Ev/D1Ev + push_back 即该多建对象）。
    StatisticsEmblemCreate(items);
    return true;
