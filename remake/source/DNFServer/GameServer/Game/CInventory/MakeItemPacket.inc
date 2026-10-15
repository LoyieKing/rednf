// ============================================================================
// CInventory::MakeItemPacket（ORIG 0x084fc6bc，0x1015 字节 / 1073 insn）。
// [N82 封顶标注] batch-10：gap 242→1（1072 vs 1073），ORIG 调用点与 .o R_386_PC32
// 调用多重集 149=149 零差集；残余 1 条 = ORIG 远跳 epilogue 共享多 1 个 `jmp`
// （trace 块布局差异，§6.1/§6.2），另有 add↔lea、shl↔sar、je↔jne 等 N82 形态别名。
// 判据：语义完成、byte 层 N 封顶，勿再按 ndiff 迭代。
// 按 INVEN_TYPE 分 4 组槽位发包：EQUIP(1) 含 0/1/2 三个特殊槽（0=money、
// 1=coin+payCoin+eventCoin、2=GetWinPoint）与普通装备槽；AVATAR(2) 走
// m_pAvatarSlot（含 jewel socket / expansion info）；CREATURE(3) 走
// m_pCreatureSlot（ITEM_TYPE 5 宠时补 GetRemainDate）；INVEN(0) 走
// m_invenItem[slot]（含 PvP 综合通道/封印数分支、avatar 扩展、宠物日期、
// make3rdChroniclePacket）。空槽 / 非期望类型返回 false。
// 依赖：CCreatureMgr::GetRemainDate（头已声明，BLOCKED——producer 提供定义）。
// ============================================================================
bool CInventory::MakeItemPacket(INVEN_TYPE invenType, int slot, PacketGuard& packet) const
{
    // b28d-solo-CInventory：1073/1072，P167 CLEAR；N82/N89 封顶。
    // [b28d-tu-CInventory] P329 大捷：删具名引用 `InterfacePacketBuf& buf`
    //   （98 处调用点内联 cast，ast_edit）⇒ 4.4 与 ORIG 0x084fc6bc 同形
    //   「每调用点 `mov 0x14(%ebp),%eax` 形参直读 + eax 先装载」，
    //   ndiff 1329→567、**struct 505→113**，P167 149/149 CLEAR 保持。
    //   方法3（case 书写序改 EQUIP→AVATAR→INVEN→CREATURE 追 ORIG 叶块物理序
    //   6f5/d93/29c/063）实测**反劣** struct 113→668/ndiff 567→789，已回滚
    //   ⇒ 4.4 密集值 switch 落体不跟随 ORIG 比较树，方法3 对本件不适用。
    //   残 113 = 帧 0x80/0x70（−16B/4 槽 N95）+ return-1 tail-merge 岛（N89）
    //   + shl/sar·je/jne 跨版本别名。
    // b28d-37 [N82/N89 封顶，ORIG 0x84fc6bc]：P183 去视图缓存已应用；P167 调用点 CLEAR；1073/1072。
    // [§6.2 闭合 · b20] P167 149/149 归零；ORIG 在本 const 方法内走
    // put_binary(const char*,int)（PKc 重载），expansion/expInfo 已常量化对齐
    // 该 call 目标（同宽同槽，零扰动）。新鲜 .o 1072 vs ORIG 1073（gap 1）。
    // [b28d-tu-CInventory] P329：删具名引用 `buf`（4.4 为其发 -0x24 槽 +
    //   prologue store/load，ORIG 0x084fc6bc 全部 98 处 this 装载直读形参槽
    //   `mov 0x14(%ebp),%eax`）⇒ 调用点改内联 cast 形（ast_edit 98 处）。
    // 本方法为 const；ORIG 0x084fc6bc 对 expansion/expInfo 走 put_binary(const
    // char*,int)（PKc 重载），故指针常量化（同 4B 槽，仅换 call 目标，零扰动）。
    const char* expansion;

    // ORIG(-O0) 在每次字段访问前完整重物化数组元素地址（无指针缓存），以下各
    // case 用 m_pXxx[slot].field 全索引形式逐段对应 0x084fc6bc 起各 put 链。
    switch (invenType)
    {
    case INVEN_TYPE_EQUIP:
        ((InterfacePacketBuf&)packet).put_short(slot);
        if (slot == 0)
        {
            ((InterfacePacketBuf&)packet).put_int(0);
            ((InterfacePacketBuf&)packet).put_int(m_money);
            ((InterfacePacketBuf&)packet).put_byte(0);
            ((InterfacePacketBuf&)packet).put_short(0);
            ((InterfacePacketBuf&)packet).put_byte(0);
            ((InterfacePacketBuf&)packet).put_int(0);
            ((InterfacePacketBuf&)packet).put_byte(0);
            ((InterfacePacketBuf&)packet).put_short(0);
            ((InterfacePacketBuf&)packet).put_byte(0);
            ((InterfacePacketBuf&)packet).put_packet(g_emptySlot);
            goto success;
        }
        if (slot == 1)
        {
            ((InterfacePacketBuf&)packet).put_int(1);
            ((InterfacePacketBuf&)packet).put_int(m_coin + m_payCoin + m_eventCoin);
            ((InterfacePacketBuf&)packet).put_byte(0);
            ((InterfacePacketBuf&)packet).put_short(0);
            ((InterfacePacketBuf&)packet).put_byte(0);
            ((InterfacePacketBuf&)packet).put_int(0);
            ((InterfacePacketBuf&)packet).put_byte(0);
            ((InterfacePacketBuf&)packet).put_short(0);
            ((InterfacePacketBuf&)packet).put_byte(0);
            ((InterfacePacketBuf&)packet).put_packet(g_emptySlot);
            goto success;
        }
        if (slot == 2)
        {
            ((InterfacePacketBuf&)packet).put_int(2);
            ((InterfacePacketBuf&)packet).put_int(m_pParent->GetWinPoint());
            ((InterfacePacketBuf&)packet).put_byte(0);
            ((InterfacePacketBuf&)packet).put_short(0);
            ((InterfacePacketBuf&)packet).put_byte(0);
            ((InterfacePacketBuf&)packet).put_int(0);
            ((InterfacePacketBuf&)packet).put_byte(0);
            ((InterfacePacketBuf&)packet).put_short(0);
            ((InterfacePacketBuf&)packet).put_byte(0);
            ((InterfacePacketBuf&)packet).put_packet(g_emptySlot);
            goto success;
        }
        if (m_pEquipSlot[slot].m_addInfo != 0)
        {
            ((InterfacePacketBuf&)packet).put_int(m_pEquipSlot[slot].m_addInfo);
            ((InterfacePacketBuf&)packet).put_int(m_pEquipSlot[slot].m_addInfo2);
            // [R18a10 2026-09-13 · ④ 语义修复（防再犯）] ORIG 0x084fca0f
            // `mov 0xc(%ebp),%eax` + 0x084fca12 `mov %eax,0x4(%esp)` ⇒ 第 2 实参是
            // **形参 invenType**（非常量）。历史 [H8InvenPvPWorld 2026-09-11] 为追
            // 机器码（1073→1075、fndiff 346）把实参写死 INVEN_TYPE_EQUIP(=1) ⇒ 调用点
            // 传 {0,2,3} 者（CUser::SendUpdateItemList 2/3/0、CMailBoxHelper::GetPackege
            // 2/3、CBossTower::InsertRewardItems、CUser::deleteSpecificItems、
            // CParty::onBloodRewardTimer）会使 GetIntegratedPvPItemAttr 内部
            // switch(invenType) 走错分支 ⇒ 出包物品属性字节错。语义优先（用户裁定：
            // 完全复刻 ORIG 逻辑）⇒ 改回形参。ORIG 另一处 0x084fcbf0 确为常量 1，
            // 与我方一致，不动。
            ((InterfacePacketBuf&)packet).put_byte(GetIntegratedPvPItemAttr(m_pEquipSlot[slot], invenType, slot));
            ((InterfacePacketBuf&)packet).put_short(m_pEquipSlot[slot].m_endurance);
            ((InterfacePacketBuf&)packet).put_byte(m_pEquipSlot[slot].m_seal);
            // [H8InvenPvPWorld 2026-09-11 · P0] ORIG 0x084fca7f-0x084fca87 无槽：
            // `call G_GameWorld; mov %eax,(%esp); call IsEnchantRevisionChannel`
            // （我方曾有 -0x20 store/load 对 ×4）⇒ 删具名 world 全表达式展开。
            if (G_GameWorld()->IsEnchantRevisionChannel())
                ((InterfacePacketBuf&)packet).put_int(0);
            else
                ((InterfacePacketBuf&)packet).put_int(m_pEquipSlot[slot].m_extendInfo);
            ((InterfacePacketBuf&)packet).put_byte(m_pEquipSlot[slot].m_amp.getAbilityType());
            ((InterfacePacketBuf&)packet).put_short(m_pEquipSlot[slot].m_amp.getAbilityValue());
            G_CDataManager()->m_itemUpgrade->make3rdChroniclePacket(
                m_pParent, m_pEquipSlot[slot], &packet);
            ((InterfacePacketBuf&)packet).put_packet(m_pEquipSlot[slot]);
            goto success;
        }
        // ORIG 0x084fcd43：外层已判 ==0，此处重复同条件 → trace(0x758) return 0。
        // 静态不可达但 ORIG -O0 实际发射，保留以对齐逐块结构。
        if (m_pEquipSlot[slot].m_addInfo != 0)
        {
            // [G8InvenPvP 2026-09-10 · P227 实测无效（已回滚）] 本 4 处改
            // `cMyTrace(...)(...)` 全表达式临时链后 ndiff 仍 184、指令 1081 不变
            // ⇒ 本件残差属 P183 已收敛后的 N82 帧/槽族，非 trace 具名式所致，
            // 维持原具名形。
            cMyTrace trace(__PRETTY_FUNCTION__, 0x758, 5);
            trace("[%s][%d]", __PRETTY_FUNCTION__, 0x758);
            return false;
        }
        ((InterfacePacketBuf&)packet).put_item_idx(0xffffffff);
        ((InterfacePacketBuf&)packet).put_int(m_pEquipSlot[slot].m_addInfo2);
        ((InterfacePacketBuf&)packet).put_byte(GetIntegratedPvPItemAttr(m_pEquipSlot[slot], INVEN_TYPE_EQUIP, slot));
        ((InterfacePacketBuf&)packet).put_short(m_pEquipSlot[slot].m_endurance);
        ((InterfacePacketBuf&)packet).put_byte(m_pEquipSlot[slot].m_seal);
        if (G_GameWorld()->IsEnchantRevisionChannel())
            ((InterfacePacketBuf&)packet).put_int(0);
        else
            ((InterfacePacketBuf&)packet).put_int(m_pEquipSlot[slot].m_extendInfo);
        ((InterfacePacketBuf&)packet).put_byte(m_pEquipSlot[slot].m_amp.getAbilityType());
        ((InterfacePacketBuf&)packet).put_short(m_pEquipSlot[slot].m_amp.getAbilityValue());
        ((InterfacePacketBuf&)packet).put_byte(0);
        ((InterfacePacketBuf&)packet).put_packet(m_pEquipSlot[slot]);
        goto success;

    case INVEN_TYPE_AVATAR:
        if (m_pAvatarSlot[slot].m_addInfo == 0)
        {
            cMyTrace trace(__PRETTY_FUNCTION__, 0x7a0, 5);
            trace("[%s][%d]", __PRETTY_FUNCTION__, 0x7a0);
            return false;
        }
        {
            int nowTick = 0;
            ((InterfacePacketBuf&)packet).put_short(slot);
            ((InterfacePacketBuf&)packet).put_int(m_pAvatarSlot[slot].m_addInfo);
            nowTick = OS_API::GetDateTimeTick();
            ((InterfacePacketBuf&)packet).put_int(m_avatarItemMgr.GetRemainDate(m_pAvatarSlot[slot].m_addInfo2,
                                                      nowTick));
            ((InterfacePacketBuf&)packet).put_byte(m_pAvatarSlot[slot].GetItemAttr());
            ((InterfacePacketBuf&)packet).put_short(m_pAvatarSlot[slot].m_endurance);
            ((InterfacePacketBuf&)packet).put_byte(m_pAvatarSlot[slot].m_seal);
            ((InterfacePacketBuf&)packet).put_int(m_pAvatarSlot[slot].m_extendInfo);
            ((InterfacePacketBuf&)packet).put_byte(0);
            ((InterfacePacketBuf&)packet).put_short(0);
            ((InterfacePacketBuf&)packet).put_int(0x1e);
            ((InterfacePacketBuf&)packet).put_binary(m_avatarItemMgr.getJewelSocketData(m_pAvatarSlot[slot].m_addInfo2),
                           0x1e);
            expansion = (const char*)m_avatarItemMgr.GetExpansionInfo(m_pAvatarSlot[slot].m_addInfo2);
            ((InterfacePacketBuf&)packet).put_int(4);
            if (expansion != 0)
            {
                ((InterfacePacketBuf&)packet).put_binary(expansion, 4);
            }
            else
            {
                stAvatarExpansionInfo_t expInfo;
                expInfo.init();
                ((InterfacePacketBuf&)packet).put_binary((const char*)&expInfo, 4);
            }
            ((InterfacePacketBuf&)packet).put_byte(0);
            ((InterfacePacketBuf&)packet).put_packet(m_pAvatarSlot[slot]);
            goto success;
        }

    case INVEN_TYPE_CREATURE:
        if (m_pCreatureSlot[slot].m_addInfo == 0)
        {
            cMyTrace trace(__PRETTY_FUNCTION__, 0x801, 5);
            trace("[%s][%d]", __PRETTY_FUNCTION__, 0x801);
            return false;
        }
        {
            int nowTick = 0;
            ((InterfacePacketBuf&)packet).put_short(slot);
            ((InterfacePacketBuf&)packet).put_int(m_pCreatureSlot[slot].m_addInfo);
            ((InterfacePacketBuf&)packet).put_int(m_pCreatureSlot[slot].m_addInfo2);
            ((InterfacePacketBuf&)packet).put_byte(m_pCreatureSlot[slot].GetItemAttr());
            ((InterfacePacketBuf&)packet).put_short(m_pCreatureSlot[slot].m_endurance);
            ((InterfacePacketBuf&)packet).put_byte(m_pCreatureSlot[slot].m_seal);
            ((InterfacePacketBuf&)packet).put_int(m_pCreatureSlot[slot].m_extendInfo);
            ((InterfacePacketBuf&)packet).put_byte(0);
            ((InterfacePacketBuf&)packet).put_short(0);
            if (m_pCreatureSlot[slot].m_itemType ==
                (unsigned char)Inven_Item::ITEM_TYPE_CREATURE_EQUIP)
            {
                nowTick = OS_API::GetDateTimeTick();
                ((InterfacePacketBuf&)packet).put_int(GetCreatureMgrR()->GetRemainDate(
                    m_pCreatureSlot[slot].m_addInfo2, nowTick));
            }
            ((InterfacePacketBuf&)packet).put_byte(0);
            ((InterfacePacketBuf&)packet).put_packet(m_pCreatureSlot[slot]);
            goto success;
        }

    // [b28d-tu-CInventory] 方法3 实测**反劣已回滚**：本件 switch 改书写序
    //   EQUIP→AVATAR→INVEN→CREATURE（ORIG 叶块物理序 6f5/d93/29c/063）⇒
    //   struct 113→668、ndiff 567→789 双反劣（4.4 对 0..3 密集值 switch 的
    //   落体布局不跟随 ORIG 比较树，方法3 对本件不适用）⇒ 保持原书写序。
    case INVEN_TYPE_INVEN:
        if (m_invenItem[slot].m_addInfo == 0)
        {
            cMyTrace trace(__PRETTY_FUNCTION__, 0x884, 5);
            trace("[%s][%d]", __PRETTY_FUNCTION__, 0x884);
            return false;
        }
        ((InterfacePacketBuf&)packet).put_short(slot);
        ((InterfacePacketBuf&)packet).put_int(m_invenItem[slot].m_addInfo);
        ((InterfacePacketBuf&)packet).put_int(m_invenItem[slot].m_addInfo2);
        if (G_GameWorld()->IsIntegratedPvPBaseChannel() &&
            G_CDataManager()->GetUpgradeRevisionPvPChannel() == 0)
        {
            unsigned char attr = 0;
            if ((unsigned int)slot <= 0x15u && (((1 << slot) & 0x3ff400) != 0))
                attr = (unsigned char)(m_invenItem[slot].GetReSealCount() << 5);
            else
                attr = (unsigned char)m_invenItem[slot].GetItemAttr();
            ((InterfacePacketBuf&)packet).put_byte(attr);
        }
        else
        {
            ((InterfacePacketBuf&)packet).put_byte(m_invenItem[slot].GetItemAttr());
        }
        ((InterfacePacketBuf&)packet).put_short(m_invenItem[slot].m_endurance);
        ((InterfacePacketBuf&)packet).put_byte(m_invenItem[slot].m_seal);
        if (G_GameWorld()->IsEnchantRevisionChannel())
            ((InterfacePacketBuf&)packet).put_int(0);
        else
            ((InterfacePacketBuf&)packet).put_int(m_invenItem[slot].m_extendInfo);
        ((InterfacePacketBuf&)packet).put_byte(m_invenItem[slot].m_amp.getAbilityType());
        ((InterfacePacketBuf&)packet).put_short(m_invenItem[slot].m_amp.getAbilityValue());
        if (m_invenItem[slot].isAvatarItemType())
        {
            ((InterfacePacketBuf&)packet).put_int(0x1e);
            ((InterfacePacketBuf&)packet).put_binary(m_avatarItemMgr.getJewelSocketData(m_invenItem[slot].m_addInfo2),
                           0x1e);
            expansion = (const char*)m_avatarItemMgr.GetExpansionInfo(m_invenItem[slot].m_addInfo2);
            ((InterfacePacketBuf&)packet).put_int(4);
            if (expansion != 0)
            {
                ((InterfacePacketBuf&)packet).put_binary(expansion, 4);
            }
            else
            {
                stAvatarExpansionInfo_t expInfo;
                expInfo.init();
                ((InterfacePacketBuf&)packet).put_binary((const char*)&expInfo, 4);
            }
        }
        if (m_invenItem[slot].m_itemType == (unsigned char)Inven_Item::ITEM_TYPE_CREATURE_EQUIP)
        {
            int nowTick = 0;
            nowTick = OS_API::GetDateTimeTick();
            ((InterfacePacketBuf&)packet).put_int(GetCreatureMgrR()->GetRemainDate(m_invenItem[slot].m_addInfo2,
                                                         nowTick));
        }
        G_CDataManager()->m_itemUpgrade->make3rdChroniclePacket(
            m_pParent, m_invenItem[slot], &packet);
        ((InterfacePacketBuf&)packet).put_packet(m_invenItem[slot]);
        goto success;

    default:
        return false;
    }
success:
    return true;
}