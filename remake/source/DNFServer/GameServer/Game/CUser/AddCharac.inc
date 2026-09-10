int UserMercenaryInfoMgr::AddCharac(CHARAC_LOAD_MERCENARY& info)
{
    // ORIG T 0x081b4300：未命中（CheckValidAppend==1）才添加；先置
    // m_avatarBonus=0xa，再按 8 个 avatar 槽物品取装备 grade 的最小值；
    // 非头像装备/非 CEquipItem 直接清 0。最后 push_back 返回 1。
    // b28d-solo-CUser：CheckValidAppend 已由 Main 代落 bool（CUser.h:793）⇒ P168a/P292
    //   裸形 `!fn()` 复现 ORIG 0x081b4300 `call; xor $0x1,%eax; test %al,%al; jne`。
    if (!CheckValidAppend(info))
        return 0;
    info.m_avatarBonus = 0xa;
    for (int i = 0; i < 8; ++i)
    {
        CItem* item = G_CDataManager()->find_item(info.m_avatarItems[i]);
        CEquipItem* equip = (item == 0) ? 0 : dynamic_cast<CEquipItem*>(item);
        if (equip == 0 || !equip->IsAvatarItem())
        {
            info.m_avatarBonus = 0;
            break;
        }
        // [b23S3C] P48/P0：ORIG get_grade 双臂各重取一次（+0xb5/+0xc9，无 grade 槽）
        if (equip->get_grade() < static_cast<int>(static_cast<signed char>(info.m_avatarBonus)))
            info.m_avatarBonus = static_cast<unsigned char>(equip->get_grade());
    }
    m_characs.push_back(info);
    return 1;
