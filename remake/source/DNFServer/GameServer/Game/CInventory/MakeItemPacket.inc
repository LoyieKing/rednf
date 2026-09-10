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
    GameWorld* world;
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
            return true;
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
            return true;
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
            return true;
        }
        if (m_pEquipSlot[slot].m_addInfo != 0)
        {
            ((InterfacePacketBuf&)packet).put_int(m_pEquipSlot[slot].m_addInfo);
            ((InterfacePacketBuf&)packet).put_int(m_pEquipSlot[slot].m_addInfo2);
            ((InterfacePacketBuf&)packet).put_byte(GetIntegratedPvPItemAttr(m_pEquipSlot[slot], INVEN_TYPE_EQUIP, slot));
            ((InterfacePacketBuf&)packet).put_short(m_pEquipSlot[slot].m_fieldb);
            ((InterfacePacketBuf&)packet).put_byte(m_pEquipSlot[slot].m_field0);
            world = G_GameWorld();
            if (world->IsEnchantRevisionChannel())
                ((InterfacePacketBuf&)packet).put_int(0);
            else
                ((InterfacePacketBuf&)packet).put_int(m_pEquipSlot[slot].m_fieldd);
            ((InterfacePacketBuf&)packet).put_byte(m_pEquipSlot[slot].m_amp.getAbilityType());
            ((InterfacePacketBuf&)packet).put_short(m_pEquipSlot[slot].m_amp.getAbilityValue());
            G_CDataManager()->m_itemUpgrade->make3rdChroniclePacket(
                m_pParent, m_pEquipSlot[slot], &packet);
            ((InterfacePacketBuf&)packet).put_packet(m_pEquipSlot[slot]);
            return true;
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
        ((InterfacePacketBuf&)packet).put_short(m_pEquipSlot[slot].m_fieldb);
        ((InterfacePacketBuf&)packet).put_byte(m_pEquipSlot[slot].m_field0);
        world = G_GameWorld();
        if (world->IsEnchantRevisionChannel())
            ((InterfacePacketBuf&)packet).put_int(0);
        else
            ((InterfacePacketBuf&)packet).put_int(m_pEquipSlot[slot].m_fieldd);
        ((InterfacePacketBuf&)packet).put_byte(m_pEquipSlot[slot].m_amp.getAbilityType());
        ((InterfacePacketBuf&)packet).put_short(m_pEquipSlot[slot].m_amp.getAbilityValue());
        ((InterfacePacketBuf&)packet).put_byte(0);
        ((InterfacePacketBuf&)packet).put_packet(m_pEquipSlot[slot]);
        return true;

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
            ((InterfacePacketBuf&)packet).put_short(m_pAvatarSlot[slot].m_fieldb);
            ((InterfacePacketBuf&)packet).put_byte(m_pAvatarSlot[slot].m_field0);
            ((InterfacePacketBuf&)packet).put_int(m_pAvatarSlot[slot].m_fieldd);
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
            return true;
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
            ((InterfacePacketBuf&)packet).put_short(m_pCreatureSlot[slot].m_fieldb);
            ((InterfacePacketBuf&)packet).put_byte(m_pCreatureSlot[slot].m_field0);
            ((InterfacePacketBuf&)packet).put_int(m_pCreatureSlot[slot].m_fieldd);
            ((InterfacePacketBuf&)packet).put_byte(0);
            ((InterfacePacketBuf&)packet).put_short(0);
            if (m_pCreatureSlot[slot].m_field1 ==
                (unsigned char)Inven_Item::ITEM_TYPE_CREATURE_EQUIP)
            {
                nowTick = OS_API::GetDateTimeTick();
                ((InterfacePacketBuf&)packet).put_int(GetCreatureMgrR()->GetRemainDate(
                    m_pCreatureSlot[slot].m_addInfo2, nowTick));
            }
            ((InterfacePacketBuf&)packet).put_byte(0);
            ((InterfacePacketBuf&)packet).put_packet(m_pCreatureSlot[slot]);
            return true;
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
        world = G_GameWorld();
        if (world->IsIntegratedPvPBaseChannel() &&
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
        ((InterfacePacketBuf&)packet).put_short(m_invenItem[slot].m_fieldb);
        ((InterfacePacketBuf&)packet).put_byte(m_invenItem[slot].m_field0);
        world = G_GameWorld();
        if (world->IsEnchantRevisionChannel())
            ((InterfacePacketBuf&)packet).put_int(0);
        else
            ((InterfacePacketBuf&)packet).put_int(m_invenItem[slot].m_fieldd);
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
        if (m_invenItem[slot].m_field1 == (unsigned char)Inven_Item::ITEM_TYPE_CREATURE_EQUIP)
        {
            int nowTick = 0;
            nowTick = OS_API::GetDateTimeTick();
            ((InterfacePacketBuf&)packet).put_int(GetCreatureMgrR()->GetRemainDate(m_invenItem[slot].m_addInfo2,
                                                         nowTick));
        }
        G_CDataManager()->m_itemUpgrade->make3rdChroniclePacket(
            m_pParent, m_invenItem[slot], &packet);
        ((InterfacePacketBuf&)packet).put_packet(m_invenItem[slot]);
        return true;

    default:
        return false;
    }
