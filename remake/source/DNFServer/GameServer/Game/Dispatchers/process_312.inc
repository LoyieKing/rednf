int Dispatcher_AvatarOptionChange::process(CUser* user, MSG_BASE& msg, ParamBase& paramBase)
{
    AocMsgView& m = *reinterpret_cast<AocMsgView*>(&msg);
    AocParamView& param = *reinterpret_cast<AocParamView*>(&paramBase);

    int err = check_error(user, msg);
    param.m_errorCode = err;
    if (param.m_errorCode > 0)
        return 0;
    if (param.m_errorCode < 0)
        return LineFunc(0x53ab,
                        __PRETTY_FUNCTION__,
                        param.m_errorCode, 0);

    CInventory* invW = user->getCurCharacInvenW();

    // ORIG 在栈上构造影子背包（CInventory + InventoryMemory 0xc140）做删除预演。
    CInventory shadowInv;
    InventoryMemory shadowMem;
    shadowInv.SetInventoryMemory(&shadowMem);
    // ORIG 0x081e083d：setCopy 参数经 getCurCharacInvenRefR 取得（与 R 同址，别名视图）。
    shadowInv.setCopy(*reinterpret_cast<const CInventory*>(user->getCurCharacInvenRefR()));

    if (shadowInv.delete_item(INVEN_TYPE_EQUIP, m.m_consumeSlot, 1,
                              (eItemDelReason)0x2f, false) != 1)
    {
        param.m_errorCode = 0x16;
        return 0;
    }
    Inven_Item* shadowRef = shadowInv.GetInvenRef(INVEN_TYPE_AVATAR, m.m_avatarSlot);
    if (shadowRef == 0)
    {
        param.m_errorCode = 0x11;
        return 0;
    }

    invW->delete_item(INVEN_TYPE_EQUIP, m.m_consumeSlot, 1, (eItemDelReason)0x2f, true);
    Inven_Item* avatarRef = invW->GetInvenRef(INVEN_TYPE_AVATAR, m.m_avatarSlot);

    // ORIG 0x081e0986：_ZN15cUserHistoryLog21UseAvatarOptionChangeEiiii（实现见
    // cUserHistoryLog.cpp:1365，CUser.h:2077 已声明）。
    user->m_historyLog.UseAvatarOptionChange(avatarRef->m_addInfo2,
        avatarRef->m_addInfo, avatarRef->m_fieldb, (int)(signed char)m.m_option);

    avatarRef->m_fieldb = (unsigned short)(signed char)m.m_option;
    invW->SendAvatarEvent(0x2c3, avatarRef->m_addInfo2,
                          (int)(signed char)m.m_option, 0, 0);

    param.m_consumeSlot = m.m_consumeSlot;
    param.m_avatarSlot = m.m_avatarSlot;
    param.m_option = (unsigned char)m.m_option;
    return 0;
