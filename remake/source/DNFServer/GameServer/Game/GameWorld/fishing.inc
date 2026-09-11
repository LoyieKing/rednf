// [b28d-solo-GameWorld] _ZN9GameWorld7fishingEP5CUser verdict=AE 263/263（b28d-solo 本轮）
// b28d-solo 修法（四处真修）：①P167 —— `CItemList* itemList=G_CDataManager()->
// getItemList(); itemList->create_item(...)` 改直读 `G_CDataManager()->m_itemList`
// 字段（ORIG 0x086c7cf1 `mov 0xc(%eax),%eax`；getItemList 访问器 4.4 -O0 不内联
// ⇒ 旧形 p167 EXTRA 1 个调用点）；②P59/P168a —— `if (check_valid_area(...)){body}
// else{fail}` 改否定早退 `if (!check_valid_area(...)) {trace;DisConnSig;return;}`
// （ORIG `call; xor $1; test %al; je <主流程>`）；③P222 —— areaType 分支交换 then/else
// 并反条件（ORIG `cmp $0x2; je <鱼获块>` then 后置）；④P227 全表达式临时链（trace
// 实参 getCurCharacName 跨 ctor 保活 %ebx）。
// ⑤本轮新增：`if (result < 1) {失败} else {成功}` 臂序反置 —— ORIG 0x086c7db6
// `jle <失败块>`（条件取反跳 else、成功块 fall-through）⇒ 改正极性
// `if (result > 0) {成功} else {失败}`（4.4 对 `< 1` 恒产 `jg <成功>` 反向布局）。
// 现 263=263 逐条归一化全等、p167 CLEAR 45=45、check_tu_routed verdict=AE。
void GameWorld::fishing(CUser* user)
{
    // b28d-solo P59/P168a：ORIG 0x086c7b.. `call check_valid_area; xor $0x1,%eax;
    // test %al,%al; je <主流程>` ⇒ 原码是否定早退形（bool callee 取反物化）。
    if (!check_valid_area(user->getCurCharacVill(), user->get_area(false)))
    {
        // ORIG 0x086c7b.. 段：getCurCharacName 结果先落 %ebx（跨 ctor 保活）⇒
        // P227 全表达式临时链。
        cMyTrace(__PRETTY_FUNCTION__, 0xafd, 0)("user %s - check_valid_area() fail",
            user->getCurCharacName());
        user->DisConnSig((DISCONN_SIG)3, 1, 0);
        return;
    }
    {
        Village* village = getUserVillage(user);
        Area* area = NULL;
        if (village != NULL)
        {
            area = village->getArea(user->get_area(false));
        }
        PacketGuard packet;
        packet.put_header(1, 0x27);
        // b28d-solo P222：ORIG 0x086c7c22 `cmp $0x2,%eax; je <鱼获块>`（then 后置）
        // ⇒ 交换 then/else 并反条件复现（4.4 对 `!=` 生 je 跳后置块）。
        if (area->m_areaType != 2)
        {
            packet.put_byte(0);
            packet.put_byte(0x13);
            packet.finalize(true);
            user->Send(packet);
        }
        else
        {
            int fishIndex = area->take_fish();
            if (fishIndex == -1)
            {
                packet.put_byte(0);
                packet.put_byte(1);
                packet.finalize(true);
                user->Send(packet);
            }
            else
            {
                Inven_Item item;
                // ORIG 0x086c7ce9：`mov %eax,-0x6f(%ebp)`（item base = ebp-0x71）
                // ＝ 4 字节写 Inven_Item +0x02（m_addInfo），非 +0x00。
                item.m_addInfo = fishIndex;
                // b28d-solo p167 修：ORIG 0x086c7cf1 `mov 0xc(%eax),%eax` 直读
                // CDataManager::m_itemList 字段（+0x0c），无 getItemList() call
                // （该访问器 4.4 -O0 不内联 ⇒ EXTRA 1 个调用点）。
                G_CDataManager()->m_itemList->create_item((ENUM_ITEM_CREATE_TYPE)0, item, 1);
                int result = user->getCurCharacInvenW()->insertItemIntoInventory(
                    item, (eItemAddReason)5, true, true);
                // ORIG 0x086c7db6 `jle <失败块>`（条件取反跳 else、成功块 fall-through）
                // ⇒ 正极性 `if (result > 0) {成功} else {失败}`；4.4 对 `result < 1`
                // 恒产 `jg <成功>` 反向布局。
                if (result > 0)
                {
                    packet.put_byte(1);
                    packet.put_short(result);
                    packet.put_int(fishIndex);
                    // ORIG `mov -0x6a(%ebp),%eax`（item+0x07）4B、
                    // `movzwl -0x66(%ebp),%eax; movzwl %ax,%eax`（item+0x0b，
                    // 2 字节零扩展）→ 均非 +0x13 的 1 字节读。
                    packet.put_int(item.m_addInfo2);
                    packet.put_short(item.m_fieldb);
                }
                else
                {
                    packet.put_byte(0);
                    packet.put_byte(4);
                }
                packet.finalize(true);
                user->Send(packet);
                packet.clear();
                packet.put_header(0, 0x19);
                packet.put_short((unsigned short)user->get_unique_id());
                packet.put_int(fishIndex);
                packet.finalize(true);
                area->send_to_all(packet);
            }
        }
    }
}