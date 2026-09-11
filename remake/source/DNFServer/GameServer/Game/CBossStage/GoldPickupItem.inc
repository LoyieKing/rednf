// ---- GoldPickupItem（ORIG 0x0814f0e6）----
// 金币按存活队员平分（拾取者拿余数），逐槽写 0x27 包并 erase 掉落物。
// [b28d-62] N82：p167 EXTRA isMemberAlive/getParty 各 1（ORIG 缓存结果）。
//   ORIG 0x0814f0e6
// [b28d-solo-CBossStage] AE：m_memberAlive 内联(P119) + m_party 直读 +
//   P226 语句表达式消 valid 槽 + baseShare 副本 + member=0 预清(P198①)。
bool CBossStage::GoldPickupItem(CUser* user, unsigned int uid,
                                const map_item& item)
{
    int liveCnt = 0;
    for (int i = 0; i < 4; ++i)
    {
        // P0/P119：ORIG 0x0814f0f6 `movzbl 0xd(%edx,%eax,1),%eax` 内联读
        //   m_memberAlive[i]（isMemberAlive 为头内 inline，ORIG 未发射调用）。
        if (m_memberAlive[i])
            ++liveCnt;
    }

    PacketGuard packet;
    packet.clear();
    if (liveCnt == 0)
    {
        packet.put_header(1, 0x2e);
        packet.put_byte(0);
        packet.put_byte(0x15);
        packet.finalize(true);
        user->Send(packet);
        return false;
    }

    int gold = item.m_item.get_add_info();        // map_item+0x10 Inven_Item
    int share = gold / liveCnt;
    int rem = gold % liveCnt;
    int bonus = share + rem;                      // 拾取者份额
    int baseShare = share;                        // ORIG 0x0814f1e5 -0x20 副本

    packet.put_header(0, 0x27);
    packet.put_short((int)uid);
    packet.put_short((int)(user->get_unique_id() & 0xffff));
    // P0：ORIG 0x0814f248/0x814f266 `mov 0x4(%eax),%eax` ⇒ m_party 直访问。
    for (int i = 0; i < 4; ++i)
    {
        CUser* member = 0;                     // ORIG 0x0814f241 预清（P198①）
        member = m_party->get_user(i);
        // P226：ORIG 0x0814f260..28d 短路 bool 物化无槽（mov$1/jmp/mov$0+统一
        //   test）⇒ 语句表达式包住条件，避免 4.4 把具名 bool 落槽。
        if (({ member != 0 && m_party->checkValidUser(i); }))
        {
            packet.put_byte(1);
            int amount = (member == user) ? bonus : baseShare;
            int gained = member->getCurCharacInvenW()->gain_money(
                amount, (eMoneyAddReason)4, true, 0);
            if (gained < amount)
            {
                if (gained < 0)
                    gained = 0;
                member->SendMoneyFullReason((ENUM_MONEY_FULL_REASON)0,
                                            (unsigned int)amount,
                                            (unsigned int)gained);
            }
            packet.put_int(gained);
            packet.put_byte(1);
            packet.put_int(0);
            packet.put_int(0);
        }
        else
        {
            packet.put_byte(0);
            packet.put_int(0);
        }
    }
    m_stageData.m_itemMap.erase((int)uid);
    packet.finalize(true);
    m_party->send_to_party(packet);
    return true;
}