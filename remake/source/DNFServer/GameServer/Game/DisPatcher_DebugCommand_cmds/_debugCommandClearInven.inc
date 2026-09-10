bool DisPatcher_DebugCommand::_debugCommandClearInven(CUser* user,
                                                      MSG_DEBUG_COMMAND& /*msg*/)
{
    // 主背包槽 9..0x137 全清零（equip 动态槽数组 +0x650 = m_pEquipSlot）
    for (int i = 9; i <= 0x137; ++i)
    {
        user->getCurCharacInvenW()->m_pEquipSlot[i].reset();    // Inven_Item::reset stride 0x3d
    }
    if (!user->send_itemspace(0))                               // +0x43 send_itemspace(INVENTORY=0)
    {
        LogManager::logFormat(
            1, "DisPatcher_DebugCommand_First.h",
            g_dbgCmdPrettyClearInven, 0x3af,
            "DisPatcher_DebugCommand::_debugCommandClearInven, pUser(%s)->"
            "send_itemspace( ENUM_ITEMSPACE_INVENTORY ) return FAILED",
            NumberToString(user->get_acc_id(), 0));
        return false;
    }
    return true;
