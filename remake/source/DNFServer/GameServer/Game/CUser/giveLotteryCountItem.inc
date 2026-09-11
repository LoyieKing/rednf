// ORIG T 0x0869154a sz0x664：抽奖计数道具发放（事件币/道具/邮件补偿）。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0869154a | _ZN5CUser20giveLotteryCountItemEib
void CUser::giveLotteryCountItem(int itemNo, bool match)
{
    // 2026-09-10（G3CUser-B 复验）：ORIG 0x086915fa `cmpl $0x0,-0x38(%ebp); je`
    //   （item==0 返回）与 0x08691623 `xor $1; test %al,%al; jne`（!verifyStackable…
    //   返回）为**两条独立守卫且各有 ORIG 直证** ⇒ 我方同形，不可删；
    //   本 .inc 具名量（item/src/i/it/slot/msg/str/rewards/packet）均有 ORIG 槽对应，
    //   残余属跨版本 codegen（N 族），判 N。
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
    for (int i = 0; i < (int)src.size(); )
    {
        Inven_Item it;                              // 默认构造后按 ORIG 填 8 字节
        // 2026-09-11（Lane2，字段错位轴修正 + P183 去具名局部）：ORIG 0x08691672/
        //   0x08691696 把两次 `operator[]` 结果直接 store 到 -0x98/-0x93
        //   （it 基址 -0x9a ⇒ **+0x02 / +0x07** = Inven_Item::m_addInfo@+0x02 /
        //   m_addInfo2@+0x07，Inven_Item.h:269/280），无具名 int 中间量。
        //   我方旧形写 it+0/+4 ⇒ (a) 两字段全错位（写到 m_field0 与 m_addInfo 高位，
        //   读回的是 C1Ev 初值 ⇒ 事件币分支与邮件分支取值全错 = 真语义 bug）；
        //   (b) 多占 itemNo/itemCnt 两槽（帧 0x20c vs ORIG 0x1fc）+ 4 条 store/load。
        it.m_addInfo = (unsigned int)src[i++];
        it.m_addInfo2 = src[i++];
        rewards.push_back(it);

        // ORIG 0x086916b1 `mov -0x98(%ebp),%eax; cmp $0x1,%eax; jne`
        //   + 0x086916c0 `mov -0x93(%ebp),%eax; test %eax,%eax; js`。
        if ((int)it.m_addInfo == 1 && it.m_addInfo2 >= 0)   // 事件币分支
        {
            // ORIG 0x086916d4..0x08691715：GetEventCoin() **独立调用两次**
            //（Set 实参一次、EventCoinAdd 实参一次；O0 无 CSE，P48）——
            // 旧版缓存 now 局部少一次调用（p167 miss=2）。
            // 实参直读 it.m_addInfo2（ORIG 0x086916e1 `mov -0x93(%ebp),%edx; lea (%eax,%edx,1),%ebx`）。
            getCurCharacInvenW()->SetEventCoin(
                getCurCharacInvenR()->GetEventCoin() + (unsigned int)it.m_addInfo2);
            m_historyLog.EventCoinAdd(
                getCurCharacInvenR()->GetEventCoin(), it.m_addInfo2, (eCoinAddReason)2);
            SaveMoneyCoin();
            SendCoin();
            continue;
        }

        G_CDataManager()->m_itemList->create_item(
            (ENUM_ITEM_CREATE_TYPE)2, it, it.m_addInfo2);

        // ORIG 0x08691782 `mov -0x98(%ebp),%eax; test; jne`（m_addInfo!=0 短路）
        //   → 0x0869178c `lea -0x9a(%ebp),%eax; call get_add_info`。
        if (!(it.m_addInfo != 0 || it.get_add_info() != 0))
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
}