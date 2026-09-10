int Dispatcher_UseRenameCard::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // [G1Dproc 2026-09-11] ORIG 0x081e731a 起 槽序直证：msg(-0x1c) 先、st(-0x18) 后；
    //   check_error 结果**直写 st+0x04**（call 后紧邻 `mov %eax,%edx; mov -0x18(%ebp),%eax;
    //   mov %edx,0x4(%eax)` 三条，值先入 %edx = P211 档③）
    //   ⇒ P0：无 `int err` 具名槽；两处使用均从 st.m_4 重读。
    char* pMsg = (char*)&msg;
    stUseRenameCard& st = *reinterpret_cast<stUseRenameCard*>(&param);

    st.m_4 = check_error(user, msg);

    if (st.m_4 != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1fe, (unsigned char)st.m_4);
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
