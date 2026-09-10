void CeraShopPurcahseCountBonus::_processBonusItem(CUser* user, int goodsIdx,
                                                   const PurcahseBonus& bonus)
{
    // P212 全头部声明：按 ORIG 槽深序（goods -0x30 → msg -0x1c）声明。
    CCeraShopGoods* goods;
    int price;
    int mileage;
    CItem* item;
    int slot;
    const char* msg;

    if (user == 0)
        return;

    // P17/P46/P251：ORIG `cmp $1;je;cmp $5;je;jmp`（值全程 %eax 无槽）⇒ switch。
    switch (bonus.m_field4)
    {
    case 1:
        // 代币通道：P0——ORIG 无 oldPay/amount/curPay 具名局部（各使用点重算
        // `(unsigned short)bonus.m_field8`，值经 %ebx 跨 receiver call 保活）。
        user->getCurCharacInvenW()->SetPayCoin(
            user->getCurCharacInvenR()->GetPayCoin() +
            (unsigned short)bonus.m_field8);

        user->m_historyLog.PayCoinAdd(user->getCurCharacInvenR()->GetPayCoin(),
                                      (unsigned short)bonus.m_field8,
                                      (eCoinAddReason)5);
        user->SaveMoneyCoin();
        user->SendCoin();

        cMyTrace(kProcessPretty, 0x45, 0)(
            "[PURCAHSE BONUS] add coin (mid:%u, coin:%d)",
            user->get_acc_id(), (unsigned short)bonus.m_field8);
        return;

    case 5:
        // 里程券通道：floor(amount/1000.0f * price) → AddMileage(reason 5)。
        goods = (CCeraShopGoods*)G_CDataManager()->FindGoods(goodsIdx);
        if (goods == 0)
        {
            cMyTrace(kProcessPretty, 0x5f, 0)(
                "[PURCAHSE BONUS] goods is null (goods:%d)", goodsIdx);
            return;
        }
        price = goods->GetCeraPrice();
        // ORIG x87 序：fild price → fild amount → flds 1000 → fdivrp → fmulp
        // ⇒ 原码 `price * ((float)(unsigned short)m_field8 / 1000.0f)`。
        mileage = (int)floor(
            (float)price *
            ((float)(unsigned short)bonus.m_field8 / 1000.0f));
        user->AddMileage(mileage, (eCoinAddReason)5);   // ORIG reason 5
        user->SendCashData(false);

        cMyTrace(kProcessPretty, 0x67, 0)(
            "[PURCAHSE BONUS] add mileage coupon (mid:%u, mileage:%d)",
            user->get_acc_id(), mileage);
        return;

    default:
        break;
    }

    // 物品通道：itemIdx 查表 → make_item → 补 idx/count → 入包。
    item = G_CDataManager()->find_item(bonus.m_field4);
    if (item == 0)
    {
        cMyTrace(kProcessPretty, 0x71, 0)(
            "[PURCAHSE BONUS] item is null (item:%u)",
            bonus.m_field4);
        return;
    }

    Inven_Item inv;
    item->make_item(inv);
    inv.m_addInfo = bonus.m_field4;              // +0x02
    inv.m_addInfo2 = (unsigned short)bonus.m_field8;  // +0x07（ORIG 零扩展）

    if (item->IsCreatureItem() || item->IsAvatarItem())
    {
        cMyTrace(kProcessPretty, 0x7c, 0)(
            "[PURCAHSE BONUS] invalid item type (mid:%u, item:%u)",
            user->get_acc_id(), bonus.m_field4);
        return;
    }

    slot = user->getCurCharacInvenW()->insertItemIntoInventory(
        inv, (eItemAddReason)0x1f, true, false);     // ORIG reason 0x1f
    // P47/P222：ORIG `cmpl $-1,slot; je <mail 外置块>`，update 直落。
    if (slot != -1)
    {
        user->SendUpdateItemList(CUser::eSendTarget_1, (ENUM_ITEMSPACE)0, slot);
        user->SaveInventory();
    }
    else
    {
        msg = g_scriptStringManager_.findString(4, "game_server_msg_104", 0);
        WongWork::CMailBoxHelper::ReqDBSendNewMail(
            user, inv, 0, user->getCurCharacNo(), true, msg, strlen(msg));
    }

    // P92：日志实参从 inv 载荷字段读取（ORIG -0xcb/-0xc6）。
    cMyTrace(kProcessPretty, 0x96, 0)(
        "[PURCAHSE BONUS] add item. (mid:%u, item:%u, count:%d)",
        user->get_acc_id(), inv.m_addInfo, inv.m_addInfo2);
