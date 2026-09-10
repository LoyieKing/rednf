bool CBossStage::RandomPickupItem(CUser* user, unsigned int uid,
                                  const map_item& item)
{
    // [b28d-solo] ORIG 0x0814e68f..e6a1 清零序：rolls(-0x44 movl)、bestRoll
    //   （-0x29 **char**：`cmp -0x29,%al; jle` 有符号字节比较）、winner(-0x28)、
    //   i(-0x24)；ORIG 全程 m_party 直读（无 party 局部槽，P0）。
    char rolls[4] = { 0 };
    char bestRoll = 0;
    CUser* winner = 0;
    for (int i = 0; i < 4; ++i)
    {
        CUser* member = 0;              // ORIG 0x0814e6ad 预清（P198①）
        member = m_party->get_user(i);
        // [b28d-solo] ORIG 0x0814e6b8..e6d0：member==0 与 !checkValidUser 各自
        //   跳 continue（P224 短路极性内联直测）。
        if (member == 0 || !m_party->checkValidUser(i))
            continue;
        int trySlot =
            member->getCurCharacInvenR()->tryInsertItemIntoInventory(item.m_item);
        if (trySlot != -1 && isMemberAlive(i))
        {
            rolls[i] = (char)(get_rand_int(100) + 1);
            // [b28d-solo] ORIG 0x0814e7d0..e7e5 两次独立读 rolls[i]（P157 去缓存）
            //   ⇒ 书写序 rolls[i] > bestRoll。
            if (rolls[i] > bestRoll)
            {
                bestRoll = rolls[i];
                winner = member;
            }
        }
        else
        {
            rolls[i] = 0;
        }
    }

    // [b28d-solo] ORIG 0x0814e80e `cmpl $0,-0x28; je <失败包 814ec36>`（失败包在
    //   主流程后）⇒ 正极包裹（P222）。
    if (winner != 0)
    {
    static const char kPretty[] =
        "bool WongWork::CBossStage::RandomPickupItem(CUser*, unsigned int, const map_item&)";
    const CItem* script = G_CDataManager()->find_item(item.m_item.m_addInfo);
    if (script == 0)
    {
        cMyTrace trace(kPretty, 0x39c, 5);
        trace("item not find : %d", item.m_item.m_addInfo);
        return false;
    }

    // ORIG 逐虚槽判定（is_stackable + GetItemType==9 / IsCreatureItem / IsAvatarItem）
    int routeKind = 0;
    if (script->is_stackable())
    {
        if (script->GetItemType() == 9)
            routeKind = 7;
    }
    else
    {
        if (script->IsCreatureItem())
            routeKind = 7;
        else if (script->IsAvatarItem())
            routeKind = 1;
    }
    int slot = -1;
    if (!item.m_item.isAvatarItemType())
    {
        slot = winner->getCurCharacInvenW()->insertItemIntoInventory(
            item.m_item, (eItemAddReason)4, true, true);
    }
    else
    {
        char agency[24];
        memset(agency, 0, sizeof(agency));
        WongWork::CGenUniqueNo::genIPGNo(
            (WongWork::CGenUniqueNo::ENUM_IPGNO_TYPE)5,
            (unsigned int)winner->getCurCharacNo(), agency);
        char periodByte = (char)(unsigned char)item.m_item.m_fieldb;
        slot = winner->addAvatarItem((unsigned long)item.m_item.m_addInfo, 0,
                                     periodByte, agency,
                                     (eAvatarItemAddReason)0x10);
    }
    if (slot == -1)
    {
        PacketGuard packet;
        packet.clear();
        packet.put_header(1, 0x2e);
        packet.put_byte(0);
        packet.put_byte(4);
        packet.finalize(true);
        winner->Send(packet);
        return false;
    }

    m_stageData.m_itemMap.erase((int)uid);
    PacketGuard packet;
    packet.clear();
    packet.put_header(0, 0x27);
    packet.put_short((int)uid);
    packet.put_short((int)(user->get_unique_id() & 0xffff));
    for (int i = 0; i < 4; ++i)
        packet.put_byte((int)rolls[i]);
    packet.put_short((int)(winner->get_unique_id() & 0xffff));
    packet.put_short(slot);
    packet.put_byte(routeKind);
    packet.finalize(true);
    m_party->send_to_party(packet);
    return true;
    }
    PacketGuard packet;
    packet.clear();
    packet.put_header(1, 0x2e);
    packet.put_byte(0);
    packet.put_byte(4);
    packet.finalize(true);
    user->Send(packet);
    return false;
