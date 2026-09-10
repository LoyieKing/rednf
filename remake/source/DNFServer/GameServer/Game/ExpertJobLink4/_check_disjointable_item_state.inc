void _check_disjointable_item_state(CUser* user, int slot, int invenType,
                                    Inven_Item& item, CItem** pItem,
                                    CUser* targetUser, unsigned short region)
{
    // 快照：GetInvenSlot(invenType, slot) 全量拷贝（0x3d 字节 sret）
    item = user->getCurCharacInvenR()->GetInvenSlot(invenType, slot);

    if (item.m_field1 == Inven_Item::ITEM_TYPE_CREATURE_EQUIP)
        throw static_cast<ENUM_ERROR>(0x11);   // 宠物装备不可拆

    if (!item.isEquipableItemType() || item.m_addInfo == 0)
        throw static_cast<ENUM_ERROR>(0x11);

    if (region != 0xffff)
    {
        Inven_Item ticket =
            user->getCurCharacInvenR()->GetInvenSlot(1, (int)region);
        CStackableItem* ticketObj =
            (CStackableItem*)G_CDataManager()->find_item(ticket.m_addInfo);
        if (ticketObj == 0)
            throw static_cast<ENUM_ERROR>(0x15);

        int ticketLevel = ticketObj->m_int324;   // +0x324 使用等级门槛
        if (ticketLevel == -1)
            throw static_cast<ENUM_ERROR>(1);   // ORIG：门槛 -1 直接抛 1

        // ORIG +0x21c：经 expert_job::CDisjointer::GetScript()（缓存单例）取脚本，
        // 非裸 G_CDataManager()->GetExpertJobScript(3)。
        // [b28d-solo-ExpertJobLink4 P0]：ORIG 0x81f8dbd `call GetScript;
        // mov %eax,-0x18` 有具名 script 槽（再回读调用），非内联接收者。
        STExpertJobScript* script = expert_job::CDisjointer::GetScript();
        int portableLimit =
            script->GetPortableDisjointPossibleLevel(ticketLevel);
        // ORIG +0x234：portableLimit==-1 亦抛 1（原实现漏该判据）。
        if (portableLimit == -1)
            throw static_cast<ENUM_ERROR>(1);

        CItem* itemObj = G_CDataManager()->find_item(item.m_addInfo);
        if (itemObj == 0)
            throw static_cast<ENUM_ERROR>(0x11);

        int usableLevel = itemObj->getUsableLevel();
        if (usableLevel == 0)
        {
            if (portableLimit < itemObj->get_grade())
                throw static_cast<ENUM_ERROR>(0xd);
        }
        else if (usableLevel > portableLimit)
        {
            throw static_cast<ENUM_ERROR>(0xd);
        }
    }

    // [b28d-solo-ExpertJobLink4 P198①]：ORIG 0x81f8ee3 先 `movl $0x0,-0x24`
    // 预清再赋值（两句式）。
    int err = 0;
    err = DisPatcher_DisJointItem::_checkDisjointableItem(item, targetUser);
    // [b28d-solo-ExpertJobLink4 P225]：ORIG 0x81f8f03 `cmpl $0,-0x24; setne
    // %al; test %al,%al; je`（4.1 bool 四段物化）⇒ while+break 形复现。
    while (err != 0)
    {
        throw static_cast<ENUM_ERROR>(err);
    }

    *pItem = G_CDataManager()->find_item(item.m_addInfo);
    if (*pItem == 0)
        throw static_cast<ENUM_ERROR>(0x11);
