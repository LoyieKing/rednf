// ORIG 0x08551a36 (T)——docs/class_func_reports/WongWork__CMailBox__CMail/MakePacket.md
void CMailBox::CMail::MakePacket(PacketGuard* packet, long nowSec,
                                 CUser const* user) const
{
    packet->put_int((int)m_mailIndex);

    packet->put_int((int)std::strlen(m_senderName));
    packet->put_str(m_senderName, (int)std::strlen(m_senderName));

    packet->put_int(m_package.m_packageMoney);
    packet->put_int(m_package.m_item.m_addInfo);            // item idx（Inven_Item+0x02）
    packet->put_byte(m_package.m_item.m_seal != 0 ? 1 : 0);         // Inven_Item+0x00

    if (isAvatarPackage())
    {
        // ORIG 8551b25/8551b34：GetRemainDate 返回即 put_int（avatar 分支也 put）
        // ORIG 8551b01：m_addInfo2 先装 ebx、链式调用直传（无具名局部，P52/P183）
        packet->put_int(const_cast<CInventory*>(user->getCurCharacInvenR())
                            ->GetAvatarItemMgrR()->GetRemainDate(
                                m_package.m_item.m_addInfo2, (int)nowSec));
    }
    else
    {
        packet->put_int(m_package.m_item.m_addInfo2);           // avatar/creature uid 域
    }

    packet->put_short((int)m_package.m_item.m_endurance);

    packet->put_byte((int)(m_package.m_item.GetItemAttr() & 0xff));

    packet->put_int(m_package.m_item.m_extendInfo);

    // +0x3d 即 Inven_Item 内嵌 stAmplifyOption_t（m_package.m_item +0x11）
    packet->put_byte((int)(m_package.m_item.m_amp.getAbilityType() & 0xff));
    packet->put_short((int)(m_package.m_item.m_amp.getAbilityValue() & 0xffff));

    if (m_package.m_item.m_itemType == 5)                               // ITEM_TYPE creature
    {
        // ORIG 8551be3：同一 m_addInfo2 先装 ebx 再链式直传（无具名局部）
        packet->put_int(const_cast<user_creature::CCreatureMgr*>(
                            const_cast<CInventory*>(user->getCurCharacInvenR())
                                ->GetCreatureMgrR())
                            ->getRemainTempPeriod(
                                m_package.m_item.m_addInfo2, (int)nowSec));
    }

    packet->put_packet(m_package.m_item);

    if (isAvatarPackage())
    {
        packet->put_int(0x1e);
        // ORIG 8551c64/8551ca2：m_addInfo2 两次独立装载（ebx），getCurCharacInvenR/
        // GetAvatarItemMgrR 各点重取（P48 去缓存，uid 不设具名局部）
        packet->put_binary(const_cast<CInventory*>(user->getCurCharacInvenR())
                               ->GetAvatarItemMgrR()->getJewelSocketData(
                                   m_package.m_item.m_addInfo2),
                           0x1e);

        const void* exp = const_cast<CInventory*>(user->getCurCharacInvenR())
                              ->GetAvatarItemMgrR()->GetExpansionInfo(
                                  m_package.m_item.m_addInfo2);
        packet->put_int(4);
        // [b28d 二轮 L6S6] ORIG 0x08551cde `je 08551cfc<+0x2c6>`：then=非空分支
        //   （put_binary(exp,4) 直落 0x2aa..0x2c4，空分支 init 后置 0x2c6）
        //   ⇒ 正极性 `exp != 0`（P59/P222）。
        if (exp != 0)
        {
            packet->put_binary(reinterpret_cast<const char*>(exp), 4);
        }
        else
        {
            stAvatarExpansionInfo_t init;
            init.init();
            packet->put_binary(reinterpret_cast<const char*>(&init), 4);
        }
    }

    // ORIG 8551d21：call isUnlimit; test %al,%al; je <max 段> ⇒ 正极性直判、then=put_int(0)
    if (isUnlimit())
    {
        packet->put_int(0);
    }
    else
    {
        // ORIG 8551d45..8551d65：无 remain 具名槽，仅 max 的两枚引用临时(-0x14/-0x10)
        // [b28d 二轮 L6S6] ORIG 先算 remain 再 store 常量 1（`sub; mov %eax,-0x14;
        //   movl $0x1,-0x10; lea -0x14→0x4(%esp); lea -0x10→(%esp)`）⇒ cdecl 右起
        //   求值要求常量作 **第二** 实参：`max(1, remain)`（4.4 才先求 remain）。
        packet->put_int(std::max<int>(1, (int)(m_period - nowSec)));
    }

    packet->put_int((int)m_letterId);
}