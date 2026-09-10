int CInventory::MakeEquipList(void* out, bool flag, ENUM_USERINFO userInfo,
                              ENUM_EQUIPSLOT equipSlot) const
{
    // b28d-solo-CInventory：658/659，帧/槽 + libstdc++ 模板内联 ⇒ N82/N89 封顶。
    // b28d-37 [N82/N89 封顶，ORIG 0x84f?]：帧/槽 + libstdc++ 模板内联残差（658/659）。
    // [§6.2 闭合 · b20] P167 80/80 归零：本 const 方法 6 处 put_binary 已按
    // ORIG 走 PKc（const char*）重载（含 pExpansion 常量化，同宽同槽零扰动）。
    // 新鲜 .o 660 vs ORIG 658（gap -2）；帧 124 vs ORIG 112（+12B/3 槽）属
    // N82 槽分配残差，判「语义完成、N 封顶」。
    // [b28d-tu-CInventory] P329：删具名 `InterfacePacketBuf* buf`（原
    //   `= (InterfacePacketBuf*)out`）、60+ 调用点内联 cast ⇒ MakeEquipList
    //   ndiff 989→945（657/660，P167 CLEAR 保持）。ORIG 虽有 buf 槽 -0x34
    //   （store+load 2 行），但内联形的**装载行序**与 ORIG 逐点对齐，
    //   净收益为正；具名形装载序反置 ⇒ 不恢复。
    // P0：ORIG 全程 94 次 -0x34(%ebp) 重载 ⇒ 原码有具名 buf 局部（非形参直读）。
    InterfacePacketBuf* buf = (InterfacePacketBuf*)out;
    if (buf == 0)
    {
        return 0;
    }
    // ORIG 序言 +0x1e：三变量依声明序初始化写 0（listCount/-0x40、count/-0x38、
    // countIdx/-0x44——countIdx 先 0 后 get_index 覆写，slot 按 AE 口径归一）。
    int listCount = 0;
    int count = 0;
    int countIdx = 0;
    if (!flag)
    {
        listCount = 0xc;
    }
    else if ((int)equipSlot == 0)
    {
        listCount = 0x1a;
    }
    else
    {
        listCount = 0xc;
    }
    countIdx = buf->get_index();
    buf->put_byte(count);
    int nowTick = OS_API::GetDateTimeTick();
    CExpandEquipslot* expand = 0;
    if (m_pParent != 0)
    {
        expand = m_pParent->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)9);
    }
    const Inven_Item* slots;
    if ((int)userInfo == 2 || (int)equipSlot == 0)
    {
        slots = m_invenItem;
    }
    else
    {
        slots = expand->getExpandEquipslotW(equipSlot);
    }
    for (int i = 0; i < listCount; ++i)
    {
        int slotNo = ((int)equipSlot == 0) ? i : i + 10;
        if (slots[i].m_addInfo == 0)
        {
            continue;
        }
        ++count;
        if (!flag)
        {
            buf->put_byte(slotNo);
            if ((int)userInfo == 2 && GetClearAvatar(slotNo) != 0)
            {
                buf->put_int(GetClearAvatar(slotNo));
            }
            else
            {
                buf->put_int(slots[i].m_addInfo);
            }
            if ((int)userInfo == 2)
            {
                stAvatarExpansionInfo_t expansion;
                expansion.init();
                if (-1 < i && i < 10)
                {
                    // ORIG 读 this+(i+0xd0)*8+8/+10 → m_avatarVisible[i].m_field4/m_field6
                    expansion.m_color[0] = m_avatarVisible[i].m_field4;
                    expansion.m_color[1] = m_avatarVisible[i].m_field6;
                }
                buf->put_int(4);
                buf->put_binary((const char*)&expansion, 4);
            }
            else if ((int)userInfo == 0)
            {
                stAvatarExpansionInfo_t expansion;
                expansion.init();
                if (-1 < i && i < 10 && slots[i].m_addInfo != 0)
                {
                    void* pExpansion =
                        m_avatarItemMgr.GetExpansionInfo(slots[i].m_addInfo2);
                    if (pExpansion != 0)
                    {
                        memcpy(&expansion, pExpansion, 4);
                    }
                }
                buf->put_int(4);
                buf->put_binary((const char*)&expansion, 4);
            }
            if (G_GameWorld()->IsIntegratedPvPBaseChannel() &&
                G_CDataManager()->GetUpgradeRevisionPvPChannel() == 0)
            {
                // ORIG +0x45d..+0x4fc：三步位折叠 —— upgradeByte(-0x17 物化)/
                // ampFlag(-0x16)/dead 结果槽(-0x4d，ORIG 读未初始化栈字节，忠实保留)。
                char upgradeByte = 0;
                if (slotNo != 10)
                {
                    if (slotNo < 10 || (unsigned int)(slotNo - 0xc) > 9)
                    {
                        upgradeByte = (char)slots[i].GetUpgrade();
                    }
                    else
                    {
                        upgradeByte = 0;
                    }
                }
                char ampFlag = 0;
                if (slots[i].m_amp.getAbilityType() != 0)
                {
                    ampFlag = 1;
                }
                unsigned char ampBit = (unsigned char)(ampFlag & 1u);
                unsigned char resByte;
                resByte = (unsigned char)((resByte & 0xfffffffeu) | (ampBit & 1u));
                resByte = (unsigned char)((resByte & 1u) |
                                          (unsigned char)((upgradeByte & 0x7f) * 2));
                buf->put_byte(resByte);
            }
            else
            {
                // ORIG +0x4fe..+0x56f：ampFlag(-0x15) 具名 + dead ampPad(-0x16)/
                // dead 结果槽(-0x4e) 三步折叠，upgrade 位 = **二次 GetUpgrade 调用**
                // （非变量复用）——未初始化读忠实保留。
                char ampFlag = 0;
                if (slots[i].m_amp.getAbilityType() != 0)
                {
                    ampFlag = 1;
                }
                unsigned char ampPad;
                unsigned char ampBit = (unsigned char)(ampPad & 1u);
                (void)ampFlag;
                unsigned char resByte;
                resByte = (unsigned char)((resByte & 0xfffffffeu) | (ampBit & 1u));
                resByte = (unsigned char)((resByte & 1u) |
                                          (unsigned char)((slots[i].GetUpgrade() & 0x7f) * 2));
                buf->put_byte(resByte);
            }
        }
        else
        {
            buf->put_byte(slotNo);
            buf->put_int(slots[i].m_addInfo);
            if (slotNo < 10)
            {
                buf->put_int(m_avatarItemMgr.GetRemainDate(slots[i].m_addInfo2, nowTick));
            }
            else
            {
                buf->put_int(slots[i].m_addInfo2);
            }
            if (G_GameWorld()->IsIntegratedPvPBaseChannel() &&
                G_CDataManager()->GetUpgradeRevisionPvPChannel() == 0)
            {
                unsigned char attrByte = 0;
                if ((unsigned int)slotNo <= 0x15u && (((1 << slotNo) & 0x3ff400) != 0))
                {
                    attrByte = (unsigned char)(slots[i].GetReSealCount() << 5);
                }
                else
                {
                    attrByte = slots[i].GetItemAttr();
                }
                buf->put_byte(attrByte);
            }
            else
            {
                buf->put_byte((unsigned char)(slots[i].GetItemAttr() & 0xff));
            }
            buf->put_short((int)slots[i].m_fieldb);
        }
        buf->put_int(GetClearAvatar(slotNo));
        if (!G_GameWorld()->IsEnchantRevisionChannel())
        {
            buf->put_int(slots[i].m_fieldd);
        }
        else
        {
            buf->put_int(0);
        }
        if (flag)
        {
            buf->put_byte((int)slots[i].m_amp.getAbilityType() & 0xff);
            buf->put_short((int)slots[i].m_amp.getAbilityValue() & 0xffff);
            if (slots[i].isAvatarItemType())
            {
                buf->put_int(0x1e);
                buf->put_binary(m_avatarItemMgr.getJewelSocketData(slots[i].m_addInfo2),
                               0x1e);
                buf->put_int(4);
                if (m_pParent != 0 &&
                    (const void*)((CUserCharacInfo*)m_pParent)->getTagCharacInvenR() ==
                        (const void*)this)
                {
                    stAvatarExpansionInfo_t expansion;
                    expansion.init();
                    if (-1 < i && i < 10)
                    {
                        expansion.m_color[0] = m_avatarVisible[i].m_field4;
                        expansion.m_color[1] = m_avatarVisible[i].m_field6;
                    }
                    buf->put_binary((const char*)&expansion, 4);
                }
                else
                {
                    const char* pExpansion =
                        (const char*)m_avatarItemMgr.GetExpansionInfo(slots[i].m_addInfo2);
                    if (pExpansion == 0)
                    {
                        stAvatarExpansionInfo_t expansion;
                        expansion.init();
                        buf->put_binary((const char*)&expansion, 4);
                    }
                    else
                    {
                        buf->put_binary(pExpansion, 4);
                    }
                }
            }
            if (slots[i].m_field1 == 5)
            {
                int creatureTick = OS_API::GetDateTimeTick();
                int remainDate =
                    GetCreatureMgrR()->GetRemainDate(slots[i].m_addInfo2, creatureTick);
                buf->put_int(remainDate);
            }
            G_CDataManager()->m_itemUpgrade->make3rdChroniclePacket(
                m_pParent, slots[i], (PacketGuard*)buf);
            buf->put_packet(slots[i]);
        }
    }
    buf->put_byte(countIdx, count);
    if (flag)
    {
        return 1;
    }
    if (G_GameWorld()->IsIntegratedPvPBaseChannel() &&
        G_CDataManager()->GetUpgradeRevisionPvPChannel() == 0)
    {
        buf->put_byte(0);
    }
    else
    {
        // ORIG 0x84febb0（MakeEquipList.md 伪 C `local_2c + 0x295`）：
        // slots + 10*0x3d + 0x33 = slots[10].m_upgradeSep（原注释 m_amp 系笔误，
        // m_amp 在 +0x11）；以单字节读取器 GetUpgradeSeparate 读取（ORIG 原样）。
        buf->put_byte(slots[10].m_upgradeSep.GetUpgradeSeparate());
    }
    return 1;
