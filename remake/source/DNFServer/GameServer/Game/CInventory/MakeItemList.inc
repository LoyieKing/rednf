// ============================================================================
// CInventory::MakeItemList（ORIG 0x084fd7b6 T，docs/class_func_reports/
// CInventory/MakeItemList.md 逐指令转写）。
// m_pParent 为空 → cMyTrace 报错返回 0；否则 interface packet 写头 0xd 后按
// INVEN_TYPE 分发：
//   EQUIP(1)：space 0 + 容量 + 计数槽；slot0/1/2 为固定货币/胜负点虚拟条目
//     （addInfo2 分别为 m_money、三币之和、win point），slot3..0x137 遍历
//     m_pEquipSlot 非空槽（IsEnchantRevisionChannel 时 fieldd 置 0、写
//     amplify type/value、make3rdChroniclePacket）；
//   AVATAR(2)：space 1，遍历 m_pAvatarSlot[0..0x68]，含剩余天数/jewel socket/
//     expansion info；
//   CREATURE(3)：space 7，遍历 m_pCreatureSlot[0..0xf1]，ITEM_TYPE 5 宠补
//     GetRemainDate。
// 各分支回填 short 计数后 finalize 返回 1。
// [§6.2 巨件核销 · b25] 无真修。P167 108/108 保持归零（0 缺 0 多）；新鲜 .o
// 726 vs ORIG 728（gap 2）。残差归因：N18a（ORIG `mov %eax,%edx` regmove 中转
// 出参 4.1 形不可造）、N82（全帧槽号平移 0x10 + push edi +4，帧 0x6c vs 0x60）、
// ORIG `lea 0x3(%eax),%edx` vs 重建 `add $0x3,%eax`（N1 族）与 4.1 尾部
// jmp 中继岛布局，判「语义完成、N 封顶」。
// ============================================================================
bool CInventory::MakeItemList(INVEN_TYPE invenType, void* out) const
{
    // b28d-solo-CInventory：728/726，P167 108/108 归零；N82/N89 封顶。
    // b28d-37 [N82/N89 封顶，ORIG 0x84f?]：同 MakeEquipList（728/726）。
    // [§6.2 闭合 · b20] P167 108/108 归零：3 处 put_binary 已对齐 ORIG 的
    // PKc（const char*）重载（去 (char*) 强转 + expansion 常量化，零扰动）。
    // 新鲜 .o 726 vs ORIG 728（gap 2）；帧 108 vs ORIG 96（+12B/3 槽）属
    // N82 槽分配残差，判「语义完成、N 封顶」。
    // b28d-tu-CInventory] P329：删具名 buf 内联调用点 ⇒ ndiff 1219→906、
    //   指令 723→**728（gap 0）**、struct 202→122，P167 108/108 CLEAR 保持。
    //   [修正版 p167 复判] 108=108，1 对 cMyTrace C2↔C1 镜像别名，豁免后零差。
    //   [§3] CInventory/MakeItemList.md 存在；本轮改动仅删具名 buf（装载序），
    //   取证以 objdump 0x084fd7b6 每调用点装载形直证（§3.2 兜底）。
    //   残 = ORIG buf 槽 store/load 4 行 + push edi/frame 0x6c vs 0x60（N82/
    //   N95）+ `lea 0x3(%eax)` N1 形 + jmp 中继岛 ⇒ 维持封顶。
    if (m_pParent == 0)
    {
        // [G8InvenPvP 2026-09-10 · P227 本件实测无效，已回滚] 改临时链后
        // ndiff 592、指令 726 均不变（本件残差 = ORIG buf 槽 store/load 4 行 +
        // push edi/帧 0x6c vs 0x60 的 N82/N95 族）⇒ 维持具名形。
        cMyTrace trace(
            __PRETTY_FUNCTION__, 0xa6b, 5);
        trace("inventory::MakeItemList m_pParent is NULL.");
        return 0;
    }

    // [b28d-tu-CInventory] P329 试验：暂删具名 buf（测 ORIG -0x34 槽形）。
    // P0：ORIG 在 cMyTrace 报错块之后 `mov 0x10(%ebp),%eax; mov %eax,-0x34(%ebp)`，
    // 全程 85 次 `mov -0x34(%ebp),%eax` 重载 ⇒ 原码有具名 out 局部（非形参直读）。
    InterfacePacketBuf* buf = (InterfacePacketBuf*)out;
    buf->put_header(0, 0xd);

    // ORIG switch(invenType)：决策树 cmp $2/$3/$1、body 落序 EQUIP→AVATAR→CREATURE
    // （fall-through 布局），default 直达 finalize。switch 形式复现块序。
    switch (invenType)
    {
    case INVEN_TYPE_EQUIP:
    {

            buf->put_byte(0);
            buf->put_short(get_inventory_capacity());
            int countIdx = buf->get_index();
            int count = 3;
            // ORIG +0x00cb..+0x00e2：count=3 槽物化 + `lea 0x3(%eax)` 二次求值后
            // put_short（固定前导 short 0x6，语义未证——物化形按 ORIG 保留）。
            buf->put_short(count + 3);

            // slot0：金币条目（addInfo=0，addInfo2=m_money）
            buf->put_short(0);
            buf->put_int(0);
            buf->put_int(m_money);
            buf->put_byte(0);
            buf->put_short(0);
            buf->put_byte(0);
            buf->put_int(0);
            buf->put_byte(0);
            buf->put_short(0);
            buf->put_byte(0);
            buf->put_packet(g_emptySlot);

            // slot1：代币条目（addInfo=1，addInfo2=coin+payCoin+eventCoin）
            buf->put_short(1);
            buf->put_int(1);
            buf->put_int(m_coin + m_payCoin + m_eventCoin);
            buf->put_byte(0);
            buf->put_short(0);
            buf->put_byte(0);
            buf->put_int(0);
            buf->put_byte(0);
            buf->put_short(0);
            buf->put_byte(0);
            buf->put_packet(g_emptySlot);

            // slot2：胜利点条目（addInfo=2，addInfo2=GetWinPoint）
            buf->put_short(2);
            buf->put_int(2);
            buf->put_int(((CUserCharacInfo*)m_pParent)->GetWinPoint());
            buf->put_byte(0);
            buf->put_short(0);
            buf->put_byte(0);
            buf->put_int(0);
            buf->put_byte(0);
            buf->put_short(0);
            buf->put_byte(0);
            buf->put_packet(g_emptySlot);

            for (int slot = 3; slot < 0x138; ++slot)
            {
                if (m_pEquipSlot[slot].m_addInfo == 0)
                    continue;
#if !remake_verify && defined(DNF_DIAG) && DNF_DIAG  // remake-only diag; off unless -DDNF_DIAG=1
                cMyTrace(__PRETTY_FUNCTION__, 0, 5)(
                    "[DNF-DIAG ITEMLIST] slot=%d id=%d found=%d cap=%d",
                    slot, m_pEquipSlot[slot].m_addInfo,
                    G_CDataManager()->find_item(m_pEquipSlot[slot].m_addInfo) != 0,
                    get_inventory_capacity());
#endif
                buf->put_short(slot);
                buf->put_int(m_pEquipSlot[slot].m_addInfo);
                buf->put_int(m_pEquipSlot[slot].m_addInfo2);
                buf->put_byte(GetIntegratedPvPItemAttr(m_pEquipSlot[slot], invenType, slot));
                buf->put_short(m_pEquipSlot[slot].m_endurance);
                buf->put_byte(m_pEquipSlot[slot].m_seal);
                // [H8InvenPvPWorld 2026-09-11 · P0] ORIG 0x84fdc3f-0x84fdc47 无槽：
                // `call G_GameWorld; mov %eax,(%esp); call IsEnchantRevisionChannel`
                // （我方曾有 world 具名 -0x44 store/load 对）⇒ 全表达式展开。
                // [H8InvenPvPWorld 2026-09-11 · P59/P222] ORIG 0x84fdc4c 是**正向**
                // `test %al,%al; je <else>`，then=put_int(0)、else=put_int(fieldd)；
                // 我方原 `if(!X)` 取反形多 `xor $0x1,%eax` 且块序倒置。
                if (G_GameWorld()->IsEnchantRevisionChannel())
                    buf->put_int(0);
                else
                    buf->put_int(m_pEquipSlot[slot].m_extendInfo);
                buf->put_byte((unsigned char)m_pEquipSlot[slot].m_amp.getAbilityType() & 0xff);
                buf->put_short(m_pEquipSlot[slot].m_amp.getAbilityValue());
                G_CDataManager()->m_itemUpgrade->make3rdChroniclePacket(
                    m_pParent, m_pEquipSlot[slot], (PacketGuard*)buf);
                buf->put_packet(m_pEquipSlot[slot]);
                ++count;
            }
            buf->put_short(countIdx, count);
            // [G8InvenPvP 子代理 G8CInvBig1 · 语义轴] ORIG 0x084fdd72 (+0x5bc)
            // `jmp 084fe2c3 <+0xb0d>`（finalize）结束 case EQUIP；原无 break 的
            // fall-through 会让 EQUIP 请求连发 AVATAR+CREATURE 包（真 bug）。
            break;
        }
    case INVEN_TYPE_AVATAR:
    {

            buf->put_byte(1);
            int countIdx = buf->get_index();
            int count = 0;
            buf->put_short(count);   // ORIG +0x05ec/+0x08db：以 count 变量传参（此刻 0）
            int nowTick = OS_API::GetDateTimeTick();
            for (int slot = 0; slot < 0x69; ++slot)
            {
                if (m_pAvatarSlot[slot].m_addInfo == 0)
                    continue;
                buf->put_short(slot);
                buf->put_int(m_pAvatarSlot[slot].m_addInfo);
                buf->put_int(m_avatarItemMgr.GetRemainDate(m_pAvatarSlot[slot].m_addInfo2, nowTick));
                buf->put_byte(m_pAvatarSlot[slot].GetItemAttr());
                buf->put_short(m_pAvatarSlot[slot].m_endurance);
                buf->put_byte(m_pAvatarSlot[slot].m_seal);
                buf->put_int(m_pAvatarSlot[slot].m_extendInfo);
                buf->put_byte(0);
                buf->put_short(0);
                ++count;
                buf->put_int(0x1e);
                buf->put_binary(m_avatarItemMgr.getJewelSocketData(m_pAvatarSlot[slot].m_addInfo2),
                0x1e);
                const char* expansion =
                    (const char*)m_avatarItemMgr.GetExpansionInfo(m_pAvatarSlot[slot].m_addInfo2);
                buf->put_int(4);
                if (expansion == 0)
                {
                    stAvatarExpansionInfo_t expInfo;
                    expInfo.init();
                    buf->put_binary((const char*)&expInfo, 4);
                }
                else
                {
                    buf->put_binary(expansion, 4);
                }
                buf->put_byte(0);
                buf->put_packet(m_pAvatarSlot[slot]);
            }
            buf->put_short(countIdx, count);
            // [G8InvenPvP 子代理 G8CInvBig1 · 语义轴] ORIG 0x084fe061 (+0x8ab)
            // `jmp 084fe2c3 <+0xb0d>`（finalize）结束 case AVATAR；同上补 break。
            break;
        }
    case INVEN_TYPE_CREATURE:
    {

            buf->put_byte(7);
            int countIdx = buf->get_index();
            int count = 0;
            buf->put_short(count);   // ORIG +0x05ec/+0x08db：以 count 变量传参（此刻 0）
            for (int slot = 0; slot < 0xf2; ++slot)
            {
                if (m_pCreatureSlot[slot].m_addInfo == 0)
                    continue;
                buf->put_short(slot);
                buf->put_int(m_pCreatureSlot[slot].m_addInfo);
                buf->put_int(m_pCreatureSlot[slot].m_addInfo2);
                buf->put_byte(m_pCreatureSlot[slot].GetItemAttr());
                buf->put_short(m_pCreatureSlot[slot].m_endurance);
                buf->put_byte(m_pCreatureSlot[slot].m_seal);
                buf->put_int(m_pCreatureSlot[slot].m_extendInfo);
                buf->put_byte(0);
                buf->put_short(0);
                if (m_pCreatureSlot[slot].m_itemType ==
                    (unsigned char)Inven_Item::ITEM_TYPE_CREATURE_EQUIP)
                {
                    int nowTick = 0;   // ORIG +0x0a50 先 0 物化再 call 回填
                    nowTick = OS_API::GetDateTimeTick();
                    buf->put_int(GetCreatureMgrR()->GetRemainDate(m_pCreatureSlot[slot].m_addInfo2, nowTick));
                }
                buf->put_byte(0);
                buf->put_packet(m_pCreatureSlot[slot]);
                ++count;
            }
            buf->put_short(countIdx, count);
        }
    }
    buf->finalize(true);
    return 1;
}