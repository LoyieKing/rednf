// ============================================================================
// _processCreature —— ORIG 0x08325d66 (T)
// 造物（creature）商品：CCreatureMgr::CheckItem 预检（失败置 outFlag=1）→
// type==5（蛋）时 SetEggItem 孵化转换（失败记 History ERROR 并置 outFlag=1）→
// insertItemIntoInventory(0x19)：成功则 InsertCreatureItem + 刷新列表 +
// SaveInventory，outCode=GetInvenSlot(3,slot).m_addInfo；失败按类型走
// Creature/CashShop 邮件兜底，outCode=item.m_addInfo、返回 0。
// ============================================================================
int CCeraShop::_processCreature(CUser* user, Inven_Item item, int& outCode,
                                int& outFlag, bool, int usablePeriod,
                                int expirationDate)
{
    int slot = -1;
    bool checkOk = true;
    if (!user->getCurCharacInvenW()->GetCreatureMgrW()->CheckItem(user, &item,
                                                                  checkOk))
        outFlag = 1;
    if (checkOk)
    {
        if (item.m_field1 == Inven_Item::ITEM_TYPE_CREATURE_EQUIP)
        {
            if (!user_creature::CCreatureItemConverter::SetEggItem(&item))
            {
                LogManager::logFormat(
                    1, "CeraShop.cpp",
                    __PRETTY_FUNCTION__,
                    0x171f, "History ERROR, CCreatureItemConverter::SetEggItem");
                outFlag = 1;
            }
        }

        slot = user->getCurCharacInvenW()->insertItemIntoInventory(
            item, (eItemAddReason)0x19, true, false);
        if (slot < 0)
        {
            const char* msg =
                g_scriptStringManager_.findString(4, "game_server_msg_104",
                                                  (bool*)0);
            if (item.m_field1 == Inven_Item::ITEM_TYPE_CREATURE_EQUIP)
                WongWork::CMailBoxHelper::ReqDBSendNewCreatureMail(
                    user, user->getCurCharacNo(), item, msg,
                    (int)std::strlen(msg));
            else
                WongWork::CMailBoxHelper::ReqDBSendNewMailCashShop(
                    user, item, 0, (unsigned int)user->getCurCharacNo(), true,
                    msg, (int)std::strlen(msg));
            outCode = item.m_addInfo;
            return 0;
        }
        else
        {
            user->getCurCharacInvenW()->GetCreatureMgrW()->InsertCreatureItem(
                &item, slot, 0x19, usablePeriod, expirationDate);
            user->SendUpdateItemList(CUser::eSendTarget_1, (ENUM_ITEMSPACE)7,
                                     slot);
            user->SaveInventory();
            Inven_Item placed = user->getCurCharacInvenR()->GetInvenSlot(3, slot);
            outCode = placed.m_addInfo;  // ORIG 读 sret 缓冲 +2 处 dword
        }
    }
    return slot;
}