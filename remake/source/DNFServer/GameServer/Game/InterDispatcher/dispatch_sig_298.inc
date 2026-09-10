int Inter_RequestTagCharacInfo::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    // ORIG 0x084e79f8 入口仅单槽 -0x14=recv（pPkt 经 cast 内联复用同槽，技巧36）。
    char* recv = data;                                 // -0x14(%ebp) 缓存消息指针
    if (user == 0)
        return 0;
    if (user->get_state() <= 2)                        // setle：signed <= 2
        return 0;

    const int tagIdx =
        user->get_charac_idx(
            reinterpret_cast<Inter_RequestTagCharacInfoPktView*>(recv)->m_0);
    if (tagIdx == -1)                                  // cmpl $0xffffffff
        return 0;

    // ORIG 0x084e7a67/0x084e7a76：operator[] 结果免槽直传（技巧37）
    user->setTagCharac(&user->m_characList[tagIdx]);   // vector @ +0x796e8

    user->getTagCharacInvenR()->set_parent(user);      // CInventory::set_parent
    user->getTagCharacSkillR()->set_parent(user);      // SkillSlot::set_parent

    user->setTagCharacInfo(tagIdx,
                           reinterpret_cast<Inter_RequestTagCharacInfoPktView*>(recv)->m_0,
                           reinterpret_cast<TAG_CHARAC_DATA*>(recv + 5));
    user->setTagCharacInven(reinterpret_cast<TAG_CHARAC_INVEN_DATA*>(recv + 0x7d));

    CQuestShop* questShop = reinterpret_cast<CQuestShop*>(
        user->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)0xb));   // 0xb 槽即 QuestShop（ORIG 指针透传）
    questShop->SetTagCharacQuestShopData(
        reinterpret_cast<CHARAC_QUEST_SHOP_DATA*>(recv + 0x40a1));

    user->setCurCharacTagCharacNo(
        reinterpret_cast<Inter_RequestTagCharacInfoPktView*>(recv)->m_0);
    user->setCurCharacStrikerSkillIndex(
        reinterpret_cast<Inter_RequestTagCharacInfoPktView*>(recv)->m_4);

    user->m_characterView.enableSaveCharacView();      // +0x796f4

    // ORIG 0x084e7b6b `movb $0x1,0x14bc(%eax)`：operator[] 重调结果具名字段直折 disp。
    user->m_characList[tagIdx].m_field14BC = 1;

    user->SendTagCharacInfo();
    return 0;
