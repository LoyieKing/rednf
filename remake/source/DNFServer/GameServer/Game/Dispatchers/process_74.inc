int Dispatcher_UseRenameCard::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    stUseRenameCard& st = *reinterpret_cast<stUseRenameCard*>(&param);
    char* pMsg = (char*)&msg;

    int err = check_error(user, msg);
    st.m_4 = err;

    if (err != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1fe, (unsigned char)err);
        return 0;
    }

    Inven_Item item;
    int ret = getItem(*user, 1, *(short*)(pMsg + 0x0e), item);
    if (ret != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1fe, (unsigned char)ret);
        return 0;
    }

    CItem* stackItem = G_CDataManager()->find_item(item.m_addInfo); // +0x02 item idx
    if (stackItem == 0)
    {
        return 0;
    }

    if (!stackItem->is_stackable())
    {
        return 0;
    }

    // TYPE_CHARACTER_NAME_CHANGE = 0x33
    if (!static_cast<CStackableItem*>(stackItem)->verifyStackableAction(
            StackableAction::TYPE_CHARACTER_NAME_CHANGE))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1fe, (unsigned char)ret);
        return 0;
    }

    user->SetCharacViewState(0xff);

    CInventory* invW = user->getCurCharacInvenW();
    int delRet = invW->delete_item((INVEN_TYPE)1, *(short*)(pMsg + 0x0e), 1,
                                   (eItemDelReason)3, true);
    if (delRet == 0)
    {
        user->SendCmdOkPacket((ENUM_CMDPACKET)0x1fe);
        user->ReturnToSelectCharacList(true);
        user->UpdateCharacView();
        return 0;
    }

    user->SetCharacViewState(0);
    user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1fe, 0x11);
    return LineFunc(0x632e,
                    __PRETTY_FUNCTION__,
                    0, user->get_acc_id());
