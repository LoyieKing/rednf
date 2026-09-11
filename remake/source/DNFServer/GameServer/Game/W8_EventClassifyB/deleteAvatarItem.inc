// deleteAvatarItem（ORIG 0x0810f4e2）
int CAvatarDisjointRandomReward::deleteAvatarItem(
    CUser* user, unsigned short slot, Inven_Item& item, CItem const* citem)
{
    if (!user->getCurCharacInvenW()->delete_item((INVEN_TYPE)2, (int)slot, 1,
                                                 (eItemDelReason)6, true))
        return 0x16;
    if (!user->getCurCharacInvenW()->GetAvatarItemMgrW()->UnRegistItem(
            item.m_addInfo2))
        return 0x16;

    user->getCurCharacInvenW()->SendAvatarEvent(0x36, item.m_addInfo2, 5,
                                                (int)slot + 10, 0);
    DB_UpdateAvatarHistory::makeRequest(user->getCurCharacNo(),
                                        item.m_addInfo2, "");
    SendDisjointAvatarStatistic(citem);
    return 0;
}