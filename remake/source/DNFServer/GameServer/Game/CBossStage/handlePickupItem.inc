bool CBossStage::handlePickupItem(CUser* user, int itemUID, bool flag,
                                  char& outRarity)
{
    (void)flag;   // ORIG 未读取第 3 参（仅按值传递占位）
    static const char kPretty[] =
        "virtual bool WongWork::CBossStage::handlePickupItem(CUser*, int, bool, char&)";

    // P0：ORIG 0x0814fa9e `mov 0x8(%ebp),%eax; lea 0xa18(%eax),%ecx` ⇒
    //   m_stageData.m_itemMap 直访问（无 MapInfo& 引用槽）。
    map_item item;                    // ORIG 0x0814fa8b map_item::C1Ev（-0xd0）
    std::map<int, map_item>::const_iterator it =
        m_stageData.m_itemMap.find(itemUID);
    // [b28d-solo] p167：ORIG 调 `operator!=`（ne）+ je 落失败块 ⇒ 正极翻转。
    if (it != m_stageData.m_itemMap.end())
    {
    item = it->second;                // ORIG 0x0814fb2f rep movsl 结构赋值
    if (item.m_item.m_addInfo == 0)
    {
        // 金币
        if (!GoldPickupItem(user, (unsigned int)itemUID, item))
            return 0;
        return 1;
    }

    CItem* script = G_CDataManager()->find_item(item.m_item.m_addInfo);
    if (script == 0)
    {
        cMyTrace trace(kPretty, 0x57f, 5);
        trace("item not find - item_id : %d, itemUID : %d",
              item.m_item.m_addInfo, itemUID);
        return 0;
    }

    // ORIG 逐虚槽分类（结果仅副作用语义，后续未读取，忠实保留）：
    //   可堆叠 && GetItemType()==9 → 7；否则生物物品 → 7；头像物品 → 1。
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
    // [b28d-solo] p167：ORIG 无 getParty 调用（0 处）⇒ m_party 直读（P0）。
    CParty* party = m_party;
    // [b28f 实验 A 轴/P0] ORIG 0x0814fbc0 段为寄存器直测（`mov $0x1,%eax; jmp;
    //   mov $0x0,%eax; test %al,%al; je`，无槽往返）⇒ 删具名 bool，条件内联。
    if (party->get_member_count() > 1 &&
        getLiveMemberCount() > 1 &&
        item.m_count != 0 &&
        (script->get_rarity() == 4 ||
         script->IsRoutingItem()))
    {
        if (!RoutingPickupItem(user, (unsigned int)itemUID, item))
            return 0;
        return 1;
    }

    outRarity = (char)script->get_rarity();
    // [b28f 实验 A 轴/P0] 同上（ORIG 同段形）。
    if (party->get_member_count() > 1 &&
        getLiveMemberCount() > 1 &&
        item.m_count != 0 &&
        (script->get_rarity() == 1 ||
         script->get_rarity() == 2 ||
         script->get_rarity() == 3))
    {
        if (!RandomPickupItem(user, (unsigned int)itemUID, item))
        {
            cMyTrace trace(kPretty, 0x5ac, 5);
            trace("RandomPickupItem false");
            return 0;
        }
    }
    else
    {
        if (!NormalPickupItem(user, (unsigned int)itemUID, item))
        {
            cMyTrace trace(kPretty, 0x5b4, 5);
            trace("NormalPickupItem false");
            return 0;
        }
    }
    return 1;
    }
    cMyTrace trace(kPretty, 0x56e, 5);
    trace("stageData_.item not find itemUID : %d", itemUID);
    PacketGuard packet;
    packet.clear();
    packet.put_header(1, 0x2e);
    packet.put_byte(0);
    packet.put_byte(4);
    packet.finalize(true);
    user->Send(packet);
    return 0;
