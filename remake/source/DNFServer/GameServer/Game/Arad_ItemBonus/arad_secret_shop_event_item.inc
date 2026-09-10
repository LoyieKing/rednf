void ARAD_ITEM::arad_secret_shop_event_item(CUser* user, unsigned long param,
                                            CStackableItem* stackable)
{
    typedef std::vector<CRabbitBox> RabbitVec;

    if (stackable->m_arad148.m_vec20.empty())
        return;
    if (stackable->get_index() == 0x28be4b)
        return;

    if (stackable->get_index() == 0x28be6a)
    {
        // 纯事件广播分支（不发放物品）。
        PacketGuard pg;
        pg.clear();
        pg.put_header(0, 0x100);
        pg.put_byte(1);
        pg.put_int(param);
        pg.put_int(0x708);
        pg.finalize(true);
        user->Send(pg);

        G_GameWorld()->send_to_area(user->getCurCharacVill(),
                                    user->get_area(false), pg);
        return;
    }

    int lastSlot = -1;
    // P74/P50：box 与 it 均默认构造（ORIG +0x15a/+0x165），循环内赋值。
    CRabbitBox box;
    RabbitVec::iterator it;
    it = stackable->m_arad148.m_vec20.begin();
    while (it != stackable->m_arad148.m_vec20.end())
    {
        box = *it;
        // P0/P48：ORIG 无 rabbitIdx 局部，getItemIndex() 在使用点各调一次
        // （+0x1bc find_item 实参 / +0x1f5 inv.m_addInfo）。
        CItem* item = G_CDataManager()->find_item(box.getItemIndex());
        if (item != 0)
        {
            Inven_Item inv;
            // ORIG 序：+0x1e7 C1Ev → +0x1ef/+0x203 两字段写 → +0x20b make_item。
            inv.m_addInfo = box.getItemIndex();            // +0x02
            inv.m_addInfo2 = box.getItemCount();           // +0x07
            item->make_item(inv);
            lastSlot = user->getCurCharacInvenW()->insertItemIntoInventory(
                inv, (eItemAddReason)0xd, true, false);   // ORIG reason 0xd
            // P225：ORIG `cmpl $-1;setne;test;je <mail>`（update 直落、mail 外置）
            // ⇒ while+goto 形；label 置于 ++it 前（ORIG +0x348 nop/+0x349 ++it）。
            while (lastSlot != -1)
            {
                if (!user->SendUpdateItemList(CUser::eSendTarget_1,
                                              (ENUM_ITEMSPACE)0, lastSlot))
                    return;                            // LAB_0819735a 提前结束
                goto slot_done;
            }
            WongWork::CMailBoxHelper::ReqDBSendNewMail(
                user, inv, 0, user->getCurCharacNo(), true, 0, 0);
        }
    slot_done:
        ++it;
    }

    PacketGuard pg;
    pg.clear();
    pg.put_header(0, 0x100);
    pg.put_byte(1);
    pg.put_int(param);
    pg.put_int(0x708);
    pg.finalize(true);

    G_GameWorld()->send_to_area(user->getCurCharacVill(),
                                user->get_area(false), pg);
