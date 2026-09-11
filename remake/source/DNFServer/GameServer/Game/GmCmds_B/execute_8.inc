// ORIG execute 0x084aaef8：清空背包槽 [3,0x137] 后 send_itemspace(INVENTORY)。
void CClearInventoryCmd::execute()
{
    CUser* user = GetUser();
    for (int slot = 3; slot <= 0x137; ++slot)
    {
        // ORIG：getCurCharacInvenW 结果经 eax 直通 +0x650 读，无具名 inven 槽。
        user->getCurCharacInvenW()->m_pEquipSlot[slot].reset();
    }
    if (user->send_itemspace(0) != 1)                     // ENUM_ITEMSPACE_INVENTORY=0
    {
        // P44：NumberToString 即时求值直压 0x14(%esp)，无 acc 具名槽。
        LogManager::logFormat(
            1, "GameMaster.cpp",
            __PRETTY_FUNCTION__, 0x63b,
            "CClearInventoryCmd::execute, pUser(%s)->send_itemspace( ENUM_ITEMSPACE_INVENTORY ) return FAILED",
            NumberToString(user->get_acc_id(), 0));
    }
}