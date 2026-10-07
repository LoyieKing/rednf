// ORIG 0x081bdc7e T process（报告 process.md ## 反编译 C 全文映射）：
// check_error → get_data → 死亡塔特殊商品(0x29f2a7)状态/塔位校验 → make_item 填充
// Inven_Item → 入包 insertItemIntoInventory(reason 0,log=1,b=0) → 生物物品特判
// （IsCreatureItem && GetItemType()==0x16 → InsertCreatureItem）→ 公会 OnBuyItem →
// use_money(m_80, reason 0, log) → 有材料则 delete_item(1, m_a4, m_9c, 0x1a, 1) →
// _onBuyItemEvent/statistic_process → WriteBuyItem/SendAradEventItemLog/
// [b28d-29] N82/P211：帧 0x8c vs 0x98 + 参数副本槽 + store 装载序，调用点全等  (ORIG 0x081bdc7e insn 373->376)
// eventDeliveryBuyItem(msg+0x15)。失败路径 SendCmdErrorPacket(0x15, 码) 返 -1。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 151（ORIG 373 / ours 376），残差=N7/N82 帧与 callee-saved 角色/P211/N18a 中转/store 序/N82 帧差；沿用上方 b28d-29 归因，封顶。
int DisPatcher_BuyItem::process(CUser* user, MSG_BASE& msg, ParamBase& param)

{
    int errNo = check_error(user, msg);
    if (errNo > 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x15, (unsigned char)errNo);
        return -1;
    }
    if (errNo < 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x15, 0x15);
        return LineFunc(0x1a1,
                        __PRETTY_FUNCTION__,
                        0, user->get_acc_id());
    }
    // [G5-ProcB] ORIG 0x081bdd27 `mov 0x14(%ebp),%eax; mov %eax,-0x2c(%ebp)` /
    // 0x081bdd2d `mov 0x10(%ebp),%eax; mov %eax,-0x28(%ebp)`：param/msg 两个视图
    // 具名局部都在两级 errNo 检查之后才初始化（函数头声明会多 2 条 store 并把
    // 槽基整体下移、丢 ORIG 的 push %edi）。
    BuyItemParam& data = static_cast<BuyItemParam&>(param);
    MSG_BUY_ITEM& req = *reinterpret_cast<MSG_BUY_ITEM*>(&msg);

    errNo = get_data(user, req, data);
    if (errNo != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x15, (unsigned char)errNo);
        return -1;
    }
    if (data.m_pair.first == 0x29f2a7)
    {   // 死亡塔专属商品：须非交易中且已开通对应死亡塔
        if (user->get_state() == 3)
        {
            return LineFunc(0x1b4,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        if (G_CGameManager()->getBossTower(user->getBossTowerIndex()) == 0)
        {
            return LineFunc(0x1b8,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
    }
    data.m_item.m_addInfo = req.m_itemIdx;      // ORIG *(int*)(param+6)=*(int*)(msg+0xd)
    data.m_a8->make_item(data.m_item);          // vtable slot +0x08
    if (!data.m_a8->IsCreatureItem() && !data.m_a8->IsAvatarItem())
    {   // 非生物/非时装：数量取自 msg+0x11（写 m_item.m_addInfo2 @param+0xb）
        data.m_item.m_addInfo2 = req.m_buyCount;
    }
    data.m_a0 = user->getCurCharacInvenW()->insertItemIntoInventory(
        data.m_item, (eItemAddReason)0, true, false);
    if (data.m_a0 < 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x15, 4);
        return -1;
    }
    // [K6-DpcProcess 2026-09-11] P0/P183：ORIG 0x081bdexx 段为
    //   `mov 0x6(%eax),%eax; mov %eax,%ebx`（实参先物化入 callee-saved ebx）
    //   → `call G_CDataManager` → `mov %ebx,0x4(%esp); mov %eax,(%esp)` → `find_item`
    // —— 原码是**单条链式表达式** `G_CDataManager()->find_item(...)`，无具名
    // `dataMgr` 槽；旧形具名局部多一条 store + 一条 load（我方正 gap +3 的主项）。
    CItem* shopItem = G_CDataManager()->find_item(data.m_item.m_addInfo);
    if (shopItem == 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x15, 0x13);
        return -1;
    }
    if (shopItem->IsCreatureItem() && shopItem->GetItemType() == 0x16)
    {   // 生物物品：按有效期限写入生物装备栏
        int expirationDate = shopItem->getExpirationDate();
        int usablePeriod = shopItem->getUsablePeriod();
        user_creature::CCreatureMgr* creatureMgr =
            user->getCurCharacInvenW()->GetCreatureMgrW();
        creatureMgr->InsertCreatureItem(&data.m_item, data.m_a0, 0,
                                        usablePeriod, expirationDate);
    }
    // [DPC_Process] P52：ORIG 0x081bddxx 段先读 data+0x94→%edi / data+0x90→%esi /
    // data+0xa8→%ebx（callee-saved 跨 call 保活），再求 receiver
    // （`call GetServerGroup` → `call GetServerProxy`，结果 %eax 直作 `mov %eax,(%esp)`）
    // ⇒ 原码为单条链式调用、无 `CGuildServerProxy* guildProxy` 具名槽（旧文多 store
    // 一条且实参退回槽读）。
    GlobalData::s_guild_proxy_mgr->GetServerProxy(user->GetServerGroup())
        ->OnBuyItem(user, data.m_a8,
                    (unsigned int)data.m_90, (unsigned int)data.m_94);
    user->getCurCharacInvenW()->use_money(data.m_80, (eMoneySubReason)0, true);
    if (data.m_pair.first != -1)
    {   // 消耗材料（如代币/兑换券）：delete_item(INVEN_TYPE 1, 槽, 数量, 0x1a, log)
        user->getCurCharacInvenW()->delete_item((INVEN_TYPE)1, data.m_a4,
                                                data.m_pair.second,
                                                (eItemDelReason)0x1a, true);
    }
    _onBuyItemEvent(data, user);
    statistic_process(user, data);
    HistoryLog::WriteBuyItem((FILE*)user->m_historyLogFp,
                             data.m_item.m_addInfo, data.m_item.m_addInfo2);
    user->SendAradEventItemLog((unsigned long)data.m_item.m_addInfo,
                               data.m_item.m_addInfo2);
    user->eventDeliveryBuyItem(req.m_deliveryType, data.m_item);
    return 0;
}
