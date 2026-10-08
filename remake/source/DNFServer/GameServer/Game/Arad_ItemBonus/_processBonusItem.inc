// ---------------------------------------------------------------------------
// _processBonusItem —— ORIG 0x081ac13e（_processBonusItem.md 全文转写）：
//   type==1 代币；type==5 里程券；其它按 itemIdx 发放物品。
// ---------------------------------------------------------------------------
// b28d-solo-Arad_ItemBonus: verdict=IDENTICAL_AE 332=332，p167 42=42 CLEAR。
// 关键旋钮：switch(m_field4) 三路 cmp/je/jmp（P17/P46/P251）；P0 无 oldPay/
// amount/curPay/mileageF 具名局部（各使用点重算，x87 序 price*(amount/1000.0f)）；
// P212 全头部声明复现槽深序（goods -0x30→msg -0x1c）；P221 cMyTrace 全表达式
// 临时链翻 sibling 槽方向；P92 尾日志读 inv 字段；P315/P254 m_field4 改 unsigned
// 翻 find_item 实参两段装载。ORIG 0x081ac13e。
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

        cMyTrace(__PRETTY_FUNCTION__, 0x45, 0)(
            "[PURCAHSE BONUS] add coin (mid:%u, coin:%d)",
            user->get_acc_id(), (unsigned short)bonus.m_field8);
        return;

    case 5:
        // 里程券通道：floor(amount/1000.0f * price) → AddMileage(reason 5)。
        goods = (CCeraShopGoods*)G_CDataManager()->FindGoods(goodsIdx);
        if (goods == 0)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x5f, 0)(
                "[PURCAHSE BONUS] goods is null (goods:%d)", goodsIdx);
            return;
        }
        price = goods->GetCeraPrice();
        // ORIG x87 序：fild price → fild amount → flds 1000 → fdivrp → fmulp
        // ⇒ 原码 `price * ((float)(unsigned short)m_field8 / 1000.0f)`。
        // [DNF-FIX 2026-10-08 c26] ORIG 0x081ac2e8 fstpl (%esp); call floor@plt ⇒ ::floor(double)，非 std::floor(float)。
        mileage = (int)floor((double)(
            (float)price *
            ((float)(unsigned short)bonus.m_field8 / 1000.0f)));
        user->AddMileage(mileage, (eCoinAddReason)5);   // ORIG reason 5
        user->SendCashData(false);

        cMyTrace(__PRETTY_FUNCTION__, 0x67, 0)(
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
        cMyTrace(__PRETTY_FUNCTION__, 0x71, 0)(
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
        cMyTrace(__PRETTY_FUNCTION__, 0x7c, 0)(
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
    cMyTrace(__PRETTY_FUNCTION__, 0x96, 0)(
        "[PURCAHSE BONUS] add item. (mid:%u, item:%u, count:%d)",
        user->get_acc_id(), inv.m_addInfo, inv.m_addInfo2);
}