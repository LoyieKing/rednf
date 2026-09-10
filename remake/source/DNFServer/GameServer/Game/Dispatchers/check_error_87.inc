int Dispatcher_StackableActionUse::check_error(CUser* user, MSG_BASE& msg)
{
    if (user == 0 || user->getCurCharacR() == 0)
    {
        return -1;
    }
    MSG_STACKABLE_ACTION_USE& req = reinterpret_cast<MSG_STACKABLE_ACTION_USE&>(msg);

    // 背包校验和不一致仅记 hack 计数，不中断（ORIG 形态）
    if (user->getCurCharacInvenCheckSum() != req.m_10)
    {
        user->getHackAnalyzer()->addServerHackCnt(
            user, (WongWork::ENUM_HACKTYPE)0x25b, 1, 0, 0);
    }
    if (user->CheckInTrade())
    {
        return 0xd8;
    }
    if (req.m_14 == 0)                                   // +0x14 action type 非零
    {
        return 0x17;
    }

    short slot = req.m_0d;                               // +0x0d 槽位
    // ORIG 0x0825e3xx：GetInvenSlot 用 getCurCharacInvenR、try_use_item 用
    // getCurCharacInvenRefR（两不同 accessor，不缓存为一处）。
    Inven_Item item = reinterpret_cast<const CInventory*>(
                          user->getCurCharacInvenR())
                          ->GetInvenSlot(1, static_cast<int>(slot));
    unsigned int itemIdx = static_cast<unsigned int>(item.m_addInfo);   // +0x02

    CItem* itemDef = G_CDataManager()->find_item(static_cast<int>(itemIdx));
    if (itemDef == 0)
    {
        return 0x11;
    }
    // ORIG test %al 单字节检验（P3 收窄）：verify 通过返回 al==1
    if ((unsigned char)user->verifyStackableItemProcess(
            static_cast<StackableAction::Type>(static_cast<char>(req.m_14)),
            itemDef) != 1u)
    {
        return 0x17;
    }
    if (user->CheckCoolTimeItem(static_cast<unsigned long>(itemIdx)))
    {
        return 0x13;
    }
    int ret = reinterpret_cast<const CInventory*>(
                  user->getCurCharacInvenRefR())
                  ->try_use_item(static_cast<int>(slot), 1,
                                 static_cast<int>(static_cast<char>(req.m_0f)));
    if (ret != 0)
    {
        return ret;
    }
    return 0;
