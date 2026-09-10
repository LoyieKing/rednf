void CUser::giveLotteryCountItem(int itemNo, bool match)
{
    if (getCurCharacR() == 0)
    {
        LogManager::logFormat(
            1, "user.cpp", __PRETTY_FUNCTION__, 0xb45f,
            "giveLotteryCountItem curchar = null id(%s) item_no(%u) match(%d)",
            NumberToString(get_acc_id(), 0), itemNo, match ? 1 : 0);
        return;
    }

    CStackableItem* item =
        (CStackableItem*)G_CDataManager()->find_item(itemNo);
    if (item == 0)
        return;
    if (!verifyStackableItemProcess((StackableAction::Type)0x36, item))
        return;

    std::vector<Inven_Item> rewards;
    // ORIG 0x086916??：m_vec3a0 以 **non-const** vector<int>& 访问
    //   （call _ZNSt6vectorIiSaIiEEixEj，非 const 重载）。
    std::vector<int>& src = item->m_vec3a0;   // ORIG +0x3a0
    // 2026-09-10（C05② 类型旋钮）：ORIG 0x0869196a 为 `cmp -0x30(%ebp),%eax; setg`
    //   （内存直比 + 有符号）⇒ 索引局部必须是有符号 int（size_t 会让 4.4 先载寄存器
    //   并退化为 setb 无符号序）。
    for (int i = 0; i < (int)src.size(); i += 2)
    {
        Inven_Item it;                              // 默认构造后按 ORIG 填 8 字节
        *(int*)((char*)&it + 0) = src[i];
        *(int*)((char*)&it + 4) = src[i + 1];
        rewards.push_back(it);

        if (*(int*)&it == 1 && src[i + 1] >= 0)     // 事件币分支
        {
            int amount = src[i + 1];
            // ORIG 0x086916d4..0x08691715：GetEventCoin() **独立调用两次**
            //（Set 实参一次、EventCoinAdd 实参一次；O0 无 CSE，P48）——
            // 旧版缓存 now 局部少一次调用（p167 miss=2）。
            getCurCharacInvenW()->SetEventCoin(
                getCurCharacInvenR()->GetEventCoin() + (unsigned int)amount);
            m_historyLog.EventCoinAdd(
                getCurCharacInvenR()->GetEventCoin(), amount, (eCoinAddReason)2);
            SaveMoneyCoin();
            SendCoin();
            continue;
        }

        G_CDataManager()->m_itemList->create_item(
            (ENUM_ITEM_CREATE_TYPE)2, it, src[i + 1]);

        bool give = (*(int*)&it != 0) || (it.get_add_info() != 0);
        if (!give)
            continue;

        int slot = getCurCharacInvenW()->insertItemIntoInventory(
            it, (eItemAddReason)0xd, true, true);
        if (slot < 0)
        {
            char msg[256];
            const char* str = g_scriptStringManager_.findString(
                4, "game_server_msg_104", (bool*)0);
            strncpy(msg, str ? str : "", 0xff);
            msg[0xff] = '\0';
            WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                getCurCharacName(), it, 0, (unsigned int)getCurCharacNo(),
                msg, (int)strlen(msg), 0, GetServerGroup(), false, false);
        }
        else
        {
            SendUpdateItem(eSendTarget_1, (ENUM_ITEMSPACE)0, slot);
        }
    }

    PacketGuard packet;
    packet.put_header(0, 0x1d0);
    packet.put_byte((int)rewards.size());
    // ORIG 0x086919??：BOOST_FOREACH 展开（contain/begin/end/deref/next/done +
    //   auto_any_base::cvb ×3；元素 Inven_Item& 直取 +2/+7 字段）。
    BOOST_FOREACH(Inven_Item& ri, rewards)
    {
        packet.put_int(ri.m_addInfo);     // +2
        packet.put_int(ri.m_addInfo2);    // +7
    }
    packet.finalize(true);
    Send(packet);
