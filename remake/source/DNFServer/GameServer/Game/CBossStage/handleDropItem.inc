bool CBossStage::handleDropItem(CUser* user, int a, int b, char c, int d, int e)
{
    if (e < 0)
        return 0;

    PacketGuard packet;
    packet.clear();
    map_item item;
    if (!dropItem(user, c, d, e, item))
    {
        packet.put_header(1, 0x32);
        packet.put_byte(0);
        packet.put_byte(0x11);
        packet.put_byte((int)(signed char)c);
        packet.finalize(true);
        user->Send(packet);
        return 0;
    }

    // [b28d-62 翻案] ORIG 0x0814ff5f..ff6e `mov 0xa34(%eax),%eax; mov %eax,[S];
    //   lea 0x1(%eax),%edx; mov %edx,0xa34(%eax)` = 后置 ++ 惯用形（拆两句
    //   多 1 条槽回读）⇒ 旧 N18a 封顶判词作废。
    int seq = m_stageData.m_dropItemCnt++;   // ORIG +0xa34（物品序号）
    if (seq == -1)
    {
        packet.put_header(1, 0x32);
        packet.put_byte(0);
        packet.put_byte(0x11);
        packet.put_byte((int)(signed char)c);
        packet.finalize(true);
        user->Send(packet);
        return 0;
    }

    // ORIG 0x0814ff4a+0x6b3 `mov -0x1c(%ebp),%eax; mov %eax,-0x78(%ebp)`：
    //   Add_Item 前把 seq 写入 item+0x04（m_itemIndex）⇒ 源码 `item.m_itemIndex = seq;`
    item.m_itemIndex = seq;
    m_stageData.Add_Item(item);

    packet.put_header(0, 0x28);
    packet.put_short(user->get_unique_id() & 0xffff);
    packet.put_short(a);
    packet.put_short(b);
    packet.put_short(item.m_itemIndex);   // ORIG 0x0815016e 读 item+0x04（-0x78）
    packet.put_int(item.m_item.m_addInfo);
    packet.put_byte((int)item.m_item.GetItemAttr());
    packet.put_int(item.m_item.get_add_info());
    packet.put_short((int)item.m_item.m_fieldb);
    packet.put_int(item.m_item.m_fieldd);
    packet.put_byte((int)item.m_item.m_amp.getAbilityType() & 0xff);
    packet.put_short((int)item.m_item.m_amp.getAbilityValue() & 0xffff);
    packet.put_packet(item.m_item);
    packet.put_byte(item.m_count ? 0 : 1);   // ORIG test/je 布局（==0→1 直落）
    packet.finalize(true);
    m_party->send_to_party(packet);          // ORIG 0x081502a1 m_party 直访问

    packet.clear();
    packet.put_header(1, 0x32);
    packet.put_byte(1);
    packet.put_byte((int)(signed char)c);
    packet.put_short(d);
    packet.put_int(e);
    packet.finalize(true);
    user->Send(packet);
    return 1;
