int Dispatcher_Enchant_3rdChronicleItem::check_error(CUser* user, MSG_BASE& msg)
{
    // [b28c-34] N18a 封顶：ORIG 0x081d34f2 段 `mov 0x15(%eax),%eax; mov %eax,%ebx;
    // call G_CDataManager` 为 4.1 两段（load→regmove 到 callee-saved）；4.4 恒折
    // 单段 `mov 0x15(%eax),%ebx`（差 1 条，后续全偏移 -2）。源侧具名局部/cast/
    // 语句表达式均不产两段（N18a 反向不可造）。语义等价。
    // 仅允许 state==3 且当前角色有效；否则错误码 7
    if (!(user->get_state() == 3 && user->getCurCharacR() != 0))
    {
        return 7;
    }

    MSG_ENCHANT& req = reinterpret_cast<MSG_ENCHANT&>(msg);

    const int secu =
        GlobalData::s_pSecuProtectionField->Check(user, (SECURITY_PROTCTION)0x27);
    if (secu != 0)
    {
        return secu;
    }

    if (user->CheckItemLock(1, (unsigned short)req.m_0d))
    {
        return 0xd5;
    }

    // msg+0x15：请求强化的物品 idx → 物品脚本必须存在
    // [b28d-solo-Dispatchers] P211 档③/P254：ORIG 0x081d34e2 `mov 0x15(%eax),%eax;
    //   mov %eax,%ebx`（两段）为「视图字段 unsigned + find_item(int) 形参异型」
    //   的转换中转形；改用 unsigned 视图字段内联实参（探针 /tmp/probe_disp.cpp t3）。
    struct __attribute__((packed)) EnchantReqView
    {
        char m_head15[0x15];
        unsigned int m_itemIdx;       // +0x15
    };
    CItem* itemScript = G_CDataManager()->find_item(
        reinterpret_cast<const EnchantReqView&>(req).m_itemIdx);
    if (itemScript == 0)
    {
        return 4;
    }

    CInventory* invenW = user->getCurCharacInvenW();
    if (invenW == 0)
    {
        return 4;
    }

    // ORIG Inven_Item ctor @0x080cb854 后整体拷贝 GetInvenSlot 结果
    Inven_Item slotItem;
    slotItem = invenW->GetInvenSlot(INVEN_TYPE_EQUIP, (unsigned short)req.m_13);

    // 背包槽内物品须与请求脚本一致，否则 0x17
    if (slotItem.m_addInfo != itemScript->get_index())
    {
        return 0x17;
    }
    return 0;
