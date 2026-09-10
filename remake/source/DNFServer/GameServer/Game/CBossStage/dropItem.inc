bool CBossStage::dropItem(CUser* user, char type, int slot, int count,
                          map_item& out)
{
    Inven_Item tmp;                               // ORIG 栈 Inven_Item
    // P208：ORIG 0x0814de2? 决策树 `movsbl -0x6c; cmp $2; je; cmp $2; jg; test; je;
    //   jmp; cmp $3; je; cmp $7; je` ⇒ 源形 switch(type)（if 链恒产 cmpb 直比）。
    switch (type)
    {
    case 0:
        // [b28d-solo] ORIG 0x0814de67..deb0：case 0 内 `slot==0` 直落金币输出块
        //   （jmp 814e1c1 跳过 GetInvenSlot），else 才取槽（P222 正向包裹）。
        if (slot == 0)
        {
            out.m_count = 0;              // byte +0x00 = 0
            out.m_item.m_field1 = 2;              // byte +0x11 = 2
            out.m_item.m_addInfo = 0;             // dword +0x12 = 0
            out.m_item.m_fieldb = 0;              // word +0x1b = 0
            out.m_item.set_add_info(count);
            out.m_item.ResetItemAttr();
        }
        else
            tmp = user->getCurCharacInvenR()->GetInvenSlot((INVEN_TYPE)1, slot);
        break;
    case 2:
        tmp = user->getCurCharacCargoR()->get_cargo_slot(slot);
        break;
    case 3:
        tmp = user->getCurCharacInvenR()->GetInvenSlot((INVEN_TYPE)0, slot);
        break;
    case 7:
        tmp = user->getCurCharacInvenR()->GetInvenSlot((INVEN_TYPE)3, slot);
        if ((unsigned char)tmp.m_field1 == 5)     // ORIG +1 字节==5 拒绝
            return false;
        break;
    default:
        return false;
    }

    int sellPrice = 0;                            // ORIG local_20
    // [b28d-solo] ORIG 0x0814e1c1..e1cd 公共段只判 tmp.m_addInfo==0（无 type/slot
    //   条件；case 0/slot==0 的金币输出已在 case 内完成）。
    // [b28d-solo] ORIG 0x0814e1c8 `mov -0x57(%ebp),%eax`（disp 直读 tmp+2）；
    //   指针 cast 解引用恒多 lea+add 两条。
    // [b28d-solo] ORIG 0x0814e1cd `je 814e360`（金币块在后）⇒ 主逻辑走 then、
    //   金币块落 else（P272 块序交换）。
    if (tmp.m_addInfo != 0)                       // dword@tmp+2 != 0 → 构建掉落物
    {
        const CItem* script =
            G_CDataManager()->find_item(tmp.m_addInfo);
        if (script == 0)
            return false;

        // ORIG 0x0814e212 跳表前为 `mov [S],%eax; cmp $0x5,%eax; ja; mov
        //   <A>(,%eax,4),%eax`（值始终在 %eax，无栈槽往返）⇒ P251/P261：
        //   原码是 `switch (script->GetAttachType())` 直切 call 结果；写成具名
        //   `int attachType=…; switch(attachType)` 在 4.4 必产 `cmpl $0x5,slot`
        //   + shl 索引（P209/N94 实测改类型无效）。
        // [b28d-solo] ORIG 0x0814e212 跳表（0x8b7511c：case1/2/5→814e21b 返 false；
        //   case3→814e225 查 tmp+0 字节；case0/4/default→814e23a 主流程）⇒ switch。
        switch (script->GetAttachType())
        {
        case 0:
            break;
        case 1:
        case 2:
            return false;
        case 3:
            if (!tmp.m_field0)
                return false;
            break;
        case 4:
            break;
        case 5:
            return false;
        default:
            break;
        }

        if (tmp.isEquipableItemType())
        {
            // ORIG 读装备对象 +0x234 int = CEquipItem::m_itemType（CEquipItem.h）
            const CEquipItem* equip =
                static_cast<const CEquipItem*>(script);
            if (equip->m_itemType == 0xb || equip->m_itemType < 10)
                return false;
        }

        // 构建 out（逐字段按 asm 宽度；无 memset，ORIG 逐字段写）
        out.m_count = 0;                          // byte +0x00 = 0
        out.m_item.m_field1 = tmp.m_field1;               // byte +0x11
        out.m_item.m_addInfo = tmp.m_addInfo;             // dword +0x12
        out.m_item.m_fieldd = tmp.m_fieldd;               // dword +0x1d
        // [b28d-solo] ORIG 0x0814e29a..e2b4 拷贝序 = amp+0x04/0x08/0x0c/0x00
        //   （guid 视图先、头部 int 后）⇒ 分两次具名视图赋值（P235/P247）。
        out.m_item.m_amp.m_guid = tmp.m_amp.m_guid;       // +0x04..0x0d
        out.m_item.m_amp.m_amp0Int = tmp.m_amp.m_amp0Int; // +0x00 四字节
        if (!tmp.isEquipableItemType())
            out.m_item.set_add_info(count);               // 非装备用丢弃数量
        else
            out.m_item.set_add_info(tmp.m_addInfo2);      // 装备沿用原 add_info2
        out.m_item.SetItemAttr(tmp.GetItemAttr());
        out.m_item.m_fieldb = tmp.m_fieldb;               // word +0x1b
        out.m_item.m_field0 = tmp.m_field0;               // byte +0x10
        G_Store()->GetSellItemPrice(
            const_cast<Inven_Item&>(tmp), script, (short)count, false,
            sellPrice);
    }
    else
    {
        out.m_count = 0;                  // byte +0x00 = 0
        out.m_item.m_field1 = 2;                  // byte +0x11 = 2
        out.m_item.m_addInfo = 0;                 // dword +0x12 = 0
        out.m_item.m_fieldb = 0;                  // word +0x1b = 0
        out.m_item.set_add_info(count);
        out.m_item.ResetItemAttr();
    }

    // 删除段
    // [b28d-solo] ORIG 0x0814e441..446 `movzbl -0x15; xor $1; test; je <成功>`
    //   ⇒ P158 bool 汇聚变量 + 取反早退。
    bool deleted = false;
    // [b28d-solo] ORIG 0x0814e3a7..e3d4 与入口同形决策树（cmp $2/$0/$3/$7）⇒
    //   删除段也是 switch(type)，case 书写序 0,2,3,7（叶块物理序）。
    switch (type)
    {
    case 0:
        if (slot == 0)
            deleted = (char)(user->getCurCharacInvenW()->use_money(
                count, (eMoneySubReason)6, true));
        else
            deleted = (char)(user->getCurCharacInvenW()->delete_item(
                (INVEN_TYPE)1, slot, count, (eItemDelReason)6, true));
        break;
    case 2:
        deleted = (char)(user->getCurCharacCargoW()->delete_item(
            slot, count, (eItemDelReason)0x10));
        break;
    case 3:
        deleted = (char)(user->getCurCharacInvenW()->delete_item(
            (INVEN_TYPE)0, slot, 1, (eItemDelReason)6, true));
        break;
    case 7:
        deleted = (char)(user->getCurCharacInvenW()->delete_item(
            (INVEN_TYPE)3, slot, 1, (eItemDelReason)6, true));
        break;
    default:
        return false;
    }

    if (!deleted)
        return false;
    if (type == 0 && slot == 0)
        GetInstanceValueStatistic()->AddValueStatistic(
            (VALUE_STATISTIC_FIELD)0x17, user, (unsigned int)count);
    else
        GetInstanceValueStatistic()->AddValueStatistic(
            (VALUE_STATISTIC_FIELD)0x16, user, (unsigned int)sellPrice);
    return true;
