// ---- Dimension::CheckupAllRentalItems —— T 0x0826f52c（同名报告全文转写）---
// [b28d-18 N7] 残差=callee-saved 角色轮换 + 单段 regmove（N7/N18a） | ORIG 0x0826f52c
// [b28d-solo-GlobalData] DIFF 封顶：p167 CLEAR（18/18）；残差=N7/N82 寄存器角色与槽位。
int RentalMachineDimension::CheckupAllRentalItems(
    CUser& user, const InstanceRentalSystem::RentInput& input)
{
    int inventoryNeed = 0;
    // ORIG 0x0826f53e：`begin()`（隐藏返回指针迭代器对象 @-0x1c）→ `jmp` 到底部
    // `end()`+`operator!=` 判定 → 体内 `operator*` 取元素指针存 @-0x10 ⇒
    // 源码为 const_iterator 三段 for 循环（非 size()/operator[] 下标循环）。
    for (std::vector<InstanceRentalSystem::RentInput::RentInElem>::const_iterator
             it = input.m_elems.begin();
         it != input.m_elems.end(); ++it)
    {
        const InstanceRentalSystem::RentInElem& elem = *it;
        EventRewardItemInfo info;
        if (!GetRentItemInfo(user, elem, info))
        {
            return 1;
        }
        CItem* script = G_CDataManager()->find_item(info.m_field0);
        if (script == 0)
        {
            return 1;
        }

        Inven_Item probe;
        probe.m_addInfo = info.m_field0;
        script->make_item(probe);

        if (elem.m_bEquip != 0)
        {
            if (user.getCurCharacInvenR()->tryInsertItemIntoEquipment(probe) < 0)
            {
                return 0xcb;
            }
        }
        else
        {
            // ORIG 0x0826f6fd：`shr $0x1f,%eax; test %al,%al; je <addl $0x1>`
            // ⇒ `< 0` 早退 + fall-through 自增（见 PcRoom 同名函数注释）。
            if (user.getCurCharacInvenR()->tryInsertItemIntoInventory(probe) < 0)
            {
                return 0xcb;
            }
            ++inventoryNeed;
        }
    }
    // ORIG 尾 `call; xor $1; test %al,%al; je <mov $0 岛>; mov $0xcb; jmp`
    // ⇒ if-return 形（P47/P191：三目与 if-return 块排布镜像，此处需 if 形）。
    if (!pc_room::CheckInventoryEquipmentEmptyCount(user, inventoryNeed))
    {
        return 0xcb;
    }
    return 0;
}