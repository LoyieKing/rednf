bool DisPatcher_DebugCommand::_debugCommandClearAvtInven(CUser* user,
                                                         MSG_DEBUG_COMMAND& /*msg*/)
{
    // [b23S3C] GmCmds_B CClearAvatarInvenCmd::execute(0x084aafb0) 同族：
    // 尾检循环（ORIG 头仅 -0xc i 预清+jmp tail，`i<=0x68` setle 形）；
    // 两读位各独立 getCurCharacInvenR、delete/event 各独立 W（P48）；
    // delete_item 直测 xor$1 形（bool 返）；NumberToString 直落实参槽；
    // send_itemspace 循环外尾段（xor$1 形——Main 已代落 CUser.h bool）。
    int avatarUid;   // [b23S3C] 试：先声明使 4.4 槽序对齐 ORIG（i=-0xc/此=-0x10）
    for (int i = 0; i <= 0x68; ++i)
    {
        if (user->getCurCharacInvenR()->m_pAvatarSlot[i].m_addInfo != 0)
        {
            avatarUid =
                user->getCurCharacInvenR()->m_pAvatarSlot[i].m_addInfo2;
            if (!user->getCurCharacInvenW()->delete_item(
                    static_cast<INVEN_TYPE>(2), i, 1,
                    static_cast<eItemDelReason>(3), true))
            {
                LogManager::logFormat(
                    1, "DisPatcher_DebugCommand_First.h",
                    g_dbgCmdPrettyClearAvtInven, 0x40c,
                    "DisPatcher_DebugCommand::_debugCommandClearAvtInven, pUser(%s)->"
                    "getCurCharacInvenW()->delete_item return FAILED",
                    NumberToString(user->get_acc_id(), 0));
                return false;
            }
            user->getCurCharacInvenW()->SendAvatarEvent(0x36, avatarUid, 1,
                                                        i + 0xa, 0);
        }
    }
    if (user->send_itemspace(1) != 1)
    {
        LogManager::logFormat(
            1, "DisPatcher_DebugCommand_First.h",
            g_dbgCmdPrettyClearAvtInven, 0x417,
            "DisPatcher_DebugCommand::_debugCommandClearAvtInven, pUser(%s)->"
            "send_itemspace( ENUM_ITEMSPACE_AVATAR ) return FAILED",
            NumberToString(user->get_acc_id(), 0));
        return false;
    }
    return true;
