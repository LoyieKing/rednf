// ---- dropItem（ORIG 0x0814de1a）----
// 按 invenType 取出物品构建掉落 map_item，再从原容器删除并记流通统计。
// type：0=背包/金币、2=仓库、3=装备栏、7=造物。
// [b28d-62] N82：p167 CLEAR。ORIG 0x0814de1a
// [b28d-solo-CBossStage] N82：p167 CLEAR。if 链改 switch(type)（P208 决策树）。
//   残差=槽/帧分区 + case 0 金币块内联结构（ORIG 金币输出在 case 0 内并 jmp 删除段）。
// [b28d-solo-CBossStage] 封顶 N-switch跳表+N76：case 序改 0,2,3,7（P46 叶块物理
//   序）、case 0 内嵌金币块+公共段只判 tmp.m_addInfo（P222）、attachType 改
//   switch（跳表 0x8b7511c）且 case3 用 `!tmp.m_seal`（P248 档②）、删除段改
//   switch+去三目+deleted bool（P158）、m_amp 按 guid/head 视图序、out+0x00 写
//   m_count 后 169→12；残差=4.4 对 6-case 用二分树 vs ORIG 跳表（跨版本 switch
//   展开，N）+m_amp0Int store 序（this-first vs value-first，P211 旋钮已穷举）。
//   p167 CLEAR。
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
            out.m_item.m_itemType = 2;              // byte +0x11 = 2
            out.m_item.m_addInfo = 0;             // dword +0x12 = 0
            out.m_item.m_endurance = 0;              // word +0x1b = 0
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
        if ((unsigned char)tmp.m_itemType == 5)     // ORIG +1 字节==5 拒绝
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
            if (!tmp.m_seal)
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
        out.m_item.m_itemType = tmp.m_itemType;               // byte +0x11
        out.m_item.m_addInfo = tmp.m_addInfo;             // dword +0x12
        out.m_item.m_extendInfo = tmp.m_extendInfo;               // dword +0x1d
        // [b28d-solo] ORIG 0x0814e29a..e2b4 拷贝序 = amp+0x04/0x08/0x0c/0x00
        //   （guid 视图先、头部 int 后）⇒ 分两次具名视图赋值（P235/P247）。
        out.m_item.m_amp.m_guid = tmp.m_amp.m_guid;       // +0x04..0x0d
        out.m_item.m_amp.m_amp0Int = tmp.m_amp.m_amp0Int; // +0x00 四字节
        if (!tmp.isEquipableItemType())
            out.m_item.set_add_info(count);               // 非装备用丢弃数量
        else
            out.m_item.set_add_info(tmp.m_addInfo2);      // 装备沿用原 add_info2
        out.m_item.SetItemAttr(tmp.GetItemAttr());
        out.m_item.m_endurance = tmp.m_endurance;               // word +0x1b
        out.m_item.m_seal = tmp.m_seal;               // byte +0x10
        G_Store()->GetSellItemPrice(
            const_cast<Inven_Item&>(tmp), script, (short)count, false,
            sellPrice);
    }
    else
    {
        out.m_count = 0;                  // byte +0x00 = 0
        out.m_item.m_itemType = 2;                  // byte +0x11 = 2
        out.m_item.m_addInfo = 0;                 // dword +0x12 = 0
        out.m_item.m_endurance = 0;                  // word +0x1b = 0
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
}