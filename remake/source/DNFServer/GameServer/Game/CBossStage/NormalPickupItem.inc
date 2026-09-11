// ---- NormalPickupItem（ORIG 0x0814ecec）----
// [b28d-62] N82：p167 CLEAR。ORIG 0x0814ecec
// [b28d-solo-CBossStage] 封顶 N7+N82：user==0 正向包裹（P222，ORIG je 814f0d5）、
//   isAvatarItemType 正极+avatar 直落（P168a③b）后 98→37；残差=N7 虚调用
//   %edx↔%eax ×3（12 条）+ agency/PacketGuard 槽 -0x34/-0x40↔-0x40/-0x4c
//   （4.4 聚合优先低槽，N82附）。p167 CLEAR。
bool CBossStage::NormalPickupItem(CUser* user, unsigned int uid,
                                  const map_item& item)
{
    // [b28d-solo] ORIG 0x0814ecfb `cmpl $0,0xc(%ebp); je 814f0d5`（共享尾失败块
    //   直落 epilogue）⇒ P222 正向包裹（4.4 早返恒 `jne; mov $0,%ebx; jmp`）。
    if (user != 0)
    {
    const CItem* script = G_CDataManager()->find_item(item.m_item.m_addInfo);
    if (script == 0)
        return false;

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
    // [b28d-solo] ORIG 0x0814eeaf `call isAvatarItemType; test %al; je <insert>`
    //   正极形 + avatar 块直落（4.4 对 `!cond` 生 xor $1，P168a③b）。
    if (item.m_item.isAvatarItemType())
    {
        char agency[24];
        memset(agency, 0, sizeof(agency));
        WongWork::CGenUniqueNo::genIPGNo(
            (WongWork::CGenUniqueNo::ENUM_IPGNO_TYPE)5,
            (unsigned int)user->getCurCharacNo(), agency);
        // 第三参取 map_item+0x1b = m_item.m_fieldb 低字节（ORIG movzwl+movsbl 内联）
        slot = user->addAvatarItem((unsigned long)item.m_item.m_addInfo, 0,
                                   (char)(unsigned char)item.m_item.m_fieldb, agency,
                                   (eAvatarItemAddReason)0x10);
    }
    else
    {
        slot = user->getCurCharacInvenW()->insertItemIntoInventory(
            item.m_item, (eItemAddReason)4, true, true);
    }
    if (slot == -1)
    {
        PacketGuard packet;
        packet.clear();
        packet.put_header(1, 0x2e);
        packet.put_byte(0);
        packet.put_byte(4);
        packet.finalize(true);
        user->Send(packet);
        return false;
    }

    m_stageData.m_itemMap.erase((int)uid);
    PacketGuard packet;
    packet.clear();
    packet.put_header(0, 0x27);
    packet.put_short((int)uid);
    packet.put_short((int)(user->get_unique_id() & 0xffff));
    for (int i = 0; i < 4; ++i)
        packet.put_byte(0);                        // 无掷点
    packet.put_short((int)(user->get_unique_id() & 0xffff));
    packet.put_short(slot);
    packet.put_byte(routeKind);
    packet.finalize(true);
    m_party->send_to_party(packet);
    return true;
    }
    return false;
}