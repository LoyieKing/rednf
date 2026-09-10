int Dispatcher_StackableActionUse::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // stStackableActionUse pad 未字段化 —— ORIG 报告实证偏移视图：
    char* pbase = reinterpret_cast<char*>(&param);
    int& err = *reinterpret_cast<int*>(pbase + 0x4);              // +0x04 错误码
    short& echoSlot = *reinterpret_cast<short*>(pbase + 0x8);     // +0x08 回显槽位
    char& echoType = *reinterpret_cast<char*>(pbase + 0xa);       // +0x0a 回显类型

    MSG_STACKABLE_ACTION_USE& req = reinterpret_cast<MSG_STACKABLE_ACTION_USE&>(msg);

    err = check_error(user, msg);
    echoType = static_cast<char>(req.m_0f);
    echoSlot = req.m_0d;

    if (err > 0)
    {
        return 0;
    }
    if (err < 0)
    {
        LogManager::logFormat(
            1, "PacketDispatcher_Impl_2.cpp",
            __PRETTY_FUNCTION__,
            0x11f, "Dispatcher Check_Error No(%d)", err);
        return LineFunc(288,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }

    CInventory* inven = const_cast<CInventory*>(user->getCurCharacInvenR());
    Inven_Item item = inven->GetInvenSlot(1, static_cast<int>(req.m_0d));
    unsigned int itemIdx = static_cast<unsigned int>(item.m_addInfo);
    CStackableItem* itemDef =
        static_cast<CStackableItem*>(G_CDataManager()->find_item(static_cast<int>(itemIdx)));
    if (itemDef == 0)
    {
        err = 0x11;
        return 0;
    }

    StackableAction::input_param inputs;   // 前 8 字节未初始化（ORIG 忠实形态）
    inputs.m_user = user;                  // +0x08
    inputs.m_item = itemDef;               // +0x0c
    err = GlobalData::g_Actions->check((int)(char)req.m_14, inputs);
    if (err == 0)
    {
        err = GlobalData::g_Actions->run((int)(char)req.m_14, inputs);
    }
    if (err == 0)
    {
        err = user->getCurCharacInvenW()->use_item(
            (int)req.m_0d, 1, (int)(char)req.m_0f);
        if (err == 0)
        {
            user->_processContinuousItem(itemIdx, itemDef);
        }
    }
    return 0;
