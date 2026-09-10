void CMailBox::CMail::MakePacket(PacketGuard* packet, long nowSec,
                                 CUser const* user) const
{
    packet->put_int((int)m_mailIndex);

    packet->put_int((int)std::strlen(m_senderName));
    packet->put_str(m_senderName, (int)std::strlen(m_senderName));

    packet->put_int(m_package.m_packageMoney);
    packet->put_int(m_package.m_item.m_addInfo);            // item idx（Inven_Item+0x02）
    packet->put_byte(m_package.m_item.m_field0 != 0 ? 1 : 0);         // Inven_Item+0x00

    if (isAvatarPackage())
    {
        CInventory* inv = const_cast<CInventory*>(user->getCurCharacInvenR());
        // ORIG 8551b25/8551b34：GetRemainDate 返回即 put_int（avatar 分支也 put）
        packet->put_int(inv->GetAvatarItemMgrR()->GetRemainDate(
            m_package.m_item.m_addInfo2, (int)nowSec));
    }
    else
    {
        packet->put_int(m_package.m_item.m_addInfo2);           // avatar/creature uid 域
    }

    packet->put_short((int)m_package.m_item.m_fieldb);

    packet->put_byte((int)(m_package.m_item.GetItemAttr() & 0xff));

    packet->put_int(m_package.m_item.m_fieldd);

    // +0x3d 即 Inven_Item 内嵌 stAmplifyOption_t（m_package.m_item +0x11）
    packet->put_byte((int)(m_package.m_item.m_amp.getAbilityType() & 0xff));
    packet->put_short((int)(m_package.m_item.m_amp.getAbilityValue() & 0xffff));

    if (m_package.m_item.m_field1 == 5)                               // ITEM_TYPE creature
    {
        CInventory* inv = const_cast<CInventory*>(user->getCurCharacInvenR());
        user_creature::CCreatureMgr* cmgr =
            const_cast<user_creature::CCreatureMgr*>(inv->GetCreatureMgrR());
        packet->put_int(
            cmgr->getRemainTempPeriod(m_package.m_item.m_addInfo2, (int)nowSec));
    }

    packet->put_packet(m_package.m_item);

    if (isAvatarPackage())
    {
        int uid = m_package.m_item.m_addInfo2;

        packet->put_int(0x1e);
        // ORIG 8551c6d/8551cab：getCurCharacInvenR/GetAvatarItemMgrR 各点重取（P48 去缓存）
        packet->put_binary(const_cast<CInventory*>(user->getCurCharacInvenR())
                               ->GetAvatarItemMgrR()->getJewelSocketData(uid),
                           0x1e);

        const void* exp = const_cast<CInventory*>(user->getCurCharacInvenR())
                              ->GetAvatarItemMgrR()->GetExpansionInfo(uid);
        packet->put_int(4);
        if (exp == 0)
        {
            stAvatarExpansionInfo_t init;
            init.init();
            packet->put_binary(reinterpret_cast<const char*>(&init), 4);
        }
        else
        {
            packet->put_binary(reinterpret_cast<const char*>(exp), 4);
        }
    }

    if (!isUnlimit())
    {
        long remain = m_period - nowSec;
        packet->put_int(std::max<int>((int)remain, 1));      // ORIG 8551d65 std::max<int>
    }
    else
    {
        packet->put_int(0);
    }

    packet->put_int((int)m_letterId);
