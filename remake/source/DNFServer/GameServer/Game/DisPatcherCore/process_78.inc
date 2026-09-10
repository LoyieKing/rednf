int DispatcherResetRandomOption::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    MSG_RESET_RANDOM_OPTION& req = *reinterpret_cast<MSG_RESET_RANDOM_OPTION*>(&msg);

    if (!(user->get_state() == 3 && user->getCurCharacR() != 0))
    {
        return 0;
    }
    if (user->CheckInTrade())
    {
        return 0;
    }
    if (user->CheckItemLock(1, (int)req.m_equipSlot))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1c8, 0xd5);
        return 0;
    }
    Inven_Item* target =
        user->getCurCharacInvenW()->GetInvenRef((int)1, (int)req.m_equipSlot);
    if (target == 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1c8, 0x11);
        return 0;
    }
    CItem* shopItem = G_CDataManager()->find_item(target->m_addInfo);
    if (shopItem == 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1c8, 0x11);
        return 0;
    }
    if (ARAD::Singleton<ServiceRestrictManager>::Get()->isRestricted(
            user, RestrictCategory::Enum_1, 0xf))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1c8, 0xd1);
        return 0;
    }
    int secuErr =
        GlobalData::s_pSecuProtectionField->Check(user, (SECURITY_PROTCTION)0x26);
    if (secuErr != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1c8, (unsigned char)secuErr);
        return 0;
    }
    if (target->m_random.m_random.empty())
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1c8, 0x13);
        return 0;
    }
    // [b28d] P196/P171：ORIG 0x081dd4e0 `movzbl 0x12(%eax); movzbl %al,%eax;
    // mov %eax,-0x20; cmpl $0,-0x20; js <err>; cmpl $2,-0x20; jle <ok>` ⇒
    // 原码有具名 int 局部 + 两个独立有符号比较（`<0` 为原码遗留死条件，照抄）。
    const int rarityChoice = static_cast<unsigned char>(req.m_rarityChoice);
    if (rarityChoice < 0 || rarityChoice > 2)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1c8, 0x13);
        return 0;
    }
    // 再铸卷轴校验：GetInvenSlot(invenR,1,scrollSlot)，idx 三选一且数量 > 0
    Inven_Item scroll = user->getCurCharacInvenR()->GetInvenSlot(
        (INVEN_TYPE)1, (int)req.m_scrollSlot);
    // ORIG 0x81dd557/0x81dd598：数量校验与 id 三选一校验各为独立 if+SendCmdErrorPacket
    //（两次 0x15 错误块，不合并）。
    if (scroll.m_addInfo2 <= 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1c8, 0x15);
        return 0;
    }
    // [b28d] P48/P0：ORIG 0x081dd5a8/0x081dd5bd/0x081dd5d2 每次独立重读
    // `mov -0x6b(%ebp),%eax; cmp $K,%eax`（无 scrollIdx 缓存局部）。
    if (!((scroll.m_addInfo == 0xf) || (scroll.m_addInfo == 0x381) ||
          (scroll.m_addInfo == (int)0x292090f2)))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1c8, 0x15);
        return 0;
    }
    // ORIG 函数级 static handle（__cxa_guard 保护），ctor 由 GlobalData.cpp 提供；
    // reset_option 定义链接期 blocked（见本 TU random_option 镜像注释）。
    static random_option::CRandomOptionItemHandle handle;
    int resetErr = (int)handle.reset_option(
        (unsigned long)target->m_addInfo, (ENUM_RARITY)shopItem->get_rarity(),
        rarityChoice, &target->m_random.m_random);   // ORIG 0x081dd68c 用 -0x20 槽值
    if (resetErr != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1c8, (unsigned char)resetErr);
        return 0;
    }
    user->m_historyLog.RandomOption(*target, randomOptionReason::T_RESET);
    statistc_proxy::add(user, "log_random_option", "reset_cnt", 1, "%d,'%s'",
                        user->get_charac_10_level_section(),
                        GlobalData::s_systemTime_.getCurDate());
    if (user->getCurCharacInvenW()->use_item((int)req.m_scrollSlot, 1, 1) != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1c8, 0x16);
        return 0;
    }
    user->SendUpdateItem(CUser::eSendTarget_1, (ENUM_ITEMSPACE)0, (int)req.m_equipSlot);
    user->SendUpdateItem(CUser::eSendTarget_1, (ENUM_ITEMSPACE)0, (int)req.m_scrollSlot);
    user->SendCmdOkPacket((ENUM_CMDPACKET)0x1c8);
    return 0;
