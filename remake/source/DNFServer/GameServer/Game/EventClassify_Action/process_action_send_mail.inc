// ============================================================================
// process_action_send_mail（ORIG 0x0810cf32）
// 返回：3=user 为空；0x11=物品表未命中；0=成功（MakeItemPacket 失败亦回 0）。
// ============================================================================
// [b28d-solo-EventClassify_Action] ORIG 0x0810cf32，381=381 AE（p167 41/41 CLEAR）。
//   本件修法：①mail.m_itemIdx 先落具名 int itemIdx（ORIG -0x18）、find_item(itemIdx)；
//   ②IsAvatarItem 两处改正极性 `if (IsAvatarItem()) {avatar} else {insert}`（ORIG
//     `test %al,%al; je <else>`）；③PacketGuard 内删 sendOk 预清局部，改
//     `if (!MakeItemPacket(...)) return 0;` 内联（ORIG `call; xor $1; test; je`）。
int CEventActionMng::process_action_send_mail(CUser* user, Action_SendMail& mail,
                                              bool flag)
{
    if (user == 0)
        return 3;

    if (mail.m_gold > 0)
    {
        process_gold_mail_send(user, mail);
        return 0;
    }

    Inven_Item item;
    int itemIdx = mail.m_itemIdx;      // +0x20
    int addInfo = mail.m_addInfo;      // +0x24

    if (mail.m_jobMap.size() != 0)
    {
        std::map<int, std::pair<int, int> >::const_iterator it =
            mail.m_jobMap.find(user->get_charac_job());
        if (it != mail.m_jobMap.end())
        {
            itemIdx = it->second.first;           // +4 pair.first
            addInfo = it->second.second;          // +8 pair.second
        }
    }
    if (addInfo == 0)
        addInfo = 1;

    int slot = -1;
    item.m_addInfo = itemIdx;          // +0x02（item idx）
    item.set_add_info(addInfo);        // +0x07 m_addInfo2

    if (item.m_addInfo == 1)           // 金币（cav）→ 直发流程
    {
        process_coin_send(user, mail, item);
    }
    else
    {
        CItem* citem = G_CDataManager()->find_item(itemIdx);
        if (citem == 0)
            return 0x11;

        citem->make_item(item);        // vtable 槽2
        bool failed = false;

        if (mail.m_field1c != 0)       // 非 0 → coin/avatar 直发
        {
            if (user->CheckInTrade())
            {
                failed = true;
            }
            else if (citem->IsAvatarItem())        // vtable 槽4
            {
                slot = process_avatar_send(user, item, citem);
                if (slot == -1)
                    failed = true;
            }
            else
            {
                slot = user->getCurCharacInvenW()->insertItemIntoInventory(
                    item, (eItemAddReason)0xd, true, true);
                if (slot == -1)
                    failed = true;
            }

            if (!failed && flag)
            {
                PacketGuard packet;
                packet.put_header(0, 0xe);
                if (citem->IsAvatarItem())
                {
                    packet.put_byte(1);
                    packet.put_short(1);
                    if (!user->getCurCharacInvenR()->MakeItemPacket(
                            (INVEN_TYPE)2, slot, packet))
                        return 0;
                }
                else
                {
                    packet.put_byte(0);
                    packet.put_short(1);
                    if (!user->getCurCharacInvenR()->MakeItemPacket(
                            (INVEN_TYPE)1, slot, packet))
                        return 0;
                }
                packet.finalize(true);
                user->Send(packet);
            }
        }

        if (mail.m_field1c == 0 || failed)
            process_mail_send(user, mail, item, citem);
    }

    // 尾部发奖记录：slot / 物品代码 / add_info /（ITEM_TYPE 2、3 记剩余
    // 时间 endurance(+0x0b)，其余记 0）→ mail.m_vec38(+0x38)。
    mail.m_vec38.push_back(slot);
    mail.m_vec38.push_back(item.m_addInfo);
    mail.m_vec38.push_back(item.m_addInfo2);
    if (item.m_itemType == 2 || item.m_itemType == 3)
        mail.m_vec38.push_back((int)item.m_endurance);
    else
        mail.m_vec38.push_back(0);

    user->SendAradEventItemLog((unsigned long)item.m_addInfo,
                               item.m_addInfo2);
    return 0;
}